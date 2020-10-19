#ifndef DUMAGEVIEW_CONV_STR_H_
#define DUMAGEVIEW_CONV_STR_H_

#include "dumageview/conv_str_fwd.h"

#include <QString>

namespace dumageview::conv {
  namespace literals {
    inline QString operator""_qstr(char const* data, size_t size) {
      return qstr(std::string_view(data, size));
    }
  }

  //
  // QString conversions
  //

  inline QString qstr(std::string const& s) {
    return QString::fromStdString(s);
  }

  inline QString qstr(std::string_view const& sv) {
    return QString::fromUtf8(sv.data(), sv.size());
  }

  //
  // std::string conversions
  //

  inline std::string str(QString const& s) {
    return s.toStdString();
  }

  inline std::string str(std::string_view const& sv) {
    return std::string(sv);
  }
}

#endif  // DUMAGEVIEW_CONV_STR_H_
