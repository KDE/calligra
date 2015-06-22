/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICOMMANDLINEOPTIONS_H
#define KEXICOMMANDLINEOPTIONS_H

#include <KLocalizedString>

#include <QCommandLineOption>

class QCommandLineParser;

//! Command line options
class KexiCommandLineOptions
{
public:
    KexiCommandLineOptions(QCommandLineParser *parser);

//    QCommandLineOption help;
//    QCommandLineOption version;
    QCommandLineOption createDb;
    QCommandLineOption createAndOpenDb;
    QCommandLineOption dropDb;
    QCommandLineOption dbDriver;
    QCommandLineOption fileType;
    QCommandLineOption connectionShortcut;
    QCommandLineOption readOnly;
    QCommandLineOption userMode;
    QCommandLineOption designMode;
    QCommandLineOption showNavigator;
    QCommandLineOption hideMenu;
    QCommandLineOption open;
    QCommandLineOption design;
    QCommandLineOption editText;
    QCommandLineOption execute;
    QCommandLineOption newObject;
    QCommandLineOption print;
    QCommandLineOption printPreview;
    QCommandLineOption user;
    QCommandLineOption host;
    QCommandLineOption port;
    QCommandLineOption localSocket;
    QCommandLineOption skipConnDialog;
    QCommandLineOption fullScreen;
};

#endif
