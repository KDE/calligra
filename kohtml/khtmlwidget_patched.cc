
#include "khtmlwidget_patched.h"
#include "khtmlwidget_patched.moc"

#include <iostream.h>

#include <kfm.h>

KHTMLWidget_Patched::KHTMLWidget_Patched(QWidget *parent = 0L, const char *name = 0L,
                      const char *pixDir = 0L)
:KHTMLWidget(parent, name, pixDir)		      
{
  cout << "hey, we're using our patched KHTMLWidget :-D" << endl;
  
  m_bMouseLockHack = false;
}		      
		      
KHTMLWidget_Patched::~KHTMLWidget_Patched()
{
}

void KHTMLWidget_Patched::draw(QPaintDevice *dev, int width, int height)
{
  if (painter) delete painter;
  painter = new QPainter(dev);

//  repaint(0, 0, width, height, false);
  QPaintEvent pe(QRect(0, 0, width, height));
  paintEvent(&pe);
  painter->end();
  delete painter;
}

void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)
{
  cerr << "void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)" << endl;
  
  if (m_bMouseLockHack) m_bMouseLockHack = false;
  else KHTMLWidget::mousePressEvent(ev);

}