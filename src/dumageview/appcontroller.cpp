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

namespace
{
  using namespace std::literals;
}

namespace dumageview
{
  AppController::AppController(cmdline::Args const& cmdArgs_)
  :
    QObject{},
    _menuMaker{},
    _imageController{},
    _mainWindow(_menuMaker.actions())
  {
    setupConnections();

    menuMaker().addActions(mainWindow());
    menuMaker().setupMenuBar(mainWindow().menuBar());

    if (cmdArgs_.imagePath)
    {
      imageController().openImage(conv::qstr(cmdArgs_.imagePath->string()));
      mainWindow().adjustSize();
    }

    mainWindow().show();
    mainWindow().centerOnScreen();
  }

  void AppController::setupConnections()
  {
    // -- close / quit actions

    qtutil::connect(
      &actions().close, &QAction::triggered,
      &mainWindow(), &MainWindow::close
    );
    qtutil::connect(
      &actions().quit, &QAction::triggered,
      &Application::singletonInstance(), &Application::closeAllWindows
    );

    // -- open / save actions

    qtutil::connect(
      &actions().open, &QAction::triggered,
      this, &AppController::openImage
    );
    qtutil::connect(
      &actions().save, &QAction::triggered,
      this, &AppController::saveImage
    );
    qtutil::connect(
      &actions().prevImage, &QAction::triggered,
      &imageController(), &ImageController::prevImage
    );
    qtutil::connect(
      &actions().nextImage, &QAction::triggered,
      &imageController(), &ImageController::nextImage
    );
    qtutil::connect(
      &actions().prevFrame, &QAction::triggered,
      &imageController(), &ImageController::prevFrame
    );
    qtutil::connect(
      &actions().nextFrame, &QAction::triggered,
      &imageController(), &ImageController::nextFrame
    );

    // -- window actions

    qtutil::connect(
      &actions().showMenuBar, &QAction::toggled,
      mainWindow().menuBar(), &QMenuBar::setVisible
    );
    qtutil::connect(
      &actions().fullScreen, &QAction::triggered,
      &mainWindow(), &MainWindow::setFullScreen
    );
    qtutil::connect(
      &actions().exitFullScreen, &QAction::triggered,
      &mainWindow(), &MainWindow::exitFullScreen
    );

    // -- zoom actions

    qtutil::connect(
      &actions().zoomIn, &QAction::triggered,
      &imageWidget(), &ImageWidget::zoomIn
    );
    qtutil::connect(
      &actions().zoomOut, &QAction::triggered,
      &imageWidget(), &ImageWidget::zoomOut
    );
    qtutil::connect(
      &actions().zoomOriginal, &QAction::triggered,
      &imageWidget(), &ImageWidget::zoomOriginal
    );
    qtutil::connect(
      &actions().zoomToFit, &QAction::triggered,
      &imageWidget(), &ImageWidget::zoomToFit
    );

    // -- image controller signals

    qtutil::connect(
      &imageController(), &ImageController::imageChanged,
      &mainWindow(), &MainWindow::resetImage
    );
    qtutil::connect(
      &imageController(), &ImageController::imageChanged,
      &menuMaker(), &MenuMaker::enableImageActions
    );
    qtutil::connect(
      &imageController(), &ImageController::openFailed,
      &mainWindow(),
      [this](QString const& msg) {
        MessageBoxRunner::critical(&mainWindow(), "Open Failed", msg);
      }
    );
    qtutil::connect(
      &imageController(), &ImageController::saveFailed,
      &mainWindow(),
      [this](QString const& msg) {
        MessageBoxRunner::critical(&mainWindow(), "Save Failed", msg);
      }
    );

    // -- image widget signals

    qtutil::connect(
      &imageWidget(), &ImageWidget::contextMenuWanted,
      &menuMaker(),
      [this](QPoint const& pos) {
        menuMaker().contextMenu().popup(pos);
      }
    );
  }

  //
  // Dialogs
  //

  QString AppController::dialogFilter() const
  {
    using ES = FileDialogRunner::ExtensionSet;
    auto const& exts = imageController().validFileExtensions();
    auto imgFilter = FileDialogRunner::makeSelectionFilter(
      "Images"s, ES(std::begin(exts), std::end(exts)));
    return imgFilter + ";;All Files (*)";
  }

  void AppController::openImage()
  {
    auto handler = [this](QString path)
    {
      if (!path.isEmpty())
        imageController().openImage(path);
    };

    FileDialogRunner::getOpenFileName(
      handler,
      &mainWindow(),
      "Open Image",
      imageController().dialogDir(),
      dialogFilter()
    );
  }

  void AppController::saveImage()
  {
    auto handler = [this](QString path)
    {
      if (!path.isEmpty())
        imageController().saveImage(path);
    };

    FileDialogRunner::getSaveFileName(
      handler,
      &mainWindow(),
      "Save Image",
      imageController().dialogDir(),
      dialogFilter()
    );
  }
}
