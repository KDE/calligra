#ifndef __khtmlview_patched_h_
#define __khtmlview_patched_h_

#include <qpainter.h>

#include <khtmlview.h>

#include <khtmlsavedpage.h>

class KHTMLView_Patched : public KHTMLView
{
  Q_OBJECT
public:
  KHTMLView_Patched(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KHTMLView_Patched *parent_view = 0L);
  ~KHTMLView_Patched();

  void draw(SavedPage *p, QPainter *painter, int width, int height, float scale);

  virtual KHTMLView *newView(QWidget *parent, const char *name = 0L, int flags = 0L);

  void setMouseLock(bool flag);  
};

#endif
