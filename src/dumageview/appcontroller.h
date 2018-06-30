#ifndef DUMAGEVIEW_APPCONTROLLER_H_
#define DUMAGEVIEW_APPCONTROLLER_H_

#include "dumageview/actionset.h"
#include "dumageview/cmdline.h"
#include "dumageview/imagecontroller.h"
#include "dumageview/mainwindow.h"
#include "dumageview/menumaker.h"

#include <QAction>
#include <QObject>

namespace dumageview
{
  class AppController : public QObject
  {
    Q_OBJECT;

   public:

    explicit AppController(cmdline::Args const& cmdArgs_);

    virtual ~AppController() = default;

    void openImage();

    void saveImage();

   private:

    void setupConnections();

    QString dialogFilter() const;

    //
    // Private accessors
    //

    ActionSet& actions() { return _menuMaker.actions(); }

    ImageController& imageController() { return _imageController; }
    ImageController const& imageController() const { return _imageController; }

    MainWindow& mainWindow() { return _mainWindow; }

    ImageWidget& imageWidget() { return _mainWindow.imageArea(); }

    MenuMaker& menuMaker() { return _menuMaker; }

    //
    // Private data
    //

    MenuMaker _menuMaker;
    ImageController _imageController;
    MainWindow _mainWindow;
  };
}

#endif // DUMAGEVIEW_APPCONTROLLER_H_
