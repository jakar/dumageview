#include "dumageview/filedialogrunner.h"

#include "dumageview/conv_str.h"

#include <fmt/format.h>

#include <QString>

#include <boost/algorithm/string.hpp>
#include <boost/hana.hpp>

#include <algorithm>
#include <iterator>
#include <locale>

namespace dumageview {
  namespace {
    template<class R, class T, class S>
    void runNewFileDialog(DialogRunner::Handler<R> const& resultHandler,
                          QWidget* parent,
                          QString const& caption,
                          QString const& dir,
                          QString const& filter,
                          QString* selectedFilter,
                          QFileDialog::Options options,
                          T&& dialogTranform,
                          S&& selector) {
      auto* dialog = new QFileDialog{parent, caption, dir, filter};
      dialog->setOptions(options | QFileDialog::DontUseNativeDialog);

      dialogTranform(dialog);

      if (selectedFilter && !selectedFilter->isEmpty()) {
        dialog->selectNameFilter(*selectedFilter);
      }

      auto execHandler =
        [dialog, selectedFilter, selector](int result) mutable {
          if (result != QDialog::Accepted) {
            return R{};
          }
          if (selectedFilter) {
            *selectedFilter = dialog->selectedNameFilter();
          }
          return selector(dialog);
        };

      auto* runner = new FileDialogRunner{dialog, parent};
      runner->safeExec(boost::hana::compose(resultHandler, execHandler));
    }
  }

  void FileDialogRunner::getExistingDirectory(Handler<QString> const& resultHandler,
                                              QWidget* parent,
                                              QString const& caption,
                                              QString const& dir,
                                              QFileDialog::Options options) {
    runNewFileDialog(
      resultHandler, parent,
      caption, dir, QString{}, nullptr, options,
      [](QFileDialog* dialog) {
        dialog->setFileMode(QFileDialog::Directory);
      },
      [](auto* d) {
        return d->selectedFiles().value(0);
      }
    );
  }

  void FileDialogRunner::getOpenFileName(Handler<QString> const& resultHandler,
                                         QWidget* parent,
                                         QString const& caption,
                                         QString const& dir,
                                         QString const& filter,
                                         QString* selectedFilter,
                                         QFileDialog::Options options) {
    runNewFileDialog(
      resultHandler, parent,
      caption, dir, filter, selectedFilter, options,
      [](QFileDialog* dialog) {
        dialog->setFileMode(QFileDialog::ExistingFile);
      },
      [](auto* d) {
        return d->selectedFiles().value(0);
      }
    );
  }

  void FileDialogRunner::getOpenFileNames(Handler<QStringList> const& resultHandler,
                                          QWidget* parent,
                                          QString const& caption,
                                          QString const& dir,
                                          QString const& filter,
                                          QString* selectedFilter,
                                          QFileDialog::Options options) {
    runNewFileDialog(
      resultHandler, parent,
      caption, dir, filter, selectedFilter, options,
      [](QFileDialog* dialog) {
        dialog->setFileMode(QFileDialog::ExistingFiles);
      },
      [](auto* d) {
        return d->selectedFiles();
      }
    );
  }

  void FileDialogRunner::getSaveFileName(Handler<QString> const& resultHandler,
                                         QWidget* parent,
                                         QString const& caption,
                                         QString const& dir,
                                         QString const& filter,
                                         QString* selectedFilter,
                                         QFileDialog::Options options) {
    runNewFileDialog(
      resultHandler, parent,
      caption, dir, filter, selectedFilter, options,
      [](QFileDialog* dialog) {
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        dialog->setFileMode(QFileDialog::AnyFile);
      },
      [](auto* d) {
        return d->selectedFiles().value(0);
      }
    );
  }

  QString FileDialogRunner::makeSelectionFilter(std::string title,
                                                ExtensionSet exts) {
    auto const& locale = std::locale::classic();
    for (auto& ext : exts) {
      ext = fmt::format("*.{} *.{}",
                        boost::to_lower_copy(ext, locale),
                        boost::to_upper_copy(ext, locale));
    }
    return conv::qstr(fmt::format("{} ({})",
                                  std::move(title),
                                  boost::join(std::move(exts), " ")));
  }
}
