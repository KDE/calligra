/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tooldockbase.h"
#include "tooldockmanager.h"
#include "stencilbarmovemanager.h"

#include <kdrawutil.h>
#include <kapplication.h>

#include <qbitmap.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qobjectlist.h>

using namespace Kivio;

ToolDockBaseBorder::ToolDockBaseBorder( Position position, ToolDockBase* parent, const char* name )
: QWidget(parent,name)
{
  connect(this,SIGNAL(resizeStart()),parent,SLOT(beginResize()));
  connect(this,SIGNAL(resizeStop()),parent,SLOT(stopResize()));

  pos = position;
  switch (pos) {
    case Left:
    case Right:
      setCursor(SizeHorCursor);
      setFixedWidth(3);
      break;
    case Top:
    case Bottom:
      setCursor(SizeVerCursor);
      setFixedHeight(3);
      break;

    case TopLeft:
    case BottomRight:
      setCursor(SizeFDiagCursor);
      setFixedSize(3,3);
      break;
    case TopRight:
    case BottomLeft:
      setCursor(SizeBDiagCursor);
      setFixedSize(3,3);
      break;
  }
}

void ToolDockBaseBorder::paintEvent( QPaintEvent* /*ev*/ )
{
  if (!parent()->parent())
    return;

  QPainter p(this,this);
  int w = width()-1;
  int h = height()-1;
  switch (pos) {
    case Left:
      p.setPen(white);
      p.drawLine(0,0,0,h);
      break;
    case Right:
      p.setPen("gray30");
      p.drawLine(w,0,w,h);
      break;
    case Top:
      p.setPen(white);
      p.drawLine(0,0,w,0);
      break;
    case Bottom:
      p.setPen("gray30");
      p.drawLine(0,h,w,h);
      break;

    case TopLeft:
      p.setPen(white);
      p.drawLine(0,2,2,0);
      break;
    case BottomRight:
      p.setPen("gray30");
      p.drawLine(w,h-2,w-2,h);
      break;
    case TopRight:
      p.setPen("gray30");
      p.drawLine(w-2,0,w,2);
      break;
    case BottomLeft:
      p.setPen("gray30");
      p.drawLine(0,h-2,2,h);
      break;
  }
  p.end();
}

void ToolDockBaseBorder::mousePressEvent( QMouseEvent* /*ev*/ )
{
  ((ToolDockBase*)parentWidget())->activate();
  emit resizeStart();
}

void ToolDockBaseBorder::mouseReleaseEvent( QMouseEvent* /*ev*/ )
{
  emit resizeStop();
}

ToolDockBaseBorder::~ToolDockBaseBorder()
{
}
/******************************************************************************/
ToolDockBaseCaptionManager::ToolDockBaseCaptionManager( ToolDockBase* parent, const char* name )
: QObject(parent,name)
{
  m_pLeft = new ToolDockBaseCaption(ToolDockLeft,parent);
  m_pRight = new ToolDockBaseCaption(ToolDockRight,parent);
  m_pTop = new ToolDockBaseCaption(ToolDockTop,parent);
  m_pBottom = new ToolDockBaseCaption(ToolDockBottom,parent);

  connect(m_pLeft,SIGNAL(doClose()),SLOT(slotClose()));
  connect(m_pRight,SIGNAL(doClose()),SLOT(slotClose()));
  connect(m_pTop,SIGNAL(doClose()),SLOT(slotClose()));
  connect(m_pBottom,SIGNAL(doClose()),SLOT(slotClose()));

  connect(m_pLeft,SIGNAL(doStick(bool)),SLOT(slotStick(bool)));
  connect(m_pRight,SIGNAL(doStick(bool)),SLOT(slotStick(bool)));
  connect(m_pTop,SIGNAL(doStick(bool)),SLOT(slotStick(bool)));
  connect(m_pBottom,SIGNAL(doStick(bool)),SLOT(slotStick(bool)));

  m_pLeft->hide();
  m_pRight->hide();
  m_pTop->hide();
  m_pBottom->hide();
}

int ToolDockBaseCaptionManager::captionHeight()
{
  return m_pTop->height();
}

