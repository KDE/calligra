/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "TestColorConversionSystem.h"

#include <QTest>

#include <DebugPigment.h>
#include <KoColorConversionSystem.h>
#include <KoColorModelStandardIds.h>
#include <KoColorProfile.h>
#include <KoColorSpaceRegistry.h>

TestColorConversionSystem::TestColorConversionSystem()
{
    countFail = 0;
    foreach (const KoID &modelId, KoColorSpaceRegistry::instance()->colorModelsList(KoColorSpaceRegistry::AllColorSpaces)) {
        foreach (const KoID &depthId, KoColorSpaceRegistry::instance()->colorDepthList(modelId, KoColorSpaceRegistry::AllColorSpaces)) {
            QList<const KoColorProfile *> profiles =
                KoColorSpaceRegistry::instance()->profilesFor(KoColorSpaceRegistry::instance()->colorSpaceId(modelId, depthId));
            foreach (const KoColorProfile *profile, profiles) {
                listModels.append(ModelDepthProfile(modelId.id(), depthId.id(), profile->name()));
            }
        }
    }
    // listModels.append(ModelDepthProfile(AlphaColorModelID.id(), Integer8BitsColorDepthID.id(), ""));
}

void TestColorConversionSystem::testConnections_data()
{
    QTest::addColumn<QString>("smodel");
    QTest::addColumn<QString>("sdepth");
    QTest::addColumn<QString>("sprofile");
    QTest::addColumn<QString>("dmodel");
    QTest::addColumn<QString>("ddepth");
    QTest::addColumn<QString>("dprofile");
    QTest::addColumn<bool>("result");

    for (int i = 0; i < listModels.count(); ++i) {
        const ModelDepthProfile &srcCS = listModels[i];
        for (int j = 0; j < listModels.size(); ++j) {
            const ModelDepthProfile &dstCS = listModels.at(j);
            QByteArray name = QString("Path: %1/%2 to %3/%4").arg(srcCS.model, srcCS.depth, dstCS.model, dstCS.depth).toLocal8Bit();
            QTest::newRow(name) << srcCS.model << srcCS.depth << srcCS.profile << dstCS.model << dstCS.depth << dstCS.profile << true;
        }
    }
}

void TestColorConversionSystem::testConnections()
{
    QFETCH(QString, smodel);
    QFETCH(QString, sdepth);
    QFETCH(QString, sprofile);
    QFETCH(QString, dmodel);
    QFETCH(QString, ddepth);
    QFETCH(QString, dprofile);
    QFETCH(bool, result);

    QCOMPARE(KoColorSpaceRegistry::instance()->colorConversionSystem()->existsPath(smodel, sdepth, sprofile, dmodel, ddepth, dprofile), result);
}

void TestColorConversionSystem::testGoodConnections_data()
{
    QTest::addColumn<QString>("smodel");
    QTest::addColumn<QString>("sdepth");
    QTest::addColumn<QString>("sprofile");
    QTest::addColumn<QString>("dmodel");
    QTest::addColumn<QString>("ddepth");
    QTest::addColumn<QString>("dprofile");
    QTest::addColumn<bool>("result");

    for (int i = 0; i < listModels.count(); ++i) {
        const ModelDepthProfile &srcCS = listModels[i];
        for (int j = 0; j < listModels.size(); ++j) {
            const ModelDepthProfile &dstCS = listModels.at(j);
            QByteArray name = QString("Path: %1/%2 to %3/%4").arg(srcCS.model, srcCS.depth, dstCS.model, dstCS.depth).toLocal8Bit();
            QTest::newRow(name) << srcCS.model << srcCS.depth << srcCS.profile << dstCS.model << dstCS.depth << dstCS.profile << true;
        }
    }
}

void TestColorConversionSystem::testGoodConnections()
{
    QFETCH(QString, smodel);
    QFETCH(QString, sdepth);
    QFETCH(QString, sprofile);
    QFETCH(QString, dmodel);
    QFETCH(QString, ddepth);
    QFETCH(QString, dprofile);
    QFETCH(bool, result);

    if (!KoColorSpaceRegistry::instance()->colorConversionSystem()->existsGoodPath(smodel, sdepth, sprofile, dmodel, ddepth, dprofile)) {
        ++countFail;
        dbgPigment << "No good path between \"" << smodel << " " << sdepth << " " << sprofile << "\" \"" << dmodel << " " << ddepth << " " << dprofile << "\"";
    }
}

void TestColorConversionSystem::testFailedConnections()
{
    int failed = 0;
    if (!KoColorSpaceRegistry::instance()->colorSpace(RGBAColorModelID.id(), Float32BitsColorDepthID.id(), nullptr)
        && KoColorSpaceRegistry::instance()->colorSpace("KS6", Float32BitsColorDepthID.id(), nullptr)) {
        failed = 42;
    }
    QVERIFY2(countFail == failed, QString("%1 tests have fails (it should have been %2)").arg(countFail).arg(failed).toLatin1());
}

QTEST_GUILESS_MAIN(TestColorConversionSystem)
