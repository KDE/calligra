/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <KoApplication.h>
#include <KoDocument.h>
#include <KoMainWindow.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <dcopclient.h>
#include "example_aboutdata.h"
#include <koffice_export.h>

static const KCmdLineOptions options[]=
{
	{"+[file]", I18N_NOOP("File to open"),0},
	KCmdLineLastOption
};

extern "C" EXAMPLE_EXPORT int kdemain( int argc, char **argv )
{
    KCmdLineArgs::init( argc, argv, newExampleAboutData() );
    KCmdLineArgs::addCmdLineOptions( options );
    KoApplication app;

    if (!app.start()) // parses command line args, create initial docs and shells
	return 1;
    return app.exec();
}

