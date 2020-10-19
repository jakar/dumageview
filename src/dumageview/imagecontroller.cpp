#include "dumageview/imagecontroller.h"

#include "dumageview/conv_str.h"
#include "dumageview/enumutil.h"
#include "dumageview/log.h"
#include "dumageview/math.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QString>

#include <boost/algorithm/string.hpp>
#include <boost/hana.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <array>
#include <iterator>

namespace dumageview::imagecontroller {
  namespace {
    namespace fs = boost::filesystem;
    namespace hana = boost::hana;

    using namespace std::literals;
    using namespace conv::literals;

    constexpr std::array<std::string_view, 16> defaultFileExtenions{
      "bmp"sv,
      "gif"sv,
      "heic"sv,
      "icns"sv,
      "jpeg"sv,
      "jpg"sv,
      "pbm"sv,
      "pgm"sv,
      "png"sv,
      "ppm"sv,
      "tga"sv,
      "tiff"sv,
      "wbmp"sv,
      "webp"sv,
      "xbm"sv,
      "xpm"sv,
    };
  }

  ImageController::ImageController()
      : QObject{}, validExtensions_(defaultFileExtenions.begin(),
                                    defaultFileExtenions.end()) {
  }

  //
  // Opening
  //

  auto ImageController::tryRead(QImageReader& reader,
                                ImageInfo const& info) -> std::variant<QString, OpenSuccess> {
    QImage image = reader.read();
    if (image.isNull()) {
      return reader.errorString();
    }

    image_ = image;
    imageInfo_ = info;
    imageInfo_->frame = reader.currentImageNumber();
    imageInfo_->numFrames = reader.imageCount();

    return OpenSuccess{};
  }

  auto ImageController::tryOpen(QString const& userPath) -> std::variant<QString, OpenSuccess> {
    fs::path relPath = conv::str(userPath);
    DUMAGEVIEW_LOG_DEBUG("Opening {}...", relPath);

    try {
      auto absPath = fs::absolute(relPath);
      auto qpath = conv::qstr(absPath.string());
      auto qname = conv::qstr(absPath.filename().string());

      auto reader = std::make_unique<QImageReader>(qpath);
      reader->setAutoTransform(true);

      auto result = tryRead(*reader, {qname, qpath});
      if (std::holds_alternative<OpenSuccess>(result)) {
        reader_ = std::move(reader);
      }
      return result;
    } catch (fs::filesystem_error const& error) {
      return "Filesystem error: %1"_qstr.arg(error.what());
    }
  }

  void ImageController::openImage(QString const& path) {
    std::visit(
      hana::overload(
        [this](OpenSuccess) {
          DUMAGEVIEW_ASSERT(image_);
          DUMAGEVIEW_ASSERT(imageInfo_);

          loadDir();
          updateImageDirInfo();
          imageChanged(*image_, *imageInfo_);
        },
        [&](QString const& error) {
          auto msg = "Could not open image: %1: %2"_qstr.arg(path, error);
          openFailed(msg);
        }
      ),
      tryOpen(path)
    );
  }

  //
  // Multi-part images
  //

  void ImageController::changeFrame(Direction direction) {
    if (!reader_ || !imageInfo_) {
      return;
    }

    int newFrame = math::mod(imageInfo_->frame + enumutil::cast(direction),
                             imageInfo_->numFrames);

    bool jumpOK = reader_->jumpToImage(newFrame);
    if (!jumpOK) {
      log::warn("Could not jump to new frame: {}", newFrame);
      return;
    }

    auto handler = hana::overload(
      [&](QString const& error) {
        log::warn("Could not read frame {}: {}", newFrame, conv::str(error));
      },
      [&](OpenSuccess) {
        DUMAGEVIEW_ASSERT(image_);
        DUMAGEVIEW_ASSERT(imageInfo_);
        imageChanged(*image_, *imageInfo_);
      }
    );
    std::visit(handler, tryRead(*reader_, *imageInfo_));
  }

  void ImageController::nextFrame() {
    changeFrame(Direction::forward);
  }

  void ImageController::prevFrame() {
    changeFrame(Direction::backward);
  }

  //
  // Dir iteration
  //

