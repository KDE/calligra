#ifndef __khtmlview_patched_h_
#define __khtmlview_patched_h_

#include <qpaintdevice.h>

#include <khtmlview.h>

class KHTMLView_Patched : public KHTMLView
{
  Q_OBJECT
public:
  KHTMLView_Patched(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KHTMLView_Patched *parent_view = 0L);
  ~KHTMLView_Patched();

  virtual void draw(QPaintDevice *dev, int width, int height);

  virtual KHTMLView *newView(QWidget *parent, const char *name = 0L, int flags = 0L);

  void setMouseLock(bool flag);  
};

#endif
