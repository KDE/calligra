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

#ifndef KEXIUTILS_STYLEPROXY_H
#define KEXIUTILS_STYLEPROXY_H

#include <qstyle.h>
#include <qstylefactory.h>
#include <qpixmap.h>

#include "kexiutils_export.h"

namespace KexiUtils {

//! @short a QStyle proxy allowing to customizing the currently used style
/*! All you need is to reimplement one or more of the methods.
 For example, you can reimpelmente drawPrimitive() and temporary 
 change the color in color group.

 You can change even the smallest part of the style for a selected widget 
 using the following code:
 \code
  class MyStyle : public KexiUtils::StyleProxy {
   //reimplement method(s) here...
  };
  QWidget *w = .....
  w->setStyle( new MyStyle(&w->style(), w) ); //this will alter w's style a bit
 \endcode

 More info at http://doc.trolltech.com/qq/qq09-q-and-a.html#style
*/
class KEXIUTILS_EXPORT StyleProxy : public QStyle
{
	public:
		/*! Creates a new style proxy object. \a parentStyle pointer will not be kept
		 (because it's most likely owned by the application: a new QStyle instance 
		 for this name will be created internally. */
		StyleProxy(QStyle* parentStyle);
		virtual ~StyleProxy();

		QStyle* parentStyle() const;
		void setParentStyle(QStyle* style);

		virtual void polish( QWidget *w ) { m_style->polish(w); }
		virtual void unPolish( QWidget *w ) { m_style->unPolish(w); }

		virtual void polish( QApplication *a ) { m_style->polish(a); }
		virtual void unPolish( QApplication *a ) { m_style->unPolish(a); }

		virtual void polish( QPalette &p ) { m_style->polish(p); };

		virtual void polishPopupMenu( QPopupMenu* p ) { m_style->polishPopupMenu(p); }

		virtual QRect itemRect( QPainter *p, const QRect &r,
			int flags, bool enabled, const QPixmap *pixmap, const QString &text, int len = -1 ) const
		{
			return m_style->itemRect( p, r, flags, enabled, pixmap, text, len );
		}

		virtual void drawItem( QPainter *p, const QRect &r,
			int flags, const QColorGroup &g, bool enabled, const QPixmap *pixmap, const QString &text,
			int len = -1, const QColor *penColor = 0 ) const
		{
			m_style->drawItem( p, r, flags, g, enabled, pixmap, text, len, penColor );
		}

		virtual void drawPrimitive( PrimitiveElement pe,
			QPainter *p, const QRect &r, const QColorGroup &cg, SFlags flags = Style_Default,
			const QStyleOption& option = QStyleOption::Default ) const
		{
			m_style->drawPrimitive( pe, p, r, cg, flags, option );
		}

		virtual void drawControl( ControlElement element,
			QPainter *p, const QWidget *widget, const QRect &r, const QColorGroup &cg,
			SFlags how = Style_Default, const QStyleOption& option = QStyleOption::Default ) const
		{
			m_style->drawControl( element, p, widget, r, cg, how, option );
		}

		virtual void drawControlMask( ControlElement element,
			QPainter *p, const QWidget *widget, const QRect &r, 
			const QStyleOption& option = QStyleOption::Default ) const
		{
			m_style->drawControlMask( element, p, widget, r, option );
		}

		virtual QRect subRect( SubRect r, const QWidget *widget ) const
		{
			return m_style->subRect( r, widget );
		}

		virtual void drawComplexControl( ComplexControl control,
			QPainter *p, const QWidget *widget, const QRect &r,
			const QColorGroup &cg, SFlags how = Style_Default,
#ifdef Q_QDOC
			SCFlags sub = SC_All,
#else
			SCFlags sub = (uint)SC_All,
#endif
			SCFlags subActive = SC_None, const QStyleOption& option = QStyleOption::Default ) const
		{
			drawComplexControl( control, p, widget, r, cg, how, sub, subActive, option );
		}

		virtual void drawComplexControlMask( ComplexControl control,
			QPainter *p, const QWidget *widget, const QRect &r,
			const QStyleOption& option = QStyleOption::Default ) const
		{
			m_style->drawComplexControlMask( control, p, widget, r, option );
		}

		virtual QRect querySubControlMetrics( ComplexControl control,
			const QWidget *widget, SubControl sc, 
			const QStyleOption& option = QStyleOption::Default ) const
		{
			return m_style->querySubControlMetrics( control, widget, sc, option );
		}

		virtual SubControl querySubControl( ComplexControl control,
			const QWidget *widget, const QPoint &pos, 
			const QStyleOption& option = QStyleOption::Default ) const
		{
			return m_style->querySubControl( control, widget, pos, option );
		}

		virtual int pixelMetric( PixelMetric metric,
			const QWidget *widget = 0 ) const
		{
			return m_style->pixelMetric( metric, widget );
		}

		virtual QSize sizeFromContents( ContentsType contents,
			const QWidget *widget, const QSize &contentsSize,
			const QStyleOption& option = QStyleOption::Default ) const
		{
			return m_style->sizeFromContents( contents, widget, contentsSize, option );
		}

		virtual int styleHint( StyleHint stylehint,
			const QWidget *widget = 0, const QStyleOption& option = QStyleOption::Default,
			QStyleHintReturn* returnData = 0 ) const
		{
			return m_style->styleHint( stylehint, widget, option, returnData );
		}

		virtual QPixmap stylePixmap( StylePixmap stylepixmap,
				const QWidget *widget = 0,
				const QStyleOption& option = QStyleOption::Default ) const
		{
			return m_style->stylePixmap( stylepixmap, widget, option );
		}

	protected:
		QStyle *m_style;
};
}

#endif
