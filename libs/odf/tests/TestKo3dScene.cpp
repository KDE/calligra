/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <Ko3dScene.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

namespace
{
// Compares two "(x y z)"-formatted ODF vectors with a small numeric tolerance,
// since Ko3dScene round-trips them through QVector3D/double formatting.
bool fuzzyVectorEquals(const QVector3D &a, const QVector3D &b)
{
    return qFuzzyCompare(a.x() + 1.0f, b.x() + 1.0f) && qFuzzyCompare(a.y() + 1.0f, b.y() + 1.0f) && qFuzzyCompare(a.z() + 1.0f, b.z() + 1.0f);
}
}

class TestKo3dScene : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoadSceneAttributesAndLights();
    void testLoadShapesAndNestedScene();
    void testLoadRejectsNonScene();
    void testSaveAndLoadRoundTrip();
};

void TestKo3dScene::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKo3dScene::testLoadSceneAttributesAndLights()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<dr3d:scene xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\""
        " dr3d:vrp=\"(0 0 10)\" dr3d:vpn=\"(0 0 1)\" dr3d:vup=\"(0 1 0)\""
        " dr3d:projection=\"parallel\" dr3d:distance=\"4.5cm\" dr3d:focal-length=\"8cm\""
        " dr3d:shadow-slant=\"20\" dr3d:shade-mode=\"phong\" dr3d:ambient-color=\"#101010\""
        " dr3d:lighting-mode=\"true\" dr3d:transform=\"rotate (0 0 1 45)\">"
        "<dr3d:light dr3d:diffuse-color=\"#ff0000\" dr3d:direction=\"(1 0 0)\" dr3d:enabled=\"true\" dr3d:specular=\"false\"/>"
        "<dr3d:light dr3d:diffuse-color=\"#00ff00\" dr3d:direction=\"(0 1 0)\" dr3d:enabled=\"false\" dr3d:specular=\"true\"/>"
        "</dr3d:scene>",
        doc);
    QVERIFY(!root.isNull());

    Ko3dScene scene;
    QVERIFY(scene.loadOdf(root));

    QVERIFY(fuzzyVectorEquals(scene.vrp(), QVector3D(0, 0, 10)));
    QVERIFY(fuzzyVectorEquals(scene.vpn(), QVector3D(0, 0, 1)));
    QVERIFY(fuzzyVectorEquals(scene.vup(), QVector3D(0, 1, 0)));
    QCOMPARE(scene.projection(), Ko3dScene::Parallel);
    QCOMPARE(scene.distance(), QString("4.5cm"));
    QCOMPARE(scene.focalLength(), QString("8cm"));
    QCOMPARE(scene.shadowSlant(), QString("20"));
    QCOMPARE(scene.shadeMode(), Ko3dScene::Phong);
    QCOMPARE(scene.ambientColor(), QColor("#101010"));
    QCOMPARE(scene.lightingMode(), true);
    QCOMPARE(scene.transform(), QString("rotate (0 0 1 45)"));

    const QVector<Ko3dScene::Lightsource> lights = scene.lights();
    QCOMPARE(lights.size(), 2);
    const Ko3dScene::Lightsource &light0 = lights.at(0);
    QCOMPARE(light0.diffuseColor(), QColor("#ff0000"));
    QVERIFY(fuzzyVectorEquals(light0.direction(), QVector3D(1, 0, 0)));
    QCOMPARE(light0.enabled(), true);
    QCOMPARE(light0.specular(), false);

    const Ko3dScene::Lightsource &light1 = lights.at(1);
    QCOMPARE(light1.diffuseColor(), QColor("#00ff00"));
    QCOMPARE(light1.enabled(), false);
    QCOMPARE(light1.specular(), true);
}

