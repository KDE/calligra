#include "kivio_birdeye_panel.h"

#include "kivio_screen_painter.h"

#include "kivio_rect.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_common.h"

#include <ktoolbar.h>
#include <kaction.h>

#include <qpixmap.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qframe.h>
#include <qcursor.h>

#define TOGGLE_ACTION(X) ((KToggleAction*)child(X))

KivioBirdEyePanel::KivioBirdEyePanel(KivioView* view, QWidget* parent, const char* name)
: KivioBirdEyePanelBase(parent, name), m_pView(view), m_pCanvas(view->canvasWidget()), m_pDoc(view->doc())
{
  handlePress = false;
  m_buffer = new QPixmap();
  canvas->installEventFilter(this);

  connect( m_pDoc, SIGNAL( sig_updateView(KivioPage*)), SLOT(slotUpdateView(KivioPage*)) );
  connect( m_pCanvas, SIGNAL(zoomChanges(float)), SLOT(canvasZoomChanged(float)));
  connect( m_pCanvas, SIGNAL(visibleAreaChanged()), SLOT(updateVisibleArea()));

  zoomIn = new KAction( i18n("Zoom In"), "kivio_zoom_plus", 0, this, SLOT(zoomPlus()), this, "zoomIn" );
  zoomOut = new KAction( i18n("Zoom Out"), "kivio_zoom_minus", 0, this, SLOT(zoomMinus()), this, "zoomOut" );
  KToggleAction* act3 = new KToggleAction( i18n("Show Page Border"), "view_pageborder", 0, this, "pageBorder" );
  KToggleAction* act4 = new KToggleAction( i18n("View Page Only"), "view_page", 0, this, "pageOnly" );
  KAction* act5 = new KAction( i18n("Auto Resize"), "window_nofullscreen", 0, this, SLOT(doAutoResizeMin()), this, "autoResizeMin" );
  KAction* act6 = new KAction( i18n("Auto Resize"), "window_fullscreen", 0, this, SLOT(doAutoResizeMax()), this, "autoResizeMax" );

  connect( act3, SIGNAL(toggled(bool)), SLOT(togglePageBorder(bool)));
  connect( act4, SIGNAL(toggled(bool)), SLOT(togglePageOnly(bool)));

  zoomIn->plug(bar);
  zoomOut->plug(bar);
  act3->plug(bar);
  act4->plug(bar);
  act5->plug(bar);
  act6->plug(bar);

  togglePageBorder(true);
  togglePageOnly(false);

  canvasZoomChanged(m_pCanvas->zoom());
}

KivioBirdEyePanel::~KivioBirdEyePanel()
{
  delete m_buffer;
}

void KivioBirdEyePanel::zoomChanged(int z)
{
//  debug(QString("zoomChanged %1 %2").arg(z).arg((float)(z/100.0)));
  m_pCanvas->setZoom((float)(z/100.0));
}

