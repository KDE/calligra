/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <koApplication.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <dcopclient.h>

#include "kontour_about.h"

static const KCmdLineOptions options[] =
{
  {"+[file]", I18N_NOOP("File to open"), 0},
  {0, 0, 0}
};

int main(int argc, char **argv)
{
  KCmdLineArgs::init(argc, argv, newKontourAboutData());
  KCmdLineArgs::addCmdLineOptions(options);

  KoApplication app;
  app.dcopClient()->attach();
  app.dcopClient()->registerAs("kontour");

  if(!app.start())
    return 1;
  return app.exec();
}
