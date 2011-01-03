/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <kcmdlineargs.h>

#include <KUniqueApplication>
#include <kiconloader.h>
#include <memory>

#include "AboutData.h"
#include "MainWindow.h"
#include "import/Global.h"
#include "RootSection.h"
#include "SectionsIO.h"

int main( int argc, char **argv )
{
  KAboutData* about = newBrainDumpAboutData();
  KCmdLineArgs::init( argc, argv, about );

  KCmdLineOptions options;
  options.add("no-unique", ki18n("Use this option when debugging so that Braindump is not launched as a KUniqueApplication"));
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication* app = 0;
  
  if(KCmdLineArgs::parsedArgs()->isSet("-unique")) {
    app = new KUniqueApplication;
  } else {
    app = new KApplication;
  }

  KIconLoader::global()->addAppDir("koffice");
  Global::initialize();

  KComponentData* m_documentData = new KComponentData(about);
  
  RootSection* doc = new RootSection;
  
  MainWindow* window = new MainWindow(doc, *m_documentData);
  window->setVisible(true);
  
  app->exec();

  // Ensure the root section is saved
  doc->sectionsIO()->save();
  
  delete doc;
  app->exit(0);
}
