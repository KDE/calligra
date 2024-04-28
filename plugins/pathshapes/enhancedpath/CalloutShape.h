/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLOUTSHAPE_H
#define CALLOUTSHAPE_H

#include <KoProperties.h>
#include <KoShapeContainer.h>

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

    PathShape *pathShape() const
    {
        return m_path;
    }

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

    // void setTextArea(const QRectF &rect);

    KoShape *text() const
    {
        return textShape();
    }
    KoTextShapeDataBase *textData() const;

    KoSubpathList subpaths() const
    {
        return m_subpaths;
    }

    void setModifiers(const QList<qreal> &modifiers);
    QList<qreal> modifiers()
    {
        return m_modifiers;
    }

    QRectF viewBound() const
    {
        return m_viewBound;
    }
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
