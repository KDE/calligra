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

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexiarrowtip.h"

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qwidget.h>
#include <qtooltip.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qtimer.h>


KexiArrowTip::KexiArrowTip(const QString& text, QWidget* parent)
 : QWidget(parent, "KexiArrowTip", Qt::WStyle_Customize | Qt::WType_Popup | Qt::WStyle_NoBorder 
	| Qt::WX11BypassWM | Qt::WDestructiveClose)
 , m_text(text)
 , m_opacity(0.0)
{
	QPalette pal( QToolTip::palette() );
	QColorGroup cg(pal.active());
	cg.setColor(QColorGroup::Foreground, Qt::red);
	pal.setActive(cg);
	setPalette(pal);

	setFocusPolicy(QWidget::NoFocus);

	QFontMetrics fm(font());
	QSize sz(fm.boundingRect(m_text).size());
	sz += QSize(14, 10); //+margins
	m_arrowHeight = sz.height()/2;
	sz += QSize(0, m_arrowHeight); //+arrow height
	resize(sz);

//	setMargin(2);
	setAutoMask( false );
//	setFrameStyle( QFrame::Plain | QFrame::Box );
//	setLineWidth( 2 );
//	setAlignment( Qt::AlignAuto | Qt::AlignTop );
//	setIndent(0);
//	polish();
//	adjustSize();

	//generate mask
	QPixmap maskPm(size());
	maskPm.fill( black );
	QPainter maskPainter(&maskPm);
	draw(maskPainter);
	QImage maskImg( maskPm.convertToImage() );
	QBitmap bm;
	bm = maskImg.createHeuristicMask();
	setMask( bm );
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
	QWidget::show();
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
		QWidget::close();
		m_opacity = 0.0;
		return;
	}
	m_opacity -= 0.10;
	setWindowOpacity(m_opacity);
	QTimer::singleShot(25, this, SLOT(decreaseOpacity()));
}

//virtual bool close( bool alsoDelete );

bool KexiArrowTip::close ( bool alsoDelete )
{
	if (!isVisible()) {
		return QWidget::close(alsoDelete);
	}
	if (m_opacity>0.0)
		decreaseOpacity();
	else
		return QWidget::close(alsoDelete);
	return m_opacity<=0.0;
}

/*void KexiArrowTip::closeEvent( QCloseEvent *  )
{
}*/

void KexiArrowTip::paintEvent( QPaintEvent *pev )
{
	QWidget::paintEvent(pev);
	QPainter p(this);
	p.drawText(QRect(0,m_arrowHeight,width(),height()-m_arrowHeight), 
		Qt::AlignCenter, m_text);
	draw(p);
}

void KexiArrowTip::draw(QPainter& p)
{
	p.setPen( Qt::red );
	/*
	   /\
	 +-  -----+
	 |  text  |
	 +--------+
	*/
	//left, bottom, right borders
	p.drawLine(0, m_arrowHeight, 0, height()-1);
	p.drawLine(0, height()-1, width()-1, height()-1);
	p.drawLine(width()-1, height()-1, width()-1, m_arrowHeight);
	//-2nd line
	p.drawLine(1, m_arrowHeight, 1, height()-2);
	p.drawLine(1, height()-2, width()-2, height()-2);
	p.drawLine(width()-2, height()-2, width()-2, m_arrowHeight);
	//arrow + top border
	const int arrowOffset = 5; //5 pixels to right
	p.drawLine(0, m_arrowHeight-1, arrowOffset, m_arrowHeight-1);
	p.drawLine(arrowOffset, m_arrowHeight-1, arrowOffset+m_arrowHeight-1, 0);
	p.drawLine(arrowOffset+m_arrowHeight-1, 0, arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight-1);
	p.drawLine(arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight-1, width()-1, m_arrowHeight-1);
	//-2nd line
	p.drawLine(0, m_arrowHeight, arrowOffset, m_arrowHeight);
	p.drawLine(arrowOffset+1, m_arrowHeight-1, arrowOffset+m_arrowHeight-1, 1);
	p.drawLine(arrowOffset+m_arrowHeight-1, 1, arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight);
	p.drawLine(arrowOffset+m_arrowHeight+m_arrowHeight-2, m_arrowHeight, width()-1, m_arrowHeight);
	//-3rd line for arrow only
	p.drawLine(arrowOffset+1, m_arrowHeight, arrowOffset+m_arrowHeight-1, 2);
	p.drawLine(arrowOffset+m_arrowHeight-1, 2, arrowOffset+m_arrowHeight+m_arrowHeight-3, m_arrowHeight);
}

/*
QImage img( "F:\\a.png" );
QPixmap p;
p.convertFromImage( img );
    if ( !p.mask() )
	if ( img.hasAlphaBuffer() ) {
	    QBitmap bm;
	    bm = img.createAlphaMask();
	    p.setMask( bm );
	} else {
	    QBitmap bm;
	    bm = img.createHeuristicMask();
	    p.setMask( bm );
	}
setBackgroundPixmap( p );
if ( p.mask() )
	setMask( *p.mask() );
*/

#include "kexiarrowtip.moc"
