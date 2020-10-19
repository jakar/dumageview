#ifndef DUMAGEVIEW_ACTIONSET_H_
#define DUMAGEVIEW_ACTIONSET_H_

#include <QAction>

#include <functional>
#include <vector>

namespace dumageview::actionset {
  /**
   * Application-wide actions.
   */
  struct ActionSet {
    QAction open{};
    QAction save{};
    QAction close{};
    QAction quit{};

    QAction zoomToFit{};
    QAction zoomOriginal{};
    QAction zoomIn{};
    QAction zoomOut{};

    QAction panUp{};
    QAction panDown{};
    QAction panLeft{};
    QAction panRight{};

    QAction prevImage{};
    QAction nextImage{};

    QAction prevFrame{};
    QAction nextFrame{};

    QAction fullScreen{};
    QAction exitFullScreen{};

    QAction showMenuBar{};
  };

  using RefList = std::vector<std::reference_wrapper<QAction>>;

  inline RefList listActions(ActionSet& actions) {
    return {
      actions.open,        actions.save,
      actions.close,       actions.quit,

      actions.zoomToFit,   actions.zoomOriginal,
      actions.zoomIn,      actions.zoomOut,

      actions.panUp,       actions.panDown,
      actions.panLeft,     actions.panRight,

      actions.prevImage,   actions.nextImage,

      actions.prevFrame,   actions.nextFrame,

      actions.fullScreen,  actions.exitFullScreen,

      actions.showMenuBar,
    };
  }
}

namespace dumageview {
  using actionset::ActionSet;
}

#endif  // DUMAGEVIEW_ACTIONSET_H_
