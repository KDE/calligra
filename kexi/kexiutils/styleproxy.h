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
		virtual void unpolish( QWidget *w ) { m_style->unpolish(w); }

		virtual void polish( QApplication *a ) { m_style->polish(a); }
		virtual void unpolish( QApplication *a ) { m_style->unpolish(a); }

		virtual void polish( QPalette &p ) { m_style->polish(p); }

		virtual QRect itemTextRect(const QFontMetrics &fm, const QRect &r,
			int flags, bool enabled, const QString &text) const
		{
			return m_style->itemTextRect(fm, r, flags, enabled, text);
		}

		virtual QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
		{
			return m_style->itemPixmapRect(r, flags, pixmap);
		}

		virtual void drawItemText(QPainter *painter, const QRect &rect,
			int flags, const QPalette &pal, bool enabled,
			const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
		{
			m_style->drawItemText(painter, rect, flags, pal, enabled, text, textRole);
		}

		virtual void drawPrimitive( PrimitiveElement element, 
			const QStyleOption * option, QPainter * painter, 
			const QWidget * widget = 0 ) const
		{
			m_style->drawPrimitive( element, option, painter, widget );
		}

		virtual void drawItemPixmap(QPainter *painter, const QRect &rect,
			int alignment, const QPixmap &pixmap) const
		{
			m_style->drawItemPixmap(painter, rect, alignment, pixmap);
		}

		virtual QPalette standardPalette() const { return m_style->standardPalette(); }

		virtual void drawControl(ControlElement element, const QStyleOption *opt, 
			QPainter *p, const QWidget *w = 0) const
		{
			m_style->drawControl(element, opt, p, w);
		}

		virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
																	const QWidget *widget = 0) const
		{
			return m_style->subElementRect(subElement, option, widget);
		}
	
		virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
			QPainter *p, const QWidget *widget = 0) const
		{
			m_style->drawComplexControl(cc, opt, p, widget);
		}
		
		virtual SubControl hitTestComplexControl(ComplexControl cc,
			const QStyleOptionComplex *opt, const QPoint &pt, const QWidget *widget = 0) const
		{
			return m_style->hitTestComplexControl(cc, opt, pt, widget);
		}
		
		virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
			SubControl sc, const QWidget *widget = 0) const
		{
			return m_style->subControlRect(cc, opt, sc, widget);
		}
	
		virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
			const QWidget *widget = 0) const
		{
			return m_style->pixelMetric(metric, option, widget);
		}
	
		virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
			const QSize &contentsSize, const QWidget *w = 0) const
		{
			return m_style->sizeFromContents(ct, opt, contentsSize, w);
		}
	
		virtual int styleHint(StyleHint stylehint, const QStyleOption *opt = 0,
			const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const
		{
			return m_style->styleHint(stylehint, opt, widget, returnData);
		}
	
		virtual QPixmap standardPixmap(StandardPixmap standardPixmap, 
			const QStyleOption *opt = 0, const QWidget *widget = 0) const
		{
			return m_style->standardPixmap(standardPixmap, opt, widget);
		}
	
		QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = 0,
			const QWidget *widget = 0) const
		{
			return m_style->standardIcon(standardIcon, option, widget);
		}
	
		virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
			const QStyleOption *opt) const
		{
			return m_style->generatedIconPixmap(iconMode, pixmap, opt);
		}

	protected:
		QStyle *m_style;
};
}

#endif
