/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "Ko3dScene.h"

#include "OdfDebug.h"

// Calligra
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

using namespace Qt::StringLiterals;

static QVector3D odfToVector3D(const QString &string);

// ----------------------------------------------------------------
//                         Ko3dScene::Private

class Q_DECL_HIDDEN Ko3dScene::Private
{
public:
    Private() = default;
    ~Private()
    {
        qDeleteAll(childScenes);
    }

    // Camera attributes
    QVector3D vrp; // Camera origin
    QVector3D vpn; // Camera direction
    QVector3D vup; // Up direction
    Projection projection;
    QString distance;
    QString focalLength;
    QString shadowSlant;

    // Rendering attributes
    Shademode shadeMode;
    QColor ambientColor;
    bool lightingMode; // True: enable lights, false: disable lights
    QString transform;

    // Children of the element.
    QVector<Lightsource> lights;
    QVector<Cube> cubes;
    QVector<Sphere> spheres;
    QVector<Extrude> extrudes;
    QVector<Rotate> rotates;
    QVector<Ko3dScene *> childScenes; // owned
};

// ----------------------------------------------------------------
//                         Lightsource

Ko3dScene::Lightsource::Lightsource() = default;

Ko3dScene::Lightsource::~Lightsource() = default;

// getters
QColor Ko3dScene::Lightsource::diffuseColor() const
{
    return m_diffuseColor;
}
QVector3D Ko3dScene::Lightsource::direction() const
{
    return m_direction;
}
bool Ko3dScene::Lightsource::enabled() const
{
    return m_enabled;
}
bool Ko3dScene::Lightsource::specular() const
{
    return m_specular;
}

// setters
void Ko3dScene::Lightsource::setDiffuseColor(const QColor &color)
{
    m_diffuseColor = color;
}

void Ko3dScene::Lightsource::setDirection(const QVector3D &direction)
{
    m_direction = direction;
}

void Ko3dScene::Lightsource::setEnabled(const bool enabled)
{
    m_enabled = enabled;
}

void Ko3dScene::Lightsource::setSpecular(const bool specular)
{
    m_specular = specular;
}

bool Ko3dScene::Lightsource::loadOdf(const KoXmlElement &lightElement)
{
    m_diffuseColor = QColor(lightElement.attributeNS(KoXmlNS::dr3d, "diffuse-color", "#ffffff"));
    QString direction = lightElement.attributeNS(KoXmlNS::dr3d, "direction");
    m_direction = odfToVector3D(direction);
    m_enabled = (lightElement.attributeNS(KoXmlNS::dr3d, "enabled") == "true"_L1);
    m_specular = (lightElement.attributeNS(KoXmlNS::dr3d, "specular") == "true"_L1);

    return true;
}

void Ko3dScene::Lightsource::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:light");

    writer.addAttribute("dr3d:diffuse-color", m_diffuseColor.name());
    writer.addAttribute("dr3d:direction",
                        (QString("(%1 %2 %3)").arg(m_direction.x(), 0, 'f', 11).arg(m_direction.y(), 0, 'f', 11).arg(m_direction.z(), 0, 'f', 11)));
    writer.addAttribute("dr3d:enabled", m_enabled);
    writer.addAttribute("dr3d:specular", m_specular);

    writer.endElement(); // dr3d:light
}

// ----------------------------------------------------------------
//                         Cube

Ko3dScene::Cube::Cube() = default;

Ko3dScene::Cube::~Cube() = default;

QVector3D Ko3dScene::Cube::minEdge() const
{
    return m_minEdge;
}
QVector3D Ko3dScene::Cube::maxEdge() const
{
    return m_maxEdge;
}
QString Ko3dScene::Cube::transform() const
{
    return m_transform;
}

void Ko3dScene::Cube::setMinEdge(const QVector3D &minEdge)
{
    m_minEdge = minEdge;
}
void Ko3dScene::Cube::setMaxEdge(const QVector3D &maxEdge)
{
    m_maxEdge = maxEdge;
}
void Ko3dScene::Cube::setTransform(const QString &transform)
{
    m_transform = transform;
}

