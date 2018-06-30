#ifndef DUMAGEVIEW_QTUTIL_H_
#define DUMAGEVIEW_QTUTIL_H_

#include "dumageview/assert.h"

#include <QObject>

#include <utility>

namespace dumageview::qtutil
{
  template<class... Args>
  QMetaObject::Connection connect(Args&&... args)
  {
    auto result = QObject::connect(std::forward<Args>(args)...);
    DUMAGEVIEW_ASSERT(result);
    return result;
  }

  template<class... Args>
  bool disconnect(Args&&... args)
  {
    auto result = QObject::disconnect(std::forward<Args>(args)...);
    DUMAGEVIEW_ASSERT(result);
    return result;
  }
}

#endif // DUMAGEVIEW_QTUTIL_H_
