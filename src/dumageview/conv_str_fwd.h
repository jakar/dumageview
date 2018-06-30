#ifndef DUMAGEVIEW_CONV_STR_FWD_H_
#define DUMAGEVIEW_CONV_STR_FWD_H_

#include <string>
#include <string_view>

class QString;

namespace dumageview::conv
{
  namespace literals
  {
    QString operator""_qstr(char const* data, size_t size);
  }

  QString qstr(std::string const& s);
  QString qstr(std::string_view const& sv);

  std::string str(QString const& s);
  std::string str(std::string_view const& sv);
}

#endif // DUMAGEVIEW_CONV_STR_FWD_H_
