#ifndef DUMAGEVIEW_FILEDIALOGRUNNER_H_
#define DUMAGEVIEW_FILEDIALOGRUNNER_H_

#include "dumageview/dialogrunner.h"

#include <QFileDialog>

#include <string>
#include <vector>

namespace dumageview {
  /**
   * Provides wrappers of QFileDialog functions to use handlers instead
   * of exec.
   */
  class FileDialogRunner : public DialogRunner {
    Q_OBJECT;

   public:
    using DialogRunner::DialogRunner;

    virtual ~FileDialogRunner() = default;

    //
    // Public member functions
    //

    QFileDialog* fileDialog() const {
      return static_cast<QFileDialog*>(dialog());
    }

    //
    // Public static member functions
    //

    using ExtensionSet = std::vector<std::string>;

    static QString makeSelectionFilter(std::string title, ExtensionSet exts);

    static void getExistingDirectory(
      Handler<QString> const& resultHandler,
      QWidget* parent = nullptr,
      QString const& caption = QString{},
      QString const& dir = QString{},
      QFileDialog::Options options = QFileDialog::ShowDirsOnly);

    static void getOpenFileName(Handler<QString> const& resultHandler,
                                QWidget* parent = nullptr,
                                QString const& caption = QString{},
                                QString const& dir = QString{},
                                QString const& filter = QString{},
                                QString* selectedFilter = nullptr,
                                QFileDialog::Options options = 0);

    static void getOpenFileNames(Handler<QStringList> const& resultHandler,
                                 QWidget* parent = nullptr,
                                 QString const& caption = QString{},
                                 QString const& dir = QString{},
                                 QString const& filter = QString{},
                                 QString* selectedFilter = nullptr,
                                 QFileDialog::Options options = 0);

    static void getSaveFileName(Handler<QString> const& resultHandler,
                                QWidget* parent = nullptr,
                                QString const& caption = QString{},
                                QString const& dir = QString{},
                                QString const& filter = QString{},
                                QString* selectedFilter = nullptr,
                                QFileDialog::Options options = 0);
  };
}

#endif  // DUMAGEVIEW_FILEDIALOGRUNNER_H_