  void ImageController::changeWithinDir(Direction direction) {
    if (!dirInfo_) {
      return;
    }

    auto& set = dirInfo_->set;

    auto wrapIndex = [&](int index) {
      return math::mod(index, set.size());
    };

    auto wrapIter = [&](PathSet::const_iterator iter) {
      if (direction == Direction::backward && iter == set.begin()) {
        return std::prev(set.end());
      }
      std::advance(iter, enumutil::cast(direction));

      if (direction == Direction::forward && iter == set.end()) {
        return set.begin();
      }
      return iter;
    };

    int nextIndex = wrapIndex(dirInfo_->index + enumutil::cast(direction));
    auto nextIter = wrapIter(dirInfo_->current);

    while (nextIter != dirInfo_->current) {
      auto path = dirInfo_->path / *nextIter;
      auto result = tryOpen(conv::qstr(path.string()));

      if (std::holds_alternative<OpenSuccess>(result)) {
        DUMAGEVIEW_ASSERT(image_);
        DUMAGEVIEW_ASSERT(imageInfo_);

        dirInfo_->index = nextIndex;
        dirInfo_->current = nextIter;
        updateImageDirInfo();

        imageChanged(*image_, *imageInfo_);
        return;
      }

      // continue without bad image
      log::warn("Could not open image: {}: {}",
                path,
                conv::str(std::get<QString>(result)));

      auto badIter = nextIter;
      nextIter = wrapIter(badIter);
      DUMAGEVIEW_ASSERT(nextIter != badIter);

      set.erase(badIter);

      if (direction == Direction::forward) {
        nextIndex = wrapIndex(nextIndex);
      } else {
        nextIndex = wrapIndex(nextIndex - 1);
      }
    }
  }

  void ImageController::nextImage() {
    changeWithinDir(Direction::forward);
  }

  void ImageController::prevImage() {
    changeWithinDir(Direction::backward);
  }

  void ImageController::loadDir() {
    DUMAGEVIEW_ASSERT(imageInfo_);

    fs::path filePath = conv::str(imageInfo_->filePath);
    auto dirPath = filePath.parent_path();

    dirInfo_ = DirInfo{};
    dirInfo_->path = dirPath;

    auto handleError = [&](auto const& error) {
      log::error("Error reading directory {}: {}", dirPath, error.what());
      dirInfo_.reset();
    };

    try {
      // add entries with ok filenames
      for (auto const& entry : fs::directory_iterator(dirPath)) {
        auto const& p = entry.path();
        if (!fs::is_regular_file(p)) {
          continue;
        }

        auto ext = p.extension().string();
        boost::trim_left_if(ext, [](char c) { return c == '.'; });
        boost::to_lower(ext, std::locale::classic());

        if (validExtensions_.count(ext) == 0) {
          continue;
        }
        dirInfo_->set.insert(p.filename());
      }

      // find current file for index
      auto fileName = filePath.filename();
      dirInfo_->current = dirInfo_->set.find(fileName);

      if (dirInfo_->current == dirInfo_->set.end()) {
        throw Error(fmt::format("could not find {} in directory", fileName));
      }

      // O(n) to find index, but we did just iterate through the dir
      auto index = std::distance(dirInfo_->set.begin(), dirInfo_->current);
      dirInfo_->index = boost::numeric_cast<int>(index);
    } catch (fs::filesystem_error const& e) {
      handleError(e);
    } catch (Error const& e) {
      handleError(e);
    }
  }

  void ImageController::updateImageDirInfo() {
    if (!dirInfo_) {
      return;
    }
    DUMAGEVIEW_ASSERT(imageInfo_);

    imageInfo_->dirIndex = dirInfo_->index;
    imageInfo_->dirSize = boost::numeric_cast<int>(dirInfo_->set.size());
  }

  //
  // Other slots
  //

  void ImageController::saveImage(QString const& path) {
    DUMAGEVIEW_LOG_DEBUG("Saving {}...", conv::str(path));

    if (!image_) {
      log::warn("no image to save");
      return;
    }

    QImageWriter writer(path);
    bool writeOK = writer.write(*image_);

    if (!writeOK) {
      saveFailed("Could not save image: %1: %2"_qstr.arg(path, writer.errorString()));
      return;
    }

    log::debug("saved image: {}", conv::str(path));
  }

  void ImageController::closeImage() {
    image_.reset();
    imageInfo_.reset();
    dirInfo_.reset();

    reader_.reset();

    imageRemoved();
  }

  //
  // Extra info
  //

  FileExtensionSet const& ImageController::getValidFileExtensions() const {
    // TODO: maybe make this list depend on available plugins
    return validExtensions_;
  }

  QString ImageController::getDialogDir() const {
    if (dirInfo_) {
      return conv::qstr(dirInfo_->path.string());
    } else {
      return QDir::currentPath();
    }
  }
}
