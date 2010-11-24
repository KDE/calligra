/* This file is part of the KDE project
   Copyright (C) 2006-2010 Jarosław Staniek <staniek@kde.org>

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

#include <QStyle>
#include <QPointer>
#include <QPixmap>

#include "kexiutils_export.h"

namespace KexiUtils
{

//! @short a QStyle proxy allowing to customizing the currently used style
/*! All you need is to reimplement one or more of the methods.
 For example, you can reimplement drawPrimitive() and temporary
 change the color in color group.

 You can change even the smallest part of the style for a selected widget
 using the following code:
 \code
  class MyStyle : public KexiUtils::StyleProxy {
   //reimplement method(s) here...
  };
  QWidget *w = .....
  MyStyle *s = new MyStyle( w->style() );
  s->setParent( w ); // the style will be owned by w
  w->setStyle( s ); // this will alter w's style a bit

 \endcode

 More info at http://doc.trolltech.com/qq/qq09-q-and-a.html#style
*/
class KEXIUTILS_EXPORT StyleProxy : public QStyle
{
public:
    /*! Creates a new style proxy object. 
     No owner is set for this object, so use QObject::setParent(QObject*) 
     to control the ownership. */
    StyleProxy(QStyle* parentStyle);

    virtual ~StyleProxy();

    QStyle* parentStyle() const;

    void setParentStyle(QStyle* style);

    virtual void polish(QWidget *w) {
        parentStyle(1)->polish(w);
        *m_method = 0;
    }

    virtual void unpolish(QWidget *w) {
        parentStyle(2)->unpolish(w);
        *m_method = 0;
    }

    virtual void polish(QApplication *a) {
        parentStyle(3)->polish(a);
        *m_method = 0;
    }

    virtual void unpolish(QApplication *a) {
        parentStyle(4)->unpolish(a);
        *m_method = 0;
    }

    virtual void polish(QPalette &p) {
        parentStyle(5)->polish(p);
        *m_method = 0;
    }

    virtual QRect itemTextRect(const QFontMetrics &fm, const QRect &r,
                               int flags, bool enabled, const QString &text) const {
        QRect result = parentStyle(6)->itemTextRect(fm, r, flags, enabled, text);
        *m_method = 0;
        return result;
    }

    virtual QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const {
        QRect result = parentStyle(7)->itemPixmapRect(r, flags, pixmap);
        *m_method = 0;
        return result;
    }

    virtual void drawItemText(QPainter *painter, const QRect &rect,
                              int flags, const QPalette &pal, bool enabled,
                              const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const {
        parentStyle(8)->drawItemText(painter, rect, flags, pal, enabled, text, textRole);
        *m_method = 0;
    }

    virtual void drawPrimitive(PrimitiveElement element,
                               const QStyleOption * option, QPainter * painter,
                               const QWidget * widget = 0) const {
        parentStyle(9)->drawPrimitive(element, option, painter, widget);
        *m_method = 0;
    }

    virtual void drawItemPixmap(QPainter *painter, const QRect &rect,
                                int alignment, const QPixmap &pixmap) const {
        parentStyle(10)->drawItemPixmap(painter, rect, alignment, pixmap);
        *m_method = 0;
    }

    virtual QPalette standardPalette() const {
        QPalette result = parentStyle(11)->standardPalette();
        *m_method = 0;
        return result;
    }

    virtual void drawControl(ControlElement element, const QStyleOption *opt,
                             QPainter *p, const QWidget *w = 0) const {
        parentStyle(12)->drawControl(element, opt, p, w);
        *m_method = 0;
    }

    virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
                                 const QWidget *widget = 0) const {
        QRect result = parentStyle(13)->subElementRect(subElement, option, widget);
        *m_method = 0;
        return result;
    }

    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                    QPainter *p, const QWidget *widget = 0) const {
        parentStyle(14)->drawComplexControl(cc, opt, p, widget);
        *m_method = 0;
    }

    virtual SubControl hitTestComplexControl(ComplexControl cc,
            const QStyleOptionComplex *opt, const QPoint &pt, const QWidget *widget = 0) const {
        SubControl result = parentStyle(15)->hitTestComplexControl(cc, opt, pt, widget);
        *m_method = 0;
        return result;
    }

    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                                 SubControl sc, const QWidget *widget = 0) const {
        QRect result = parentStyle(16)->subControlRect(cc, opt, sc, widget);
        *m_method = 0;
        return result;
    }

    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const {
        int result = parentStyle(17)->pixelMetric(metric, option, widget);
        *m_method = 0;
        return result;
    }

    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                                   const QSize &contentsSize, const QWidget *w = 0) const {
        QSize result = parentStyle(18)->sizeFromContents(ct, opt, contentsSize, w);
        *m_method = 0;
        return result;
    }

    virtual int styleHint(StyleHint stylehint, const QStyleOption *opt = 0,
                          const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const {
        int result = parentStyle(19)->styleHint(stylehint, opt, widget, returnData);
        *m_method = 0;
        return result;
    }

    virtual QPixmap standardPixmap(StandardPixmap standardPixmap,
                                   const QStyleOption *opt = 0, const QWidget *widget = 0) const {
        QPixmap result = parentStyle(20)->standardPixmap(standardPixmap, opt, widget);
        *m_method = 0;
        return result;
    }

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = 0,
                       const QWidget *widget = 0) const {
        QIcon result = parentStyle(21)->standardIcon(standardIcon, option, widget);
        *m_method = 0;
        return result;
    }

    virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const {
        QPixmap result = parentStyle(22)->generatedIconPixmap(iconMode, pixmap, opt);
        *m_method = 0;
        return result;
    }

protected:
    //! Helper that returns m_style if not called recursively, otherwise returns QApplication::style()
    //! to avoid infinite loop. *m_method is used to tracke two nested calls to the same method.
    QStyle* parentStyle(int methodIndex) const;

    QPointer<QStyle> m_style;
    int *m_method;
};
}

#endif
