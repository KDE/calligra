/* This file is part of the KDE libraries
    Copyright (c) 1997,1998 Matthias Kalle Dalheimer <kalle@kde.org>
    Copyright (c) 1999 Espen Sand <espen@kde.org>
    Copyright (c) 2000-2004 Frerich Raabe <raabe@kde.org>
    Copyright (c) 2003,2004 Oswald Buddenhagen <ossi@kde.org>
    Copyright (c) 2006 Thiago Macieira <thiago@kde.org>
    Copyright (C) 2008 Aaron Seigo <aseigo@kde.org>

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

#include "ktoolinvocation.h"

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kmimetypetrader.h>

#include "kconfig.h"
#include "kshell.h"
#include "kmacroexpander.h"
#include "klocalizedstring.h"
#include "kmessage.h"
#include "kservice.h"

#include <QDebug>
#include <qstandardpaths.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtDBus/QtDBus>

static QStringList splitEmailAddressList( const QString & aStr )
{
    // This is a copy of KPIM::splitEmailAddrList().
    // Features:
    // - always ignores quoted characters
    // - ignores everything (including parentheses and commas)
    //   inside quoted strings
    // - supports nested comments
    // - ignores everything (including double quotes and commas)
    //   inside comments

    QStringList list;

    if (aStr.isEmpty())
        return list;

    QString addr;
    uint addrstart = 0;
    int commentlevel = 0;
    bool insidequote = false;

    for (int index=0; index<aStr.length(); index++) {
        // the following conversion to latin1 is o.k. because
        // we can safely ignore all non-latin1 characters
        switch (aStr[index].toLatin1()) {
        case '"' : // start or end of quoted string
            if (commentlevel == 0)
                insidequote = !insidequote;
            break;
        case '(' : // start of comment
            if (!insidequote)
                commentlevel++;
            break;
        case ')' : // end of comment
            if (!insidequote) {
                if (commentlevel > 0)
                    commentlevel--;
                else {
                    //qDebug() << "Error in address splitting: Unmatched ')'"
                    //          << endl;
                    return list;
                }
            }
            break;
        case '\\' : // quoted character
            index++; // ignore the quoted character
            break;
        case ',' :
            if (!insidequote && (commentlevel == 0)) {
                addr = aStr.mid(addrstart, index-addrstart);
                if (!addr.isEmpty())
                    list += addr.simplified();
                addrstart = index+1;
            }
            break;
        }
    }
    // append the last address to the list
    if (!insidequote && (commentlevel == 0)) {
        addr = aStr.mid(addrstart, aStr.length()-addrstart);
        if (!addr.isEmpty())
            list += addr.simplified();
    }
    //else
    //  qDebug() << "Error in address splitting: "
    //            << "Unexpected end of address list"
    //            << endl;

    return list;
}

void KToolInvocation::invokeMailer(const QString &_to, const QString &_cc, const QString &_bcc,
                                   const QString &subject, const QString &body,
                                   const QString & /*messageFile TODO*/, const QStringList &attachURLs,
                                   const QByteArray& startup_id )
{
    if (!isMainThreadActive())
        return;

    KConfig config(QString::fromLatin1("emaildefaults"));
    KConfigGroup defaultsGrp(&config, "Defaults");

    QString group = defaultsGrp.readEntry("Profile","Default");

    KConfigGroup profileGrp(&config, QString::fromLatin1("PROFILE_%1").arg(group) );
    QString command = profileGrp.readPathEntry("EmailClient", QString());

    QString to, cc, bcc;
    if (command.isEmpty() || command == QLatin1String("kmail")
        || command.endsWith(QLatin1String("/kmail")))
    {
        command = QLatin1String("kmail --composer -s %s -c %c -b %b --body %B --attach %A -- %t");
        if ( !_to.isEmpty() )
        {
            QUrl url;
            url.setScheme(QLatin1String("mailto"));
            url.setPath(_to);
            to = QString::fromLatin1(url.toEncoded());
        }
        if ( !_cc.isEmpty() )
        {
            QUrl url;
            url.setScheme(QLatin1String("mailto"));
            url.setPath(_cc);
            cc = QString::fromLatin1(url.toEncoded());
        }
        if ( !_bcc.isEmpty() )
        {
            QUrl url;
            url.setScheme(QLatin1String("mailto"));
            url.setPath(_bcc);
            bcc = QString::fromLatin1(url.toEncoded());
        }
    } else {
        to = _to;
        cc = _cc;
        bcc = _bcc;
        if( !command.contains( QLatin1Char('%') ))
            command += QLatin1String(" %u");
    }

    if (profileGrp.readEntry("TerminalClient", false))
    {
        KConfigGroup confGroup( KSharedConfig::openConfig(), "General" );
        QString preferredTerminal = confGroup.readPathEntry("TerminalApplication", QString::fromLatin1("konsole"));
        command = preferredTerminal + QString::fromLatin1(" -e ") + command;
    }

    QStringList cmdTokens = KShell::splitArgs(command);
    QString cmd = cmdTokens.takeFirst();

    QUrl url;
    QUrlQuery query;
    if (!to.isEmpty())
    {
        QStringList tos = splitEmailAddressList( to );
        url.setPath( tos.first() );
        tos.erase( tos.begin() );
        for (QStringList::ConstIterator it = tos.constBegin(); it != tos.constEnd(); ++it)
            query.addQueryItem(QString::fromLatin1("to"), *it);
    }
    const QStringList ccs = splitEmailAddressList( cc );
    for (QStringList::ConstIterator it = ccs.constBegin(); it != ccs.constEnd(); ++it)
        query.addQueryItem(QString::fromLatin1("cc"), *it);
    const QStringList bccs = splitEmailAddressList( bcc );
    for (QStringList::ConstIterator it = bccs.constBegin(); it != bccs.constEnd(); ++it)
        query.addQueryItem(QString::fromLatin1("bcc"), *it);
    for (QStringList::ConstIterator it = attachURLs.constBegin(); it != attachURLs.constEnd(); ++it)
        query.addQueryItem(QString::fromLatin1("attach"), *it);
    if (!subject.isEmpty())
        query.addQueryItem(QString::fromLatin1("subject"), subject);
    if (!body.isEmpty())
        query.addQueryItem(QString::fromLatin1("body"), body);

    url.setQuery(query);

    if ( ! (to.isEmpty() && (!url.hasQuery())) )
        url.setScheme(QString::fromLatin1("mailto"));

    QHash<QChar, QString> keyMap;
    keyMap.insert(QLatin1Char('t'), to);
    keyMap.insert(QLatin1Char('s'), subject);
    keyMap.insert(QLatin1Char('c'), cc);
    keyMap.insert(QLatin1Char('b'), bcc);
    keyMap.insert(QLatin1Char('B'), body);
    keyMap.insert(QLatin1Char('u'), url.toString());

    QString attachlist = attachURLs.join(QString::fromLatin1(","));
    attachlist.prepend(QLatin1Char('\''));
    attachlist.append(QLatin1Char('\''));
    keyMap.insert(QLatin1Char('A'), attachlist);

    for (QStringList::Iterator it = cmdTokens.begin(); it != cmdTokens.end(); )
    {
        if (*it == QLatin1String("%A"))
        {
            if (it == cmdTokens.begin()) // better safe than sorry ...
                continue;
            QStringList::ConstIterator urlit = attachURLs.begin();
            QStringList::ConstIterator urlend = attachURLs.end();
            if ( urlit != urlend )
            {
                QStringList::Iterator previt = it;
                --previt;
                *it = *urlit;
                ++it;
                while ( ++urlit != urlend )
                {
                    cmdTokens.insert( it, *previt );
                    cmdTokens.insert( it, *urlit );
                }
            } else {
                --it;
                it = cmdTokens.erase( cmdTokens.erase( it ) );
            }
        } else {
            *it = KMacroExpander::expandMacros(*it, keyMap);
            ++it;
        }
    }

    QString error;
    // TODO this should check if cmd has a .desktop file, and use data from it, together
    // with sending more ASN data
    if (kdeinitExec(cmd, cmdTokens, &error, NULL, startup_id ))
    {
      KMessage::message(KMessage::Error,
                      i18n("Could not launch the mail client:\n\n%1", error),
                      i18n("Could not launch Mail Client"));
    }
}

