/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

//#include <qapplication.h>
//#include <main/keximainwindowimpl.h>
#include <KoApplication.h>
#include <kcmdlineargs.h>

#include "KexiAboutData.h"

static const KCmdLineOptions options[]=
{
    {"+[file]", I18N_NOOP("File to open"), 0},
    KCmdLineLastOption
};

extern "C" KDE_EXPORT int kdemain(int argc, char *argv[])
{
    KCmdLineArgs::init( argc, argv, newAboutData() );
    KCmdLineArgs::addCmdLineOptions( options );

    KoApplication app;
    if(! app.start() )
        return 1;
    app.exec();
    return 0;

    /*
    int result = KexiMainWindowImpl::create(argc, argv);
    if (!qApp)
        return result;

    result = qApp->exec();
    delete qApp;
    return result;
    */
}
