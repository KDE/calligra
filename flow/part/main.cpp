/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#include <kcmdlineargs.h>

#include <KoApplication.h>
#include <memory>

#include "FlowAboutData.h"
#include "FlowDocument.h"

extern "C" KDE_EXPORT int kdemain( int argc, char **argv )
{
  std::auto_ptr<KAboutData> about( newFlowAboutData() );
  KCmdLineArgs::init( argc, argv, about.get() );

  KCmdLineOptions options;
  options.add("+[file]", ki18n("File to open"));
  KCmdLineArgs::addCmdLineOptions( options );

  KoApplication app(MIME_TYPE);

  if (!app.start())
      return 1;
  app.exec();

  return 0;
}
