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

#include <koPageLayoutDia.h>

#define TOGGLE_ACTION(X) ((KToggleAction*)child(X))

KivioBirdEyePanel::KivioBirdEyePanel(KivioView* view, QWidget* parent, const char* name)
: KivioBirdEyePanelBase(parent, name), m_pView(view), m_pCanvas(view->canvasWidget()), m_pDoc(view->doc())
{
  m_buffer = new QPixmap();
  canvas->installEventFilter(this);

  connect( m_pDoc, SIGNAL( sig_updateView(KivioPage*)), SLOT(slotUpdateView(KivioPage*)) );
  connect( m_pCanvas, SIGNAL(zoomChanges(int)), SLOT(canvasZoomChanged(int)));

  KAction* act1 = new KAction( i18n("Zoom In"), "kivio_zoom_plus", 0, this, SLOT(zoomPlus()), this, "zoomIn" );
  KAction* act2 = new KAction( i18n("Zoom Out"), "kivio_zoom_minus", 0, this, SLOT(zoomMinus()), this, "zoomOut" );
  KToggleAction* act3 = new KToggleAction( i18n("Show Page Border"), "view_pageborder", 0, this, "pageBorder" );
  KToggleAction* act4 = new KToggleAction( i18n("View Only Page"), "view_page", 0, this, "pageOnly" );
  KAction* act5 = new KAction( i18n("Auto Resize"), "window_nofullscreen", 0, this, SLOT(doAutoResizeMin()), this, "autoResizeMin" );
  KAction* act6 = new KAction( i18n("Auto Resize"), "window_fullscreen", 0, this, SLOT(doAutoResizeMax()), this, "autoResizeMax" );

  connect( act3, SIGNAL(toggled(bool)), SLOT(togglePageBorder(bool)));
  connect( act4, SIGNAL(toggled(bool)), SLOT(togglePageOnly(bool)));

  act1->plug(bar);
  act2->plug(bar);
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
  m_pCanvas->setZoom(z);
}

void KivioBirdEyePanel::zoomMinus()
{
  m_pCanvas->zoomOut(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::zoomPlus()
{
  m_pCanvas->zoomIn(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::canvasZoomChanged(int z)
{
  slider->blockSignals(true);
  slider->setMaxValue(QMAX(z,500));
  slider->setValue(z);
  zoomBox->setValue(z);
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
    bitBlt(canvas,0,0,m_buffer);
    return true;
  }

  return KivioBirdEyePanelBase::eventFilter(o, ev);
}

void KivioBirdEyePanel::updateView()
{
  float zc = m_pCanvas->zoom()/100.0;
  QSize s1 = canvas->size();
  QSize s2;
  if (m_bPageOnly) {
    KoPageLayout pl = m_pView->activePage()->paperLayout();
    int pw = (int)(cvtMmToPt(pl.mmWidth)*zc);
    int ph = (int)(cvtMmToPt(pl.mmHeight)*zc);
    s2 = QSize(pw,ph);
  } else {
    s2 = m_pCanvas->actualSize();
  }

  float zx = s1.width()/(float)s2.width();
  float zy = s1.height()/(float)s2.height();
  float zxy = QMIN(zx,zy);
  float zoom = zc*zxy;

  KoPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = (int)(cvtMmToPt(pl.mmWidth)*zoom);
  int ph = (int)(cvtMmToPt(pl.mmHeight)*zoom);
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
  m_pDoc->paintContent(kpainter, rect, false, m_pView->activePage(), p0, zoom);
  kpainter.stop();

  bitBlt(canvas,0,0,m_buffer);

  QPainter painter(this,this);
  KivioRect vr = m_pCanvas->visibleArea();
/*
  QPoint p0 = actualPaperOrigin();

  int pw = (int)actualPaperSizePt().w;
  int ph = (int)actualPaperSizePt().h;
  int px0 = (width()-pw)/2;
  int py0 = (height()-ph)/2;
*/

  int x = (int)(vr.x()*zoom + px0);
  int y = (int)(vr.y()*zoom + py0);
  painter.setPen(red);
  painter.drawRect(x, y, vr.w()*zoom, vr.h()*zoom);
  painter.end();
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