ToolDockBaseCaptionManager::~ToolDockBaseCaptionManager()
{
}

void ToolDockBaseCaptionManager::slotClose()
{
  emit doClose();
}

void ToolDockBaseCaptionManager::slotStick(bool f)
{
  emit doStick(f);
  m_pLeft->stick(f);
  m_pRight->stick(f);
  m_pTop->stick(f);
  m_pBottom->stick(f);
}

ToolDockBaseCaption* ToolDockBaseCaptionManager::captionWidget( ToolDockPosition pos )
{
  switch (pos) {
    case ToolDockLeft:
      return m_pLeft;
      break;
    case ToolDockRight:
      return m_pRight;
      break;
    case ToolDockTop:
      return m_pTop;
      break;
    case ToolDockBottom:
      return m_pBottom;
      break;
    default:
      break;
  }
  return 0L;
}

void ToolDockBaseCaptionManager::setView( ToolDockPosition pos )
{
  m_pLeft->hide();
  m_pRight->hide();
  m_pTop->hide();
  m_pBottom->hide();

  switch (pos) {
    case ToolDockLeft:
      if(!kapp->reverseLayout()) {
        m_pLeft->show();
      } else {
        m_pRight->show();
      }
      break;
    case ToolDockRight:
      if(!kapp->reverseLayout()) {
        m_pRight->show();
      } else {
        m_pLeft->show();
      }
      break;
    case ToolDockTop:
      m_pTop->show();
      break;
    case ToolDockBottom:
      m_pBottom->show();
      break;
    default:
      break;
  }
}
/******************************************************************************/
ToolDockBaseCaption::ToolDockBaseCaption( ToolDockPosition pos, ToolDockBase* parent, const char* name )
: QWidget(parent,name)
{
  setCursor(ArrowCursor);

  m_bPressed = false;
  m_bMove = false;
  position = pos;

  connect(this,SIGNAL(moveStart()),parent,SLOT(beginMove()));
  connect(this,SIGNAL(moveStop()),parent,SLOT(stopMove()));

  stickButton = new ToolDockButton(this);
  closeButton = new ToolDockButton(this);

  connect(closeButton,SIGNAL(clicked()),SLOT(slotClose()));
  connect(stickButton,SIGNAL(toogled(bool)),SLOT(slotStick(bool)));

  stickButton->setToggled(true);

  const char* close_xpm[]={
  "5 5 2 1",
  "# c black",
  ". c None",
  "#...#",
  ".#.#.",
  "..#..",
  ".#.#.",
  "#...#"};
  const char* stick_xpm[]={
  "8 7 2 1",
  "# c black",
  ". c None",
  "...#....",
  "...##.##",
  "...#.#.#",
  "####...#",
  "...#.#.#",
  "...##.##",
  "...#...."};
  stickButton->setPixmap(stick_xpm);
  closeButton->setPixmap(close_xpm);

  if ( pos == ToolDockTop || pos == ToolDockBottom ) {
    setFixedHeight(14);
    setMinimumWidth(100);
    QHBoxLayout* layout = new QHBoxLayout(this,1,2);
    layout->addStretch(1);
    layout->addWidget(stickButton);
    layout->addWidget(closeButton);
  } else {
    setFixedWidth(14);
    setMinimumHeight(100);
    QVBoxLayout* layout = new QVBoxLayout(this,1,2);
    if ( pos == ToolDockLeft ) {
      layout->addWidget(closeButton);
      layout->addWidget(stickButton);
      layout->addStretch(1);
    } else {
      layout->addStretch(1);
      layout->addWidget(stickButton);
      layout->addWidget(closeButton);
    }
  }

  QFont f(font());
  f.setBold(true);
  setFont(f);
}

ToolDockBaseCaption::~ToolDockBaseCaption()
{
}

void ToolDockBaseCaption::slotClose()
{
  emit doClose();
}

void ToolDockBaseCaption::slotStick(bool f)
{
  emit doStick(f);
}

void ToolDockBaseCaption::stick(bool f)
{
  stickButton->setDown(f);
}

