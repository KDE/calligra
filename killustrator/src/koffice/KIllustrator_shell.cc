#include "KIllustrator_shell.h"
#include "KIllustrator_shell.moc"

#include <op_app.h>
#include <qmsgbox.h>

KIllustratorShell::KIllustratorShell () {
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
    m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Ne&w...")), 
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
}

void KIllustratorShell::fileOpen () {
}

void KIllustratorShell::fileSave () {
}

void KIllustratorShell::fileSaveAs () {
}

void KIllustratorShell::fileClose () {
}

void KIllustratorShell::helpAbout () {
}
  
void KIllustratorShell::cleanUp () {
  if (m_bIsClean)
    return;

  m_vView = 0L;
  DefaultShell_impl::cleanUp ();
  m_rDoc = 0L;
}
