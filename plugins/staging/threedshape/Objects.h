/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef OBJECTS_H
#define OBJECTS_H

// Qt
#include <QVector3D>

// Calligra

// Shape
#include "Object3D.h"

class KoXmlElement;

class Sphere : public Object3D, public KoShape
{
public:
    explicit Sphere(Object3D *parent);
    ~Sphere() override;

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context) override;
    bool loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    // Really save the object.  See the explanation in Object3D.h.
    void saveObjectOdf(KoShapeSavingContext &context) const override;

    // getters
    QVector3D sphereCenter() const
    {
        return m_center;
    }
    QVector3D sphereSize() const
    {
        return m_size;
    }

private:
    QVector3D m_center;
    QVector3D m_size;
};

class Cube : public Object3D, public KoShape
{
public:
    explicit Cube(Object3D *parent);
    ~Cube() override;

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context) override;
    bool loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    // Really save the object.  See the explanation in Object3D.h.
    void saveObjectOdf(KoShapeSavingContext &context) const override;

    // getters
    QVector3D minEdge() const
    {
        return m_minEdge;
    }
    QVector3D maxEdge() const
    {
        return m_maxEdge;
    }

private:
    QVector3D m_minEdge;
    QVector3D m_maxEdge;
};

class Extrude : public Object3D, public KoShape
{
public:
    explicit Extrude(Object3D *parent);
    ~Extrude() override;

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context) override;
    bool loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    void loadStyle(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    QString saveStyle(KoGenStyle &style, KoShapeSavingContext &context) const override;

    // Really save the object.  See the explanation in Object3D.h.
    void saveObjectOdf(KoShapeSavingContext &context) const override;

    // getters
    QString path() const
    {
        return m_path;
    }
    QString viewBox() const
    {
        return m_viewBox;
    }
    int depth() const
    {
        return m_depth;
    }
    bool closeFront() const
    {
        return m_closeFront;
    }
    bool closeBack() const
    {
        return m_closeBack;
    }
    qreal backScale() const
    {
        return m_backScale;
    }

private:
    QString m_path; // The polygon
    QString m_viewBox; // Defines the coordinate system for svg:d
    qreal m_depth;
    bool m_closeFront;
    bool m_closeBack;
    qreal m_backScale;
};

class Rotate : public Object3D, public KoShape
{
public:
    explicit Rotate(Object3D *parent);
    ~Rotate() override;

    // reimplemented from KoShape
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context) override;
    bool loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    void loadStyle(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    QString saveStyle(KoGenStyle &style, KoShapeSavingContext &context) const override;

    // Really save the object.  See the explanation in Object3D.h.
    void saveObjectOdf(KoShapeSavingContext &context) const override;

    // getters
    QString path() const
    {
        return m_path;
    }
    QString viewBox() const
    {
        return m_viewBox;
    }
    int horizontalSegments() const
    {
        return m_horizontalSegments;
    }
    int verticalSegments() const
    {
        return m_verticalSegments;
    }
    qreal endAngle() const
    {
        return m_endAngle;
    }
    bool closeFront() const
    {
        return m_closeFront;
    }
    bool closeBack() const
    {
        return m_closeBack;
    }
    qreal backScale() const
    {
        return m_backScale;
    }

private:
    QString m_path; // The polygon
    QString m_viewBox; // Defines the coordinate system for svg:d
    int m_horizontalSegments; // Defined in ODF
    int m_verticalSegments; // Produced by OOo(!)
    qreal m_endAngle;
    bool m_closeFront;
    bool m_closeBack;
    qreal m_backScale;
};

#endif