void ToolDockBaseCaption::paintEvent( QPaintEvent* )
{
  QPainter p(this,this);

  int w;
  int h;
  bool f = false;
  int dl = 1;

  if ( position == ToolDockTop || position == ToolDockBottom ) {
    w = width();
    h = height();
  } else {
    w = height();
    h = width();
    if ( position == ToolDockLeft ) {
      p.rotate(-90);
      p.translate(-w,0);
    } else {
      p.rotate(90);
      p.translate(0,-h);
      f = true;
      dl++;
    }
  }

  QString t(parentWidget()->caption());
  int tw = w-37-3;
  int textw = p.fontMetrics().width(t);
  int lw = 10 + QMAX(0,tw-textw);

  p.drawText(3,0,w-40,h,AlignLeft|AlignVCenter,t);

	QPoint p1(w-lw-25, (h-5)/2+dl);
	QPoint p2(p1);
	p1 += QPoint(lw-5,0);

	qDrawShadeLine( &p, p1, p2, colorGroup(), f, 1, 0 );
	p1 += QPoint(0,3);
	p2 += QPoint(0,3);
	qDrawShadeLine( &p, p1, p2, colorGroup(), f, 1, 0 );

  p.end();
}

void ToolDockBaseCaption::mouseMoveEvent( QMouseEvent* ev )
{
  if (m_bPressed) {
    QPoint p = m_ppoint;
    p -= ev->pos();
    if (p.manhattanLength() > 2 && !m_bMove ) {
      m_bMove = true;
      emit moveStart();
    }
  }
}

void ToolDockBaseCaption::mousePressEvent( QMouseEvent* ev )
{
  ((ToolDockBase*)parentWidget())->activate();
  m_bPressed = true;
  m_ppoint = ev->pos();
}

void ToolDockBaseCaption::mouseReleaseEvent( QMouseEvent* )
{
  m_bPressed = false;
  if (m_bMove) {
    m_bMove = false;
    emit moveStop();
  }
}

/******************************************************************************/
ToolDockBase::ToolDockBase( QWidget* parent, const QString& caption, const char* name )
: QWidget(parent,name,WStyle_Customize|WStyle_NoBorder|WResizeNoErase|WRepaintNoErase)
{
  stick = false;
  hStatus = hnone;

  connect(&hideTimer,SIGNAL(timeout()),SLOT(slotHideTimeOut()));
  connect(&hideProcessTimer,SIGNAL(timeout()),SLOT(slotHideProcessTimeOut()));

  setCaption(caption);

  mrManager = new StencilBarMoveManager;
  m_pView = parent;

  connect(mrManager,SIGNAL(sizeChanged()),SLOT(sizeChanged()));
  connect(mrManager,SIGNAL(positionChanged()),SLOT(positionChanged()));
  connect(mrManager,
          SIGNAL(fixPosition(int&, int&, int&, int&)),
          SLOT(fixPosition(int&, int&, int&, int&)));

  connect(mrManager,
          SIGNAL(fixSize(int&, int&, int&, int&)),
          SLOT(fixSize(int&, int&, int&, int&)));

  m_pLayout = new QGridLayout(this,5,5,0,0);
  m_pLayout->setResizeMode(QLayout::Minimum);

  m_pBaseLayout = new QGridLayout(3,3);
  m_pBaseLayout->setResizeMode(QLayout::Minimum);

  m_pBaseLayout->setRowStretch(1,10);
  m_pBaseLayout->setColStretch(1,10);

  m_pBorderLeft = new ToolDockBaseBorder(ToolDockBaseBorder::Left,this);
  m_pBorderRight = new ToolDockBaseBorder(ToolDockBaseBorder::Right,this);
  m_pBorderTop = new ToolDockBaseBorder(ToolDockBaseBorder::Top,this);
  m_pBorderBottom = new ToolDockBaseBorder(ToolDockBaseBorder::Bottom,this);

  m_pBorderTopLeft = new ToolDockBaseBorder(ToolDockBaseBorder::TopLeft,this);
  m_pBorderTopRight = new ToolDockBaseBorder(ToolDockBaseBorder::TopRight,this);
  m_pBorderBottomRight = new ToolDockBaseBorder(ToolDockBaseBorder::BottomRight,this);
  m_pBorderBottomLeft = new ToolDockBaseBorder(ToolDockBaseBorder::BottomLeft,this);

  m_pCaptionManager = new ToolDockBaseCaptionManager(this);
  connect(m_pCaptionManager,SIGNAL(doClose()),SLOT(hide()));
  connect(m_pCaptionManager,SIGNAL(doStick(bool)),SLOT(slotStick(bool)));

  m_pLayout->addWidget(m_pBorderTopLeft,0,0);
  m_pLayout->addMultiCellWidget(m_pBorderTop,0,0,1,3);
  m_pLayout->addWidget(m_pBorderTopRight,0,4);

  m_pLayout->addMultiCellWidget(m_pCaptionManager->captionWidget(ToolDockTop),1,1,1,3);

  m_pLayout->addMultiCellWidget(m_pBorderLeft,1,3,0,0);
  m_pLayout->addMultiCellWidget(m_pCaptionManager->captionWidget(ToolDockLeft),1,3,1,1);
  m_pLayout->addLayout(m_pBaseLayout,2,2);
  m_pLayout->addMultiCellWidget(m_pCaptionManager->captionWidget(ToolDockRight),1,3,3,3);
  m_pLayout->addMultiCellWidget(m_pBorderRight,1,3,4,4);

  m_pLayout->addMultiCellWidget(m_pCaptionManager->captionWidget(ToolDockBottom),3,3,1,3);

  m_pLayout->addWidget(m_pBorderBottomLeft,4,0);
  m_pLayout->addMultiCellWidget(m_pBorderBottom,4,4,1,3);
  m_pLayout->addWidget(m_pBorderBottomRight,4,4);

  m_pCaptionManager->setView(ToolDockTop);
}

