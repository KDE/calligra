/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005, 2006 Sven Lüppken <sven@kde.org>
   Copyright (C) 2008 - 2009 Dag Andersen <danders@get2net.dk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "application.h"
#include "part.h"
#include "mainwindow.h"
#include "aboutdata.h"

#include <kiconloader.h>
#include <KLocalizedString>
#include <KAboutData>
#include <KStartupInfo>
#include <KWindowSystem>
#include <kmessagebox.h>

#include <QDir>

#include "debugarea.h"

KPlatoWork_Application::KPlatoWork_Application(int argc, char **argv)
    : QApplication(argc, argv),
    m_mainwindow( 0 )
{
    KAboutData *aboutData = KPlatoWork::newAboutData();
    KAboutData::setApplicationData( *aboutData );
    setWindowIcon(QIcon::fromTheme(QStringLiteral("calligraplanwork")));

    aboutData->setupCommandLine(&m_commandLineParser);
    m_commandLineParser.addHelpOption();
    m_commandLineParser.addVersionOption();
    m_commandLineParser.addPositionalArgument(QStringLiteral("[file]"), i18n("File to open"));

    m_commandLineParser.process(*this);

    aboutData->processCommandLine(&m_commandLineParser);

    // Tell the iconloader about share/apps/calligra/icons
/*    KIconLoader::global()->addAppDir("calligra");

    // Initialize all Calligra directories etc.
    KoGlobal::initialize();

    new KoApplicationAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/application", this);*/

    delete aboutData;
}

KPlatoWork_Application::~KPlatoWork_Application()
{
}

void KPlatoWork_Application::handleActivateRequest(const QStringList &arguments, const QString &workingDirectory)
{
    Q_UNUSED(workingDirectory);

    m_commandLineParser.parse(arguments);

    handleCommandLine(QDir(workingDirectory));

    // terminate startup notification and activate the mainwindow
    KStartupInfo::setNewStartupId(m_mainwindow, KStartupInfo::startupId());
    KWindowSystem::forceActiveWindow(m_mainwindow->winId());

}

void KPlatoWork_Application::handleCommandLine(const QDir &workingDirectory)
{
    debugPlanWork<<"starting------------------------";
    QList<KMainWindow*> lst = KMainWindow::memberList();
    debugPlanWork<<"windows"<<lst.count();
    if ( lst.count() > 1 ) {
        debugPlanWork<<"windows"<<lst.count();
        return; // should never happen
    }
    if ( lst.isEmpty() ) {
        Q_ASSERT( m_mainwindow == 0 );
    }
    if ( m_mainwindow == 0 ) {
        m_mainwindow = new KPlatoWork_MainWindow();
        m_mainwindow->show();
    }

    // Get the command line arguments which we have to parse
    const QStringList fileUrls = m_commandLineParser.positionalArguments();
    // TODO: remove once Qt has proper handling itself
    const QRegExp withProtocolChecker( QStringLiteral("^[a-zA-Z]+:") );
    foreach(const QString &fileUrl, fileUrls) {
        // convert to an url
        const bool startsWithProtocol = (withProtocolChecker.indexIn(fileUrl) == 0);
        const QUrl url = startsWithProtocol ?
            QUrl::fromUserInput(fileUrl) :
            QUrl::fromLocalFile(workingDirectory.absoluteFilePath(fileUrl));

        // For now create an empty document
            if ( ! m_mainwindow->openDocument(url) ) {
                KMessageBox::error(0, i18n("Failed to open document") );
            }
    }

    // not calling this before since the program will quit there.
    debugPlanWork<<"started------------------------";
}
