/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

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

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>


int main(int argc, char **argv) 
{
  KAboutData aboutData("wfd", NULL, ki18n("Web Forms Daemon"),
		       "0.1", ki18n("Exports Kexi Forms to standard HTML pages"),
		       KAboutData::License_GPL_V2,
		       ki18n("(C) Copyright 2008, Lorenzo Villani"));

  KCmdLineArgs::init(argc, argv, &aboutData);
  

  KCmdLineOptions options;
  options.add("port", ki18n("Listen port"));
  options.add("address", ki18n("Listen address"));
  options.add("file", ki18n("Path to Kexi database file"));

  KCmdLineArgs::addCmdLineOptions(options);
  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
  
  return 0;
}

