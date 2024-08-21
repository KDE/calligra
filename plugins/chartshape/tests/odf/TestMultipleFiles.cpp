/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Johannes Simon <johannes.simon@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "TestMultipleFiles.h"

// Qt
#include <QDir>
#include <QTextDocument>

// Calligra
#include <KoOdfReadStore.h>
#include <KoStore.h>
#include <KoTextShapeDataBase.h>

// KoChart
#include "ChartDocument.h"
#include "ChartShape.h"

static void filterMessages(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString cat(context.category);
    switch (type) {
    case QtDebugMsg:
        //             fprintf(stderr, "%s\n%s\n%s\n%d\n%d\n", context.category, context.file, context.function, context.version, context.line);
        if (cat.startsWith("calligra.plugin.chart.odf")) {
            fprintf(stderr, "%d: %s\n", context.line, localMsg.constData());
        }
        break;
    default:
        fprintf(stderr, "%s\n", localMsg.constData());
        break;
    }
}

using namespace KoChart;

TestMultipleFiles::TestMultipleFiles()
    : QObject()
{
    ChartShape::setEnableUserInteraction(false);
}

void TestMultipleFiles::initTestCase()
{
    qInstallMessageHandler(filterMessages);
    m_chart = new ChartShape(nullptr);
    currentDoc = 0;
}

void TestMultipleFiles::cleanupTestCase()
{
    // overrides TestLoadingBase
}

void TestMultipleFiles::init()
{
    QVERIFY(currentDoc < documents.count());
    qInfo() << "------ Testing:" << documents.at(currentDoc);
    m_chart = new ChartShape(nullptr);

    QDir srcdir(QFINDTESTDATA(documents.at(currentDoc)));
    QVERIFY(srcdir.exists());

    // Go back up, we only used the cd as a test.
    srcdir.cdUp();

    KoStore *store = KoStore::createStore(srcdir.absolutePath(), KoStore::Read);
    QVERIFY(store->enterDirectory(documents.at(currentDoc)));

    QString errorMsg;
    KoOdfReadStore odfReadStore(store);
    bool success = odfReadStore.loadAndParse(errorMsg);
    if (!success) {
        qWarning() << "Error in odfReadStore.loadAndParse(): " << errorMsg;
    }
    QVERIFY(success);
    QVERIFY(m_chart->document()->loadOdf(odfReadStore));
}

void TestMultipleFiles::cleanup()
{
    ++currentDoc;
    delete m_chart;
    m_chart = nullptr;
}
