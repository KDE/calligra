/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <koApplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kiconloader.h>

#include <dcopclient.h>

#include <qlabel.h>
#include <qapplication.h>
#include <qdatetime.h>

#include "kivio_aboutdata.h"

#include <unistd.h>

static const KCmdLineOptions options[]=
{
	{"+[file]", I18N_NOOP("File To Open"),0},
	{0,0,0}
};

int main( int argc, char **argv )
{

    KCmdLineArgs::init( argc, argv, newKivioAboutData() );
    KCmdLineArgs::addCmdLineOptions( options );

    KoApplication app;

    QLabel* splash = 0;
    if ( bool showSplash = true ) {
      QString icon;
      int h = QTime::currentTime().hour();
      if ( h >= 5 && h < 11 )
        icon = "kiviosplash";
      if ( h >= 11 && h < 17 )
        icon = "kiviosplash";
      if ( h >= 17 && h < 23 )
        icon = "kiviosplash";
      if ( h == 23 || h < 5 )
        icon = "kiviosplash";

      splash = new QLabel(0, "splash", Qt::WDestructiveClose | Qt::WStyle_Customize | Qt::WStyle_NoBorder);
      splash->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
      splash->setPixmap(BarIcon(icon));
      splash->adjustSize();
      splash->setCaption( "Kivio by theKompany.com");
      QRect r = QApplication::desktop()->geometry();
      splash->move( r.center() - splash->rect().center() );
      splash->show();
      splash->repaint(false);
      QApplication::flushX();
    }

    app.dcopClient()->attach();
    app.dcopClient()->registerAs("kivio");

    if (!app.start()) {
      delete splash;
      return 1;
    }
    delete splash;
    app.exec();
    return 0;
}
