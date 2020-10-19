#ifndef DUMAGEVIEW_APPCONTROLLER_H_
#define DUMAGEVIEW_APPCONTROLLER_H_

#include "dumageview/actionset.h"
#include "dumageview/cmdline.h"
#include "dumageview/imagecontroller.h"
#include "dumageview/mainwindow.h"
#include "dumageview/menumaker.h"

#include <QAction>
#include <QObject>

namespace dumageview {
  class AppController : public QObject {
    Q_OBJECT;

   public:
    explicit AppController(cmdline::Args const& cmdArgs);

    virtual ~AppController() = default;

    void openImage();

    void saveImage();

   private:
    void setupConnections();

    QString dialogFilter() const;

    //
    // Private accessors
    //

    ActionSet& getActions() {
      return menuMaker_.getActions();
    }

    ImageController& getImageController() {
      return imageController_;
    }
    ImageController const& getImageController() const {
      return imageController_;
    }

    MainWindow& getMainWindow() {
      return mainWindow_;
    }

    ImageWidget& getImageWidget() {
      return mainWindow_.getImageArea();
    }

    MenuMaker& getMenuMaker() {
      return menuMaker_;
    }

    //
    // Private data
    //

    MenuMaker menuMaker_;
    ImageController imageController_;
    MainWindow mainWindow_;
  };
}

#endif  // DUMAGEVIEW_APPCONTROLLER_H_
