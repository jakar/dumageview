#ifndef DUMAGEVIEW_MESSAGEBOXRUNNER_H_
#define DUMAGEVIEW_MESSAGEBOXRUNNER_H_

#include "dumageview/dialogrunner.h"

#include <QMessageBox>

#include <string>

namespace dumageview {
  /**
   * Provides wrappers of QMessageBox functions to use handlers instead
   * of exec.
   */
  class MessageBoxRunner : public DialogRunner {
    Q_OBJECT;

   public:
    using DialogRunner::DialogRunner;

    virtual ~MessageBoxRunner() = default;

    QMessageBox* getMessageBox() const {
      return static_cast<QMessageBox*>(getDialog());
    }

    //
    // Handler-taking versions
    //

    static void critical(
      Handler<QMessageBox::StandardButton> const& resultHandler,
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void information(
      Handler<QMessageBox::StandardButton> const& resultHandler,
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void question(
      Handler<QMessageBox::StandardButton> const& resultHandler,
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void warning(
      Handler<QMessageBox::StandardButton> const& resultHandler,
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    //
    // Empty handler versions
    //

    static void critical(
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void information(
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void question(
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void warning(
      QWidget* parent,
      QString const& title,
      QString const& text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

   protected:
    virtual void setupConnections() override;

   private Q_SLOTS:

    void handleResult(QAbstractButton* button);
  };
}

#endif  // DUMAGEVIEW_MESSAGEBOXRUNNER_H_
