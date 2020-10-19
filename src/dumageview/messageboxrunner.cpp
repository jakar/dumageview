#include "dumageview/messageboxrunner.h"

#include "dumageview/assert.h"
#include "dumageview/qtutil.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QString>

#include <boost/hana.hpp>

namespace dumageview {
  namespace {
    namespace hana = boost::hana;

    using StandardButton = QMessageBox::StandardButton;
    using StandardButtons = QMessageBox::StandardButtons;

    void runNewMessageBox(
      DialogRunner::Handler<StandardButton> const& resultHandler,
      QWidget* parent,
      QMessageBox::Icon icon,
      QString const& title,
      QString const& text,
      StandardButtons buttons,
      StandardButton defaultButton) {
      // set up the buttons like the Qt source code does
      QMessageBox* msgBox =
        new QMessageBox{icon, title, text, QMessageBox::NoButton, parent};

      QDialogButtonBox* buttonBox = msgBox->findChild<QDialogButtonBox*>();
      DUMAGEVIEW_ASSERT(buttonBox);

      // FirstButton and LastButton are undocumented.
      // FirstButton == Ok
      // LastButton == RestoreDefaults
      unsigned mask = QMessageBox::FirstButton;

      while (mask <= QMessageBox::LastButton) {
        unsigned sbutton = buttons & mask;
        mask <<= 1;

        if (!sbutton) continue;

        QPushButton* button =
          msgBox->addButton(static_cast<StandardButton>(sbutton));

        // Choose the first accept role as the default
        if (msgBox->defaultButton()) continue;

        bool isAcceptRole =
          defaultButton == QMessageBox::NoButton
          && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole;

        bool isDefault = defaultButton != QMessageBox::NoButton
                         && sbutton == static_cast<unsigned>(defaultButton);

        if (isAcceptRole || isDefault) msgBox->setDefaultButton(button);
      }

      auto execHandler = [msgBox](int result) -> StandardButton {
        // Qt converts NoButton to -1 and doesn't document it
        if (result == -1)
          return QMessageBox::Cancel;
        else
          return msgBox->standardButton(msgBox->clickedButton());
      };

      MessageBoxRunner* runner = new MessageBoxRunner(msgBox, parent);
      runner->safeExec(hana::compose(resultHandler, execHandler));
    }
  }

  void MessageBoxRunner::setupConnections() {
    qtutil::connect(messageBox(),
                    &QMessageBox::buttonClicked,
                    this,
                    &MessageBoxRunner::handleResult);
  }

  void MessageBoxRunner::handleResult(QAbstractButton* button) {
    int buttonID = messageBox()->standardButton(button);

    // Qt returns -1 in exec() for no button
    int execResult = (buttonID == QMessageBox::NoButton) ? -1 : buttonID;

    unwindHandlers(execResult);
  }

  //
  // Static member functions
  //

  void MessageBoxRunner::critical(Handler<StandardButton> const& resultHandler,
                                  QWidget* parent,
                                  QString const& title,
                                  QString const& text,
                                  StandardButtons buttons,
                                  StandardButton defaultButton) {
    runNewMessageBox(resultHandler,
                     parent,
                     QMessageBox::Critical,
                     title,
                     text,
                     buttons,
                     defaultButton);
  }

  void MessageBoxRunner::information(
    Handler<StandardButton> const& resultHandler,
    QWidget* parent,
    QString const& title,
    QString const& text,
    StandardButtons buttons,
    StandardButton defaultButton) {
    runNewMessageBox(resultHandler,
                     parent,
                     QMessageBox::Information,
                     title,
                     text,
                     buttons,
                     defaultButton);
  }

  void MessageBoxRunner::question(Handler<StandardButton> const& resultHandler,
                                  QWidget* parent,
                                  QString const& title,
                                  QString const& text,
                                  StandardButtons buttons,
                                  StandardButton defaultButton) {
    runNewMessageBox(resultHandler,
                     parent,
                     QMessageBox::Question,
                     title,
                     text,
                     buttons,
                     defaultButton);
  }

  void MessageBoxRunner::warning(Handler<StandardButton> const& resultHandler,
                                 QWidget* parent,
                                 QString const& title,
                                 QString const& text,
                                 StandardButtons buttons,
                                 StandardButton defaultButton) {
    runNewMessageBox(resultHandler,
                     parent,
                     QMessageBox::Warning,
                     title,
                     text,
                     buttons,
                     defaultButton);
  }

  //
  // Overloads with empty handlers
  //

  void MessageBoxRunner::critical(QWidget* parent,
                                  QString const& title,
                                  QString const& text,
                                  StandardButtons buttons,
                                  StandardButton defaultButton) {
    critical([](StandardButton) {},
             parent,
             title,
             text,
             buttons,
             defaultButton);
  }

  void MessageBoxRunner::information(QWidget* parent,
                                     QString const& title,
                                     QString const& text,
                                     StandardButtons buttons,
                                     StandardButton defaultButton) {
    information([](StandardButton) {},
                parent,
                title,
                text,
                buttons,
                defaultButton);
  }

  void MessageBoxRunner::question(QWidget* parent,
                                  QString const& title,
                                  QString const& text,
                                  StandardButtons buttons,
                                  StandardButton defaultButton) {
    question([](StandardButton) {},
             parent,
             title,
             text,
             buttons,
             defaultButton);
  }

  void MessageBoxRunner::warning(QWidget* parent,
                                 QString const& title,
                                 QString const& text,
                                 StandardButtons buttons,
                                 StandardButton defaultButton) {
    warning([](StandardButton) {}, parent, title, text, buttons, defaultButton);
  }
}