ToolDockBase::~ToolDockBase()
{
}

void ToolDockBase::slotStick(bool f)
{
  stick = f;
}

void ToolDockBase::show()
{
  snaps.clear();

  QPoint p(x(),y());

  if ( QABS(m_pView->height() - (p.y()+height())) < 17 ) {
    p.setY(m_pView->height() - height());
    snaps.set(ToolDockBottom,m_pView);
  }
  if ( QABS(p.y()) < 17 ) {
    p.setY(0);
    snaps.set(ToolDockTop,m_pView);
  }
  if ( QABS(p.x()) < 17 ) {
    p.setX(0);
    snaps.set(ToolDockLeft,m_pView);
  }
  if ( QABS(m_pView->width() - (p.x()+width())) < 17 ) {
    p.setX(m_pView->width() - width());
    snaps.set(ToolDockRight,m_pView);
  }
  updateCaption();

  QWidget::show();
  raise();

  emit visibleChange(true);
}

void ToolDockBase::hide()
{
  QWidget::hide();
  emit visibleChange(false);
}

void ToolDockBase::makeVisible(bool v)
{
  if (v)
    show();
  else
    hide();
}

void ToolDockBase::setView( QWidget* w )
{
  if (w->parentWidget()!=this)
    w->reparent(this,QPoint(0,0),true);
  m_pBaseLayout->addWidget(w,1,1);
}

void ToolDockBase::resizeEvent( QResizeEvent* ev )
{
  QWidget::resizeEvent(ev);

  int w = width();
  int h = height();

  QPoint p(x(),y());
  if ((m_pView->height() - (p.y()+h) < 17) || (m_pView->height() < (p.y()+h))) {
    p.setY(m_pView->height() - h);
  }
  if (p.y() < 17 || p.y() < 0) {
    p.setY(0);
  }
  if (p.x() < 17 || p.x() < 0) {
    p.setX(0);
  }
  if ((m_pView->width() - (p.x()+w) < 17) || (m_pView->width() < (p.x()+w))) {
    p.setX(m_pView->width() - w);
  }
  move(p);

  QRegion r1(2,0,w-4,h);
  QRegion r2(0,2,w,h-4);
  QRegion r3(1,1,w-2,h-2);
  setMask(r1.unite(r2).unite(r3));
}

