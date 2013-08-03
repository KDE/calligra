/*
    Tests for KConfig Compiler

    Copyright (c) 2005      by Duncan Mac-Vicar       <duncan@kde.org>
    Copyright (c) 2009      by Pino Toscano           <pino@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <qstandardpaths.h>
#include "kconfigcompiler_test.h"

// QT5 TODO QTEST_GUILESS_MAIN(KConfigCompiler_Test)
QTEST_MAIN(KConfigCompiler_Test)

typedef const char * CompilerTestSet[];

static CompilerTestSet testCases =
{
	"test1.cpp", "test1.h",
	"test2.cpp", "test2.h",
	"test3.cpp", "test3.h",
	"test3a.cpp", "test3a.h",
	"test4.cpp", "test4.h",
	"test5.cpp", "test5.h",
	"test6.cpp", "test6.h",
	"test7.cpp", "test7.h",
	"test8a.cpp", "test8a.h",
	"test8b.cpp", "test8b.h",
	"test9.h", "test9.cpp",
	"test10.h", "test10.cpp",
	"test11.h", "test11.cpp",
	"test11a.h", "test11a.cpp",
	"test_dpointer.cpp", "test_dpointer.h",
	"test_signal.cpp", "test_signal.h",
	NULL
};

static CompilerTestSet testCasesToRun =
{
    "test1",
    "test2",
    "test3",
    "test3a",
    "test4",
    "test5",
    "test6",
    "test7",
    "test8",
    "test9",
    "test10",
    "test11",
    "test_dpointer",
    "test_signal",
    0
};

#if 0
static CompilerTestSet willFailCases =
{
	// where is that QDir coming from?
	//"test9.cpp", NULL
	NULL
};
#endif

void KConfigCompiler_Test::initTestCase()
{
    m_diffExe = QStandardPaths::findExecutable("diff");
    if (!m_diffExe.isEmpty()) {
        m_diff.setFileName(QDir::currentPath() + QLatin1String("/kconfigcompiler_test_differences.diff"));
        if (m_diff.exists()) {
            m_diff.remove();
        }
    }
}

void KConfigCompiler_Test::testBaselineComparison_data()
{
    QTest::addColumn<QString>("testName");

    for (const char **it = testCases; *it; ++it) {
        QTest::newRow(*it) << QString::fromLatin1(*it);
    }
}

void KConfigCompiler_Test::testBaselineComparison()
{
    QFETCH(QString, testName);

    QFile file(QFINDTESTDATA(testName));
    QFile fileRef(QFINDTESTDATA(testName + QLatin1String(".ref")));

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << file.fileName();
        QFAIL("Can't open file for comparison");
    }
    if (!fileRef.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << fileRef.fileName();
        QFAIL("Can't open file for comparison");
    }
    QString content = file.readAll();
    QString contentRef = fileRef.readAll();

    if (content != contentRef)  {
        appendFileDiff(fileRef.fileName(), file.fileName());
    }
    // use split('\n') to avoid
    // the whole output shown inline
    QCOMPARE(content.split('\n'), contentRef.split('\n'));
    QVERIFY(content == contentRef);
}

void KConfigCompiler_Test::testRunning_data()
{
    QTest::addColumn<QString>("testName");

    for (const char **it = testCasesToRun; *it; ++it) {
        QTest::newRow(*it) << QString::fromLatin1(*it);
    }
}

void KConfigCompiler_Test::testRunning()
{
    QFETCH(QString, testName);

    QString program = QFINDTESTDATA(testName);
    QVERIFY2(QFile::exists(program), qPrintable(program + QLatin1String(" must exist!")));
    QProcess process;
    process.start(program, QIODevice::ReadOnly);
    if (process.waitForStarted()) {
        QVERIFY(process.waitForFinished());
    }
    QCOMPARE((int)process.error(), (int)QProcess::UnknownError);
    QCOMPARE(process.exitCode(), 0);
}


void KConfigCompiler_Test::appendFileDiff(const QString &oldFile, const QString &newFile)
{
    if (m_diffExe.isEmpty()) {
        return;
    }
    if (!m_diff.isOpen()) {
        if (!m_diff.open(QIODevice::WriteOnly)) {
            return;
        }
    }

    QStringList args;
    args << "-u";
    args << QFileInfo(oldFile).absoluteFilePath();
    args << QFileInfo(newFile).absoluteFilePath();

    QProcess process;
    process.start(m_diffExe, args, QIODevice::ReadOnly);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitCode() == 1) {
        QByteArray out = process.readAllStandardOutput();
        m_diff.write(out);
    }
}
