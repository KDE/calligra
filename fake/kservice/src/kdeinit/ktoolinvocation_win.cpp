/*
   This file is part of the KDE libraries
   Copyright (C) 2004-2008 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2006 Ralf Habacker <ralf.habacker@freenet.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

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

#include "kmessage.h"
#include "klocalizedstring.h"

#include <QUrl>
#include <QProcess>
#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QtDBus/QtDBus>

#include "windows.h"
#include "shellapi.h"


void KToolInvocation::invokeBrowser( const QString &url, const QByteArray& startup_id )
{
#ifndef _WIN32_WCE
    QString sOpen = QString::fromLatin1("open");
    ShellExecuteW(0, ( LPCWSTR )sOpen.utf16(), ( LPCWSTR )url.utf16(), 0, 0, SW_NORMAL);
#else
    SHELLEXECUTEINFO cShellExecuteInfo = {0};
    cShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    cShellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    cShellExecuteInfo.hwnd = NULL;
    cShellExecuteInfo.lpVerb = L"Open";
    cShellExecuteInfo.lpFile = ( LPCWSTR )url.utf16();
    cShellExecuteInfo.nShow = SW_SHOWNORMAL;
    ShellExecuteEx(&cShellExecuteInfo);
#endif
}

void KToolInvocation::invokeMailer(const QString &_to, const QString &_cc, const QString &_bcc,
                                const QString &subject, const QString &body,
                                const QString & /*messageFile TODO*/, const QStringList &attachURLs,
                                const QByteArray& startup_id )
{
  QUrl url(QLatin1String("mailto:")+_to);
  url.setQuery(QLatin1String("?subject=")+subject);
  url.addQueryItem(QLatin1String("cc"), _cc);
  url.addQueryItem(QLatin1String("bcc"), _bcc);
  url.addQueryItem(QLatin1String("body"), body);
  foreach (const QString& attachURL, attachURLs)
    url.addQueryItem(QLatin1String("attach"), attachURL);

#ifndef _WIN32_WCE
   QString sOpen = QLatin1String( "open" );
   ShellExecuteW(0, ( LPCWSTR )sOpen.utf16(), ( LPCWSTR )url.url().utf16(), 0, 0, SW_NORMAL);
#else
    SHELLEXECUTEINFO cShellExecuteInfo = {0};
    cShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    cShellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    cShellExecuteInfo.hwnd = NULL;
    cShellExecuteInfo.lpVerb = L"Open";
    cShellExecuteInfo.lpFile = ( LPCWSTR )url.url().utf16();
    cShellExecuteInfo.nShow = SW_SHOWNORMAL;
    ShellExecuteEx(&cShellExecuteInfo);
#endif
}

void KToolInvocation::invokeTerminal(const QString &command, const QString &workdir, const QByteArray &startup_id)
{
    //TODO
}
