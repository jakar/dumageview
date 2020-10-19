#include "dumageview/appcontroller.h"

#include "dumageview/application.h"
#include "dumageview/conv_str.h"
#include "dumageview/filedialogrunner.h"
#include "dumageview/mainwindow.h"
#include "dumageview/messageboxrunner.h"
#include "dumageview/qtutil.h"

#include <QDir>
#include <QKeySequence>
#include <QMenuBar>
#include <QString>

#include <string>

namespace {
  using namespace std::literals;
}

namespace dumageview {
  AppController::AppController(cmdline::Args const& cmdArgs)
      : QObject{}, menuMaker_{}, imageController_{},
        mainWindow_(menuMaker_.getActions()) {
    setupConnections();

    getMenuMaker().addActions(getMainWindow());
    getMenuMaker().setupMenuBar(getMainWindow().menuBar());

    if (cmdArgs.imagePath) {
      getImageController().openImage(conv::qstr(cmdArgs.imagePath->string()));
      getMainWindow().adjustSize();
    }

    getMainWindow().show();
    getMainWindow().centerOnScreen();
  }

  void AppController::setupConnections() {
    // -- close / quit actions

    qtutil::connect(&getActions().close,
                    &QAction::triggered,
                    &getMainWindow(),
                    &MainWindow::close);
    qtutil::connect(&getActions().quit,
                    &QAction::triggered,
                    &Application::getSingletonInstance(),
                    &Application::closeAllWindows);

    // -- open / save actions

    qtutil::connect(&getActions().open,
                    &QAction::triggered,
                    this,
                    &AppController::openImage);
    qtutil::connect(&getActions().save,
                    &QAction::triggered,
                    this,
                    &AppController::saveImage);
    qtutil::connect(&getActions().prevImage,
                    &QAction::triggered,
                    &getImageController(),
                    &ImageController::prevImage);
    qtutil::connect(&getActions().nextImage,
                    &QAction::triggered,
                    &getImageController(),
                    &ImageController::nextImage);
    qtutil::connect(&getActions().prevFrame,
                    &QAction::triggered,
                    &getImageController(),
                    &ImageController::prevFrame);
    qtutil::connect(&getActions().nextFrame,
                    &QAction::triggered,
                    &getImageController(),
                    &ImageController::nextFrame);

    // -- window actions

    qtutil::connect(&getActions().showMenuBar,
                    &QAction::toggled,
                    getMainWindow().menuBar(),
                    &QMenuBar::setVisible);
    qtutil::connect(&getActions().fullScreen,
                    &QAction::triggered,
                    &getMainWindow(),
                    &MainWindow::setFullScreen);
    qtutil::connect(&getActions().exitFullScreen,
                    &QAction::triggered,
                    &getMainWindow(),
                    &MainWindow::exitFullScreen);

    // -- zoom actions

    qtutil::connect(&getActions().zoomIn,
                    &QAction::triggered,
                    &getImageWidget(),
                    &ImageWidget::zoomIn);
    qtutil::connect(&getActions().zoomOut,
                    &QAction::triggered,
                    &getImageWidget(),
                    &ImageWidget::zoomOut);
    qtutil::connect(&getActions().zoomOriginal,
                    &QAction::triggered,
                    &getImageWidget(),
                    &ImageWidget::zoomOriginal);
    qtutil::connect(&getActions().zoomToFit,
                    &QAction::triggered,
                    &getImageWidget(),
                    &ImageWidget::zoomToFit);

    // -- image controller signals

    qtutil::connect(&getImageController(),
                    &ImageController::imageChanged,
                    &getMainWindow(),
                    &MainWindow::resetImage);
    qtutil::connect(&getImageController(),
                    &ImageController::imageChanged,
                    &getMenuMaker(),
                    &MenuMaker::enableImageActions);
    qtutil::connect(
      &getImageController(),
      &ImageController::openFailed,
      &getMainWindow(),
      [this](QString const& msg) {
        MessageBoxRunner::critical(&getMainWindow(), "Open Failed", msg);
      }
    );
    qtutil::connect(
      &getImageController(),
      &ImageController::saveFailed,
      &getMainWindow(),
      [this](QString const& msg) {
        MessageBoxRunner::critical(&getMainWindow(), "Save Failed", msg);
      }
    );

    // -- image widget signals

    qtutil::connect(&getImageWidget(),
                    &ImageWidget::contextMenuWanted,
                    &getMenuMaker(),
                    [this](QPoint const& pos) {
                      getMenuMaker().getContextMenu().popup(pos);
                    });
  }

  //
  // Dialogs
  //

  QString AppController::dialogFilter() const {
    using ES = FileDialogRunner::ExtensionSet;
    auto const& exts = getImageController().getValidFileExtensions();
    auto imgFilter = FileDialogRunner::makeSelectionFilter(
      "Images"s,
      ES(std::begin(exts), std::end(exts))
    );
    return imgFilter + ";;All Files (*)";
  }

  void AppController::openImage() {
    auto handler = [this](QString path) {
      if (!path.isEmpty()) {
        getImageController().openImage(path);
      }
    };

    FileDialogRunner::getOpenFileName(handler,
                                      &getMainWindow(),
                                      "Open Image",
                                      getImageController().getDialogDir(),
                                      dialogFilter());
  }

  void AppController::saveImage() {
    auto handler = [this](QString path) {
      if (!path.isEmpty()) {
        getImageController().saveImage(path);
      }
    };

    FileDialogRunner::getSaveFileName(handler,
                                      &getMainWindow(),
                                      "Save Image",
                                      getImageController().getDialogDir(),
                                      dialogFilter());
  }
}
