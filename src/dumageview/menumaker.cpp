#include "dumageview/menumaker.h"

#include "dumageview/assert.h"

#include <QKeySequence>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>

#include <utility>

namespace dumageview {
  MenuMaker::MenuMaker() : QObject{}, actions_{} {
    setupActions();
    setupContextMenu();
    disableImageActions();
  }

  void MenuMaker::setupActions() {
    auto setA = [](QAction& action,
                   QString const& text,
                   QList<QKeySequence> const& keys) {
      action.setText(text);
      action.setShortcuts(keys);
    };

    //
    // Opening / closing
    //

    setA(actions_.open, "&Open...", {QKeySequence::Open});
    setA(actions_.save, "&Save...", {QKeySequence::Save});
    setA(actions_.close, "&Close", {QKeySequence::Close});
    setA(actions_.quit, "&Quit", {QKeySequence::Quit});

    //
    // Zooming
    //

    setA(actions_.zoomIn,
         "Zoom In",
         {Qt::Key_Plus,
          Qt::CTRL + Qt::Key_Plus,
          Qt::Key_Equal,
          Qt::CTRL + Qt::Key_Equal});

    setA(actions_.zoomOut,
         "Zoom Out",
         {Qt::Key_Minus, Qt::CTRL + Qt::Key_Minus});

    setA(actions_.zoomOriginal,
         "Original Size",
         {Qt::Key_1, Qt::CTRL + Qt::Key_1});

    setA(actions_.zoomToFit,
         "Fit to Window",
         {Qt::Key_0, Qt::CTRL + Qt::Key_0, Qt::Key_Z});

    // image widget sets enabled state for zoomToFit
    actions_.zoomToFit.setEnabled(false);

    //
    // Panning
    //

    setA(actions_.panUp, "Pan Up", {Qt::Key_Up});
    setA(actions_.panDown, "Pan Down", {Qt::Key_Down});
    setA(actions_.panLeft, "Pan Left", {Qt::Key_Left});
    setA(actions_.panRight, "Pan Right", {Qt::Key_Right});

    //
    // Dir navigation
    //

    setA(actions_.prevImage,
         "Previous Image",
         {Qt::Key_PageUp,
          QKeySequence::Back,
          Qt::Key_K,
          Qt::Key_P,
          Qt::SHIFT + Qt::Key_Space});

    setA(actions_.nextImage,
         "Next Image",
         {Qt::Key_PageDown,
          QKeySequence::Forward,
          Qt::Key_J,
          Qt::Key_N,
          Qt::Key_Space});

    //
    // Sequence navigation
    //

    setA(actions_.prevFrame, "Previous Frame", {Qt::Key_Comma});

    setA(actions_.nextFrame, "Next Frame", {Qt::Key_Period});

    //
    // Window manipulation
    //

    setA(actions_.exitFullScreen, "Exit Full Screen", {Qt::Key_Escape});

    setA(actions_.fullScreen,
         "Full Screen",
         {Qt::Key_F, QKeySequence::FullScreen});

    actions_.fullScreen.setCheckable(true);

    setA(actions_.showMenuBar,
         "Show Menu Bar",
         {Qt::Key_M, Qt::CTRL + Qt::SHIFT + Qt::Key_M});

    actions_.showMenuBar.setCheckable(true);
  }

  void MenuMaker::enableImageActions() {
    for (QAction& action : getImageActions())
      action.setEnabled(true);
  }

  void MenuMaker::disableImageActions() {
    for (QAction& action : getImageActions())
      action.setEnabled(false);
  }

  actionset::RefList MenuMaker::getImageActions() {
    return {
      actions_.save,
      actions_.zoomOriginal,
      actions_.zoomIn,
      actions_.zoomOut,
      actions_.panUp,
      actions_.panDown,
      actions_.panLeft,
      actions_.panRight,
      actions_.prevImage,
      actions_.nextImage,
      actions_.prevFrame,
      actions_.nextFrame,
    };
  }

  //
  // Menus
  //

  void MenuMaker::setupContextMenu() {
    contextMenu_.addAction(&actions_.prevImage);
    contextMenu_.addAction(&actions_.nextImage);
    contextMenu_.addSeparator();
    contextMenu_.addAction(&actions_.open);
    contextMenu_.addAction(&actions_.save);
    contextMenu_.addSeparator();
    contextMenu_.addAction(&actions_.zoomToFit);
    contextMenu_.addAction(&actions_.zoomOriginal);
    contextMenu_.addAction(&actions_.zoomIn);
    contextMenu_.addAction(&actions_.zoomOut);
    contextMenu_.addSeparator();
    contextMenu_.addAction(&actions_.prevFrame);
    contextMenu_.addAction(&actions_.nextFrame);
    contextMenu_.addSeparator();
    contextMenu_.addAction(&actions_.showMenuBar);
    contextMenu_.addAction(&actions_.fullScreen);
  }

  void MenuMaker::setupMenuBar(QMenuBar* menuBar) {
    DUMAGEVIEW_ASSERT(menuBar);

    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction(&actions_.open);
    fileMenu->addAction(&actions_.save);
    fileMenu->addAction(&actions_.close);
    fileMenu->addAction(&actions_.quit);
    fileMenu->addSeparator();
    fileMenu->addAction(&actions_.prevImage);
    fileMenu->addAction(&actions_.nextImage);

    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction(&actions_.zoomIn);
    viewMenu->addAction(&actions_.zoomOut);
    viewMenu->addAction(&actions_.zoomToFit);
    viewMenu->addAction(&actions_.zoomOriginal);
    fileMenu->addSeparator();
    fileMenu->addAction(&actions_.prevFrame);
    fileMenu->addAction(&actions_.nextFrame);
    viewMenu->addSeparator();
    viewMenu->addAction(&actions_.showMenuBar);
    viewMenu->addAction(&actions_.fullScreen);

    menuBar->hide();
    actions_.showMenuBar.setChecked(false);
  }

  void MenuMaker::addActions(QWidget& widget) {
    for (QAction& action : listActions(actions_)) {
      widget.addAction(&action);
    }
  }
}
