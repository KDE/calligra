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

#include "KIllustrator_view.h"

#include <kfiledialog.h>
#include <qmsgbox.h>

list<KIllustratorShell*>* KIllustratorShell::s_lstShells = 0L;

KIllustratorShell::KIllustratorShell () {
  m_pDoc = 0L;
  m_pView = 0L;

  if (s_lstShells == 0L)
    s_lstShells = new list<KIllustratorShell*>;

  s_lstShells->push_back (this);
}

KIllustratorShell::~KIllustratorShell () {
  cleanUp ();
  s_lstShells->remove (this);
}

bool KIllustratorShell::isModified () {
  return (m_pDoc != 0L ? m_pDoc->isModified () : false);
}

void KIllustratorShell::cleanUp () {
  releaseDocument ();
  KoMainWindow::cleanUp ();
}

void KIllustratorShell::setDocument (KIllustratorDocument* doc) {
  if (m_pDoc)
    releaseDocument ();

  m_pDoc = doc;
  m_pDoc->_ref ();
  m_pView = doc->createKIllustratorView ();
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());

  setRootPart (m_pView->id ());
  interface ()->setActivePart (m_pView->id ());

  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Quit, true);
  }
  
//  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
//  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);
}

bool KIllustratorShell::newDocument () {
  if (m_pDoc) {
    KIllustratorShell *shell = new KIllustratorShell ();
    shell->show ();
    shell->newDocument ();
    return true;
  }
  
  m_pDoc = new KIllustratorDocument ();
  if (! m_pDoc->init ()) {
    cerr << "ERROR: Could not initialize document" << endl;
    return false;
  }

  cout << "create KIllustratorView ..." << endl;
  m_pView = m_pDoc->createKIllustratorView ();
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());
  cout << "setMainWindow done ..." << endl;

  setRootPart (m_pView->id());
  interface ()->setActivePart (m_pView->id ());
  cout << "setActivePart done ..." << endl;
  
  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Quit, true);
  }
  
  //  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
  //  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);

  return true;
}

bool KIllustratorShell::openDocument (const char* url, const char* fmt) {
  if (fmt == 0L || *fmt == '\0')
    fmt = "application/x-killustrator";
  
  if (m_pDoc && m_pDoc->objectCount () == 0)
    releaseDocument ();
  else if (m_pDoc && m_pDoc->objectCount  ()> 0) {
    KIllustratorShell *shell = new KIllustratorShell ();
    shell->show ();
    return shell->openDocument (url, fmt);
  }
  
  m_pDoc = new KIllustratorDocument ();
  if (! m_pDoc->loadFromURL (url, fmt))
    return false;

  m_pView = m_pDoc->createKIllustratorView ();
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());

  setRootPart (m_pView->id());
  interface ()->setActivePart (m_pView->id ());
  
  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Quit, true);
  }
  
  //  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
  //  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);
  m_pDoc->setURL (url);
  
  return true;
}

bool KIllustratorShell::saveDocument (const char* _url, const char* fmt) {
  assert (m_pDoc != 0L);

  CORBA::String_var url;
  if (_url == 0L || *_url == '\0') {
    url = m_pDoc->url ();
    _url = url.in ();
  }
  
  QString file;
  if (_url == 0L || *_url == '\0') {
    file = KFileDialog::getSaveFileName (getenv ("HOME"));

    if (file.isNull ())
      return false;
    _url = file.data ();
    m_pDoc->setURL (_url);
  }
  
  if (fmt == 0L || *fmt == '\0')
    fmt = "application/x-killustrator";
  
  return m_pDoc->saveToURL (_url, fmt);
}

bool KIllustratorShell::closeDocument () {
  if (isModified ()) {
    if (! requestClose ())
      return false;
  }

  return true;
}

bool KIllustratorShell::closeAllDocuments () {
  list<KIllustratorShell*>::iterator i;
  for (i = s_lstShells->begin (); i != s_lstShells->end (); i++) {
    if ((*i)->isModified ()) {
      if (! (*i)->requestClose ())
	return false;
    }
  }
  return true;
}

bool KIllustratorShell::printDlg () {
  assert (m_pView != 0L);

  return m_pView->printDlg ();
}

void KIllustratorShell::slotFileNew () {
  if (! newDocument ())
    QMessageBox::critical (this, i18n ("KIllustrator Error"), 
			   i18n ("Could not create new document"), 
			   i18n ("Ok"));
}

void KIllustratorShell::slotFileOpen () {
  QString file = KFileDialog::getOpenFileName (getenv ("HOME"));

  if (file.isNull ())
    return;
  
  if (! openDocument (file, "")) {
    QMessageBox::critical (this, i18n ("KIllustrator Error"), 
			   i18n ("Could not open file"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFileSave () {
  assert (m_pDoc != 0L);
  
  CORBA::String_var url = m_pDoc->url ();
  if (strlen (url.in ()) == 0) {
    slotFileSaveAs ();
    return;
  }
  
  if (! saveDocument (url.in (), "")) {
    QMessageBox::critical (this, i18n ("KIllustrator Error"), 
			   i18n ("Could not save file"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFileSaveAs () {
  if (! saveDocument ("", "")) {
    QMessageBox::critical (this, i18n ("KIllustrator Error"), 
			   i18n ("Could not save file"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFileClose () {
  if (documentCount () <= 1) {
    slotFileQuit ();
    return;
  }
  
  if (isModified () && ! requestClose ())
    return;
  
  delete this;
}

void KIllustratorShell::slotFilePrint () {
  assert (m_pView != 0L);
  m_pView->printDlg ();
}

void KIllustratorShell::slotFileQuit () {
  if (! closeAllDocuments ())
    return;

  delete this;
  kapp->exit ();
}

void KIllustratorShell::slotHelpAbout () {
}

int KIllustratorShell::documentCount () {
  return s_lstShells->size ();
}

bool KIllustratorShell::requestClose () {
  int result = 
    QMessageBox::warning (0L, i18n ("Warning"), 
			  i18n ("The document has been modified\nDo you want to save it ?" ),
			  i18n ("Yes"), i18n ("No"), i18n ("Cancel"));
  
  if (result == 0)
    return saveDocument ("", "");
  
  if (result == 1)
    return true;
  
  return false;
}
  
void KIllustratorShell::releaseDocument () {
  int views = 0;
  if (m_pDoc)
    views = m_pDoc->viewCount();
  setRootPart (0);
  interface ()->setActivePart (0);
  if (m_pView)
    m_pView->decRef ();
  if (m_pDoc && views <= 1)
    m_pDoc->cleanUp ();
  if (m_pDoc)
    CORBA::release (m_pDoc);
  m_pView = 0L;
  m_pDoc = 0L;
}
