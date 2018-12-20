/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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

#ifndef CALLOUTSHAPE_H
#define CALLOUTSHAPE_H

#include <KoShapeContainer.h>
#include <KoProperties.h>

#include "EnhancedPathShape.h"

#include <QSizeF>

#define CalloutShapeId "CalloutShape"
#define CalloutPathShapeId "CalloutPathShape"

class KoTextShape;
class KoTextShapeDataBase;
class KoPathPoint;

class PathShape;

/**
 * A callout shape reprecents a callout
 */

class CalloutShape : public KoShapeContainer
{
public:
    explicit CalloutShape(const KoProperties *params = nullptr);
    ~CalloutShape() override;

    void setType(const QString &type);
    QString type() const;

    void setTextArea(const QRectF &rect);

    KoTextShapeDataBase *textData() const;

    QRectF boundingRect() const override;

    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    PathShape *pathShape() const { return m_path; }

protected:
    void saveOdf(KoShapeSavingContext &context) const override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    PathShape *m_path;
    QString m_type;
};

class PathShape : public EnhancedPathShape
{
public:
    explicit PathShape(const QRect &viewBox = QRect());
    
    //     void setSize(const QSizeF &size);
    
    //void setTextArea(const QRectF &rect);
    
    KoShape *text() const { return textShape(); }
    KoTextShapeDataBase *textData() const;
    
    KoSubpathList subpaths() const { return m_subpaths; }
    
    void setModifiers(const QList<qreal> &modifiers);
    QList<qreal> modifiers() { return m_modifiers; }
    
    QRectF viewBound() const { return m_viewBound; }
    /// Sets the viewbox of the enhanced path shape
    void setViewBox(const QRect &box);
    void prepare();
    
    QList<qreal> parseModifiers(const QString &m) const;
    void setParameters(const KoProperties &params);
    KoProperties parameters() const;

protected:
//     virtual void shapeChanged(ChangeType type, KoShape *shape = 0);

};

#endif // CALLOUTSHAPE_H
