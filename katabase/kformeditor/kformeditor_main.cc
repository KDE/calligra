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

#include <qprinter.h>

#include <opAutoLoader.h>

#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <koQueryTypes.h>

#include "kformeditor_shell.h"
#include "kformeditor_main.h"
#include "kformeditor_doc.h"
#include "kformeditor.h"

KOFFICE_DOCUMENT_FACTORY( KformEditorDoc, KformEditorFactory, KformEditor::DocumentFactory_skel )
typedef OPAutoLoader<KformEditorFactory> KformEditorAutoLoader;

KformEditorApp::KformEditorApp( int& argc, char** argv ) 
  : KoApplication( argc, argv, "kformeditor" )
{
}

KoMainWindow* KformEditorApp::createNewShell()
{
  return new KformEditorShell;
}

int main( int argc, char** argv )
{
  KformEditorAutoLoader loader( "IDL:KformEditor/DocumentFactory:1.0", "KformEditor" );
  KformEditorApp app( argc, argv );

  app.exec();

  return 0;
}

#include "kformeditor_main.moc"
