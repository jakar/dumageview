#ifndef DUMMAGEVIEW_DIALOGRUNNER_H_
#define DUMMAGEVIEW_DIALOGRUNNER_H_

#include <QDialog>
#include <QObject>

#include <boost/noncopyable.hpp>

#include <functional>
#include <type_traits>
#include <variant>

namespace dumageview {
  /**
   * Opens a dialog and runs a handler when it closes.
   *
   * This class is intended to replace QDialog::exec() and functions that use it
   * behind the scenes. Instances of this class and the dialog it contains are
   * deleted after running handlers.
   */
  class DialogRunner : public QObject, private boost::noncopyable {
    Q_OBJECT;

   public:
    template<class A>
    using Handler = std::function<void(A)>;

    DialogRunner(QDialog* dialog, QObject* parent, bool ownDialog = true);

    virtual ~DialogRunner();

    /**
     * Opens dialog and connects slots to clean up when it finishes.
     * Returns immediately.
     */
    void safeExec();

    /**
     * Opens dialog and connects slots to run handler when it finishes.
     * Returns immediately.
     */
    template<class H>
    void safeExec(H&& handler) {
      finishHandler_ = std::forward<H>(handler);
      safeExec();
    }

    QDialog* getDialog() const {
      return dialog_;
    }

   protected:
    virtual void setupConnections();

    virtual void cleanup();

    void unwindHandlers(int result);

   private:
    QDialog* dialog_;
    bool deleteDialogWhenFinished_;
    Handler<int> finishHandler_ = [](int) {};
  };
}

#endif  // DUMMAGEVIEW_DIALOGRUNNER_H_
