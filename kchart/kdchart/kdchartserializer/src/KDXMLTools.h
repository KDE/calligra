/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/
#ifndef __KDXMLTOOLS_H__
#define __KDXMLTOOLS_H__

#include <qdom.h>
#include <QBoxLayout>
#include <QPen>
#include <QString>
#include <QColor>
#include <QBrush>
#include <QSize>
#include <QRect>
#include <QFont>
#include <QStringList>
#include <QDateTime>
#include <QModelIndex>

class QVariant;

#include "kdchartserializer_export.h"

namespace KDXML {
    QString KDCHARTSERIALIZER_EXPORT penStyleToString( Qt::PenStyle style );
    QString KDCHARTSERIALIZER_EXPORT penCapStyleToString( Qt::PenCapStyle style );
    QString KDCHARTSERIALIZER_EXPORT penJoinStyleToString( Qt::PenJoinStyle style );

    Qt::PenStyle KDCHARTSERIALIZER_EXPORT stringToPenStyle( const QString& style );
    Qt::PenCapStyle KDCHARTSERIALIZER_EXPORT stringToPenCapStyle( const QString& style );
    Qt::PenJoinStyle KDCHARTSERIALIZER_EXPORT stringToPenJoinStyle( const QString& style );
    QString KDCHARTSERIALIZER_EXPORT brushStyleToString( Qt::BrushStyle style );
    Qt::BrushStyle KDCHARTSERIALIZER_EXPORT stringToBrushStyle( const QString& style );

    void KDCHARTSERIALIZER_EXPORT setBoolAttribute( QDomElement& element,
                                                    const QString& attrName, bool value );
    void KDCHARTSERIALIZER_EXPORT setDoubleAttribute( QDomElement& element,
                                                      const QString& attrName, double value );

    void KDCHARTSERIALIZER_EXPORT createNodeWithAttribute( QDomDocument& doc, QDomNode& parent,
                                                           const QString& elementName,
                                                           const QString& attrName,
                                                           const QString& attrText );

