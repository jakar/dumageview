#ifndef DUMAGEVIEW_CONV_VEC_H_
#define DUMAGEVIEW_CONV_VEC_H_

#include "dumageview/conv_vec_fwd.h"

#include <glm/glm.hpp>

#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>

#include <boost/numeric/conversion/cast.hpp>

namespace dumageview::conv
{
  //
  // Converter types
  //

  template<>
  struct VecConv<QPoint>
  {
    template<class U>
    QPoint operator()(U&& u) const
    {
      return qpoint(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<QPointF>
  {
    template<class U>
    QPointF operator()(U&& u) const
    {
      return qpointf(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<QSize>
  {
    template<class U>
    QSize operator()(U&& u) const
    {
      return qsize(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<QSizeF>
  {
    template<class U>
    QSizeF operator()(U&& u) const
    {
      return qsizef(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<glm::vec2>
  {
    template<class U>
    glm::vec2 operator()(U&& u) const
    {
      return vec(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<glm::dvec2>
  {
    template<class U>
    glm::dvec2 operator()(U&& u) const
    {
      return dvec(std::forward<U>(u));
    }
  };

  template<>
  struct VecConv<glm::ivec2>
  {
    template<class U>
    glm::ivec2 operator()(U&& u) const
    {
      return ivec(std::forward<U>(u));
    }
  };

  //
  // glm::ivec2 conversions
  //

  inline glm::ivec2 ivec(QPoint const& p)
  {
    return {p.x(), p.y()};
  }

  inline glm::ivec2 ivec(QPointF const& p)
  {
    return ivec(dvec(p));
  }

  inline glm::ivec2 ivec(QSize const& s)
  {
    return {s.width(), s.height()};
  }

  inline glm::ivec2 ivec(QSizeF const& s)
  {
    return ivec(dvec(s));
  }

  inline glm::ivec2 ivec(glm::ivec2 const& v)
  {
    return v;
  }

  inline glm::ivec2 ivec(glm::dvec2 const& v)
  {
    return {v};  // truncates
  }

  inline glm::ivec2 ivec(glm::vec2 const& v)
  {
    return {v};  // truncates
  }

  inline glm::ivec2 ivec(int a)
  {
    return glm::ivec2{a};
  }

  inline glm::ivec2 ivec(double a)
  {
    return ivec(dvec(a));
  }

  inline glm::ivec2 ivec(float a)
  {
    return ivec(dvec(a));
  }

  //
  // glm::dvec2 conversions
  //

  inline glm::dvec2 dvec(QPoint const& p)
  {
    return dvec(ivec(p));
  }

  inline glm::dvec2 dvec(QPointF const& p)
  {
    return {p.x(), p.y()};
  }

  inline glm::dvec2 dvec(QSize const& s)
  {
    return dvec(ivec(s));
  }

  inline glm::dvec2 dvec(QSizeF const& s)
  {
    return {s.width(), s.height()};
  }

  inline glm::dvec2 dvec(glm::ivec2 const& v)
  {
    return {v};  // widens
  }

  inline glm::dvec2 dvec(glm::dvec2 const& v)
  {
    return v;
  }

  inline glm::dvec2 dvec(glm::vec2 const& v)
  {
    return {v};  // widens
  }

  inline glm::dvec2 dvec(int a)
  {
    return dvec(ivec(a));
  }

  inline glm::dvec2 dvec(double a)
  {
    return glm::dvec2{a};
  }

  inline glm::dvec2 dvec(float a)
  {
    return glm::dvec2{a};
  }

  //
  // glm::vec2 conversions
  //

  inline glm::vec2 vec(QPoint const& p)
  {
    return vec(ivec(p));
  }

  inline glm::vec2 vec(QPointF const& p)
  {
    return vec(dvec(p));
  }

  inline glm::vec2 vec(QSize const& s)
  {
    return vec(ivec(s));
  }

  inline glm::vec2 vec(QSizeF const& s)
  {
    return vec(dvec(s));
  }

  inline glm::vec2 vec(glm::ivec2 const& v)
  {
    return {v};  // casts
  }

  inline glm::vec2 vec(glm::dvec2 const& v)
  {
    return {v};  // narrows
  }

  inline glm::vec2 vec(glm::vec2 const& v)
  {
    return v;
  }

  inline glm::vec2 vec(int a)
  {
    return vec(ivec(a));
  }

  inline glm::vec2 vec(double a)
  {
    return vec(dvec(a));
  }

  inline glm::vec2 vec(float a)
  {
    return glm::vec2{a};
  }

  //
  // QPoint conversions
  //

  inline QPoint qpoint(QPoint const& p)
  {
    return p;
  }

  inline QPoint qpoint(QPointF const& p)
  {
    return qpoint(ivec(p));
  }

  inline QPoint qpoint(QSize const& s)
  {
    return {s.width(), s.height()};
  }

  inline QPoint qpoint(QSizeF const& s)
  {
    return qpoint(ivec(s));
  }

  inline QPoint qpoint(glm::ivec2 const& v)
  {
    return {v.x, v.y};
  }

  inline QPoint qpoint(glm::dvec2 const& v)
  {
    return qpoint(ivec(v));
  }

  inline QPoint qpoint(glm::vec2 const& v)
  {
    return qpoint(ivec(v));
  }

  inline QPoint qpoint(int a)
  {
    return qpoint(ivec(a));
  }

  inline QPoint qpoint(double a)
  {
    return qpoint(ivec(a));
  }

  inline QPoint qpoint(float a)
  {
    return qpoint(ivec(a));
  }

  //
  // QSize conversions
  //

  inline QSize qsize(QPoint const& p)
  {
    return {p.x(), p.y()};
  }

  inline QSize qsize(QPointF const& p)
  {
    return qsize(ivec(p));
  }

  inline QSize qsize(QSize const& s)
  {
    return s;
  }

  inline QSize qsize(QSizeF const& s)
  {
    return qsize(ivec(s));
  }

  inline QSize qsize(glm::ivec2 const& v)
  {
    return {v.x, v.y};
  }

  inline QSize qsize(glm::dvec2 const& v)
  {
    return qsize(ivec(v));
  }

  inline QSize qsize(glm::vec2 const& v)
  {
    return qsize(ivec(v));
  }

  inline QSize qsize(int a)
  {
    return qsize(ivec(a));
  }

  inline QSize qsize(double a)
  {
    return qsize(ivec(a));
  }

  inline QSize qsize(float a)
  {
    return qsize(ivec(a));
  }

  //
  // QPointF conversions
  //

  inline QPointF qpointf(QPoint const& p)
  {
    return qpointf(dvec(p));
  }

  inline QPointF qpointf(QPointF const& p)
  {
    return p;
  }

  inline QPointF qpointf(QSize const& s)
  {
    return qpointf(dvec(s));
  }

  inline QPointF qpointf(QSizeF const& s)
  {
    return {s.width(), s.height()};
  }

  inline QPointF qpointf(glm::ivec2 const& v)
  {
    return qpointf(dvec(v));
  }

  inline QPointF qpointf(glm::dvec2 const& v)
  {
    return {v.x, v.y};
  }

  inline QPointF qpointf(glm::vec2 const& v)
  {
    return qpointf(dvec(v));
  }

  inline QPointF qpointf(int a)
  {
    return qpointf(dvec(a));
  }

  inline QPointF qpointf(double a)
  {
    return qpointf(dvec(a));
  }

  inline QPointF qpointf(float a)
  {
    return qpointf(dvec(a));
  }

  //
  // QSizeF conversions
  //

  inline QSizeF qsizef(QPoint const& p)
  {
    return qsizef(dvec(p));
  }

  inline QSizeF qsizef(QPointF const& p)
  {
    return {p.x(), p.y()};
  }

  inline QSizeF qsizef(QSize const& s)
  {
    return qsizef(dvec(s));
  }

  inline QSizeF qsizef(QSizeF const& s)
  {
    return s;
  }

  inline QSizeF qsizef(glm::ivec2 const& v)
  {
    return qsizef(dvec(v));
  }

  inline QSizeF qsizef(glm::dvec2 const& v)
  {
    return {v.x, v.y};
  }

  inline QSizeF qsizef(glm::vec2 const& v)
  {
    return qsizef(dvec(v));
  }

  inline QSizeF qsizef(int a)
  {
    return qsizef(dvec(a));
  }

  inline QSizeF qsizef(double a)
  {
    return qsizef(dvec(a));
  }

  inline QSizeF qsizef(float a)
  {
    return qsizef(dvec(a));
  }
}

#endif // DUMAGEVIEW_CONV_VEC_H_
