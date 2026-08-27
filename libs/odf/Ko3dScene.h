/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO3DSCENE_H
#define KO3DSCENE_H

// Qt
#include <QColor>
#include <QVector3D>
#include <QVector>

class KoXmlWriter;
class KoXmlElement;

/** A scene in which to show 3d objects.
 *
 * The scene parameters include camera parameters (origin, direction
 * and up direction), the projection to be used and a shadow
 * slant. All these are attributes of the element.
 *
 * The scene can also have a number of child elements: light sources
 * (dr3d:light), 3d primitive shapes (dr3d:cube, dr3d:sphere,
 * dr3d:extrude, dr3d:rotate) and nested scenes (dr3d:scene), covering
 * the full dr3d module as specified by ODF (unchanged from 1.0
 * through 1.4).
 *
 * A scene description can be part of a dr3d:scene or chart:plot-area
 * if the chart also has 3D mode set.
 */

#include "koodf_export.h"

class KOODF_EXPORT Ko3dScene
{
public:
    enum Projection {
        Parallel,
        Perspective
    };

    enum Shademode {
        Flat,
        Gouraud,
        Phong,
        Draft // Wireframe
    };

    class Lightsource
    {
    public:
        Lightsource();
        ~Lightsource();

        bool loadOdf(const KoXmlElement &lightElement);
        void saveOdf(KoXmlWriter &writer) const;

        // getters
        QColor diffuseColor() const;
        QVector3D direction() const;
        bool enabled() const;
        bool specular() const;

        // setters
        void setDiffuseColor(const QColor &color);
        void setDirection(const QVector3D &direction);
        void setEnabled(const bool enabled);
        void setSpecular(const bool specular);

    private:
        QColor m_diffuseColor;
        QVector3D m_direction;
        bool m_enabled;
        bool m_specular;
    };

    /** A dr3d:cube 3d primitive shape. */
    class Cube
    {
    public:
        Cube();
        ~Cube();

        bool loadOdf(const KoXmlElement &element);
        void saveOdf(KoXmlWriter &writer) const;

        QVector3D minEdge() const;
        QVector3D maxEdge() const;
        QString transform() const;

        void setMinEdge(const QVector3D &minEdge);
        void setMaxEdge(const QVector3D &maxEdge);
        void setTransform(const QString &transform);

    private:
        QVector3D m_minEdge;
        QVector3D m_maxEdge;
        QString m_transform;
    };

    /** A dr3d:sphere 3d primitive shape. */
    class Sphere
    {
    public:
        Sphere();
        ~Sphere();

        bool loadOdf(const KoXmlElement &element);
        void saveOdf(KoXmlWriter &writer) const;

        QVector3D center() const;
        QVector3D size() const;
        QString transform() const;

        void setCenter(const QVector3D &center);
        void setSize(const QVector3D &size);
        void setTransform(const QString &transform);

    private:
        QVector3D m_center;
        QVector3D m_size;
        QString m_transform;
    };

    /** A dr3d:extrude 3d primitive shape: a 2d path extruded along the z axis. */
    class Extrude
    {
    public:
        Extrude();
        ~Extrude();

        bool loadOdf(const KoXmlElement &element);
        void saveOdf(KoXmlWriter &writer) const;

        QString viewBox() const;
        QString path() const;
        QString transform() const;

        void setViewBox(const QString &viewBox);
        void setPath(const QString &path);
        void setTransform(const QString &transform);

    private:
        QString m_viewBox;
        QString m_path;
        QString m_transform;
    };

    /** A dr3d:rotate 3d primitive shape: a 2d path rotated around the y axis. */
    class Rotate
    {
    public:
        Rotate();
        ~Rotate();

        bool loadOdf(const KoXmlElement &element);
        void saveOdf(KoXmlWriter &writer) const;

        QString viewBox() const;
        QString path() const;
        QString transform() const;

        void setViewBox(const QString &viewBox);
        void setPath(const QString &path);
        void setTransform(const QString &transform);

    private:
        QString m_viewBox;
        QString m_path;
        QString m_transform;
    };

    Ko3dScene();
    ~Ko3dScene();

    bool loadOdf(const KoXmlElement &sceneElement);
    void saveOdfAttributes(KoXmlWriter &writer) const;
    void saveOdfChildren(KoXmlWriter &writer) const;

    // getters
    QVector3D vrp() const;
    QVector3D vpn() const;
    QVector3D vup() const;
    Projection projection() const;
    QString distance() const;
    QString focalLength() const;
    QString shadowSlant() const;
    Shademode shadeMode() const;
    QColor ambientColor() const;
    bool lightingMode() const;
    QString transform() const;

    // setters
    void setVrp(const QVector3D &vrp);
    void setVpn(const QVector3D &vpn);
    void setVup(const QVector3D &vup);
    void setProjection(Projection projection);
    void setDistance(const QString &distance);
    void setFocalLength(const QString &focalLength);
    void setShadowSlant(const QString &shadowSlant);
    void setShadeMode(Shademode shadeMode);
    void setAmbientColor(const QColor &ambientColor);
    void setLightingMode(bool lightingMode);
    void setTransform(const QString &transform);

    // child light sources
    QVector<Lightsource> lights() const;
    void addLight(const Lightsource &light);

    // child 3d primitive shapes
    QVector<Cube> cubes() const;
    void addCube(const Cube &cube);
    QVector<Sphere> spheres() const;
    void addSphere(const Sphere &sphere);
    QVector<Extrude> extrudes() const;
    void addExtrude(const Extrude &extrude);
    QVector<Rotate> rotates() const;
    void addRotate(const Rotate &rotate);

    /** Nested (child) scenes. Ko3dScene takes ownership of scenes added with addChildScene(). */
    QVector<Ko3dScene *> childScenes() const;
    void addChildScene(Ko3dScene *scene);

private:
    class Private;
    Private *const d;
};

Q_DECLARE_TYPEINFO(Ko3dScene::Lightsource, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Ko3dScene::Cube, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Ko3dScene::Sphere, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Ko3dScene::Extrude, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Ko3dScene::Rotate, Q_MOVABLE_TYPE);

/** Try to load a 3d scene from an element and return a pointer to a
 * Ko3dScene if it succeeded.
 */
KOODF_EXPORT Ko3dScene *load3dScene(const KoXmlElement &element);

#endif
