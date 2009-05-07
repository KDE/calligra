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

#include <KApplication>
#include <memory>

#include "AboutData.h"
#include "Document.h"
#include <KoMainWindow.h>

int main( int argc, char **argv )
{
  KAboutData* about = newBrainDumpAboutData();
  KCmdLineArgs::init( argc, argv, about );

  KCmdLineOptions options;
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication app;

  Document* doc = new Document(0, 0);
  KoMainWindow* shell = new KoMainWindow(doc->componentData());
  doc->addShell(shell);
  shell->setRootDocument(doc);
  shell->setVisible(true);
  app.exec();

  return 0;
}
