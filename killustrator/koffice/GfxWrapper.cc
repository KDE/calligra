#include "GfxWrapper.h"
#include "KIllustrator_doc.h"

GfxWrapper::GfxWrapper (KIllustratorDocument *doc, GObject *o) {
  document = doc;
  o->setWrapper (this);
}

CORBA::Long GfxWrapper::id () {
  return 0;
}

void GfxWrapper::applyFillColor (CORBA::Short r, CORBA::Short g, 
				 CORBA::Short b) {
  cout << "fill color = " << r << ", " << g << ", " << b << endl;
  GObject::FillInfo info;
  info.color  = QColor (r, g, b);
  info.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  info.fstyle = GObject::FillInfo::SolidFill;
  obj->setFillInfo (info);
}

void GfxWrapper::getFillColor (CORBA::Short& r, CORBA::Short& g, 
			       CORBA::Short& b) {
}

void GfxWrapper::applyPenColor (CORBA::Short r, CORBA::Short g, 
				CORBA::Short b) {
  cout << "pen color = " << r << ", " << g << ", " << b << endl;
  GObject::OutlineInfo info;
  info.color  = QColor (r, g, b);
  info.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;;
  info.style = SolidLine;
  obj->setOutlineInfo (info);
}

void GfxWrapper::getPenColor (CORBA::Short& r, CORBA::Short& g, 
			      CORBA::Short& b) {
}

void GfxWrapper::setPosition (CORBA::Float x, CORBA::Float y) {
}

void GfxWrapper::getPosition (CORBA::Float& x, CORBA::Float& y) {
}

void GfxWrapper::getCenter (CORBA::Float& x, CORBA::Float& y) {
  Coord cp = obj->boundingBox ().center ();
  x = cp.x (); y = cp.y ();
}

void GfxWrapper::translate (CORBA::Float dx, CORBA::Float dy) {
  QWMatrix m;
  m.translate (dx, dy);
  obj->transform (m, true);
}

void GfxWrapper::rotate (CORBA::Float angle) {
  QWMatrix m;
  m.rotate (angle);
  obj->transform (m, true);
}

void GfxWrapper::rotateAround (CORBA::Float angle, 
			       CORBA::Float xp, CORBA::Float yp) {
  QWMatrix m1, m2, m3;
  m1.translate (-xp, -yp);
  m2.rotate (angle);
  m3.translate (xp, yp);

  obj->ttransform (m1);
  obj->ttransform (m2);
  obj->ttransform (m3, true);
}

::KIllustrator::GfxObject::ObjectType GfxWrapper::getObjectType () {
  ::KIllustrator::GfxObject::ObjectType result = 0;
 
  const char *cname = obj->className ();
  if (::strcmp (cname, "GPolyline") == 0) 
    result = ::KIllustrator::GfxObject::Polyline;
  if (::strcmp (cname, "GOval") == 0) 
    result = ::KIllustrator::GfxObject::Ellipse;
  if (::strcmp (cname, "GPolygon") == 0) 
    result = ::KIllustrator::GfxObject::Polygon;
  if (::strcmp (cname, "GText") == 0) 
    result = ::KIllustrator::GfxObject::Text;
  if (::strcmp (cname, "GBezier") == 0) 
    result = ::KIllustrator::GfxObject::BezierCurve;
  if (::strcmp (cname, "GGroup") == 0) 
    result = ::KIllustrator::GfxObject::Group;
  if (::strcmp (cname, "GPixmap") == 0) 
    result = ::KIllustrator::GfxObject::Pixmap;
  if (::strcmp (cname, "GClipart") == 0) 
    result = ::KIllustrator::GfxObject::Clipart;
  if (::strcmp (cname, "GPart") == 0) 
    result = ::KIllustrator::GfxObject::Part;

  return result;
}

CORBA::Boolean GfxWrapper::isSelected () {
  return obj->isSelected ();
}

::KIllustrator::GfxObject_ptr GfxWrapper::duplicateObject () {
  GObject* newobj = obj->copy ();
  newobj->ref ();
  document->insertObject (newobj);
  GfxWrapper *wobj = new GfxWrapper (document, newobj);
  return KIllustrator::GfxObject::_duplicate (wobj);
}

void GfxWrapper::deleteObject () {
  document->deleteObject (obj);
}

void GfxWrapper::orderBackOne () {
  unsigned int idx = document->findIndexOfObject (obj);
  if (idx > 0) idx--;
  document->moveObjectToIndex (obj, idx);
}

void GfxWrapper::orderForwardOne () {
  unsigned int idx = document->findIndexOfObject (obj);
  if (idx < obj->getLayer ()->objectCount () - 1) idx++;
  document->moveObjectToIndex (obj, idx);
}
