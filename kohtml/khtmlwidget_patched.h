#ifndef __khtmlwidget_patched_h_
#define __khtmlwidget_patched_h_

#include <qpaintdevice.h>
#include <qstrlist.h>

#include <khtml.h>

class KHTMLWidget_Patched : public KHTMLWidget
{
  Q_OBJECT
public:
  KHTMLWidget_Patched(QWidget *parent = 0L, const char *name = 0L,
                      const char *pixDir = 0L);
  ~KHTMLWidget_Patched();

  void draw(QPaintDevice *dev, int width, int height);
  
  void setMouseLock(bool flag) { m_bMouseLockHack = flag; }
  
protected:
  virtual void mousePressEvent(QMouseEvent *ev);

private:
  bool m_bMouseLockHack;    
};  		        

#endif
