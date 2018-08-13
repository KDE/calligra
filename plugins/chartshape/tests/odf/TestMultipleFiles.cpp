/* This file is part of the KDE project
 *
 * Copyright 2018 Johannes Simon <johannes.simon@gmail.com>
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

// Own
#include "TestMultipleFiles.h"

// Qt
#include <QTextDocument>
#include <QDir>

// Calligra
#include <KoStore.h>
#include <KoOdfReadStore.h>
#include <KoTextShapeDataBase.h>

// KoChart
#include "ChartShape.h"
#include "ChartDocument.h"

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
    m_chart = new ChartShape(0);
    currentDoc = 0;
}

void TestMultipleFiles::cleanupTestCase()
{
    // overrides TestLoadingBase
}

void TestMultipleFiles::init()
{
    QVERIFY(currentDoc < documents.count());
    qInfo()<<"------ Testing:"<<documents.at(currentDoc);
    m_chart = new ChartShape(0);

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
    m_chart = 0;
}

