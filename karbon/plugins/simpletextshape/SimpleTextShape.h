/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SIMPLETEXTSHAPE_H
#define SIMPLETEXTSHAPE_H

#include <KoShape.h>

#include <QtGui/QFont>

class QPainter;
class KoPathShape;

#define SimpleTextShapeID "SimpleText"

class SimpleTextShape : public KoShape
{
public:
    SimpleTextShape();
    virtual ~SimpleTextShape();

    /// reimplemented to be empty (this shape is fully non-printing)
    void paint(QPainter &painter, const KoViewConverter &converter);
    /// reimplemented
    void paintDecorations(QPainter &painter, const KoViewConverter &converter, const KoCanvasBase *canvas);
    /// reimplemented
    virtual void saveOdf(KoShapeSavingContext & context) const;
    /// reimplemented
    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context );
    /// reimplemented
    virtual QSizeF size() const;
    /// reimplemented
    virtual void setSize( const QSizeF &size );
    /// reimplemented
    virtual const QPainterPath outline() const;

    /// Sets the text to display
    void setText( const QString & text );

    /// Returns the text content
    QString text() const;

    /// Sets the font used for drawing
    void setFont( const QFont & font );

    /// Returns the font
    QFont font() const;

    /// Attaches this text shape to the given path shape
    bool attach( KoPathShape * path );

    /// Detaches this text shape from an already attached path shape
    void detach();

    /// Returns if shape is attached to a path shape
    bool isAttached() const;

private:
    void updateSizeAndPosition();
    void cacheOutlines();
    bool pathHasChanged() const;

    QPainterPath textOutline() const;
    QString m_text; ///< the text content
    QFont m_font; ///< the font to use for drawing
    KoPathShape * m_path; ///< the path shape we are attached to
    QList<QPainterPath> m_charOutlines; ///< cached character oulines
};

#endif // SIMPLETEXTSHAPE_H
