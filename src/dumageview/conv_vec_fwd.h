#ifndef DUMAGEVIEW_CONV_VEC_FWD_H_
#define DUMAGEVIEW_CONV_VEC_FWD_H_

#include <glm/fwd.hpp>

// forward decl
class QPoint;
class QPointF;
class QSize;
class QSizeF;
class QString;

namespace dumageview::conv {
  template<class Vec>
  struct VecConv;

  QPoint qpoint(QPoint const& p);
  QPoint qpoint(QPointF const& p);
  QPoint qpoint(QSize const& s);
  QPoint qpoint(QSizeF const& s);
  QPoint qpoint(glm::ivec2 const& v);
  QPoint qpoint(glm::dvec2 const& v);
  QPoint qpoint(glm::vec2 const& v);
  QPoint qpoint(int a);
  QPoint qpoint(double a);
  QPoint qpoint(float a);

  QSize qsize(QPoint const& p);
  QSize qsize(QPointF const& p);
  QSize qsize(QSize const& s);
  QSize qsize(QSizeF const& s);
  QSize qsize(glm::ivec2 const& v);
  QSize qsize(glm::dvec2 const& v);
  QSize qsize(glm::vec2 const& v);
  QSize qsize(int a);
  QSize qsize(double a);
  QSize qsize(float a);

  QPointF qpointf(QPoint const& p);
  QPointF qpointf(QPointF const& p);
  QPointF qpointf(QSize const& s);
  QPointF qpointf(QSizeF const& s);
  QPointF qpointf(glm::ivec2 const& v);
  QPointF qpointf(glm::dvec2 const& v);
  QPointF qpointf(glm::dvec2 const& v);
  QPointF qpointf(int a);
  QPointF qpointf(double a);
  QPointF qpointf(float a);

  QSizeF qsizef(QPoint const& p);
  QSizeF qsizef(QPointF const& p);
  QSizeF qsizef(QSize const& s);
  QSizeF qsizef(QSizeF const& s);
  QSizeF qsizef(glm::ivec2 const& v);
  QSizeF qsizef(glm::dvec2 const& v);
  QSizeF qsizef(glm::vec2 const& v);
  QSizeF qsizef(int a);
  QSizeF qsizef(double a);
  QSizeF qsizef(float a);

  glm::ivec2 ivec(QPoint const& p);
  glm::ivec2 ivec(QPointF const& p);
  glm::ivec2 ivec(QSize const& s);
  glm::ivec2 ivec(QSizeF const& s);
  glm::ivec2 ivec(glm::ivec2 const& v);
  glm::ivec2 ivec(glm::dvec2 const& v);
  glm::ivec2 ivec(glm::vec2 const& v);
  glm::ivec2 ivec(int a);
  glm::ivec2 ivec(double a);
  glm::ivec2 ivec(float a);

  glm::dvec2 dvec(QPoint const& p);
  glm::dvec2 dvec(QPointF const& p);
  glm::dvec2 dvec(QSize const& s);
  glm::dvec2 dvec(QSizeF const& s);
  glm::dvec2 dvec(glm::ivec2 const& v);
  glm::dvec2 dvec(glm::dvec2 const& v);
  glm::dvec2 dvec(glm::vec2 const& v);
  glm::dvec2 dvec(int a);
  glm::dvec2 dvec(double a);
  glm::dvec2 dvec(float a);

  glm::vec2 vec(QPoint const& p);
  glm::vec2 vec(QPointF const& p);
  glm::vec2 vec(QSize const& s);
  glm::vec2 vec(QSizeF const& s);
  glm::vec2 vec(glm::ivec2 const& v);
  glm::vec2 vec(glm::dvec2 const& v);
  glm::vec2 vec(glm::vec2 const& v);
  glm::vec2 vec(int a);
  glm::vec2 vec(double a);
  glm::vec2 vec(float a);
}

#endif  // DUMAGEVIEW_CONV_VEC_FWD_H_
