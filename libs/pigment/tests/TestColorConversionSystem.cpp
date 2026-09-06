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

#include <utility>

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

#ifndef CCS_TEST_GOOD_ONLY
void TestColorConversionSystem::testConnections()
{
    const auto *conversionSystem = KoColorSpaceRegistry::instance()->colorConversionSystem();
    for (const ModelDepthProfile &srcCS : std::as_const(listModels)) {
        for (const ModelDepthProfile &dstCS : std::as_const(listModels)) {
            QVERIFY2(conversionSystem->existsPath(srcCS.model, srcCS.depth, srcCS.profile, dstCS.model, dstCS.depth, dstCS.profile),
                     QString("No path between %1 / %2 and %3 / %4").arg(srcCS.model, srcCS.depth, dstCS.model, dstCS.depth).toLocal8Bit());
        }
    }
}
#endif

#ifndef CCS_TEST_CONNECTIONS_ONLY
void TestColorConversionSystem::testGoodConnections()
{
    const auto *conversionSystem = KoColorSpaceRegistry::instance()->colorConversionSystem();
    for (const ModelDepthProfile &srcCS : std::as_const(listModels)) {
        for (const ModelDepthProfile &dstCS : std::as_const(listModels)) {
            if (!conversionSystem->existsGoodPath(srcCS.model, srcCS.depth, srcCS.profile, dstCS.model, dstCS.depth, dstCS.profile)) {
                ++countFail;
                dbgPigment << "No good path between \"" << srcCS.model << " " << srcCS.depth << " " << srcCS.profile << "\" \"" << dstCS.model << " "
                           << dstCS.depth << " " << dstCS.profile << "\"";
            }
        }
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
#endif

QTEST_GUILESS_MAIN(TestColorConversionSystem)
