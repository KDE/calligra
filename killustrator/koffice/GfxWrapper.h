#ifndef GfxWrapper_h_
#define GfxWrapper_h_

#include "GObject.h"
#include "KIllustrator.h"

class KIllustratorDocument;

class GfxWrapper : public SWrapper, 
		   virtual public KIllustrator::GfxObject_skel {
public:
  GfxWrapper (KIllustratorDocument *doc, GObject *o);

  virtual long int id ();
  virtual void applyFillColor (short int r, short int g, short int b);
  virtual void getFillColor (short int& r, short int& g, 
			     short int& b);
  virtual void applyPenColor (short int r, short int g, short int b);
  virtual void getPenColor (short int& r, short int& g, short int& b);
  virtual void setPosition (float x, float y);
  virtual void getPosition (float& x, float& y);
  virtual void getCenter (float& x, float& y);
  virtual void translate (float dx, float dy);
  virtual void rotate (float angle);
  virtual void rotateAround (float angle, 
			     float xp, float yp);
  virtual ObjectType getObjectType ();
  virtual bool isSelected ();
  virtual ::KIllustrator::GfxObject_ptr duplicateObject ();
  virtual void deleteObject ();
  virtual void orderBackOne ();
  virtual void orderForwardOne ();

private:
  KIllustratorDocument *document;
};

#endif
