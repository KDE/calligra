/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Shell                                       */
/******************************************************************/

#include "kpresenter_shell.h"
#include "kpresenter_shell.moc"

/******************************************************************/
/* class KPresenterShell_impl                                     */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterShell_impl::KPresenterShell_impl()
{
  filename = 0;
  format = 0;
}

/*======================= destrcutor =============================*/
KPresenterShell_impl::~KPresenterShell_impl()
{
  sdeb("KPresenterShell_impl::~KPresenterShell_impl()\n");
  cleanUp();
  edeb("...KPresenterShell_impl::~KPresenterShell_impl()\n");
}

/*========================= save file ============================*/
void KPresenterShell_impl::fileSave()
{
  if (!filename)
    fileSaveAs();
  else
    m_rDoc->saveAs(filename,format);
}

/*====================== set document ============================*/ 
void KPresenterShell_impl::setDocument(KPresenterDocument_impl *_doc)
{
  m_rDoc = OPParts::Document::_duplicate(_doc);

  m_vView = _doc->createView();  
  m_vView->setPartShell(this);
  setRootPart(m_vView);
}

/*======================== open document =========================*/
bool KPresenterShell_impl::openDocument(const char *_filename)
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate(new KPresenterDocument_impl);
  if (!m_rDoc->open(_filename)) return false;
  
  m_vView = m_rDoc->createView();
  m_vView->setPartShell(this);
  setRootPart(m_vView);

  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rMenuBar->setItemEnabled(m_idMenuFile_Save,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Save,true);
  m_rToolBarFile->setFullWidth(false);

  filename = qstrdup(_filename);

  QFileInfo tmp(_filename);
  
  if (tmp.extension().isEmpty())
    format = qstrdup("kpr");
  else
    format = qstrdup(tmp.extension());

  return true;
}

/*========================== save document =======================*/
bool KPresenterShell_impl::saveDocument(const char *_filename,const char *_format)
{
  assert(!CORBA::is_nil(m_rDoc));

  if (_format == 0L || *_format == 0) _format = "kpr";
  
  if (filename) delete filename;
  if (format) delete format;

  filename = qstrdup(_filename);
  format = qstrdup(_format);
  
  return m_rDoc->saveAs(_filename,_format);
}

/*========================= file new ============================*/
void KPresenterShell_impl::fileNew()
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate(new KPresenterDocument_impl);
  
  if (!m_rDoc->init())
    {
      QMessageBox::critical(this,i18n("KPresenter Error"),i18n("Could not init"),i18n("OK"));
      return;
    }
  
  if (filename) delete filename;
  if (format) delete format;
  filename = 0;
  format = 0;

  m_vView = m_rDoc->createView();
  m_vView->setPartShell(this);
  setRootPart(m_vView);
  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rMenuBar->setItemEnabled(m_idMenuFile_Save,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Save,true);
  m_rToolBarFile->setFullWidth(false);
}

/*======================== clean up ==============================*/
void KPresenterShell_impl::cleanUp()
{
  if (m_bIsClean) return;

  DefaultShell_impl::cleanUp();

  mdeb("========DOC=======================\n");
  m_rDoc = 0L;
}

/*======================== help about ============================*/
void KPresenterShell_impl::helpAbout()
{
  KoAboutDia::about(KoAboutDia::KPresenter,"0.0.1");
}

/*========================= file print ===========================*/
bool KPresenterShell_impl::printDlg()
{
  assert(!CORBA::is_nil(m_vView));

  return m_vView->printDlg();
}
