/* -*- Mode: C++ -*-
   $Id$
   KDGantt - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/

#ifndef __KDGANTTXMLTOOLS_H__
#define __KDGANTTXMLTOOLS_H__

#include <qpen.h>
#include <qdom.h>
#include <qstring.h>
#include <qcolor.h>
#include <qrect.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qdatetime.h>

namespace KDGanttXML {
    QString penStyleToString( Qt::PenStyle style );
    Qt::PenStyle stringToPenStyle( const QString& style );
    QString brushStyleToString( Qt::BrushStyle style );
    Qt::BrushStyle stringToBrushStyle( const QString& style );

    void createBoolNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, bool value );
    void createSizeNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QSize& value );
    void createIntNode( QDomDocument& doc, QDomNode& parent,
                        const QString& elementName, int value );
    void createDoubleNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName, double value );
    void createStringNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName,
                           const QString& text );
    void createColorNode( QDomDocument& doc, QDomNode& parent,
                          const QString& elementName, const QColor& color );
    void createBrushNode( QDomDocument& doc, QDomNode& parent,
                          const QString& elementName, const QBrush& brush );
    void createPixmapNode( QDomDocument& doc, QDomNode& parent,
                           const QString& elementName, const QPixmap& pixmap );
    void createRectNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QRect& rect );
    void createStringListNodes( QDomDocument& doc, QDomNode& parent,
                                const QString& elementName,
                                const QStringList* list );
    void createFontNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QFont& font );

    void createPenNode( QDomDocument& doc, QDomNode& parent,
                        const QString& elementName, const QPen& pen );
    void createDateTimeNode( QDomDocument& doc, QDomNode& parent,
                             const QString& elementName,
                             const QDateTime& datetime );
    void createDateNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QDate& date );
    void createTimeNode( QDomDocument& doc, QDomNode& parent,
                         const QString& elementName, const QTime& time );
    bool readIntNode( const QDomElement& element, int& value );
    bool readStringNode( const QDomElement& element, QString& value );
    bool readDoubleNode( const QDomElement& element, double& value );
    bool readBoolNode( const QDomElement& element, bool& value );
    bool readColorNode( const QDomElement& element, QColor& value );
    bool readBrushNode( const QDomElement& element, QBrush& brush );
    bool readPixmapNode( const QDomElement& element, QPixmap& pixmap );
    bool readRectNode( const QDomElement& element, QRect& value );
    bool readFontNode( const QDomElement& element, QFont& font );
    bool readPenNode( const QDomElement& element, QPen& pen );
    bool readDateTimeNode( const QDomElement& element, QDateTime& datetime );
    bool readDateNode( const QDomElement& element, QDate& date );
    bool readTimeNode( const QDomElement& element, QTime& time );
}
#endif
