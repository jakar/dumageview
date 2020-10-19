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

    ActionSet& getActions() {
      return actions_;
    }

    QMenu& getContextMenu() {
      return contextMenu_;
    }

   private:
    void setupActions();

    void setupContextMenu();

    actionset::RefList getImageActions();

    //
    // Private data
    //

    ActionSet actions_;

    QMenu contextMenu_;
  };
}

#endif  // DUMAGEVIEW_MENUMAKER_H_