bool Ko3dScene::Cube::loadOdf(const KoXmlElement &element)
{
    m_minEdge = odfToVector3D(element.attributeNS(KoXmlNS::dr3d, "min-edge", "(0 0 0)"));
    m_maxEdge = odfToVector3D(element.attributeNS(KoXmlNS::dr3d, "max-edge", "(1 1 1)"));
    m_transform = element.attributeNS(KoXmlNS::dr3d, "transform");

    return true;
}

void Ko3dScene::Cube::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:cube");

    writer.addAttribute("dr3d:min-edge", (QString("(%1 %2 %3)").arg(m_minEdge.x(), 0, 'f', 11).arg(m_minEdge.y(), 0, 'f', 11).arg(m_minEdge.z(), 0, 'f', 11)));
    writer.addAttribute("dr3d:max-edge", (QString("(%1 %2 %3)").arg(m_maxEdge.x(), 0, 'f', 11).arg(m_maxEdge.y(), 0, 'f', 11).arg(m_maxEdge.z(), 0, 'f', 11)));
    if (!m_transform.isEmpty()) {
        writer.addAttribute("dr3d:transform", m_transform);
    }

    writer.endElement(); // dr3d:cube
}

// ----------------------------------------------------------------
//                         Sphere

Ko3dScene::Sphere::Sphere() = default;

Ko3dScene::Sphere::~Sphere() = default;

QVector3D Ko3dScene::Sphere::center() const
{
    return m_center;
}
QVector3D Ko3dScene::Sphere::size() const
{
    return m_size;
}
QString Ko3dScene::Sphere::transform() const
{
    return m_transform;
}

void Ko3dScene::Sphere::setCenter(const QVector3D &center)
{
    m_center = center;
}
void Ko3dScene::Sphere::setSize(const QVector3D &size)
{
    m_size = size;
}
void Ko3dScene::Sphere::setTransform(const QString &transform)
{
    m_transform = transform;
}

bool Ko3dScene::Sphere::loadOdf(const KoXmlElement &element)
{
    m_center = odfToVector3D(element.attributeNS(KoXmlNS::dr3d, "center", "(0 0 0)"));
    m_size = odfToVector3D(element.attributeNS(KoXmlNS::dr3d, "size", "(1 1 1)"));
    m_transform = element.attributeNS(KoXmlNS::dr3d, "transform");

    return true;
}

void Ko3dScene::Sphere::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:sphere");

    writer.addAttribute("dr3d:center", (QString("(%1 %2 %3)").arg(m_center.x(), 0, 'f', 11).arg(m_center.y(), 0, 'f', 11).arg(m_center.z(), 0, 'f', 11)));
    writer.addAttribute("dr3d:size", (QString("(%1 %2 %3)").arg(m_size.x(), 0, 'f', 11).arg(m_size.y(), 0, 'f', 11).arg(m_size.z(), 0, 'f', 11)));
    if (!m_transform.isEmpty()) {
        writer.addAttribute("dr3d:transform", m_transform);
    }

    writer.endElement(); // dr3d:sphere
}

// ----------------------------------------------------------------
//                         Extrude

Ko3dScene::Extrude::Extrude() = default;

Ko3dScene::Extrude::~Extrude() = default;

QString Ko3dScene::Extrude::viewBox() const
{
    return m_viewBox;
}
QString Ko3dScene::Extrude::path() const
{
    return m_path;
}
QString Ko3dScene::Extrude::transform() const
{
    return m_transform;
}

void Ko3dScene::Extrude::setViewBox(const QString &viewBox)
{
    m_viewBox = viewBox;
}
void Ko3dScene::Extrude::setPath(const QString &path)
{
    m_path = path;
}
void Ko3dScene::Extrude::setTransform(const QString &transform)
{
    m_transform = transform;
}

bool Ko3dScene::Extrude::loadOdf(const KoXmlElement &element)
{
    m_viewBox = element.attributeNS(KoXmlNS::svg, "viewBox");
    m_path = element.attributeNS(KoXmlNS::svg, "d");
    m_transform = element.attributeNS(KoXmlNS::dr3d, "transform");

    return true;
}

