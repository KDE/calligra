/* -*- Mode: C++ -*-

  $Id$

  KDXMLTools - reading and writing Qt-based structures from and to XML files

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDXMLTOOLS_H__
#define __KDXMLTOOLS_H__

#include <qpen.h>
#include <qdom.h>
#include <qstring.h>
#include <qcolor.h>
#include <qrect.h>
#include <qfont.h>
#include <qstringlist.h>

namespace KDXML {
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
};
#endif
