/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
#include "kohtml_shell.h"
#include "kohtml_shell.moc"
#include "kohtml_doc.h"
#include "kohtml_view.h"

#include <koAboutDia.h>
#include <kfiledialog.h>
#include <opMainWindowIf.h>
#include <kapp.h>
#include <qmsgbox.h>

QList <KoHTMLShell> *KoHTMLShell::s_lstShells = 0L;

KoHTMLShell::KoHTMLShell()
{
  m_pDoc = 0L;
  m_pView = 0L;
  
  if (s_lstShells == 0L)
     s_lstShells = new QList<KoHTMLShell>;
     
  s_lstShells->append(this);
}

KoHTMLShell::~KoHTMLShell()
{
  cleanUp();

  s_lstShells->removeRef(this);
}

bool KoHTMLShell::isModified()
{
  if (m_pDoc)
     return (bool)m_pDoc->isModified();
     
  return false;
}

bool KoHTMLShell::requestClose()
{
  int res = QMessageBox::warning( 0L, i18n("Warning"), i18n("The document has been modified\nDo you want to save it ?" ),
				  i18n("Yes"), i18n("No"), i18n("Cancel") );
				  
  if ( res == 0 )
    return saveDocument( "", "" );
  
  if ( res == 1 )
    return true;
  
  return false;
}

void KoHTMLShell::cleanUp()
{
  releaseDocument();
  
  KoMainWindow::cleanUp();
}

void KoHTMLShell::setDocument(KoHTMLDoc *_doc)
{
  if (m_pDoc)
     releaseDocument();
     
  m_pDoc = _doc;
  m_pDoc->_ref();
  m_pView = _doc->createKoHTMLView();
  m_pView->incRef();
  m_pView->setMode(KOffice::View::RootMode);
  m_pView->setMainWindow(interface());
  
  setRootPart(m_pView->id());
  
  interface()->setActivePart(m_pView->id());
  
  if (m_pFileMenu)
     {
       m_pFileMenu->setItemEnabled(m_idMenuFile_Save, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_SaveAs, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Close, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Quit, true);
     }
     
  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);     
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);
}

bool KoHTMLShell::newDocument()
{

  if (m_pDoc)
     {
       KoHTMLShell *s = new KoHTMLShell;
       s->show();
       s->newDocument();
       return true;
     }
     
  m_pDoc = new KoHTMLDoc;
  
  if (!m_pDoc->init())
     {
       cerr << "ERROR: Could not initialize document" << endl;
       return false;
     }
     
  m_pView = m_pDoc->createKoHTMLView();
  m_pView->incRef();
  m_pView->setMode(KOffice::View::RootMode);
  m_pView->setMainWindow(interface());

  setRootPart(m_pView->id());
  
  interface()->setActivePart(m_pView->id());
  
  if (m_pFileMenu)
     {
       m_pFileMenu->setItemEnabled(m_idMenuFile_Save, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_SaveAs, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Close, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Quit, true);
     }
     
  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);     
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);
  
  return true;
}

bool KoHTMLShell::openDocument(const char *filename, const char *format)
{
  if (format == 0L || *format == 0)
     format = MIME_TYPE;
     
  if (m_pDoc && m_pDoc->isEmpty())
     releaseDocument();
  else if (m_pDoc && !m_pDoc->isEmpty())
     {
       KoHTMLShell *s = new KoHTMLShell();
       s->show();
       return s->openDocument(filename, format);
     }          
     
  m_pDoc = new KoHTMLDoc;

  if (!m_pDoc->loadFromURL(filename, format))
     return false;

  m_pView = m_pDoc->createKoHTMLView();
  m_pView->incRef();
  m_pView->setMode(KOffice::View::RootMode);
  m_pView->setMainWindow(interface());
  
  setRootPart(m_pView->id());
  interface()->setActivePart(m_pView->id());
  
  if (m_pFileMenu)
     {
       m_pFileMenu->setItemEnabled(m_idMenuFile_Save, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_SaveAs, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Close, true);
       m_pFileMenu->setItemEnabled(m_idMenuFile_Quit, true);
     }
     
  opToolBar()->setItemEnabled(TOOLBAR_PRINT, true);     
  opToolBar()->setItemEnabled(TOOLBAR_SAVE, true);

//  m_pDoc->setURL(filename);
  
  return true;
}