    void KDCHARTSERIALIZER_EXPORT createBoolNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, bool value );
    void KDCHARTSERIALIZER_EXPORT createOrientationNode( QDomDocument& doc, QDomNode& parent,
                                                         const QString& elementName, Qt::Orientation value );
    void KDCHARTSERIALIZER_EXPORT createAlignmentNode( QDomDocument& doc, QDomNode& parent,
                                                       const QString& elementName, Qt::Alignment value );
    void KDCHARTSERIALIZER_EXPORT createBoxLayoutDirectionNode( QDomDocument& doc, QDomNode& parent,
                                                                const QString& elementName, QBoxLayout::Direction value );
    void KDCHARTSERIALIZER_EXPORT createSizeNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, const QSize& value );
    void KDCHARTSERIALIZER_EXPORT createSizeFNode( QDomDocument& doc, QDomNode& parent,
                                                   const QString& elementName, const QSizeF& value );
    void KDCHARTSERIALIZER_EXPORT createIntNode( QDomDocument& doc, QDomNode& parent,
                                                 const QString& elementName, int value );
    void KDCHARTSERIALIZER_EXPORT createDoubleNode( QDomDocument& doc, QDomNode& parent,
                                                    const QString& elementName, double value );
    void KDCHARTSERIALIZER_EXPORT createRealNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, qreal value );
    void KDCHARTSERIALIZER_EXPORT createStringNode( QDomDocument& doc, QDomNode& parent,
                                                    const QString& elementName,
                                                    const QString& text );
    void KDCHARTSERIALIZER_EXPORT createStringNodeIfContent( QDomDocument& doc, QDomNode& parent,
                                                             const QString& elementName,
                                                             const QString& text );
    void KDCHARTSERIALIZER_EXPORT createPointFNode( QDomDocument& doc, QDomNode& parent,
                                                    const QString& elementName,
                                                    const QPointF& point );
    void KDCHARTSERIALIZER_EXPORT createColorNode( QDomDocument& doc, QDomNode& parent,
                                                   const QString& elementName, const QColor& color );
    void KDCHARTSERIALIZER_EXPORT createBrushNode( QDomDocument& doc, QDomNode& parent,
                                                   const QString& elementName, const QBrush& brush );
    void KDCHARTSERIALIZER_EXPORT createPixmapNode( QDomDocument& doc, QDomNode& parent,
                                                    const QString& elementName, const QPixmap& pixmap );
    void KDCHARTSERIALIZER_EXPORT createRectNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, const QRect& rect );
    void KDCHARTSERIALIZER_EXPORT createStringListNodes( QDomDocument& doc, QDomNode& parent,
                                                         const QString& elementName,
                                                         const QStringList* thelist ); // PCH: This is required to avoid MSVC compiler warnings and errors.
    void KDCHARTSERIALIZER_EXPORT createFontNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, const QFont& font );

    void KDCHARTSERIALIZER_EXPORT createPenNode( QDomDocument& doc, QDomNode& parent,
                                                 const QString& elementName, const QPen& pen );
    void KDCHARTSERIALIZER_EXPORT createDateTimeNode( QDomDocument& doc, QDomNode& parent,
                                                      const QString& elementName,
                                                      const QDateTime& datetime );
    void KDCHARTSERIALIZER_EXPORT createDateNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, const QDate& date );
    void KDCHARTSERIALIZER_EXPORT createTimeNode( QDomDocument& doc, QDomNode& parent,
                                                  const QString& elementName, const QTime& time );
    void KDCHARTSERIALIZER_EXPORT createModelIndexNode( QDomDocument& doc, QDomNode& parent,
                                                        const QString& elementName, const QModelIndex& idx );

    void KDCHARTSERIALIZER_EXPORT createRealPairNode( QDomDocument& doc, QDomNode& parent,
                                                      const QString& elementName, const QPair<qreal, qreal> & pair );

    void KDCHARTSERIALIZER_EXPORT createPositionBooleansNode(
        QDomDocument& doc, QDomNode& parent, const QString& elementName,
        bool unknown, bool center,
        bool northWest, bool north, bool northEast,
        bool east, bool southEast, bool south, bool southWest, bool west,
        bool floating );

    void KDCHARTSERIALIZER_EXPORT createQVariantNode(
        QDomDocument& doc, QDomNode& parent, const QString& name, const QVariant& );

    bool KDCHARTSERIALIZER_EXPORT findStringAttribute( const QDomElement& e, const QString & name, QString& attr );
    bool KDCHARTSERIALIZER_EXPORT findIntAttribute(    const QDomElement& e, const QString & name, int&  attr );
    bool KDCHARTSERIALIZER_EXPORT findDoubleAttribute( const QDomElement& e, const QString & name, double& attr );
    bool KDCHARTSERIALIZER_EXPORT findBoolAttribute(   const QDomElement& e, const QString & name, bool& attr );

    bool KDCHARTSERIALIZER_EXPORT readIntNode( const QDomElement& element, int& value );
    bool KDCHARTSERIALIZER_EXPORT readStringNode( const QDomElement& element, QString& value );
    bool KDCHARTSERIALIZER_EXPORT readStringListNode( const QDomElement& element, QStringList& value );
    bool KDCHARTSERIALIZER_EXPORT readDoubleNode( const QDomElement& element, double& value );
    bool KDCHARTSERIALIZER_EXPORT readRealNode( const QDomElement& element, qreal& value );
    bool KDCHARTSERIALIZER_EXPORT readBoolNode( const QDomElement& element, bool& value );
    bool KDCHARTSERIALIZER_EXPORT readOrientationNode( const QDomElement& element, Qt::Orientation& value );
    bool KDCHARTSERIALIZER_EXPORT readAlignmentNode(const QDomElement& element, Qt::Alignment& value );
    bool KDCHARTSERIALIZER_EXPORT readBoxLayoutDirectionNode(const QDomElement& element,
                                                             QBoxLayout::Direction& value );
    bool KDCHARTSERIALIZER_EXPORT readSizeNode( const QDomElement& element, QSize& value );
    bool KDCHARTSERIALIZER_EXPORT readSizeFNode( const QDomElement& element, QSizeF& value );
    bool KDCHARTSERIALIZER_EXPORT readPointFNode( const QDomElement& element, QPointF& value );
    bool KDCHARTSERIALIZER_EXPORT readColorNode( const QDomElement& element, QColor& value );
    bool KDCHARTSERIALIZER_EXPORT readBrushNode( const QDomElement& element, QBrush& brush );
    bool KDCHARTSERIALIZER_EXPORT readPixmapNode( const QDomElement& element, QPixmap& pixmap );
    bool KDCHARTSERIALIZER_EXPORT readRectNode( const QDomElement& element, QRect& value );
    bool KDCHARTSERIALIZER_EXPORT readFontNode( const QDomElement& element, QFont& font );
    bool KDCHARTSERIALIZER_EXPORT readPenNode( const QDomElement& element, QPen& pen );
    bool KDCHARTSERIALIZER_EXPORT readDateTimeNode( const QDomElement& element, QDateTime& datetime );
    bool KDCHARTSERIALIZER_EXPORT readDateNode( const QDomElement& element, QDate& date );
    bool KDCHARTSERIALIZER_EXPORT readTimeNode( const QDomElement& element, QTime& time );
    bool KDCHARTSERIALIZER_EXPORT readRealPairNode( const QDomElement& element, QPair<qreal, qreal> & pair );
    bool KDCHARTSERIALIZER_EXPORT readQVariantNode( const QDomElement& element, QVariant& v, QString& name );
    bool KDCHARTSERIALIZER_EXPORT readPositionBooleansNode(
        const QDomElement& element,
        bool& unknown, bool& center,
        bool& northWest, bool& north, bool& northEast,
        bool& east, bool& southEast, bool& south, bool& southWest, bool& west,
        bool& floating );
    bool KDCHARTSERIALIZER_EXPORT readModelIndexNode(const QDomElement& element,
                                                     const QAbstractItemModel& model,
                                                     QModelIndex& idx );
    bool KDCHARTSERIALIZER_EXPORT readModelIndexNode(const QDomElement& element,
                                                     bool& isValid,
                                                     int& column,
                                                     int& row );
}
#endif
