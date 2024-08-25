// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

#include <QApplication>
#include <QtGlobal>

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>

#include "calligra-version.h"
#include <KAboutData>
#include <KCrash>
#include <KLocalizedContext>
#include <KLocalizedString>

#ifdef Q_OS_WINDOWS
#include <QFont>
#include <Windows.h>
#endif

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(u"org.kde.desktop"_s);
    }

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    KLocalizedString::setApplicationDomain("calligralauncher");
    QCoreApplication::setOrganizationName(u"KDE"_s);

    KAboutData aboutData(u"calligralauncher"_s,
                         i18nc("@title", "Calligra Launcher"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Launcher"),
                         KAboutLicense::GPL,
                         i18n("(c) 2024"));
    aboutData.addAuthor(i18nc("@info:credit", "Carl Schwan"), i18nc("@info:credit", "Maintainer"), u"carl@carlschwan.eu"_s, u"https://carlschwan.eu.com"_s);
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KCrash::initialize();
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(u"org.kde.calligralauncher"_s));

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.calligra", u"Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}