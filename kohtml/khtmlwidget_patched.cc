
#include "khtmlwidget_patched.h"
#include "khtmlwidget_patched.moc"

#include <iostream.h>


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
  cerr << "drawinggggggg" << endl;

//  if (painter) delete painter
  
  QPainter::redirect(this, dev);
  QPaintEvent pe(QRect(x_offset, y_offset, x_offset+width, y_offset+height));
  QApplication::sendEvent(this, &pe);
  QPainter::redirect(this, 0);
  
  cerr << "done :-))))))" << endl;
}

void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)
{
  cerr << "void KHTMLWidget_Patched::mousePressEvent(QMouseEvent *ev)" << endl;
  
  if (m_bMouseLockHack) m_bMouseLockHack = false;
  else KHTMLWidget::mousePressEvent(ev);

}