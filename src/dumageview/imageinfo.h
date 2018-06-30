#ifndef DUMAGEVIEW_IMAGEINFO_H_
#define DUMAGEVIEW_IMAGEINFO_H_

#include <QString>

namespace dumageview
{
  /**
   * Image metadata that is not included in QImage.
   */
  struct ImageInfo
  {
    QString fileName;
    QString filePath;  // absolute

    int dirIndex{0};  // zero-based; add one when displaying
    int dirSize{1};
  };
}

#endif // DUMAGEVIEW_IMAGEINFO_H_
