// krazy:excludeall=i18ncheckarg
/*  This file is part of the KDE libraries
    Copyright (C) 2006 Chusslove Illich <caslav.ilic@gmx.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "klocalizedstringtest.h"


#include <locale.h>

#include <QDebug>
#include <QtTest/QtTest>
#include <qstandardpaths.h>

#include <libintl.h>

#include <klocalizedstring.h>

#include <QtCore/QString>


void KLocalizedStringTest::initTestCase()
{
    KLocalizedString::setApplicationDomain("ki18n-test");

    m_hasFrench = true;
    if (m_hasFrench) {
        setlocale(LC_ALL, "fr_FR.utf8");
        if (setlocale(LC_ALL, NULL) != QByteArray("fr_FR.utf8")) {
            qDebug() << "Failed to set locale to fr_FR.utf8.";
            m_hasFrench = false;
        }
    }
    if (m_hasFrench) {
        if (!m_tempDir.isValid()) {
            qDebug() << "Failed to create temporary directory for test data.";
            m_hasFrench = false;
        }
    }
    QDir dataDir(m_tempDir.path());
    if (m_hasFrench) {
        m_hasFrench = compileCatalogs(dataDir);
    }
    if (m_hasFrench) {
        qputenv("XDG_DATA_DIRS",
                qgetenv("XDG_DATA_DIRS") + ":" + QFile::encodeName(dataDir.path()));
        // bind... dataDir.path()
        QStringList languages;
        languages.append("fr");
        KLocalizedString::setLanguages(languages);
    }

    #if 0 // until locale system is ready
    if (m_hasFrench) {
        KLocale::global()->setLanguage(QStringList() << "fr" << "en_US");
    }
    KLocale::global()->setThousandsSeparator(QLatin1String(","));
    KLocale::global()->setDecimalSymbol(QLatin1String("."));
    #endif
}

bool KLocalizedStringTest::compileCatalogs (const QDir &dataDir)
{
    if (!dataDir.mkpath("locale/fr/LC_MESSAGES")) {
        qDebug() << "Failed to create locale subdirectory "
                    "inside temporary directory.";
        return false;
    }
    QString msgfmt = QStandardPaths::findExecutable(QLatin1String("msgfmt"));
    if (msgfmt.isEmpty()) {
        qDebug() << "msgfmt(1) not found in path.";
        return false;
    }
    QStringList testPoPaths;
    testPoPaths << QFINDTESTDATA("po/fr/ki18n-test.po");
    testPoPaths << QFINDTESTDATA("po/fr/ki18n-test-qt.po");
    foreach (const QString &testPoPath, testPoPaths) {
        int pos_1 = testPoPath.lastIndexOf(QLatin1Char('/'));
        int pos_2 = testPoPath.lastIndexOf(QLatin1Char('.'));
        QString domain = testPoPath.mid(pos_1 + 1, pos_2 - pos_1 - 1);
        QString testMoPath;
        testMoPath = QString::fromLatin1("%1/locale/fr/LC_MESSAGES/%2.mo")
                                        .arg(dataDir.path(), domain);
        QProcess process;
        QStringList arguments;
        arguments << testPoPath << QLatin1String("-o") << testMoPath;
        process.start(msgfmt, arguments);
        process.waitForFinished(10000);
        if (process.exitCode() != 0) {
            qDebug() << QString::fromLatin1("msgfmt(1) could not compile %1.")
                                            .arg(testPoPath);
            return false;
        }
    }
    return true;
}

void KLocalizedStringTest::correctSubs ()
{
    // Warm up.
    QCOMPARE(i18n("Daisies, daisies"),
             QString("Daisies, daisies"));

    // Placeholder in the middle.
    QCOMPARE(i18n("Fault in %1 unit", QString("AE35")),
             QString("Fault in AE35 unit"));
    // Placeholder at the start.
    QCOMPARE(i18n("%1, Tycho Magnetic Anomaly 1", QString("TMA-1")),
             QString("TMA-1, Tycho Magnetic Anomaly 1"));
    // Placeholder at the end.
    QCOMPARE(i18n("...odd things happening at %1", QString("Clavius")),
             QString("...odd things happening at Clavius"));
    QCOMPARE(i18n("Group %1", 1),
            QString("Group 1"));

    // Two placeholders.
    QCOMPARE(i18n("%1 and %2", QString("Bowman"), QString("Poole")),
             QString("Bowman and Poole"));
    // Two placeholders in inverted order.
    QCOMPARE(i18n("%2 and %1", QString("Poole"), QString("Bowman")),
             QString("Bowman and Poole"));

    // % which is not of placeholder.
    QCOMPARE(i18n("It's going to go %1% failure in 72 hours.", 100),
             QString("It's going to go 100% failure in 72 hours."));

    // Usual plural.
    QCOMPARE(i18np("%1 pod", "%1 pods", 1),
             QString("1 pod"));
    QCOMPARE(i18np("%1 pod", "%1 pods", 10),
             QString("10 pods"));

    // No plural-number in singular.
    QCOMPARE(i18np("A pod", "%1 pods", 1),
             QString("A pod"));
    QCOMPARE(i18np("A pod", "%1 pods", 10),
             QString("10 pods"));

    // No plural-number in singular or plural.
    QCOMPARE(i18np("A pod", "Few pods", 1),
             QString("A pod"));
    QCOMPARE(i18np("A pod", "Few pods", 10),
             QString("Few pods"));

    // First of two arguments as plural-number.
    QCOMPARE(i18np("A pod left on %2", "%1 pods left on %2",
                   1, QString("Discovery")),
             QString("A pod left on Discovery"));
    QCOMPARE(i18np("A pod left on %2", "%1 pods left on %2",
                   2, QString("Discovery")),
             QString("2 pods left on Discovery"));

    // Second of two arguments as plural-number.
    QCOMPARE(i18np("%1 has a pod left", "%1 has %2 pods left",
                   QString("Discovery"), 1),
             QString("Discovery has a pod left"));
    QCOMPARE(i18np("%1 has a pod left", "%1 has %2 pods left",
                   QString("Discovery"), 2),
             QString("Discovery has 2 pods left"));

    // No plural-number in singular or plural, but another argument present.
    QCOMPARE(i18np("A pod left on %2", "Some pods left on %2",
                   1, QString("Discovery")),
             QString("A pod left on Discovery"));
    QCOMPARE(i18np("A pod left on %2", "Some pods left on %2",
                   2, QString("Discovery")),
             QString("Some pods left on Discovery"));

    // Visual formatting.
    // FIXME: Needs much more tests.
    QCOMPARE(xi18n("E = mc^2"),
             QString("E = mc^2"));
    QCOMPARE(xi18n("E &lt; mc^2"),
             QString("E < mc^2"));
    QCOMPARE(xi18n("E ? <emphasis>mc^2</emphasis>"),
             QString("E ? *mc^2*"));
    QCOMPARE(xi18n("E &lt; <emphasis>mc^2</emphasis>"),
             QString("E < *mc^2*"));
    QCOMPARE(xi18nc("@label", "E &lt; <emphasis>mc^2</emphasis>"),
             QString("E < *mc^2*"));
    QCOMPARE(xi18nc("@info", "E &lt; <emphasis>mc^2</emphasis>"),
             QString("<html>E &lt; <i>mc^2</i></html>"));
    QCOMPARE(xi18n("E = mc^&#x0032;"),
             QString("E = mc^2"));
    QCOMPARE(xi18n("E = mc^&#0050;"),
             QString("E = mc^2"));

    // Number formatting.
    QCOMPARE(ki18n("%1").subs(42).toString(),
             QString("42"));
    QCOMPARE(ki18n("%1").subs(42, 5).toString(),
             QString("   42"));
    QCOMPARE(ki18n("%1").subs(42, -5, 10, QChar('_')).toString(),
             QString("42___"));
    QCOMPARE(ki18n("%1").subs(4.2, 5, 'f', 2).toString(),
             QString(" 4.20"));
}

void KLocalizedStringTest::wrongSubs()
{
    #ifndef NDEBUG
    // Too many arguments.
    QVERIFY(i18n("Europa", 1)
            != QString("Europa"));

    // Too few arguments.
    QVERIFY(i18n("%1, %2 and %3", QString("Hunter"), QString("Kimball"))
            != QString("Hunter, Kimball and %3"));

    // Gaps in placheholder numbering.
    QVERIFY(ki18n("Beyond the %2").subs("infinity").toString()
            != QString("Beyond the infinity"));

    // Plural argument not supplied.
    QVERIFY(ki18np("1 pod", "%1 pods").toString()
            != QString("1 pod"));
    QVERIFY(ki18np("1 pod", "%1 pods").toString()
            != QString("%1 pods"));
    #endif
}

void
KLocalizedStringTest::removeAcceleratorMarker()
{
    // No accelerator marker.
    QCOMPARE(KLocalizedString::removeAcceleratorMarker(QString()),
             QString());
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo bar"),
             QString("Foo bar"));

    // Run of the mill.
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("&Foo bar"),
             QString("Foo bar"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo &bar"),
             QString("Foo bar"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo b&ar"),
             QString("Foo bar"));
    // - presence of escaped ampersands
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo && Bar"),
             QString("Foo & Bar"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo && &Bar"),
             QString("Foo & Bar"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("&Foo && Bar"),
             QString("Foo & Bar"));

    // CJK-style markers.
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo bar (&F)"),
             QString("Foo bar"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("(&F) Foo bar"),
             QString("Foo bar"));
    // - interpunction after/before parenthesis still qualifies CJK marker
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo bar (&F):"),
             QString("Foo bar:"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo bar (&F)..."),
             QString("Foo bar..."));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("...(&F) foo bar"),
             QString("...foo bar"));
    // - alphanumerics around parenthesis disqualify CJK marker
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo (&F) bar"),
             QString("Foo (F) bar"));
    // - something removed raw ampersands, leaving dangling reduced CJK markers.
    // Remove reduced markers only if CJK characters are found in the string.
    QCOMPARE(KLocalizedString::removeAcceleratorMarker(QString::fromUtf8("Foo bar (F)")),
             QString::fromUtf8("Foo bar (F)"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker(QString::fromUtf8("印刷(P)...")),
             QString::fromUtf8("印刷..."));

    // Shady cases, where ampersand is obviously not a marker
    // and should have been escaped, but it was not.
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("&"),
             QString("&"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo bar &"),
             QString("Foo bar &"));
    QCOMPARE(KLocalizedString::removeAcceleratorMarker("Foo & Bar"),
             QString("Foo & Bar"));
}

void KLocalizedStringTest::miscMethods()
{
    KLocalizedString k;
    QVERIFY(k.isEmpty());
}

// Same as translateToFrench, but using libintl directly (bindtextdomain+dgettext).
// Useful for debugging. This changes global state, though, so it's skipped by default.
void KLocalizedStringTest::translateToFrenchLowlevel()
{
    if (!m_hasFrench) {
        QSKIP("French test files not usable.");
    }
    QSKIP("Skipped by default to avoid changing global state.");
    // fr_FR locale was set by initTestCase already.
    if (QFile::exists("/usr/share/locale/fr/LC_MESSAGES/ki18n-test.mo")) {
        bindtextdomain("ki18n-test", "/usr/share/locale");
        QCOMPARE(QString::fromUtf8(dgettext("ki18n-test", "Loadable modules")), QString::fromUtf8("Modules chargeables"));
    }
}

void KLocalizedStringTest::translateToFrench()
{
    if (!m_hasFrench) {
        QSKIP("French test files not usable.");
    }
    QCOMPARE(i18n("Loadable modules"), QString::fromUtf8("Modules chargeables"));
    QCOMPARE(i18n("Job"), QString::fromUtf8("Tâche"));
}

void KLocalizedStringTest::translateQt()
{
    KLocalizedString::insertQtDomain("ki18n-test-qt");
    QString result = KLocalizedString::translateQt("QPrintPreviewDialog", "Landscape", 0 , 0);
    // When we use the default language, translateQt returns an empty string.
    QString expected = m_hasFrench ? QString("Paysage") : QString();
    QCOMPARE(result, expected);
    #if 0 // KLocalizedString no longer does anything with QTranslator, this needed?
    result = QCoreApplication::translate("QPrintPreviewDialog", "Landscape");
    QString expected2 = m_hasFrench ? QString("Paysage") : QString("Landscape");
    QCOMPARE(result, expected2);
    #endif

    #if 0 // translateRaw no longer public, this needed?
    // So let's use translateRaw instead for the threaded test
    QString lang;
    KLocale::global()->translateRaw("Landscape", &lang, &result);
    QCOMPARE(lang, m_hasFrench ? QString("fr") : QString("en_US"));
    QCOMPARE(result, m_hasFrench ? QString("Paysage") : QString("Landscape"));
    #endif
    KLocalizedString::removeQtDomain("ki18n-test-qt");
}

#include <QThreadPool>
#include <qtconcurrentrun.h>
#include <qstandardpaths.h>
#include <QFutureSynchronizer>

void KLocalizedStringTest::testThreads()
{
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    QFutureSynchronizer<void> sync;
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::correctSubs));
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::correctSubs));
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::correctSubs));
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::translateQt));
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::translateQt));
    sync.addFuture(QtConcurrent::run(this, &KLocalizedStringTest::translateToFrench));
    sync.waitForFinished();
    QThreadPool::globalInstance()->setMaxThreadCount(1); // delete those threads
}

QTEST_MAIN(KLocalizedStringTest)
