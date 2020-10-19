#include "dumageview/dialogrunner.h"

#include "dumageview/assert.h"
#include "dumageview/qtutil.h"

namespace dumageview {
  DialogRunner::DialogRunner(QDialog* dialog, QObject* parent, bool ownDialog)
      : QObject{parent},
        dialog_{dialog},
        deleteDialogWhenFinished_{ownDialog} {
    DUMAGEVIEW_ASSERT(dialog_);
    qtutil::connect(dialog_, &QDialog::destroyed, this, &DialogRunner::cleanup);
  }

  DialogRunner::~DialogRunner() {
    cleanup();
  }

  void DialogRunner::setupConnections() {
    qtutil::connect(dialog_,
                    &QDialog::finished,
                    this,
                    &DialogRunner::unwindHandlers);
  }

  void DialogRunner::safeExec() {
    setupConnections();
    dialog_->open();
  }

  void DialogRunner::unwindHandlers(int result) {
    finishHandler_(result);
    cleanup();
  }

  void DialogRunner::cleanup() {
    finishHandler_ = [](int) {};

    if (dialog_ && deleteDialogWhenFinished_) {
      dialog_->deleteLater();
      dialog_ = nullptr;
    }

    deleteLater();
  }
}
