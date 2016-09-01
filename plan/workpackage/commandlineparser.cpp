/* This file is part of the KDE project
 * Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "commandlineparser.h"
#include "part.h"
#include "mainwindow.h"
#include "aboutdata.h"

#include <kiconloader.h>
#include <KLocalizedString>
#include <KAboutData>
#include <KStartupInfo>
#include <KWindowSystem>
#include <KMessageBox>

#include <QApplication>
#include <QDir>

#include "debugarea.h"

CommandLineParser::CommandLineParser()
    : QObject(),
    m_mainwindow( 0 )
{
    KAboutData *aboutData = KPlatoWork::newAboutData();
    KAboutData::setApplicationData( *aboutData );
    qApp->setWindowIcon(QIcon::fromTheme(QStringLiteral("calligraplanwork")));

    aboutData->setupCommandLine(&m_commandLineParser);
    m_commandLineParser.addHelpOption();
    m_commandLineParser.addVersionOption();
    m_commandLineParser.addPositionalArgument(QStringLiteral("[file]"), i18n("File to open"));

    m_commandLineParser.process(*qApp);

    aboutData->processCommandLine(&m_commandLineParser);

    delete aboutData;
}

CommandLineParser::~CommandLineParser()
{
}

void CommandLineParser::handleActivateRequest(const QStringList &arguments, const QString &workingDirectory)
{
    m_commandLineParser.parse(arguments);

    handleCommandLine(QDir(workingDirectory));

    // terminate startup notification and activate the mainwindow
    KStartupInfo::setNewStartupId(m_mainwindow, KStartupInfo::startupId());
    KWindowSystem::forceActiveWindow(m_mainwindow->winId());

}

void CommandLineParser::handleCommandLine(const QDir &workingDirectory)
{
    QList<KMainWindow*> lst = KMainWindow::memberList();
    if ( lst.count() > 1 ) {
        warnPlanWork<<"windows count > 1:"<<lst.count();
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
}
