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

#include "KIllustrator_shell.h"
#include "KIllustrator_shell.moc"

#include <op_app.h>
#include <qmsgbox.h>

#include <utils.h>

KIllustratorShell::KIllustratorShell () {
  setWidget ((KTopLevelWidget *) this);
}

KIllustratorShell::~KIllustratorShell () {
  cleanUp ();
}

void KIllustratorShell::setDocument (KIllustratorDocument* doc) {
  m_rDoc = OPParts::Document::_duplicate (doc);
  
  m_vView = doc->createView ();
  m_vView->setPartShell (this);
  setRootPart (m_vView);
}

bool KIllustratorShell::openDocument (const char* filename) {
  return false;
}

bool KIllustratorShell::saveDocument (const char* filename, const char* fmt) {
  return false;
}

void KIllustratorShell::enableMenuBar () {
  PartShell_impl::enableMenuBar ();

  m_rMenuBar = m_pMenuBarFactory->createMenuBar (this);
  m_idMenuFile = m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&File")));
  
  m_idMenuFile_New = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&New...")), 
			    m_idMenuFile, this, CORBA::string_dup ("fileNew"));
  m_idMenuFile_Open = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Open...")), 
			    m_idMenuFile, this, 
			    CORBA::string_dup ("fileOpen"));
  m_rMenuBar->insertSeparator (m_idMenuFile);
  m_idMenuFile_Save = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Save")), 
			    m_idMenuFile, this, 
			    CORBA::string_dup ("fileSave"));
  m_idMenuFile_SaveAs = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("S&ave as...")), 
			    m_idMenuFile, this, 
			    CORBA::string_dup ("fileSaveAs"));
  m_idMenuFile_Close = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Close")), 
			    m_idMenuFile, this, 
			    CORBA::string_dup ("fileClose"));
  m_rMenuBar->insertSeparator (m_idMenuFile);
  m_idMenuFile_Exit = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("E&xit")), 
			    m_idMenuFile, this, 
			    CORBA::string_dup ("fileExit"));

  CORBA::Long id = m_rMenuBar->helpMenuId ();
  m_idMenuHelp_About = 
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("About")), 
			     id, this, 
			     CORBA::string_dup ("helpAbout"));
  fillMenuBar ();
}

void KIllustratorShell::enableToolBars () {
  PartShell_impl::enableToolBars ();

  m_rToolBarFile = 
    m_pToolBarFactory->createToolBar (this, CORBA::string_dup ("File"));
  m_rToolBarFile->setFileToolBar (true);

  QString tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/filenew.xpm";
  QString pix = loadPixmap (tmp);
  m_idButtonFile_Open = 
      m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				    CORBA::string_dup (i18n ("New")),
				    this, CORBA::string_dup ("fileNew" ));

  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/fileopen.xpm";
  pix = loadPixmap (tmp);
  m_idButtonFile_Open = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Open")),
				  this, CORBA::string_dup ("fileOpen" ));
  
  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/filefloppy.xpm";
  pix = loadPixmap (tmp);
  m_idButtonFile_Save = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Save")),
				  this, CORBA::string_dup ("fileSave"));
  m_rToolBarFile->setItemEnabled (m_idButtonFile_Save, false);
  
  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/fileprint.xpm";
  pix = loadPixmap (tmp);
  m_idButtonFile_Print = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Print")),
				  this, CORBA::string_dup ("filePrint"));
  m_rToolBarFile->setItemEnabled (m_idButtonFile_Print, false);
    
  m_rToolBarFile->insertSeparator ();

  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editcopy.xpm";
  pix = loadPixmap (tmp);
  m_idButtonEdit_Copy = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Copy")),
				  this, CORBA::string_dup ("editCopy"));
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Copy, false);
    
  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editpaste.xpm";
  pix = loadPixmap (tmp);
  m_idButtonEdit_Paste = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Paste")),
				  this, CORBA::string_dup ("editPaste"));
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Paste, false);
    
  tmp = kapp->kde_toolbardir ().copy ();
  tmp += "/editcut.xpm";
  pix = loadPixmap (tmp);
  m_idButtonEdit_Cut = 
    m_rToolBarFile->insertButton (CORBA::string_dup (pix), 
				  CORBA::string_dup (i18n ("Cut")),
				  this, CORBA::string_dup ("editCut"));
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Cut, false);
    
  m_rToolBarFile->insertSeparator ();

  m_idComboFile_Zoom =
    m_rToolBarFile->insertCombo (true, 
				 CORBA::string_dup (i18n ("Zoom Factor")),
				 50, this, 
				 CORBA::string_dup ("setZoomFactor"));
  float zFactors [] = { 0.5, 1.0, 1.5, 2.0, 4.0 };
  for (int i = 0; i < 5; i++) {
    char buf[10];
    sprintf (buf, "%3.0f%%", zFactors[i] * 100);
    m_rToolBarFile->insertComboItem (m_idComboFile_Zoom, 
				     CORBA::string_dup (buf), -1);
  }
  m_rToolBarFile->setCurrentComboItem (m_idComboFile_Zoom, 1);

  m_rToolBarFile->setItemEnabled (m_idComboFile_Zoom, false);
}

void KIllustratorShell::enableButtons (bool enable) {
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Copy, enable);
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Paste, enable);
  m_rToolBarFile->setItemEnabled (m_idButtonEdit_Cut, enable);
  m_rToolBarFile->setItemEnabled (m_idComboFile_Zoom, enable);
}

void KIllustratorShell::fileQuit () {
  opapp_orb->shutdown (true);
//  exit (0);
}

void KIllustratorShell::fileNew () {
  m_rDoc = 0L;
  m_rDoc = OPParts::Document::_duplicate (new KIllustratorDocument);
  if (! m_rDoc->init ()) {
    QMessageBox::critical (this, i18n ("KIllustrator Error"),
			   i18n ("Could not initialize document !"),
			   i18n ("Ok"));
    return;
  }

  m_vView = m_rDoc->createView ();
  m_vView->setPartShell (this);
  setRootPart (m_vView);
  enableButtons (true);
}

void KIllustratorShell::fileOpen () {
}

void KIllustratorShell::fileSave () {
}

void KIllustratorShell::fileSaveAs () {
}

void KIllustratorShell::fileClose () {
}

void KIllustratorShell::filePrint () {
}

void KIllustratorShell::editCut () {
}

void KIllustratorShell::editCopy () {
}

void KIllustratorShell::editPaste () {
}

void KIllustratorShell::helpAbout () {
}
  
void KIllustratorShell::cleanUp () {
  if (m_bIsClean)
    return;

  m_vView = 0L;
  PartShell_impl::cleanUp ();
  m_rDoc = 0L;
}

void KIllustratorShell::setZoomFactor (const char* factor) {
}
  