void Ko3dScene::Extrude::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:extrude");

    writer.addAttribute("svg:viewBox", m_viewBox);
    writer.addAttribute("svg:d", m_path);
    if (!m_transform.isEmpty()) {
        writer.addAttribute("dr3d:transform", m_transform);
    }

    writer.endElement(); // dr3d:extrude
}

// ----------------------------------------------------------------
//                         Rotate

Ko3dScene::Rotate::Rotate() = default;

Ko3dScene::Rotate::~Rotate() = default;

QString Ko3dScene::Rotate::viewBox() const
{
    return m_viewBox;
}
QString Ko3dScene::Rotate::path() const
{
    return m_path;
}
QString Ko3dScene::Rotate::transform() const
{
    return m_transform;
}

void Ko3dScene::Rotate::setViewBox(const QString &viewBox)
{
    m_viewBox = viewBox;
}
void Ko3dScene::Rotate::setPath(const QString &path)
{
    m_path = path;
}
void Ko3dScene::Rotate::setTransform(const QString &transform)
{
    m_transform = transform;
}

bool Ko3dScene::Rotate::loadOdf(const KoXmlElement &element)
{
    m_viewBox = element.attributeNS(KoXmlNS::svg, "viewBox");
    m_path = element.attributeNS(KoXmlNS::svg, "d");
    m_transform = element.attributeNS(KoXmlNS::dr3d, "transform");

    return true;
}

void Ko3dScene::Rotate::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:rotate");

    writer.addAttribute("svg:viewBox", m_viewBox);
    writer.addAttribute("svg:d", m_path);
    if (!m_transform.isEmpty()) {
        writer.addAttribute("dr3d:transform", m_transform);
    }

    writer.endElement(); // dr3d:rotate
}

// ----------------------------------------------------------------
//                         Ko3dScene

Ko3dScene::Ko3dScene()
    : d(new Private())
{
}

Ko3dScene::~Ko3dScene()
{
    delete d;
}

// getters
QVector3D Ko3dScene::vrp() const
{
    return d->vrp;
}
QVector3D Ko3dScene::vpn() const
{
    return d->vpn;
}
QVector3D Ko3dScene::vup() const
{
    return d->vup;
}
Ko3dScene::Projection Ko3dScene::projection() const
{
    return d->projection;
}
QString Ko3dScene::distance() const
{
    return d->distance;
}
QString Ko3dScene::focalLength() const
{
    return d->focalLength;
}
QString Ko3dScene::shadowSlant() const
{
    return d->shadowSlant;
}
Ko3dScene::Shademode Ko3dScene::shadeMode() const
{
    return d->shadeMode;
}
QColor Ko3dScene::ambientColor() const
{
    return d->ambientColor;
}
bool Ko3dScene::lightingMode() const
{
    return d->lightingMode;
}
QString Ko3dScene::transform() const
{
    return d->transform;
}

// setters
void Ko3dScene::setVrp(const QVector3D &vrp)
{
    d->vrp = vrp;
}
void Ko3dScene::setVpn(const QVector3D &vpn)
{
    d->vpn = vpn;
}
void Ko3dScene::setVup(const QVector3D &vup)
{
    d->vup = vup;
}
void Ko3dScene::setProjection(Projection projection)
{
    d->projection = projection;
}
void Ko3dScene::setDistance(const QString &distance)
{
    d->distance = distance;
}
void Ko3dScene::setFocalLength(const QString &focalLength)
{
    d->focalLength = focalLength;
}
void Ko3dScene::setShadowSlant(const QString &shadowSlant)
{
    d->shadowSlant = shadowSlant;
}
void Ko3dScene::setShadeMode(Shademode shadeMode)
{
    d->shadeMode = shadeMode;
}
void Ko3dScene::setAmbientColor(const QColor &ambientColor)
{
    d->ambientColor = ambientColor;
}
void Ko3dScene::setLightingMode(bool lightingMode)
{
    d->lightingMode = lightingMode;
}
void Ko3dScene::setTransform(const QString &transform)
{
    d->transform = transform;
}

// children
QVector<Ko3dScene::Lightsource> Ko3dScene::lights() const
{
    return d->lights;
}
void Ko3dScene::addLight(const Lightsource &light)
{
    d->lights.append(light);
}