bool KoHTMLShell::saveDocument(const char *filename, const char *format)
{
  assert( m_pDoc != 0L );
 
  CORBA::String_var url;
  if (filename == 0L || *filename == 0)
     {
       url = m_pDoc->url();
       filename = url.in();
     }
     
  QString file;
  
  if (filename == 0L || *filename == 0)
     {
       file = KFileDialog::getSaveFileName( getenv("HOME") );
       
       if (file.isNull())
          return false;
	  
       filename = file.data();
       m_pDoc->setURL(filename);	  
     }     
     
  if (format == 0L || *format == 0)
     format = MIME_TYPE;
     
  return m_pDoc->saveToURL(filename, format);          
}

bool KoHTMLShell::printDlg()
{
  assert(m_pView != 0L);
  
  return m_pView->printDlg();
}

void KoHTMLShell::helpAbout()
{
}

bool KoHTMLShell::closeDocument()
{
  if (isModified())
  {
    if (!requestClose())
       return false;
  }
  
  return true;
}

bool KoHTMLShell::closeAllDocuments()
{
  KoHTMLShell *s;
  for (s = s_lstShells->first(); s != 0L; s = s_lstShells->next());
  {
    if (s->isModified())
    {
      if (!s->requestClose())
        return false;
    }
  }
  return true;
}

int KoHTMLShell::documentCount()
{
  return s_lstShells->count();
}

void KoHTMLShell::releaseDocument()
{
  int views = 0;
  
  if (m_pDoc)
     views = m_pDoc->viewCount();
     
  setRootPart(0);
  
  interface()->setActivePart(0);
  
  if (m_pView)
     m_pView->decRef();
     
  if (m_pDoc && views <= 1)
     m_pDoc->cleanUp();
     
  if (m_pDoc)
     CORBA::release(m_pDoc);
     
  m_pView = 0L;
  m_pDoc = 0L;
}

void KoHTMLShell::slotFileNew()
{
  if (!newDocument())
     QMessageBox::critical(this, i18n("KoHTML Error"), i18n("Could not create new document"), i18n("Ok"));
}

void KoHTMLShell::slotFileOpen()
{
  QString file = KFileDialog::getOpenFileName( getenv("HOME") );
  
  if (file.isNull())
     return;
     
  if (!openDocument(file, ""))
  {
    QString msg;
    msg.sprintf(i18n("Could not open\n%s"), file.data());
    QMessageBox::critical(this, i18n("IO Error"), msg, i18n("Ok"));
  }
}

void KoHTMLShell::slotFileSave()
{
  assert(m_pDoc != 0L);
  
  CORBA::String_var url = m_pDoc->url();
 
  if (strlen(url.in()) == 0)
  {
    slotFileSaveAs();
    return;
  }
  
  if (!saveDocument(url.in(), ""))
  {
    QString msg;
    msg.sprintf(i18n("Could not save\n%s"), url.in());
    QMessageBox::critical(this, i18n("IO Error"), msg, i18n("Ok"));
  }
}

void KoHTMLShell::slotFileSaveAs()
{
  if (!saveDocument("", ""))
    QMessageBox::critical(this, i18n("IO Error"), i18n("Could not save file"), i18n("Ok"));
}

void KoHTMLShell::slotFileClose()
{
  if (documentCount() <= 1)
  {
    slotFileQuit();
    return;
  }
  
  if (isModified())
     if (!requestClose())
       return;
       
  delete this;       
}

void KoHTMLShell::slotFilePrint()
{
  assert(m_pView);
  
  m_pView->printDlg();
}

void KoHTMLShell::slotFileQuit()
{
  if (!closeAllDocuments())
     return;
     
  delete this;
  kapp->exit();
}

KOffice::Document_ptr KoHTMLShell::document()
{
  return KOffice::Document::_duplicate( m_pDoc );
}

KOffice::View_ptr KoHTMLShell::view()
{
  return KOffice::View::_duplicate( m_pView );
}
