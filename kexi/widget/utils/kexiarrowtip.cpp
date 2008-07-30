/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiarrowtip.h"

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtooltip.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <QPolygon>

#include <kexiutils/utils.h>

#ifdef __GNUC__
#warning KexiArrowTip ported to Qt4 but not tested
#else
#pragma WARNING( KexiArrowTip ported to Qt4 but not tested )
#endif

KexiArrowTip::KexiArrowTip(const QString& text, QWidget* parent)
 : KexiToolTip(text, parent)
 , m_opacity(0.0)
{
  QPalette pal( palette() );
  pal.setColor( QPalette::WindowText, Qt::red );
/*	QColorGroup cg(pal.active());
  cg.setColor(QColorGroup::Foreground, Qt::red);
  pal.setActive(cg);*/
  setPalette(pal);

  QFontMetrics fm(font());
  QSize sz(fm.boundingRect(m_value.toString()).size());
  sz += QSize(14, 10); //+margins
  m_arrowHeight = sz.height()/2;
  sz += QSize(0, m_arrowHeight); //+arrow height
  resize(sz);

//removed: 2.0 setAutoMask( false );

  //generate mask
  QPixmap maskPm(size());
  maskPm.fill( Qt::black );
  QPainter maskPainter(&maskPm);
  drawFrame(maskPainter);
  QImage maskImg( maskPm.toImage() );
  setMask( QBitmap::fromImage( maskImg.createHeuristicMask() ) );
}

KexiArrowTip::~KexiArrowTip()
{
}

void KexiArrowTip::show()
{
  if (isVisible())
    return;

  m_opacity = 0.0;
  setWindowOpacity(0.0);
  KexiToolTip::show();
  increaseOpacity();
}

void KexiArrowTip::hide()
{
  if (!isVisible())
    return;

  decreaseOpacity();
}

void KexiArrowTip::increaseOpacity()
{
  m_opacity += 0.10;
  setWindowOpacity(m_opacity);
  if (m_opacity < 1.0)
    QTimer::singleShot(25, this, SLOT(increaseOpacity()));
}

void KexiArrowTip::decreaseOpacity()
{
  if (m_opacity<=0.0) {
    KexiToolTip::close();
    m_opacity = 0.0;
    return;
  }
  m_opacity -= 0.10;
  setWindowOpacity(m_opacity);
  QTimer::singleShot(25, this, SLOT(decreaseOpacity()));
}

void KexiArrowTip::closeEvent( QCloseEvent * event )
{
  if (!isVisible()) {
    KexiToolTip::closeEvent(event);
  }
  if (m_opacity>0.0)
    decreaseOpacity();
  else
    KexiToolTip::closeEvent(event);
  //return m_opacity<=0.0;
}

void KexiArrowTip::drawContents(QPainter& p)
{
  p.setPen( QPen( palette().color( QPalette::WindowText ), 1) );
  //p.setPen( QPen(palette().active().foreground(), 1) );
  p.drawText(QRect(0,m_arrowHeight,width(),height()-m_arrowHeight), 
    Qt::AlignCenter, m_value.toString());
}

void KexiArrowTip::drawFrame(QPainter& p)
{
  //QPen pen(palette().active().foreground(), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
  p.setPen(
    QPen( palette().color( QPalette::WindowText ), 1, 
      Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin )
  );
  /*
     /\
   +-  -----+
   |  text  |
   +--------+
  */
  //1st line
  const int arrowOffset = 5; //5 pixels to right
  QPolygon pa(8);
  pa.setPoint(0, 0, m_arrowHeight-1);
  pa.setPoint(1, 0, height()-1);
  pa.setPoint(2, width()-1, height()-1);
  pa.setPoint(3, width()-1, m_arrowHeight-1);
  pa.setPoint(4, arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight-1);
  pa.setPoint(5, arrowOffset+m_arrowHeight-1, 0);
  pa.setPoint(6, arrowOffset, m_arrowHeight-1);
  pa.setPoint(7, 0, m_arrowHeight-1);
  p.drawPolyline(pa);
  //-2nd, internal line
  pa.resize(12);
  pa.setPoint(0, 1, m_arrowHeight);
  pa.setPoint(1, 1, height()-2);
  pa.setPoint(2, width()-2, height()-2);
  pa.setPoint(3, width()-2, m_arrowHeight);
  pa.setPoint(4, arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight);
  pa.setPoint(5, arrowOffset+m_arrowHeight-1, 1);
  pa.setPoint(6, arrowOffset, m_arrowHeight);
  pa.setPoint(7, 0, m_arrowHeight);
  pa.setPoint(8, arrowOffset+1, m_arrowHeight);
  pa.setPoint(9, arrowOffset+m_arrowHeight-1, 2);
  pa.setPoint(10, arrowOffset+m_arrowHeight+m_arrowHeight-3, m_arrowHeight);
  pa.setPoint(11, width()-2, m_arrowHeight);
  p.drawPolyline(pa);
}

#include "kexiarrowtip.moc"