QVector<Ko3dScene::Cube> Ko3dScene::cubes() const
{
    return d->cubes;
}
void Ko3dScene::addCube(const Cube &cube)
{
    d->cubes.append(cube);
}

QVector<Ko3dScene::Sphere> Ko3dScene::spheres() const
{
    return d->spheres;
}
void Ko3dScene::addSphere(const Sphere &sphere)
{
    d->spheres.append(sphere);
}

QVector<Ko3dScene::Extrude> Ko3dScene::extrudes() const
{
    return d->extrudes;
}
void Ko3dScene::addExtrude(const Extrude &extrude)
{
    d->extrudes.append(extrude);
}

QVector<Ko3dScene::Rotate> Ko3dScene::rotates() const
{
    return d->rotates;
}
void Ko3dScene::addRotate(const Rotate &rotate)
{
    d->rotates.append(rotate);
}

QVector<Ko3dScene *> Ko3dScene::childScenes() const
{
    return d->childScenes;
}
void Ko3dScene::addChildScene(Ko3dScene *scene)
{
    d->childScenes.append(scene);
}

bool Ko3dScene::loadOdf(const KoXmlElement &sceneElement)
{
    QString dummy;

    // Check if there is a 3d scene at all in this element. We
    // approximate that by checking if there are any camera parameters.
    if (!sceneElement.hasAttributeNS(KoXmlNS::dr3d, "vrp") && !sceneElement.hasAttributeNS(KoXmlNS::dr3d, "vpn")
        && !sceneElement.hasAttributeNS(KoXmlNS::dr3d, "vup")) {
        return false;
    }

    // 1. Load the scene attributes.

    // Camera attributes
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vrp");
    d->vrp = odfToVector3D(dummy);
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vpn");
    d->vpn = odfToVector3D(dummy);
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vup", "(0.0 0.0 1.0)");
    d->vup = odfToVector3D(dummy);

    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "projection", "perspective");
    if (dummy == "parallel"_L1) {
        d->projection = Parallel;
    } else {
        d->projection = Perspective;
    }

    d->distance = sceneElement.attributeNS(KoXmlNS::dr3d, "distance");
    d->focalLength = sceneElement.attributeNS(KoXmlNS::dr3d, "focal-length");
    d->shadowSlant = sceneElement.attributeNS(KoXmlNS::dr3d, "shadow-slant");
    d->ambientColor = QColor(sceneElement.attributeNS(KoXmlNS::dr3d, "ambient-color", "#888888"));

    // Rendering attributes
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "shade-mode", "gouraud");
    if (dummy == "flat"_L1) {
        d->shadeMode = Flat;
    } else if (dummy == "phong"_L1) {
        d->shadeMode = Phong;
    } else if (dummy == "draft"_L1) {
        d->shadeMode = Draft;
    } else {
        d->shadeMode = Gouraud;
    }

    d->lightingMode = (sceneElement.attributeNS(KoXmlNS::dr3d, "lighting-mode") == "true"_L1);
    d->transform = sceneElement.attributeNS(KoXmlNS::dr3d, "transform");

    // 2. Load the children: light sources, 3d primitive shapes and nested scenes.
    // (ODF's dr3d module -- shared by ODF 1.0 through 1.4 -- allows dr3d:light,
    // dr3d:cube, dr3d:sphere, dr3d:extrude, dr3d:rotate and nested dr3d:scene
    // elements as children of dr3d:scene, in any order.)
    KoXmlElement elem;
    forEachElement(elem, sceneElement)
    {
        if (elem.namespaceURI() != KoXmlNS::dr3d) {
            continue;
        }
        const QString name = elem.localName();
        if (name == "light"_L1) {
            Lightsource light;
            light.loadOdf(elem);
            d->lights.append(light);
        } else if (name == "cube"_L1) {
            Cube cube;
            cube.loadOdf(elem);
            d->cubes.append(cube);
        } else if (name == "sphere"_L1) {
            Sphere sphere;
            sphere.loadOdf(elem);
            d->spheres.append(sphere);
        } else if (name == "extrude"_L1) {
            Extrude extrude;
            extrude.loadOdf(elem);
            d->extrudes.append(extrude);
        } else if (name == "rotate"_L1) {
            Rotate rotate;
            rotate.loadOdf(elem);
            d->rotates.append(rotate);
        } else if (name == "scene"_L1) {
            Ko3dScene *childScene = load3dScene(elem);
            if (childScene) {
                d->childScenes.append(childScene);
            }
        }
    }

    // debugOdf << "Lights:" << d->lights.size();

    return true;
}

