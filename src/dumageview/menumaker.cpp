#include "dumageview/menumaker.h"

#include "dumageview/assert.h"

#include <QKeySequence>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>

#include <utility>

namespace dumageview
{
  MenuMaker::MenuMaker()
  :
    QObject{},
    _actions{}
  {
    setupActions();
    setupContextMenu();
    disableImageActions();
  }

  void MenuMaker::setupActions()
  {
    auto setA = [](QAction& action, QString const& text,
                   QList<QKeySequence> const& keys)
    {
      action.setText(text);
      action.setShortcuts(keys);
    };

    //
    // Opening / closing
    //

    setA(_actions.open, "&Open...", {QKeySequence::Open});
    setA(_actions.save, "&Save...", {QKeySequence::Save});
    setA(_actions.close, "&Close", {QKeySequence::Close});
    setA(_actions.quit, "&Quit", {QKeySequence::Quit});

    //
    // Zooming
    //

    setA(_actions.zoomIn, "Zoom In",
         {Qt::Key_Plus, Qt::CTRL + Qt::Key_Plus,
          Qt::Key_Equal, Qt::CTRL + Qt::Key_Equal});

    setA(_actions.zoomOut, "Zoom Out",
         {Qt::Key_Minus, Qt::CTRL + Qt::Key_Minus});

    setA(_actions.zoomOriginal, "Original Size",
         {Qt::Key_1, Qt::CTRL + Qt::Key_1});

    setA(_actions.zoomToFit, "Fit to Window",
         {Qt::Key_0, Qt::CTRL + Qt::Key_0,
          Qt::Key_Z});

    // image widget sets enabled state for zoomToFit
    _actions.zoomToFit.setEnabled(false);

    //
    // Panning
    //

    setA(_actions.panUp, "Pan Up", {Qt::Key_Up});
    setA(_actions.panDown, "Pan Down", {Qt::Key_Down});
    setA(_actions.panLeft, "Pan Left", {Qt::Key_Left});
    setA(_actions.panRight, "Pan Right", {Qt::Key_Right});

    //
    // Dir navigation
    //

    setA(_actions.prevImage, "Previous Image",
         {Qt::Key_PageUp, QKeySequence::Back,
          Qt::Key_K, Qt::Key_P, Qt::SHIFT + Qt::Key_Space});

    setA(_actions.nextImage, "Next Image",
         {Qt::Key_PageDown, QKeySequence::Forward,
          Qt::Key_J, Qt::Key_N, Qt::Key_Space});

    //
    // Sequence navigation
    //

    setA(_actions.prevFrame, "Previous Frame",
         {Qt::Key_Comma});

    setA(_actions.nextFrame, "Next Frame",
         {Qt::Key_Period});

    //
    // Window manipulation
    //

    setA(_actions.exitFullScreen, "Exit Full Screen",
         {Qt::Key_Escape});

    setA(_actions.fullScreen, "Full Screen",
         {Qt::Key_F, QKeySequence::FullScreen});

    _actions.fullScreen.setCheckable(true);

    setA(_actions.showMenuBar, "Show Menu Bar",
         {Qt::Key_M, Qt::CTRL + Qt::SHIFT + Qt::Key_M});

    _actions.showMenuBar.setCheckable(true);
  }

  void MenuMaker::enableImageActions()
  {
    for (QAction& action : imageActions())
      action.setEnabled(true);
  }

  void MenuMaker::disableImageActions()
  {
    for (QAction& action : imageActions())
      action.setEnabled(false);
  }

  actionset::RefList MenuMaker::imageActions()
  {
    return {
      _actions.save,
      _actions.zoomOriginal,
      _actions.zoomIn,
      _actions.zoomOut,
      _actions.panUp,
      _actions.panDown,
      _actions.panLeft,
      _actions.panRight,
      _actions.prevImage,
      _actions.nextImage,
      _actions.prevFrame,
      _actions.nextFrame,
    };
  }

  //
  // Menus
  //

  void MenuMaker::setupContextMenu()
  {
    _contextMenu.addAction(&_actions.prevImage);
    _contextMenu.addAction(&_actions.nextImage);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_actions.open);
    _contextMenu.addAction(&_actions.save);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_actions.zoomToFit);
    _contextMenu.addAction(&_actions.zoomOriginal);
    _contextMenu.addAction(&_actions.zoomIn);
    _contextMenu.addAction(&_actions.zoomOut);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_actions.prevFrame);
    _contextMenu.addAction(&_actions.nextFrame);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_actions.showMenuBar);
    _contextMenu.addAction(&_actions.fullScreen);
  }

  void MenuMaker::setupMenuBar(QMenuBar* menuBar)
  {
    DUMAGEVIEW_ASSERT(menuBar);

    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction(&_actions.open);
    fileMenu->addAction(&_actions.save);
    fileMenu->addAction(&_actions.close);
    fileMenu->addAction(&_actions.quit);
    fileMenu->addSeparator();
    fileMenu->addAction(&_actions.prevImage);
    fileMenu->addAction(&_actions.nextImage);

    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction(&_actions.zoomIn);
    viewMenu->addAction(&_actions.zoomOut);
    viewMenu->addAction(&_actions.zoomToFit);
    viewMenu->addAction(&_actions.zoomOriginal);
    fileMenu->addSeparator();
    fileMenu->addAction(&_actions.prevFrame);
    fileMenu->addAction(&_actions.nextFrame);
    viewMenu->addSeparator();
    viewMenu->addAction(&_actions.showMenuBar);
    viewMenu->addAction(&_actions.fullScreen);

    menuBar->hide();
    _actions.showMenuBar.setChecked(false);
  }

  void MenuMaker::addActions(QWidget& widget)
  {
    for (QAction& action : listActions(_actions))
    {
      widget.addAction(&action);
    }
  }
}
