#ifndef GfxWrapper_h_
#define GfxWrapper_h_

#include "GObject.h"
#include "KIllustrator.h"

class KIllustratorDocument;

class GfxWrapper : public SWrapper, 
		   virtual public KIllustrator::GfxObject_skel {
public:
  GfxWrapper (KIllustratorDocument *doc, GObject *o);

  virtual CORBA::Long id ();
  virtual void applyFillColor (CORBA::Short r, CORBA::Short g, CORBA::Short b);
  virtual void getFillColor (CORBA::Short& r, CORBA::Short& g, 
			     CORBA::Short& b);
  virtual void applyPenColor (CORBA::Short r, CORBA::Short g, CORBA::Short b);
  virtual void getPenColor (CORBA::Short& r, CORBA::Short& g, CORBA::Short& b);
  virtual void setPosition (CORBA::Float x, CORBA::Float y);
  virtual void getPosition (CORBA::Float& x, CORBA::Float& y);
  virtual void getCenter (CORBA::Float& x, CORBA::Float& y);
  virtual void translate (CORBA::Float dx, CORBA::Float dy);
  virtual void rotate (CORBA::Float angle);
  virtual void rotateAround (CORBA::Float angle, 
			     CORBA::Float xp, CORBA::Float yp);
  virtual ObjectType getObjectType ();
  virtual CORBA::Boolean isSelected ();
  virtual ::KIllustrator::GfxObject_ptr duplicateObject ();
  virtual void deleteObject ();
  virtual void orderBackOne ();
  virtual void orderForwardOne ();

private:
  KIllustratorDocument *document;
};

#endif