void ToolDockBase::beginResize()
{
  ToolDockBaseBorder* b = (ToolDockBaseBorder*)sender();
  switch (b->position()) {
    case ToolDockBaseBorder::Left:
      mrManager->doXResize(this,true);
      break;
    case ToolDockBaseBorder::Right:
      mrManager->doXResize(this,false);
      break;
    case ToolDockBaseBorder::Top:
      mrManager->doYResize(this,true);
      break;
    case ToolDockBaseBorder::Bottom:
      mrManager->doYResize(this,false);
      break;
    case ToolDockBaseBorder::TopLeft:
      mrManager->doXYResize(this,true,true);
      break;
    case ToolDockBaseBorder::BottomRight:
      mrManager->doXYResize(this,false,false);
      break;
    case ToolDockBaseBorder::TopRight:
      mrManager->doXYResize(this,false,true);
      break;
    case ToolDockBaseBorder::BottomLeft:
      mrManager->doXYResize(this,true,false);
      break;
  }
}

void ToolDockBase::stopResize()
{
  mrManager->stop();
  resize(mrManager->geometry().size());
  move(parentWidget()->mapFromGlobal(mrManager->geometry().topLeft()));
  updateCaption();
}

void ToolDockBase::beginMove()
{
  mrManager->doMove(this);
}

void ToolDockBase::stopMove()
{
  mrManager->stop();
  move(parentWidget()->mapFromGlobal(mrManager->geometry().topLeft()));
  updateCaption();
}

void ToolDockBase::positionChanged()
{
  snaps.clear();

  QPoint p(mrManager->x(),mrManager->y());
  p = m_pView->mapFromGlobal(p);

  if ( QABS(m_pView->height() - (p.y()+height())) < 17 ) {
    p.setY(m_pView->height() - height());
    snaps.set(ToolDockBottom,m_pView);
  }
  if ( QABS(p.y()) < 17 ) {
    p.setY(0);
    snaps.set(ToolDockTop,m_pView);
  }
  if ( QABS(p.x()) < 17 ) {
    p.setX(0);
    snaps.set(ToolDockLeft,m_pView);
  }
  if ( QABS(m_pView->width() - (p.x()+width())) < 17 ) {
    p.setX(m_pView->width() - width());
    snaps.set(ToolDockRight,m_pView);
  }
  mrManager->move(m_pView->mapToGlobal(p));
}

void ToolDockBase::sizeChanged()
{
  snaps.clear();

  QPoint p(mrManager->x(),mrManager->y());
  p = m_pView->mapFromGlobal(p);
  QRect r(p,QSize(mrManager->width(),mrManager->height()));

  if ( QABS(m_pView->height() - r.bottom()) < 17 ) {
    r.setHeight(m_pView->height() - r.y());
    snaps.set(ToolDockBottom,m_pView);
  }
  if ( QABS(r.y()) < 17 ) {
    r.setY(0);
    snaps.set(ToolDockTop,m_pView);
  }
  if ( QABS(r.x()) < 17 ) {
    r.setX(0);
    snaps.set(ToolDockLeft,m_pView);
  }
  if ( QABS(m_pView->width() - r.right()) < 17 ) {
    r.setWidth(m_pView->width() - r.x());
    snaps.set(ToolDockRight,m_pView);
  }

  r.moveTopLeft(m_pView->mapToGlobal(r.topLeft()));
  mrManager->setGeometry(r);
}

void ToolDockBase::fixPosition(int& x, int& y, int& w, int& h)
{
  QRect r(m_pView->mapToGlobal(QPoint(0,0)),m_pView->size());
  x = QMIN(QMAX(r.x(),x),r.right()-w+1);
  y = QMIN(QMAX(r.y(),y),r.bottom()-h+1);
}

