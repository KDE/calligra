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
#include "Preview.h"
#include "DocumentInfo.h"
#include "filter/FilterManager.h"
#include "FilterInfo.h"

#include <kfiledialog.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kimgio.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qfileinfo.h>
#include <klocale.h>
#include <kglobal.h>

list<KIllustratorShell*>* KIllustratorShell::s_lstShells = 0L;
bool KIllustratorShell::previewHandlerRegistered = false;

KIllustratorShell::KIllustratorShell () {
  m_pDoc = 0L;
  m_pView = 0L;

  if (s_lstShells == 0L)
    s_lstShells = new list<KIllustratorShell*>;

  s_lstShells->push_back (this);

  if (! previewHandlerRegistered) {
    kimgioRegister ();

    KFilePreviewDialog::registerPreviewModule ("kil", kilPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("wmf", wmfPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("gif", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("jpg", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("xpm", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("xbm", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("png", pixmapPreviewHandler,
					       PreviewPixmap);
    previewHandlerRegistered = true;
  }

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

void KIllustratorShell::createFileMenu (OPMenuBar* mbar) {
  // Do we loose control over the menubar ?
  if ( mbar == 0L) {
    m_pFileMenu = 0L;
    return;
  }

  KStdAccel stdAccel;
  m_pFileMenu = new OPMenu;

  m_idMenuFile_New =
    m_pFileMenu->insertItem (BarIcon ("filenew.xpm" ), i18n ("&New"),
			     this, SLOT (slotFileNew ()), stdAccel.openNew());
  m_idMenuFile_Open =
    m_pFileMenu->insertItem (BarIcon ("fileopen.xpm"), i18n ("&Open..."),
			     this, SLOT (slotFileOpen ()), stdAccel.open());
  m_pFileMenu->insertSeparator (-1);
  m_idMenuFile_Save =
    m_pFileMenu->insertItem (BarIcon ("filefloppy.xpm"), i18n ("&Save"),
			     this, SLOT (slotFileSave ()), stdAccel.save());
  m_pFileMenu->setItemEnabled (m_idMenuFile_Save, false);

  m_idMenuFile_SaveAs =
    m_pFileMenu->insertItem (i18n ("S&ave as..."), this,
			     SLOT (slotFileSaveAs ()));
  m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, false);

  m_pFileMenu->insertSeparator (-1);

  m_idMenuFile_Import =
    m_pFileMenu->insertItem (i18n ("Import..."), this,
			     SLOT (slotFileImport ()));
  m_idMenuFile_Export =
    m_pFileMenu->insertItem (i18n ("Export..."), this,
			     SLOT (slotFileExport ()));

  m_pFileMenu->insertSeparator (-1);
  m_idMenuFile_Print =
    m_pFileMenu->insertItem (BarIcon ("fileprint.xpm"), i18n ("&Print..."), this,
			     SLOT (slotFilePrint ()),  stdAccel.print());
  m_pFileMenu->setItemEnabled (m_idMenuFile_Print, false);

  m_idMenuFile_Info =
    m_pFileMenu->insertItem (i18n ("Document Info..."), this,
			     SLOT (slotFileInfo ()));

  m_pFileMenu->insertSeparator (-1);
  m_idMenuFile_Close =
    m_pFileMenu->insertItem (i18n ("&Close"), this,
			     SLOT (slotFileClose ()), stdAccel.close());
  m_pFileMenu->setItemEnabled (m_idMenuFile_Close, false);

  m_idMenuFile_Quit =
    m_pFileMenu->insertItem (BarIcon( "exit.xpm" ), i18n ("&Quit"), this,
			     SLOT (slotFileQuit ()), stdAccel.quit());

  mbar->insertItem (i18n ("&File"), m_pFileMenu, -1, 0);
}

void KIllustratorShell::setDocument (KIllustratorDocument* doc) {
  if (m_pDoc)
    releaseDocument ();

  m_pDoc = doc;
  m_pDoc->_ref ();
  m_pView = doc->createKIllustratorView ( frame() );
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());

  setRootPart (m_pView);
  interface ()->setActivePart (m_pView->id ());

  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Print, true);
  }

  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);
}

bool KIllustratorShell::newDocument () {
  if (m_pDoc) {
    KIllustratorShell *shell = new KIllustratorShell ();
    shell->show ();
    shell->newDocument ();
    return true;
  }

  m_pDoc = new KIllustratorDocument ();
  if (! m_pDoc->initDoc ()) {
    releaseDocument();
    cerr << "ERROR: Could not initialize document" << endl;
    return false;
  }

  cout << "create KIllustratorView ..." << endl;
  m_pView = m_pDoc->createKIllustratorView ( frame() );
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());
  cout << "setMainWindow done ..." << endl;

  setRootPart (m_pView);
  interface ()->setActivePart (m_pView->id ());
  cout << "setActivePart done ..." << endl;

  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Print, true);
  }

  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);

  return true;
}