void KivioBirdEyePanel::zoomMinus()
{
  m_pCanvas->zoomOut(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::zoomPlus()
{
  m_pCanvas->zoomIn(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::canvasZoomChanged(float z)
{
  int iz = (int)(z*100.1f);
/*
<<<<<<< kivio_birdeye_panel.cpp
  int iz = (int)(z*100.1f);

=======
  if(z<=5)
        zoomOut->setEnabled(false);
  else
        zoomOut->setEnabled(true);
  if(z>=10000)
        zoomIn->setEnabled(false);
  else
        zoomIn->setEnabled(true);

>>>>>>> 1.3
*/
  slider->blockSignals(true);
  zoomBox->blockSignals(true);

  zoomBox->setValue(iz);
  slider->setMaxValue(QMAX(iz,500));
  slider->setValue(iz);

  zoomBox->blockSignals(false);
  slider->blockSignals(false);

  slotUpdateView(m_pView->activePage());
}

void KivioBirdEyePanel::slotUpdateView(KivioPage* page)
{
  if (!isVisible() || !page || m_pView->activePage() != page)
    return;

  updateView();
}

bool KivioBirdEyePanel::eventFilter(QObject* o, QEvent* ev)
{
  if (o == canvas && ev->type() == QEvent::Show) {
    updateView();
  }

  if (o == canvas && ev->type() == QEvent::Resize) {
    m_buffer->resize(canvas->size());
    slotUpdateView(m_pView->activePage());
  }

  if (o == canvas && ev->type() == QEvent::Paint) {
    updateVisibleArea();
    return true;
  }

  if (o == canvas && ev->type() == QEvent::MouseMove) {
    QMouseEvent* me = (QMouseEvent*)ev;
    if (me->state() == LeftButton)
      handleMouseMoveAction(me->pos());
    else
      handleMouseMove(me->pos());

    lastPos = me->pos();
    return true;
  }

  if (o == canvas && ev->type() == QEvent::MouseButtonPress) {
    QMouseEvent* me = (QMouseEvent*)ev;
    if (me->button() == LeftButton)
      handleMousePress(me->pos());

    return true;
  }

  return KivioBirdEyePanelBase::eventFilter(o, ev);
}

void KivioBirdEyePanel::updateView()
{
  float zc = m_pCanvas->zoom();
  QSize s1 = canvas->size();
  QSize s2;
  if (m_bPageOnly) {
    TKPageLayout pl = m_pView->activePage()->paperLayout();
    int pw = (int)(pl.ptWidth()*zc);
    int ph = (int)(pl.ptHeight()*zc);
    s2 = QSize(pw,ph);
  } else {
    s2 = m_pCanvas->actualSize();
  }

  float zx = s1.width()/(float)s2.width();
  float zy = s1.height()/(float)s2.height();
  float zxy = QMIN(zx,zy);

  zoom = zc*zxy;

  TKPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = (int)(pl.ptWidth()*zoom);
  int ph = (int)(pl.ptHeight()*zoom);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  int pcw = (int)(s2.width()*zxy);
  int pch = (int)(s2.height()*zxy);
  int pcx0 = (s1.width()-pcw)/2;
  int pcy0 = (s1.height()-pch)/2;

  cMinSize = QSize((int)(s2.width()*QMIN(zx,zy)), (int)(s2.height()*QMIN(zx,zy)));
  cMaxSize = QSize((int)(s2.width()*QMAX(zx,zy)), (int)(s2.height()*QMAX(zx,zy)));

  QPoint p0 = QPoint(px0,py0);

  QRect rect(QPoint(0,0),s1);

  KivioScreenPainter kpainter;
  kpainter.start(m_buffer);
  kpainter.painter()->fillRect(rect, QColor(120, 120, 120));

  if (m_bShowPageBorders) {
    kpainter.painter()->fillRect(pcx0, pcy0, pcw, pch, QColor(200, 200, 200));
    kpainter.painter()->fillRect(px0, py0, pw, ph, white);
  } else {
    kpainter.painter()->fillRect(pcx0, pcy0, pcw, pch, white);
  }

  kpainter.painter()->translate(px0, py0);
  m_pDoc->paintContent(kpainter, rect, false, m_pView->activePage(), p0, zoom, false);
  kpainter.stop();

  updateVisibleArea();
}

void KivioBirdEyePanel::togglePageBorder(bool b)
{
  TOGGLE_ACTION("pageBorder")->setChecked(b);
  m_bShowPageBorders = b;

  slotUpdateView(m_pView->activePage());
}

void KivioBirdEyePanel::togglePageOnly(bool b)
{
  TOGGLE_ACTION("pageOnly")->setChecked(b);
  m_bPageOnly = b;

  slotUpdateView(m_pView->activePage());
}

void KivioBirdEyePanel::doAutoResizeMin()
{
  parentWidget()->resize(parentWidget()->width() - canvas->width() + cMinSize.width(), parentWidget()->height() - canvas->height() + cMinSize.height());
}

void KivioBirdEyePanel::doAutoResizeMax()
{
  parentWidget()->resize(parentWidget()->width() - canvas->width() + cMaxSize.width(), parentWidget()->height() - canvas->height() + cMaxSize.height());
}

void KivioBirdEyePanel::show()
{
  KivioBirdEyePanelBase::show();
  doAutoResizeMax();
}

void KivioBirdEyePanel::updateVisibleArea()
{
  bitBlt(canvas,0,0,m_buffer);

  KivioRect vr = m_pCanvas->visibleArea();
  QSize s1 = canvas->size();
  TKPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = (int)(pl.ptWidth()*zoom);
  int ph = (int)(pl.ptHeight()*zoom);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  int x = (int)(vr.x()*zoom + px0);
  int y = (int)(vr.y()*zoom + py0);
  int w = (int)(vr.w()*zoom);
  int h = (int)(vr.h()*zoom);

  QPainter painter(canvas,canvas);
  painter.setPen(red);
  painter.drawRect(x, y, w, h);
  painter.setPen(red.light());
  painter.drawRect(x-1, y-1, w+2, h+2);
  painter.end();

  varea.setRect(x,y,w,h);
}

void KivioBirdEyePanel::handleMouseMove(QPoint p)
{
  handlePress = true;

  QRect r1 = QRect(varea.x()-1, varea.y()-1, 3, varea.height()+2);
  if (r1.contains(p)) {
    canvas->setCursor(sizeHorCursor);
    apos = AlignLeft;
    return;
  }

  r1.moveBy(varea.width(),0);
  if (r1.contains(p)) {
    canvas->setCursor(sizeHorCursor);
    apos = AlignRight;
    return;
  }

  QRect r2 = QRect(varea.x()-1, varea.y()-1, varea.width()+2, 3);
  if (r2.contains(p)) {
    canvas->setCursor(sizeVerCursor);
    apos = AlignTop;
    return;
  }

  r2.moveBy(0, varea.height());
  if (r2.contains(p)) {
    canvas->setCursor(sizeVerCursor);
    apos = AlignBottom;
    return;
  }

  if (varea.contains(p)) {
    canvas->setCursor(sizeAllCursor);
    apos = AlignCenter;
    return;
  }

  canvas->setCursor(arrowCursor);
  handlePress = false;
}

void KivioBirdEyePanel::handleMouseMoveAction(QPoint p)
{
  if (!handlePress)
    return;

  p -= lastPos;

  if (apos == AlignCenter) {
    float z = m_pCanvas->zoom()/zoom;
    m_pCanvas->setUpdatesEnabled(false);
    m_pCanvas->scrollDx(-(int)(p.x()*z));
    m_pCanvas->scrollDy(-(int)(p.y()*z));
    m_pCanvas->setUpdatesEnabled(true);
    return;
  }

  float dx = p.x() / zoom;
  float dy = p.y() / zoom;

  if (apos == AlignRight) {
    KivioRect vr = m_pCanvas->visibleArea();
    vr.setW(QMAX(10.0, vr.w() + dx));
    m_pCanvas->setVisibleAreaByWidth(vr);
    return;
  }

  if (apos == AlignLeft) {
    KivioRect vr = m_pCanvas->visibleArea();
    vr.setX(vr.x() + dx);
    vr.setW(QMAX(10.0, vr.w() - dx));
    m_pCanvas->setVisibleAreaByWidth(vr);
    return;
  }

  if (apos == AlignTop) {
    KivioRect vr = m_pCanvas->visibleArea();
    vr.setY(vr.y() + dy);
    vr.setH(QMAX(10.0 ,vr.h() - dy));
    m_pCanvas->setVisibleAreaByHeight(vr);
    return;
  }

  if (apos == AlignBottom) {
    KivioRect vr = m_pCanvas->visibleArea();
    vr.setH(QMAX(10.0 ,vr.h() + dy));
    m_pCanvas->setVisibleAreaByHeight(vr);
    return;
  }
}

void KivioBirdEyePanel::handleMousePress(QPoint p)
{
  if (handlePress)
    return;

  QSize s1 = canvas->size();
  TKPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = (int)(pl.ptWidth()*zoom);
  int ph = (int)(pl.ptHeight()*zoom);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  float x = (p.x() - px0) / zoom;
  float y = (p.y() - py0) / zoom;

  m_pCanvas->setViewCenterPoint(KivioPoint(x,y));
}
#include "kivio_birdeye_panel.moc"