void TestKo3dScene::testLoadShapesAndNestedScene()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<dr3d:scene xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\""
        " xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\""
        " dr3d:vrp=\"(0 0 1)\" dr3d:vpn=\"(0 0 1)\" dr3d:vup=\"(0 1 0)\">"
        "<dr3d:cube dr3d:min-edge=\"(0 0 0)\" dr3d:max-edge=\"(1 1 1)\" dr3d:transform=\"translate (1 0 0)\"/>"
        "<dr3d:sphere dr3d:center=\"(0 0 0)\" dr3d:size=\"(2 2 2)\"/>"
        "<dr3d:extrude svg:viewBox=\"0 0 100 100\" svg:d=\"M 0 0 L 100 0 100 100 0 100 Z\" dr3d:transform=\"scale (2 2 2)\"/>"
        "<dr3d:rotate svg:viewBox=\"0 0 50 50\" svg:d=\"M 0 0 L 50 0 25 50 Z\"/>"
        "<dr3d:scene dr3d:vrp=\"(0 0 5)\" dr3d:vpn=\"(0 0 1)\" dr3d:vup=\"(0 1 0)\">"
        "<dr3d:cube dr3d:min-edge=\"(0 0 0)\" dr3d:max-edge=\"(0.5 0.5 0.5)\"/>"
        "</dr3d:scene>"
        "</dr3d:scene>",
        doc);
    QVERIFY(!root.isNull());

    Ko3dScene scene;
    QVERIFY(scene.loadOdf(root));

    QCOMPARE(scene.cubes().size(), 1);
    QVERIFY(fuzzyVectorEquals(scene.cubes().at(0).minEdge(), QVector3D(0, 0, 0)));
    QVERIFY(fuzzyVectorEquals(scene.cubes().at(0).maxEdge(), QVector3D(1, 1, 1)));
    QCOMPARE(scene.cubes().at(0).transform(), QString("translate (1 0 0)"));

    QCOMPARE(scene.spheres().size(), 1);
    QVERIFY(fuzzyVectorEquals(scene.spheres().at(0).center(), QVector3D(0, 0, 0)));
    QVERIFY(fuzzyVectorEquals(scene.spheres().at(0).size(), QVector3D(2, 2, 2)));

    QCOMPARE(scene.extrudes().size(), 1);
    QCOMPARE(scene.extrudes().at(0).viewBox(), QString("0 0 100 100"));
    QCOMPARE(scene.extrudes().at(0).path(), QString("M 0 0 L 100 0 100 100 0 100 Z"));
    QCOMPARE(scene.extrudes().at(0).transform(), QString("scale (2 2 2)"));

    QCOMPARE(scene.rotates().size(), 1);
    QCOMPARE(scene.rotates().at(0).viewBox(), QString("0 0 50 50"));
    QCOMPARE(scene.rotates().at(0).path(), QString("M 0 0 L 50 0 25 50 Z"));

    QCOMPARE(scene.childScenes().size(), 1);
    Ko3dScene *child = scene.childScenes().at(0);
    QVERIFY(child);
    QVERIFY(fuzzyVectorEquals(child->vrp(), QVector3D(0, 0, 5)));
    QCOMPARE(child->cubes().size(), 1);
    QVERIFY(fuzzyVectorEquals(child->cubes().at(0).maxEdge(), QVector3D(0.5, 0.5, 0.5)));
}

void TestKo3dScene::testLoadRejectsNonScene()
{
    // An element with none of dr3d:vrp/vpn/vup is not recognized as a 3d scene.
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<dr3d:scene xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\"/>",
        doc);
    QVERIFY(!root.isNull());

    Ko3dScene scene;
    QVERIFY(!scene.loadOdf(root));
}