void KToolInvocation::invokeBrowser( const QString &url, const QByteArray& startup_id )
{
    if (!isMainThreadActive())
        return;

    QStringList args;
    args << url;
    QString error;

    // This method should launch a webbrowser, preferably without doing a mimetype
    // check first, like KRun (i.e. kde-open) would do.

    // In a KDE session, honour BrowserApplication if set, otherwise use preferred app for text/html if any,
    // otherwise xdg-open, otherwise kde-open (which does a mimetype check first though).

    // Outside KDE, call xdg-open if present, otherwise fallback to the above logic.

    QString exe; // the binary we are going to launch.

    const QString xdg_open = QStandardPaths::findExecutable(QString::fromLatin1("xdg-open"));
    if (qgetenv("KDE_FULL_SESSION").isEmpty()) {
        exe = xdg_open;
    }

    if (exe.isEmpty()) {
        // We're in a KDE session (or there's no xdg-open installed)
        KConfigGroup config(KSharedConfig::openConfig(), "General");
        const QString browserApp = config.readPathEntry("BrowserApplication", QString());
        if (!browserApp.isEmpty()) {
            exe = browserApp;
            if (exe.startsWith(QLatin1Char('!'))) {
                exe = exe.mid(1); // Literal command
                QStringList cmdTokens = KShell::splitArgs(exe);
                exe = cmdTokens.takeFirst();
                args = cmdTokens + args;
            } else {
                // desktop file ID
                KService::Ptr service = KService::serviceByStorageId(exe);
                if (service) {
                    //qDebug() << "Starting service" << service->entryPath();
                    if (startServiceByDesktopPath(service->entryPath(), args,
                            &error, 0, 0, startup_id)) {
                        KMessage::message(KMessage::Error,
                                          // TODO: i18n("Could not launch %1:\n\n%2", exe, error),
                                          i18n("Could not launch the browser:\n\n%1", error),
                                          i18n("Could not launch Browser"));
                    }
                    return;
                }
            }
        } else {
            const KService::Ptr htmlApp = KMimeTypeTrader::self()->preferredService(QLatin1String("text/html"));
            if (htmlApp) {
                // WORKAROUND: For bugs 264562 and 265474:
                // In order to correctly handle non-HTML urls we change the service
                // desktop file name to "kfmclient.desktop" whenever the above query
                // returns "kfmclient_html.desktop".Otherwise, the hard coded mime-type
                // "text/html" mime-type parameter in the kfmclient_html will cause all
                // URLs to be treated as if they are HTML page.
                QString entryPath = htmlApp->entryPath();
                if (entryPath.endsWith(QLatin1String("kfmclient_html.desktop"))) {
                    entryPath.remove(entryPath.length()-13, 5);
                }
                QString error;
                int pid = 0;
                int err = startServiceByDesktopPath(entryPath, url, &error, 0, &pid, startup_id);
                if (err != 0) {
                    KMessage::message(KMessage::Error,
                                      // TODO: i18n("Could not launch %1:\n\n%2", htmlApp->exec(), error),
                                      i18n("Could not launch the browser:\n\n%1", error),
                                      i18n("Could not launch Browser"));
                } else { // success
                    return;
                }
            } else {
                exe = xdg_open;
            }
        }
    }

    if (exe.isEmpty()) {
        exe = QString::fromLatin1("kde-open"); // it's from kdebase-runtime, it has to be there.
    }

    //qDebug() << "Using" << exe << "to open" << url;
    if (kdeinitExec(exe, args, &error, NULL, startup_id ))
    {
        KMessage::message(KMessage::Error,
                          // TODO: i18n("Could not launch %1:\n\n%2", exe, error),
                          i18n("Could not launch the browser:\n\n%1", error),
                          i18n("Could not launch Browser"));
    }
}

