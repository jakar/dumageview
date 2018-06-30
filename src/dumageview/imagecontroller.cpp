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

namespace dumageview::imagecontroller
{
  namespace
  {
    namespace fs = boost::filesystem;
    namespace hana = boost::hana;

    using namespace std::literals;
    using namespace conv::literals;

    constexpr std::array<std::string_view, 16> defaultFileExtenions{
      "bmp"sv, "gif"sv, "heic"sv, "icns"sv, "jpeg"sv,
      "jpg"sv, "pbm"sv, "pgm"sv, "png"sv, "ppm"sv,
      "tga"sv, "tiff"sv, "wbmp"sv, "webp"sv, "xbm"sv,
      "xpm"sv,
    };
  }

  ImageController::ImageController()
  :
    QObject{},
    _validExtensions(defaultFileExtenions.begin(),
                     defaultFileExtenions.end())
  {
  }

  //
  // Opening
  //

  auto ImageController::tryOpen(QString const& userPath)
    -> std::variant<QString, OpenSuccess>
  {
    fs::path relPath = conv::str(userPath);
    DUMAGEVIEW_LOG_DEBUG("Opening {}...", relPath);

    try
    {
      auto absPath = fs::absolute(relPath);
      auto qpath = conv::qstr(absPath.string());
      auto qname = conv::qstr(absPath.filename().string());

      QImageReader reader(qpath);
      reader.setAutoTransform(true);
      QImage image = reader.read();

      if (image.isNull())
        return reader.errorString();

      _image = image;
      _imageInfo = {qname, qpath};

      return OpenSuccess{};
    }
    catch (fs::filesystem_error const& error)
    {
      return "Filesystem error: %1"_qstr.arg(error.what());
    }
  }

  void ImageController::openImage(QString const& path)
  {
    std::visit(
      hana::overload(
        [this](OpenSuccess) {
          DUMAGEVIEW_ASSERT(_image);
          DUMAGEVIEW_ASSERT(_imageInfo);

          loadDir();
          updateImageDirInfo();
          imageChanged(*_image, *_imageInfo);
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
  // Dir iteration
  //

  void ImageController::changeWithinDir(Direction direction)
  {
    if (!_dirInfo)
      return;

    auto& set = _dirInfo->set;

    auto wrapIndex = [&](int index)
    {
      return math::mod(index, set.size());
    };

    auto wrapIter = [&](PathSet::const_iterator iter)
    {
      if (direction == Direction::backward && iter == set.begin())
        return std::prev(set.end());

      std::advance(iter, enumutil::cast(direction));

      if (direction == Direction::forward && iter == set.end())
        return set.begin();

      return iter;
    };

    int nextIndex = wrapIndex(_dirInfo->index + enumutil::cast(direction));
    auto nextIter = wrapIter(_dirInfo->current);

    while (nextIter != _dirInfo->current)
    {
      auto path = _dirInfo->path / *nextIter;
      auto result = tryOpen(conv::qstr(path.string()));

      if (std::holds_alternative<OpenSuccess>(result))
      {
        DUMAGEVIEW_ASSERT(_image);
        DUMAGEVIEW_ASSERT(_imageInfo);

        _dirInfo->index = nextIndex;
        _dirInfo->current = nextIter;
        updateImageDirInfo();

        imageChanged(*_image, *_imageInfo);
        return;
      }

      // continue without bad image
      log::warn("Could not open image: {}: {}",
                path, conv::str(std::get<QString>(result)));

      auto badIter = nextIter;
      nextIter = wrapIter(badIter);
      DUMAGEVIEW_ASSERT(nextIter != badIter);

      set.erase(badIter);

      if (direction == Direction::forward)
        nextIndex = wrapIndex(nextIndex);
      else
        nextIndex = wrapIndex(nextIndex - 1);
    }
  }

  void ImageController::nextImage()
  {
    changeWithinDir(Direction::forward);
  }

  void ImageController::prevImage()
  {
    changeWithinDir(Direction::backward);
  }

  void ImageController::loadDir()
  {
    DUMAGEVIEW_ASSERT(_imageInfo);

    fs::path filePath = conv::str(_imageInfo->filePath);
    auto dirPath = filePath.parent_path();

    _dirInfo = DirInfo{};
    _dirInfo->path = dirPath;

    auto handleError = [&](auto const& error)
    {
      log::error("Error reading directory {}: {}",
                 dirPath, error.what());
      _dirInfo.reset();
    };

    try
    {
      // add entries with ok filenames
      for (auto const& entry : fs::directory_iterator(dirPath))
      {
        auto const& p = entry.path();
        if (!fs::is_regular_file(p))
          continue;

        auto ext = p.extension().string();
        boost::trim_left_if(ext, [](char c) { return c == '.'; });
        boost::to_lower(ext, std::locale::classic());

        if (auto it = _validExtensions.find(ext); it == _validExtensions.end())
          continue;

        _dirInfo->set.insert(p.filename());
      }

      // find current file for index
      auto fileName = filePath.filename();
      _dirInfo->current = _dirInfo->set.find(fileName);

      if (_dirInfo->current == _dirInfo->set.end())
        throw Error(fmt::format("could not find {} in directory", fileName));

      // O(n) to find index, but we did just iterate through the dir
      auto index = std::distance(_dirInfo->set.begin(), _dirInfo->current);
      _dirInfo->index = boost::numeric_cast<int>(index);
    }
    catch (fs::filesystem_error const& e)
    {
      handleError(e);
    }
    catch (Error const& e)
    {
      handleError(e);
    }
  }

  void ImageController::updateImageDirInfo()
  {
    if (!_dirInfo)
      return;

    DUMAGEVIEW_ASSERT(_imageInfo);

    _imageInfo->dirIndex = _dirInfo->index;
    _imageInfo->dirSize = boost::numeric_cast<int>(_dirInfo->set.size());
  }

  //
  // Other slots
  //

  void ImageController::saveImage(QString const& path)
  {
    DUMAGEVIEW_LOG_DEBUG("Saving {}...", conv::str(path));

    if (!_image)
    {
      log::warn("no image to save");
      return;
    }

    QImageWriter writer(path);
    bool writeOK = writer.write(*_image);

    if (!writeOK)
    {
      saveFailed("Could not save image: %1: %2"_qstr
                 .arg(path, writer.errorString()));
      return;
    }

    log::debug("saved image: {}", conv::str(path));
  }

  void ImageController::closeImage()
  {
    _image.reset();
    _imageInfo.reset();
    _dirInfo.reset();
    imageRemoved();
  }

  //
  // Extra info
  //

  FileExtensionSet const& ImageController::validFileExtensions() const
  {
    // TODO: maybe make this list depend on available plugins
    return _validExtensions;
  }

  QString ImageController::dialogDir() const
  {
    if (_dirInfo)
      return conv::qstr(_dirInfo->path.string());
    else
      return QDir::currentPath();
  }
}
