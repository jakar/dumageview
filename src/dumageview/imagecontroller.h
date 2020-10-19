#ifndef DUMAGEVIEW_IMAGECONTROLLER_H_
#define DUMAGEVIEW_IMAGECONTROLLER_H_

#include "dumageview/imageinfo.h"

#include <QImage>
#include <QImageReader>
#include <QObject>
#include <QString>

#include <boost/filesystem.hpp>

#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace dumageview::imagecontroller {
  using Path = boost::filesystem::path;
  using PathSet = std::set<Path>;

  struct DirInfo {
    Path path;
    PathSet set;  // never empty after load
    PathSet::iterator current;  // always valid after load
    int index{0};
  };

  enum class Direction : int {
    backward = -1,
    forward = 1,
  };

  using FileExtensionSet = std::set<std::string_view>;

  class ImageController : public QObject {
    Q_OBJECT;

   public:
    ImageController();
    virtual ~ImageController() = default;

    void openImage(QString const& path);
    void saveImage(QString const& path);
    void closeImage();

    void nextImage();
    void prevImage();

    void nextFrame();
    void prevFrame();

    QString dialogDir() const;
    FileExtensionSet const& validFileExtensions() const;

   Q_SIGNALS:

    void imageChanged(QImage const& image, ImageInfo const& info);
    void imageRemoved();

    void openFailed(QString const& message);
    void saveFailed(QString const& message);

   private:
    struct OpenSuccess {};

    std::variant<QString, OpenSuccess> tryRead(QImageReader& reader,
                                               ImageInfo const& info);

    std::variant<QString, OpenSuccess> tryOpen(QString const& path);

    void loadDir();
    void updateImageDirInfo();

    void changeFrame(Direction direction);
    void changeWithinDir(Direction direction);

    //
    // Private data
    //

    std::optional<QImage> _image;
    std::optional<ImageInfo> _imageInfo;
    std::optional<DirInfo> _dirInfo;

    std::unique_ptr<QImageReader> _reader;

    FileExtensionSet _validExtensions;
  };

  class Error : virtual public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };
}

namespace dumageview {
  using imagecontroller::ImageController;
}

#endif  // DUMAGEVIEW_IMAGECONTROLLER_H_
