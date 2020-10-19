#include "dumageview/mainwindow.h"

#include "dumageview/appcontroller.h"
#include "dumageview/application.h"
#include "dumageview/conv_vec.h"
#include "dumageview/messageboxrunner.h"
#include "dumageview/qtutil.h"

#include <QAction>
#include <QDesktopWidget>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QSize>
#include <QWidget>

namespace dumageview {
  MainWindow::MainWindow(ActionSet& actions)
      : QMainWindow{},
        actions_{actions},
        imageArea_(actions, this) {
    setCentralWidget(&imageArea_);
    centerOnScreen();
  }

  //
  // Window movement
  //

  void MainWindow::centerOnScreen() {
    QRect screenRect = QApplication::desktop()->availableGeometry(this);
    QSize margin = screenRect.size() - size();
    move(conv::qpoint(margin / 2));
  }

  void MainWindow::setFullScreen(bool enable) {
    if (enable) {
      showFullScreen();
    } else {
      showNormal();
    }
  }

  void MainWindow::exitFullScreen() {
    showNormal();
    actions_.fullScreen.setChecked(false);
  }

  //
  // Image change handling
  //

  void MainWindow::resetImage(QImage const& image, ImageInfo const& info) {
    setWindowTitle(
      QString("%1 : %2 (%3 / %4) - %5")
        .arg(info.fileName)
        .arg(info.frame)
        .arg(info.dirIndex + 1)
        .arg(info.dirSize)
        .arg(Application::getSingletonInstance().applicationDisplayName())
    );
    getImageArea().resetImage(image);
  }

  void MainWindow::removeImage() {
    setWindowTitle(Application::getSingletonInstance().applicationDisplayName());
    getImageArea().removeImage();
  }
}