void KToolInvocation::invokeTerminal(const QString &command,
                                     const QString &workdir,
                                     const QByteArray &startup_id)
{
    if (!isMainThreadActive()) {
        return;
    }

    KConfigGroup confGroup( KSharedConfig::openConfig(), "General" );
    QString exec = confGroup.readPathEntry("TerminalApplication", QString::fromLatin1("konsole"));

    if (!command.isEmpty()) {
        if (exec == QLatin1String("konsole")) {
            exec += QString::fromLatin1(" --noclose");
        } else if (exec == QLatin1String("xterm")) {
            exec += QString::fromLatin1(" -hold");
        }

        exec += QString::fromLatin1(" -e ") + command;
    }

    QStringList cmdTokens = KShell::splitArgs(exec);
    QString cmd = cmdTokens.takeFirst();

    if (exec == QLatin1String("konsole") && !workdir.isEmpty()) {
        cmdTokens << QString::fromLatin1("--workdir");
        cmdTokens << workdir;
        // For other terminals like xterm, we'll simply change the working
        // directory before launching them, see below.
    }

    QString error;
    if (self()->startServiceInternal("kdeinit_exec_with_workdir",
                                    cmd, cmdTokens, &error, 0, NULL, startup_id, false, workdir)) {
      KMessage::message(KMessage::Error,
                      i18n("Could not launch the terminal client:\n\n%1", error),
                      i18n("Could not launch Terminal Client"));
    }
}