void ToolDockBase::fixSize(int& x, int& y, int& w, int& h)
{
  QRect r(m_pView->mapToGlobal(QPoint(0,0)),m_pView->size());
  if (r.x()>x) {
    w -= r.x()-x;
    x = r.x();
  }
  if (r.y()>y) {
    h -= r.y()-y;
    y = r.y();
  }
  if (x+w>r.right()) {
    w -= x + w -r.right()-1;
  }
  if (y+h>r.bottom()) {
    h -= y + h -r.bottom()-1;
  }
}

void ToolDockBase::mousePressEvent( QMouseEvent* )
{
  activate();
}

void ToolDockBase::activate()
{
  raise();
}
/******************************************************************************/
void ToolDockSnap::clear()
{
  snaps.clear();
}

void ToolDockSnap::set( ToolDockPosition pos, QWidget* widget )
{
  snaps.replace(pos,widget);
}

QWidget* ToolDockSnap::get( ToolDockPosition pos )
{
  return snaps[pos];
}

void ToolDockBase::updateCaption()
{
  m_pCaptionManager->setView(getCaptionPos());
}

ToolDockPosition ToolDockBase::getCaptionPos(bool* f)
{
  bool l = snaps.get(ToolDockLeft);
  bool r = snaps.get(ToolDockRight);
  bool t = snaps.get(ToolDockTop);
  bool b = snaps.get(ToolDockBottom);
  if (f)
    *f = true;

  if (l&&r&&t&&b)
    return ToolDockTop;

  if (l&&r&&t)
    return ToolDockBottom;

  if (l&&r&&b)
    return ToolDockTop;

  if (l)
    return ToolDockRight;

  if (r)
    return ToolDockLeft;

  if (t)
    return ToolDockBottom;

  if (b)
    return ToolDockTop;

  if (f)
    *f = false;

  return ToolDockTop;
}
/******************************************************************************/
ToolDockButton::ToolDockButton( QWidget* parent, const char* name )
: QFrame(parent,name)
{
  setFixedSize(12,12);
  tg = false;
  down = false;
  isin = false;
  mdown = false;
  pixmap = 0L;
}

ToolDockButton::~ToolDockButton()
{
    delete pixmap;
}

void ToolDockButton::paintEvent( QPaintEvent* ev )
{
  QFrame::paintEvent(ev);
  if (!pixmap)
    return;

  QPainter p(this,this);
  p.drawPixmap((width()-pixmap->width())/2,(height()-pixmap->height())/2,*pixmap);
  p.end();
}

void ToolDockButton::mousePressEvent( QMouseEvent* )
{
  mdown = true;
  setFrameStyle(Panel|Sunken);
}

void ToolDockButton::mouseReleaseEvent( QMouseEvent* )
{
  mdown = false;
  if (tg) {
    if (isin) {
      setDown(!down);
      emit toogled(down);
    }  else {
      setFrameStyle( NoFrame );
    }
  }  else {
    setFrameStyle( isin ? Panel|Raised : NoFrame );
  }

  if (isin)
    emit clicked();
}

void ToolDockButton::enterEvent( QEvent* )
{
  isin = true;
  if (!down && !mdown)
    setFrameStyle(Panel|Raised);
}

void ToolDockButton::leaveEvent( QEvent* )
{
  isin = false;
  if (!down && !mdown)
  setFrameStyle(NoFrame);
}

void ToolDockButton::setToggled( bool f )
{
  tg = f;
}

void ToolDockButton::setDown( bool f )
{
  down = f;
  setFrameStyle(down?Panel|Sunken:NoFrame);
}

void ToolDockButton::setPixmap( const QPixmap& p )
{
  pixmap = p.isNull() ? 0L : new QPixmap(p);
  repaint();
}

void ToolDockBase::mouseStatus(bool f)
{
  if (!isVisible())
    return;

  if (f || mrManager->isWorking()) {
    hideTimer.stop();
    if (hStatus == hstarthide)
      hStatus = hnone;
  } else {
    if (hStatus!=hnone || stick)
      return;

    bool f;
    hideDirection = getCaptionPos(&f);
    switch (hideDirection) {
      case ToolDockLeft:
        hideDirection = ToolDockRight;
        break;
      case ToolDockRight:
        hideDirection = ToolDockLeft;
        break;
      case ToolDockTop:
        hideDirection = ToolDockBottom;
        break;
      case ToolDockBottom:
        hideDirection = ToolDockTop;
        break;
      default:
        break;
    }

    if (f) {
      hStatus = hstarthide;
      hideTimer.start(1000,true);
    }
  }
}

