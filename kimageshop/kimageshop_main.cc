/*
 *  kimageshop_main.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch    <mkoch@kde.org>
 *                1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qprinter.h>

#include <opAutoLoader.h>

#include <koFactory.h>
#include <koDocument.h>
#include <koQueryTypes.h>

#include "kimageshop_shell.h"
#include "kimageshop_main.h"
#include "kimageshop_doc.h"
#include "kimageshop.h"

KOFFICE_DOCUMENT_FACTORY( KImageShopDoc, KImageShopFactory, KImageShop::DocumentFactory_skel )
typedef OPAutoLoader<KImageShopFactory> KImageShopAutoLoader;

KImageShopApp::KImageShopApp(int& argc, char** argv) : KoApplication(argc, argv, "kimageshop"){}

KoMainWindow* KImageShopApp::createNewShell()
{
  return new KImageShopShell;
}

int main(int argc, char** argv)
{
  KImageShopAutoLoader loader("IDL:KImageShop/DocumentFactory:1.0", "KImageShop");
  KImageShopApp app(argc, argv);

  app.exec();

  return 0;
}

#include "kimageshop_main.moc"
