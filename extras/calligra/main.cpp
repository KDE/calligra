/* This file is part of the KDE project
   Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include <QTextStream>
#include <QCommandLineParser>
#include <QApplication>
#include <QUrl>

#include <KAboutData>
#include <klocalizedstring.h>
#include <kmimetype.h>
#include <KoServiceLocator.h>
#include <kmimetypetrader.h>
#include <kdebug.h>
#include <krun.h>
#include <ktoolinvocation.h>
#include <kmessagebox.h>
#include <kguiitem.h>

#include <calligraversion.h>

static void listApplicationNames()
{
    const KService::List services = KoServiceLocator::instance()->entries("Calligra/Application");
    QTextStream out(stdout, QIODevice::WriteOnly);
    QStringList names;
    foreach (KService::Ptr service, services) {
        names.append(service->desktopEntryName());
    }
    names.sort();
    foreach (const QString& name, names) {
        out << name << '\n';
    }
}

static void showWelcomeWindow()
{
}

static bool startService(KService::Ptr service, const QUrl& url)
{
    kDebug() << "service->entryPath():" << service->entryPath();
    QString error;
    QString serviceName;
    int pid = 0;
    int res = KToolInvocation::startServiceByDesktopPath(
        service->entryPath(), url.url(), &error, &serviceName, &pid
    );
    // could not check result - does not work for custom KDEDIRS: ok = res == 0;
    //ok = true;
    kDebug() << "KToolInvocation::startServiceByDesktopPath:" << res << pid << error << serviceName;
    return res == 0;
}

static int handleUrls(const QStringList& files)
{
    KMimeTypeTrader* mimeTrader = KMimeTypeTrader::self();
    QList<QUrl> notHandledUrls;
    const QRegExp withProtocolChecker( QStringLiteral("^[a-zA-Z]+:") );
    foreach(const QString& file, files) {
        // convert to an url
        const bool startsWithProtocol = (withProtocolChecker.indexIn(file) == 0);
        QUrl url = startsWithProtocol ? QUrl::fromUserInput(file) : QUrl::fromLocalFile(file);
        KMimeType::Ptr mimetype = KMimeType::findByUrl(url);
        if (mimetype->name() == KMimeType::defaultMimeType()) {
            KMessageBox::error(0, i18nc("@info", "Mimetype for <filename>%1</filename> not found!",
                                        url.toString()));
            return 1;
        }
        kDebug() << url << mimetype->name();
        /* 
            Find apps marked with Calligra/Applications service type
            and having given mimetype on the X-Calligra-Default-MimeTypes list.
            This is needed because, single mimetype can be supported by more than
            one Calligra application but only one application should be selected
            for opening given document using the calligra command.
            
            Example: application/vnd.oasis.opendocument.graphic, supported
            by Flow and Karbon already; out of these two, Karbon is selected
            for opening the document.
        */
        const QString constraint = QString("'%1' in [X-Calligra-DefaultMimeTypes]").arg(mimetype->name());
        kDebug() << constraint;
        KService::List services;
        KService::List offers = KoServiceLocator::instance()->entries("Calligra/Application");
        foreach(KService::Ptr offer, offers) {
            QStringList defaultMimeTypes = offer->property("X-Calligra-DefaultMimeTypes").toStringList();
            if (defaultMimeTypes.contains(mimetype->name())) {
                services << offer;
            }
        }

        KService::Ptr service;
        //bool ok = false;
        if (!services.isEmpty()) {
            service = services.first();
            kDebug() << "-" << service->name() << service->property("X-Calligra-DefaultMimeTypes");
#if 0
            ok = KRun::run(*service.data(), QList<QUrl>() << url, 0);
            kDebug() << "KRun::run:" << ok;
#else
            startService(service, url);
            return 0;
#endif
        }
        //if (!ok) {
        // if above not found or app cannot be executed:
        KService::List mimeServices = mimeTrader->query(mimetype->name(), "Calligra/Application");
        kDebug() << "Found" << mimeServices.count() << "services by MimeType field:";
        foreach (KService::Ptr service, mimeServices) {
            //kDebug() << "-" << service->name() << service->property("X-DBUS-ServiceName", QVariant::String);
            kDebug() << "-" << service->name() << service->hasServiceType("Calligra/Application")
                << service->hasMimeType(mimetype->name());
            //QVariant isCalligraApp = service->property("X-Calligra-App", QVariant::Bool);
            /*if (isCalligraApp.isValid() && isCalligraApp.toBool()) {
                kDebug() << "FOUND:" << service->name();
            }*/
        }
        if (mimeServices.isEmpty()) {
            service = mimeTrader->preferredService(mimetype->name());
            kDebug() << "mimeTrader->preferredService():" << service->name();
            if (service) {
                KGuiItem openItem(KStandardGuiItem::open());
                openItem.setText(i18nc("@action:button", "Open with <application>%1</application>",
                                      service->property("Name").toString()));
                if (KMessageBox::Yes != KMessageBox::questionYesNo(
                    0, 
                    i18nc("@info",
                         "Could not find Calligra application suitable for opening <filename>%1</filename>.<nl/>"
                         "Do you want to open the file using default application <application>%2</application>?",
                         url.url(), service->property("Name").toString()),
                    i18nc("@title:window", "Calligra Application Not Found"), openItem, KStandardGuiItem::cancel()))
                {
                    return 2;
                }
            }
        }
        else {
            service = mimeServices.first();
        }
        if (service) {
            startService(service, url);
            return 0;
        }
        else {
            notHandledUrls.append(url);
        }
        //}
    }
    if (!notHandledUrls.isEmpty()) {
        KRun::displayOpenWithDialog(notHandledUrls, 0);
        return 0;
    }
    return 0;
}

int main( int argc, char **argv )
{
    KLocalizedString::setApplicationDomain( "calligra-opener" );

    KAboutData aboutData("calligra", i18n("Calligra Opener"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Document Opener"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2010-%1 Calligra developers").arg(CALLIGRA_YEAR));
    aboutData.addAuthor(i18n("Jarosław Staniek"), QString(), "staniek@kde.org");

    QApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("apps"), i18n("Lists names of all available Calligra applications")));
    parser.addPositionalArgument(QStringLiteral("[FILE(S)]"), i18n("Files to open"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (parser.isSet("apps")) {
        listApplicationNames();
        return 0;
    }
    const QStringList files = parser.positionalArguments();
    if (files.isEmpty()) {
        //! @todo show cool Welcome window for app selection
        showWelcomeWindow();
    }
    else {
        return handleUrls(files);
    }
    return 0;
}