bool KIllustratorShell::openDocument (const char* url, const char* fmt) {
  if (fmt == 0L || *fmt == '\0')
    fmt = "application/x-killustrator";

  if (m_pDoc && m_pDoc->isEmpty ()) {
    cout << "release document" << endl;
    releaseDocument ();
  }
  if (m_pDoc && ! m_pDoc->isEmpty ()) {
    KIllustratorShell *shell = new KIllustratorShell ();
    shell->show ();
    return shell->openDocument (url, fmt);
  }

  cout << "create new document" << endl;
  m_pDoc = new KIllustratorDocument ();
  if (! m_pDoc->loadFromURL (url, fmt))
    return false;

  cout << "create new view" << endl;
  m_pView = m_pDoc->createKIllustratorView ( frame() );
  m_pView->incRef ();
  m_pView->setMode (KOffice::View::RootMode);
  m_pView->setMainWindow (interface ());

  cout << "set active part" << endl;
  setRootPart (m_pView);
  interface ()->setActivePart (m_pView->id ());

  if (m_pFileMenu) {
    m_pFileMenu->setItemEnabled (m_idMenuFile_Save, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_SaveAs, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Close, true);
    m_pFileMenu->setItemEnabled (m_idMenuFile_Print, true);
  }

  opToolBar ()->setItemEnabled (TOOLBAR_PRINT, true);
  opToolBar ()->setItemEnabled (TOOLBAR_SAVE, true);
  m_pDoc->setURL (url);

  cout << "loading done !" << endl;
  return true;
}

bool KIllustratorShell::saveDocument () {
  assert (m_pDoc != 0L);
  return KoMainWindow::saveDocument( "application/x-killustrator", "*.kil" );
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
			   i18n ("OK"));
}

void KIllustratorShell::slotFileOpen () {
  QString fname =
    KFilePreviewDialog::getOpenFileURL (QString::null, "*.kil | KIllustrator File", this);

  if (fname.isNull ())
    return;

  if (! openDocument (fname, "")) {
    QMessageBox::critical (this, i18n ("KIllustrator Error"),
			   i18n ("Could not open file"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFileSave () {
  assert (m_pDoc != 0L);
  (void) saveDocument();
}

void KIllustratorShell::slotFileSaveAs () {
  QString _url = m_pDoc->url();
  m_pDoc->setURL( "" );

  if ( !saveDocument() )
    m_pDoc->setURL( _url );
}

void KIllustratorShell::slotFileClose () {

  if (isModified () && ! requestClose ())
    return;

  releaseDocument();
}

void KIllustratorShell::slotFileImport () {
  FilterManager* filterMgr = FilterManager::instance ();
  QString filter = filterMgr->importFilters ();

  QString fname =
    KFilePreviewDialog::getOpenFileName (QString::null, (const char *) filter, this);
  if (! fname.isEmpty ()) {
    QFileInfo finfo ((const char *) fname);
    if (!finfo.isFile () || !finfo.isReadable ())
      return;

    FilterInfo* filterInfo = filterMgr->findFilter (fname,
						    FilterInfo::FKind_Import);
    if (filterInfo) {
      ImportFilter* filter = filterInfo->importFilter ();
      if (filter->setup (m_pDoc, filterInfo->extension ())) {
	filter->setInputFileName (fname);
	filter->importFromFile (m_pDoc);
      }
      else
	QMessageBox::critical (this, i18n ("KIllustrator Error"),
			       i18n ("Cannot import from file"), i18n ("OK"));
    }
    else
      QMessageBox::critical (this, i18n ("KIllustrator Error"),
			     i18n ("Unknown import format"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFileExport () {
  FilterManager* filterMgr = FilterManager::instance ();
  QString filter = filterMgr->exportFilters ();

  QString fname =
    KFileDialog::getSaveFileName (QString::null, (const char *) filter, this);

  if (! fname.isEmpty ()) {
    FilterInfo* filterInfo = filterMgr->findFilter (fname,
						    FilterInfo::FKind_Export);

    if (filterInfo) {
      ExportFilter* filter = filterInfo->exportFilter ();
      if (filter->setup (m_pDoc, filterInfo->extension ())) {
	filter->setOutputFileName (fname);
	filter->exportToFile (m_pDoc);
      }
      else
	QMessageBox::critical (this, i18n ("KIllustrator Error"),
			       i18n ("Cannot export to file"), i18n ("OK"));
    }
    else
      QMessageBox::critical (this, i18n ("KIllustrator Error"),
			     i18n ("Unknown export format"), i18n ("OK"));
  }
}

void KIllustratorShell::slotFilePrint () {
  assert (m_pView != 0L);
  m_pView->printDlg ();
}

void KIllustratorShell::slotFileInfo () {
  if (m_pDoc)
    DocumentInfo::showInfo (m_pDoc);
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
    return saveDocument ();

  if (result == 1)
    return true;

  return false;
}

void KIllustratorShell::releaseDocument () {
  int views = 0;
  if (m_pDoc)
    views = m_pDoc->viewCount();
  setRootPart( (unsigned long int)(OpenParts::Id)0 );
  interface ()->setActivePart (0);
  if (m_pView)
    m_pView->decRef ();
  if (m_pDoc && views <= 1)
    m_pDoc->cleanUp ();
  if (m_pDoc)
    CORBA::release (m_pDoc);
  m_pView = 0L;
  m_pDoc = 0L;
  if( m_pFileMenu )
  {
    m_pFileMenu->setItemEnabled( m_idMenuFile_Save, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_SaveAs, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Close, false );
    m_pFileMenu->setItemEnabled( m_idMenuFile_Print, false );
  }

  opToolBar()->setItemEnabled( TOOLBAR_PRINT, false );
  opToolBar()->setItemEnabled( TOOLBAR_SAVE, false );
}

KOffice::Document_ptr KIllustratorShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KIllustratorShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}
