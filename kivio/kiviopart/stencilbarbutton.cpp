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
#include "stencilbarbutton.h"

#include <kiconloader.h>
#include <kstringhandler.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <kdebug.h>
#include "qdrawutil.h"

using namespace Kivio;

DragBarButton::DragBarButton( const QString& text, QWidget* parent, const char* name )
: QPushButton(text,parent,name)
{
  m_pIcon = 0L;
  m_bPressed = false;
  m_bDragged = false;
  m_bMouseOn = false;
  m_bClose = false;

  const char* stencil_xpm[] = {
  "12 12 17 1",
  ".	c #08009A",
  "+	c #9A9C10",
  "@	c None",
  "#	c #0800A1",
  "$	c #FAFD00",
  "%	c #080092",
  "&	c #F9FC07",
  "*	c #F9FC00",
  "=	c #070094",
  "-	c #F4F70C",
  ";	c #F6F900",
  ">	c #08008D",
  ",	c #F7FA00",
  "'	c #C00101",
  ")	c #C20904",
  "!	c #C90000",
  "~	c #BC0000",
  ".......+++@@",
  "..#...$$$$+@",
  "..%..&$***$+",
  ".=..-***$;*+",
  ".>.$**$*$,*+",
  "..;**$$$$,*+",
  ".$')!!!!!!!~",
  "..-!!!!!!!!+",
  "...-!!!!!!!+",
  "....-!!!!!++",
  "....%-!!+++@",
  "......++++@@"};

  setPixmap(stencil_xpm);

  const QColorGroup& g = colorGroup();
  QColor c = g.buttonText();

  int cr, cg, cb;
  QString line;
  c.rgb( &cr, &cg, &cb );
  line.sprintf(". c #%02X%02X%02X",cr,cg,cb);

  const char* close_xpm[] = {
  "8 7 2 1",
  "x c None",
  (const char*)line.ascii(),
  "xxxxxxxx",
  "x..xx..x",
  "xx....xx",
  "xxx..xxx",
  "xx....xx",
  "x..xx..x",
  "xxxxxxxx"
  };
  m_pClosePix = new QPixmap(close_xpm);

  QToolTip::add(this, text);
}

DragBarButton::~DragBarButton()
{
  delete m_pClosePix;
  delete m_pIcon;

  kdDebug(43000) << "DragBarButton - AHHHHHH I'M DYING!" << endl;
}

void DragBarButton::drawButton( QPainter* paint )
{
#ifdef __GNUC__
#warning "Left out for now, lacking a style expert (Werner)"
#endif
  const QColorGroup& g = colorGroup();
  style().drawControl( QStyle::CE_PushButton, paint, this, QRect(0, 0, width(), height()), colorGroup() );

  int m = 3;
  int tw = 0;
  int pw = 0;

  if ( m_pIcon ) {
    pw = m_pIcon->width();
    style().drawItem( paint, QRect( m, 0, pw, height() ),
                      AlignLeft | AlignVCenter,
                      colorGroup(), isEnabled(),
                      m_pIcon, QString::null, -1,
                      &g.buttonText());
  }

  if ( !text().isEmpty() ) {
    QFontMetrics fm = fontMetrics();
    QSize sz = fm.size( ShowPrefix, text() );
    tw = sz.width();
    int x = m + pw + (tw!=0 && pw!=0 ?m:0);
    QString t = KStringHandler::rPixelSqueeze(text(), fm, width() - (x + m + 22));

    style().drawItem( paint, QRect( x, 0, tw, height() ),
                      AlignLeft | AlignVCenter|ShowPrefix,
                      colorGroup(), isEnabled(),
                      0L, t, -1,
                      &g.buttonText());
  }


  QPoint p1(width()-10,0);
  QPoint p2(width()-10,height());
  qDrawShadeLine( paint, p1, p2, g, true, 0, 1 );

  p1 += QPoint(2,0);
  p2 += QPoint(2,0);
  qDrawShadeLine( paint, p1, p2, g, true, 0, 1 );

// Temporary fix
//  if (m_bMouseOn) {
    int z = m_bClose ? 1:0;
    paint->drawPixmap(width()-20+z, (height()-m_pClosePix->height())/2+z, *m_pClosePix );
//  }
}

void DragBarButton::setIcon( const QString& name )
{
  m_pIcon = new QPixmap(BarIcon(name));
}

void DragBarButton::setPixmap( const QPixmap& pixmap )
{
  m_pIcon = new QPixmap(pixmap);
}

QSize DragBarButton::sizeHint() const
{
  constPolish();
  int m = 3;
  int tw = 0;
  int th = 0;
  int pw = 0;
  int ph = 0;

  if ( !text().isEmpty() ) {
    QFontMetrics fm = fontMetrics();
    QSize sz = fm.size( ShowPrefix, text() );
    tw = 64; // Minimum size
    th = sz.height();
  }

  if ( m_pIcon ) {
    pw = m_pIcon->width();
    ph = m_pIcon->height();
  }

  int h = QMAX(ph,th)+2*m;
  int w = m + pw + (tw!=0 && pw!=0 ?m:0) + tw + m + 22;

  return QSize(w,h).expandedTo( QApplication::globalStrut() );
}

void DragBarButton::mousePressEvent( QMouseEvent* ev )
{
  m_bClose = false;
  QRect closeRect(width()-20,0,m_pClosePix->width(),height());
  if ( closeRect.contains(ev->pos())) {
    m_bClose = true;
    repaint();
    return;
  }

  m_bPressed = true;
  m_ppoint = ev->pos();
}

void DragBarButton::mouseReleaseEvent( QMouseEvent* ev )
{
  if ( m_bClose ) {
    m_bClose = false;
    repaint();
    QRect closeRect(width()-20,0,m_pClosePix->width(),height());
    if ( closeRect.contains(ev->pos()))
    {
       kdDebug(43000) << "DragBarButton::mouseReleaseEvent() - Emitting closeRequest" << endl;
        emit closeRequired(this);
    }
    return;
  }

  m_bPressed = false;
  if (m_bDragged) {
    m_bDragged = false;
    emit finishDrag();
  } else {
    emit clicked();
  }
}

void DragBarButton::mouseMoveEvent( QMouseEvent* ev )
{
  if (m_bPressed) {
    QPoint p = m_ppoint;
    p -= ev->pos();
    if (p.manhattanLength() > 2 && !m_bDragged ) {
      m_bDragged = true;
      emit beginDrag();
    }
  }
}

void DragBarButton::enterEvent( QEvent* ev )
{
  QPushButton::enterEvent(ev);
  m_bMouseOn = true;
  repaint(false);
}

void DragBarButton::leaveEvent( QEvent* ev )
{
  QPushButton::leaveEvent(ev);
  m_bMouseOn = false;
  repaint();
}

#include "stencilbarbutton.moc"
