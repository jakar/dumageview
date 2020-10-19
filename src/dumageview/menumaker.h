#ifndef DUMAGEVIEW_MENUMAKER_H_
#define DUMAGEVIEW_MENUMAKER_H_

#include "dumageview/actionset.h"

#include <QAction>
#include <QMenu>
#include <QObject>

#include <functional>
#include <vector>

class QMenuBar;
class QWidget;

namespace dumageview {
  class MenuMaker : public QObject {
    Q_OBJECT;

   public:
    explicit MenuMaker();

    virtual ~MenuMaker() = default;

    void addActions(QWidget& widget);

    void setupMenuBar(QMenuBar* menuBar);

    void enableImageActions();

    void disableImageActions();

    //
    // Public data access
    //

    ActionSet& actions() {
      return _actions;
    }

    QMenu& contextMenu() {
      return _contextMenu;
    }

   private:
    void setupActions();

    void setupContextMenu();

    actionset::RefList imageActions();

    //
    // Private data
    //

    ActionSet _actions;

    QMenu _contextMenu;
  };
}

#endif  // DUMAGEVIEW_MENUMAKER_H_
