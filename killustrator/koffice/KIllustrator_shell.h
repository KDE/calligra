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

#include <ktopwidget.h>

#include <default_shell_impl.h>
#include <document_impl.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"

class KIllustratorShell : public KTopLevelWidget,
			  virtual public PartShell_impl,
			  virtual KIllustrator::Shell_skel {
  Q_OBJECT
public:
  KIllustratorShell ();
  ~KIllustratorShell ();

  // set a document
  void setDocument (KIllustratorDocument* doc);

  // open a document
  bool openDocument (const char* filename);

  // save the document
  bool saveDocument (const char* filename, const char* fmt);

  void enableMenuBar ();
  void enableToolBars ();

  void fileNew ();
  void fileOpen ();
  void fileSave ();
  void fileSaveAs ();
  void fileClose ();
  void fileQuit ();
  void filePrint ();
  void helpAbout ();

  void editCut ();
  void editCopy ();
  void editPaste ();

  void setZoomFactor (const char* factor);

  void cleanUp ();

protected:
  void enableButtons (bool enable);

  Document_ref m_rDoc;
  OPParts::View_var m_vView;

  MenuBar_ref m_rMenuBar;
  CORBA::Long m_idMenuFile, m_idMenuFile_New, m_idMenuFile_Open,
    m_idMenuFile_Save, m_idMenuFile_SaveAs, m_idMenuFile_Close,
    m_idMenuFile_Exit;
  ToolBar_ref m_rToolBarFile;
  CORBA::Long m_idButtonFile_Open, m_idButtonFile_Save, 
    m_idButtonFile_Print;
  CORBA::Long m_idButtonEdit_Copy, m_idButtonEdit_Cut, 
    m_idButtonEdit_Paste;
  CORBA::Long m_idComboFile_Zoom;
  CORBA::Long m_idMenuHelp_About;
};

#endif
