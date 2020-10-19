#include "dumageview/dialogrunner.h"

#include "dumageview/assert.h"
#include "dumageview/qtutil.h"

namespace dumageview {
  DialogRunner::DialogRunner(QDialog* dialog_, QObject* parent_, bool ownDialog)
      : QObject{parent_}, _dialog{dialog_}, _deleteDialogWhenFinished{
                                              ownDialog} {
    DUMAGEVIEW_ASSERT(_dialog);
    qtutil::connect(_dialog, &QDialog::destroyed, this, &DialogRunner::cleanup);
  }

  DialogRunner::~DialogRunner() {
    cleanup();
  }

  void DialogRunner::setupConnections() {
    qtutil::connect(_dialog,
                    &QDialog::finished,
                    this,
                    &DialogRunner::unwindHandlers);
  }

  void DialogRunner::safeExec() {
    setupConnections();
    _dialog->open();
  }

  void DialogRunner::unwindHandlers(int result) {
    _finishHandler(result);
    cleanup();
  }

  void DialogRunner::cleanup() {
    _finishHandler = [](int) {};

    if (_dialog && _deleteDialogWhenFinished) {
      _dialog->deleteLater();
      _dialog = nullptr;
    }

    deleteLater();
  }
}
