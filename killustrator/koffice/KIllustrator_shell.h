/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef KIllustrator_shell_h_
#define KIllustrator_shell_h_

#include <koMainWindow.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"
#include "KIllustrator_view.h"

#include <list>

class KIllustratorShell : public KoMainWindow {
  Q_OBJECT
public:
  // C++
  KIllustratorShell ();
  ~KIllustratorShell ();

  // C++
  virtual void cleanUp ();
  void setDocument (KIllustratorDocument* doc);
  virtual void createFileMenu (OPMenuBar* mbar);

  // C++
  virtual bool newDocument ();
  virtual bool openDocument (const char* filename);
  virtual bool saveDocument ();
  virtual bool closeDocument ();
  virtual bool closeAllDocuments ();

protected slots:
  void slotFileNew ();
  void slotFileOpen ();
  void slotFileSave ();
  void slotFileSaveAs ();
  void slotFileImport ();
  void slotFileExport ();
  void slotFilePrint ();
  void slotFileInfo ();
  void slotFileClose ();
  void slotFileQuit ();
  void slotHelpAbout ();

protected:
  // C++
  virtual KOffice::Document_ptr document ();
  virtual KOffice::View_ptr view ();
  void releaseDocument ();
  virtual bool printDlg ();
  int documentCount ();
  bool requestClose ();
  bool isModified ();

  KIllustratorDocument* m_pDoc;
  KIllustratorView* m_pView;

  int m_idMenuFile_Import;
  int m_idMenuFile_Export;
  int m_idMenuFile_Info;

  static list<KIllustratorShell*>* s_lstShells;
  static bool previewHandlerRegistered;
};

#endif