void Ko3dScene::saveOdfAttributes(KoXmlWriter &writer) const
{
    // 1. Write scene attributes
    // Camera attributes
    writer.addAttribute("dr3d:vrp", (QString("(%1 %2 %3)").arg(d->vrp.x(), 0, 'f', 11).arg(d->vrp.y(), 0, 'f', 11).arg(d->vrp.z(), 0, 'f', 11)));
    writer.addAttribute("dr3d:vpn", (QString("(%1 %2 %3)").arg(d->vpn.x(), 0, 'f', 11).arg(d->vpn.y(), 0, 'f', 11).arg(d->vpn.z(), 0, 'f', 11)));
    writer.addAttribute("dr3d:vup", (QString("(%1 %2 %3)").arg(d->vup.x(), 0, 'f', 11).arg(d->vup.y(), 0, 'f', 11).arg(d->vup.z(), 0, 'f', 11)));

    writer.addAttribute("dr3d:projection", (d->projection == Parallel) ? "parallel" : "perspective");

    writer.addAttribute("dr3d:distance", d->distance);
    writer.addAttribute("dr3d:focal-length", d->focalLength);
    writer.addAttribute("dr3d:shadow-slant", d->shadowSlant);
    writer.addAttribute("dr3d:ambient-color", d->ambientColor.name());

    // Rendering attributes
    switch (d->shadeMode) {
    case Flat:
        writer.addAttribute("dr3d:shade-mode", "flat");
        break;
    case Phong:
        writer.addAttribute("dr3d:shade-mode", "phong");
        break;
    case Draft:
        writer.addAttribute("dr3d:shade-mode", "draft");
        break;
    case Gouraud:
    default:
        writer.addAttribute("dr3d:shade-mode", "gouraud");
        break;
    }

    writer.addAttribute("dr3d:lighting-mode", d->lightingMode);
    if (!d->transform.isEmpty()) {
        writer.addAttribute("dr3d:transform", d->transform);
    }
}

void Ko3dScene::saveOdfChildren(KoXmlWriter &writer) const
{
    // Write light sources.
    foreach (const Lightsource &light, d->lights) {
        light.saveOdf(writer);
    }

    // Write 3d primitive shapes.
    foreach (const Cube &cube, d->cubes) {
        cube.saveOdf(writer);
    }
    foreach (const Sphere &sphere, d->spheres) {
        sphere.saveOdf(writer);
    }
    foreach (const Extrude &extrude, d->extrudes) {
        extrude.saveOdf(writer);
    }
    foreach (const Rotate &rotate, d->rotates) {
        rotate.saveOdf(writer);
    }

    // Write nested scenes.
    foreach (Ko3dScene *childScene, d->childScenes) {
        writer.startElement("dr3d:scene");
        childScene->saveOdfAttributes(writer);
        childScene->saveOdfChildren(writer);
        writer.endElement(); // dr3d:scene
    }
}

// ----------------------------------------------------------------
//                         Public functions

KOODF_EXPORT Ko3dScene *load3dScene(const KoXmlElement &element)
{
    Ko3dScene *scene = new Ko3dScene();

    if (scene->loadOdf(element)) {
        return scene;
    }

    delete scene;
    return nullptr;
}

// ----------------------------------------------------------------
//                         Static functions

QVector3D odfToVector3D(const QString &string)
{
    // The string comes into this function in the form "(0 3.5 0.3)".
    QStringList elements = string.mid(1, string.size() - 2).split(' ', Qt::SkipEmptyParts);
    if (elements.size() == 3) {
        return QVector3D(elements[0].toDouble(), elements[1].toDouble(), elements[2].toDouble());
    } else {
        return QVector3D(0, 0, 1);
    }
}