void TestKo3dScene::testSaveAndLoadRoundTrip()
{
    // Build a scene entirely through the public API, covering camera/rendering
    // attributes, a light, all four 3d primitive shapes and a nested scene.
    Ko3dScene scene;
    scene.setVrp(QVector3D(1, 2, 3));
    scene.setVpn(QVector3D(0, 0, -1));
    scene.setVup(QVector3D(0, 1, 0));
    scene.setProjection(Ko3dScene::Parallel);
    scene.setDistance("5cm");
    scene.setFocalLength("9cm");
    scene.setShadowSlant("15");
    scene.setShadeMode(Ko3dScene::Draft);
    scene.setAmbientColor(QColor("#123456"));
    scene.setLightingMode(true);
    scene.setTransform("rotate (1 0 0 30)");

    Ko3dScene::Lightsource light;
    light.setDiffuseColor(QColor("#abcdef"));
    light.setDirection(QVector3D(0, -1, 0));
    light.setEnabled(true);
    light.setSpecular(true);
    scene.addLight(light);

    Ko3dScene::Cube cube;
    cube.setMinEdge(QVector3D(-1, -1, -1));
    cube.setMaxEdge(QVector3D(1, 1, 1));
    cube.setTransform("translate (2 0 0)");
    scene.addCube(cube);

    Ko3dScene::Sphere sphere;
    sphere.setCenter(QVector3D(0, 0, 0));
    sphere.setSize(QVector3D(3, 3, 3));
    scene.addSphere(sphere);

    Ko3dScene::Extrude extrude;
    extrude.setViewBox("0 0 10 10");
    extrude.setPath("M 0 0 L 10 0 5 10 Z");
    extrude.setTransform("scale (1 1 2)");
    scene.addExtrude(extrude);

    Ko3dScene::Rotate rotate;
    rotate.setViewBox("0 0 20 20");
    rotate.setPath("M 0 0 L 20 0 10 20 Z");
    scene.addRotate(rotate);

    auto *childScene = new Ko3dScene();
    childScene->setVrp(QVector3D(0, 0, 42));
    childScene->setVpn(QVector3D(0, 0, 1));
    childScene->setVup(QVector3D(0, 1, 0));
    Ko3dScene::Sphere childSphere;
    childSphere.setCenter(QVector3D(1, 1, 1));
    childSphere.setSize(QVector3D(0.5, 0.5, 0.5));
    childScene->addSphere(childSphere);
    scene.addChildScene(childScene); // scene takes ownership

    // Save it, then reparse the result -- a full ODF write/read round trip.
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::writeAndReparse(
        "dr3d:scene",
        {{"dr3d", KoXmlNS::dr3d}, {"svg", KoXmlNS::svg}},
        [&scene](KoXmlWriter &writer) {
            scene.saveOdfAttributes(writer);
            scene.saveOdfChildren(writer);
        },
        doc);
    QVERIFY(!root.isNull());

    Ko3dScene loaded;
    QVERIFY(loaded.loadOdf(root));

    QVERIFY(fuzzyVectorEquals(loaded.vrp(), scene.vrp()));
    QVERIFY(fuzzyVectorEquals(loaded.vpn(), scene.vpn()));
    QVERIFY(fuzzyVectorEquals(loaded.vup(), scene.vup()));
    QCOMPARE(loaded.projection(), scene.projection());
    QCOMPARE(loaded.distance(), scene.distance());
    QCOMPARE(loaded.focalLength(), scene.focalLength());
    QCOMPARE(loaded.shadowSlant(), scene.shadowSlant());
    QCOMPARE(loaded.shadeMode(), scene.shadeMode());
    QCOMPARE(loaded.ambientColor(), scene.ambientColor());
    QCOMPARE(loaded.lightingMode(), scene.lightingMode());
    QCOMPARE(loaded.transform(), scene.transform());

    QCOMPARE(loaded.lights().size(), 1);
    QCOMPARE(loaded.lights().at(0).diffuseColor(), light.diffuseColor());
    QVERIFY(fuzzyVectorEquals(loaded.lights().at(0).direction(), light.direction()));
    QCOMPARE(loaded.lights().at(0).enabled(), light.enabled());
    QCOMPARE(loaded.lights().at(0).specular(), light.specular());

    QCOMPARE(loaded.cubes().size(), 1);
    QVERIFY(fuzzyVectorEquals(loaded.cubes().at(0).minEdge(), cube.minEdge()));
    QVERIFY(fuzzyVectorEquals(loaded.cubes().at(0).maxEdge(), cube.maxEdge()));
    QCOMPARE(loaded.cubes().at(0).transform(), cube.transform());

    QCOMPARE(loaded.spheres().size(), 1);
    QVERIFY(fuzzyVectorEquals(loaded.spheres().at(0).center(), sphere.center()));
    QVERIFY(fuzzyVectorEquals(loaded.spheres().at(0).size(), sphere.size()));

    QCOMPARE(loaded.extrudes().size(), 1);
    QCOMPARE(loaded.extrudes().at(0).viewBox(), extrude.viewBox());
    QCOMPARE(loaded.extrudes().at(0).path(), extrude.path());
    QCOMPARE(loaded.extrudes().at(0).transform(), extrude.transform());

    QCOMPARE(loaded.rotates().size(), 1);
    QCOMPARE(loaded.rotates().at(0).viewBox(), rotate.viewBox());
    QCOMPARE(loaded.rotates().at(0).path(), rotate.path());

    QCOMPARE(loaded.childScenes().size(), 1);
    Ko3dScene *loadedChild = loaded.childScenes().at(0);
    QVERIFY(fuzzyVectorEquals(loadedChild->vrp(), childScene->vrp()));
    QCOMPARE(loadedChild->spheres().size(), 1);
    QVERIFY(fuzzyVectorEquals(loadedChild->spheres().at(0).size(), childSphere.size()));
}

QTEST_MAIN(TestKo3dScene)
#include "TestKo3dScene.moc"
