#ifndef DUMAGEVIEW_MAINWINDOW_H_
#define DUMAGEVIEW_MAINWINDOW_H_

#include "dumageview/actionset.h"
#include "dumageview/classtools.h"
#include "dumageview/imageinfo.h"
#include "dumageview/imagewidget.h"

#include <QMainWindow>

namespace dumageview {
  class MainWindow : public QMainWindow {
    Q_OBJECT;

   public:
    explicit MainWindow(ActionSet& actions_);

    virtual ~MainWindow() = default;

    //
    // Window modifications
    //

    void centerOnScreen();

    void setFullScreen(bool enabled);

    void exitFullScreen();

    //
    // Image changes
    //

    void resetImage(QImage const& image, ImageInfo const& info);

    void removeImage();

    //
    // Public accessors
    //

    ImageWidget& imageArea() {
      return _imageArea;
    }

   private:
    //
    // Private data
    //

    ActionSet& _actions;
    ImageWidget _imageArea;
  };
}

#endif  // DUMAGEVIEW_MAINWINDOW_H_