void ToolDockBase::slotHideTimeOut()
{
  m_pBaseLayout->setEnabled(false);
  m_pLayout->setEnabled(false);

  hpixmap = new QPixmap(QPixmap::grabWidget(this));
  hminsize = minimumSize();
  hmaxsize = maximumSize();
  hsize = size();

  int m = m_pCaptionManager->captionHeight()+3;

  int wb = 0;
  switch (hideDirection) {
    case ToolDockLeft:
    case ToolDockRight:
      wb = width();
      break;
    case ToolDockTop:
    case ToolDockBottom:
      wb = height();
      break;
    default:
      break;
  }
  hdx = QMAX((wb-m)/50,1);
  hStatus = hprocesshide;
  setFixedSize(size());

  QObjectList* ch = (QObjectList*)children();
  for (QObject* o = ch->first();o;o=ch->next())
    if (o->isWidgetType())
      ((QWidget*)o)->hide();

  hideProcessTimer.start(1);
}

void ToolDockBase::slotHideProcessTimeOut()
{
  QPoint p(pos());
  QSize s(size());
  int z;
  int m = m_pCaptionManager->captionHeight()+3;

  switch (hideDirection) {
    case ToolDockLeft:
      s.setWidth(QMIN(QMAX(s.width()-hdx,m),hsize.width()));
      break;
    case ToolDockRight:
      z = s.width();
      s.setWidth(QMIN(QMAX(s.width()-hdx,m),hsize.width()));
      p.setX(p.x()+(z-s.width()));
      break;
    case ToolDockTop:
      s.setHeight(QMIN(QMAX(s.height()-hdx,m),hsize.height()));
      break;
    case ToolDockBottom:
      z = s.height();
      s.setHeight(QMIN(QMAX(s.height()-hdx,m),hsize.height()));
      p.setY(p.y()+(z-s.height()));
      break;
    default:
      break;
  }
  setFixedSize(s);
  move(p);

  if (s.height()==m || s.width()==m)
    hideProcessStop();

  if (s.height()==hsize.height() && s.width()==hsize.width())
    showProcessStop();
}

void ToolDockBase::hideProcessStop()
{
  hideProcessTimer.stop();
  hStatus = hdonehide;
}

void ToolDockBase::paintEvent( QPaintEvent* )
{
  if (!(hStatus==hprocesshide||hStatus==hdonehide||hStatus==hprocessshow))
    return;

  QPainter p(this,this);
  switch (hideDirection) {
    case ToolDockLeft:
      p.drawPixmap(width() - hpixmap->width(),0,*hpixmap);
      break;
    case ToolDockRight:
    case ToolDockBottom:
      p.drawPixmap(0,0,*hpixmap);
      break;
    case ToolDockTop:
      p.drawPixmap(0,height() - hpixmap->height(),*hpixmap);
      break;
    default:
      break;
  }
  p.end();
}

void ToolDockBase::enterEvent( QEvent* )
{
  if (hStatus == hdonehide) {
    hStatus = hprocessshow;
    hdx = -2*hdx;
    hideProcessTimer.start(1);
  }
}

void ToolDockBase::showProcessStop()
{
  hideProcessTimer.stop();
  hStatus = hnone;

  QObjectList* ch = (QObjectList*)children();
  for (QObject* o = ch->first();o;o=ch->next())
    if (o->isWidgetType() && !o->isA("ToolDockBaseCaption"))
      ((QWidget*)o)->show();

  updateCaption();
  setMinimumSize(hminsize);
  setMaximumSize(hmaxsize);
  delete hpixmap;

  m_pBaseLayout->setEnabled(true);
  m_pLayout->setEnabled(true);

  hsize.setWidth(hsize.width()+1);
  resize(hsize);

  hsize.setWidth(hsize.width()-1);
  resize(hsize);
}
#include "tooldockbase.moc"
