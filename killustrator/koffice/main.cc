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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koApplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <dcopclient.h>

static const char* description=I18N_NOOP("KOffice Illustrator Program");
static const char* version="0.1";

static const KCmdLineOptions options[]=
{
	{"+[file]", I18N_NOOP("File To Open"),0},
	{0,0,0}
};

int main( int argc, char **argv )
{
    KAboutData aboutData( "killustrator", I18N_NOOP("KIllustrator"),
        version, description, KAboutData::License_GPL,
        "(c) 1998-2000, Kai-Uwe Sattler");
    aboutData.addAuthor("Kai-Uwe Sattler",0, "kus@iti.cs.uni-magdeburg.de");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KoApplication app;

    app.dcopClient()->attach();
    app.dcopClient()->registerAs( "killustrator" );

    app.start();
    app.exec();

    return 0;
}
