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
}

/*======================= destrcutor =============================*/
KPresenterShell_impl::~KPresenterShell_impl()
{
  sdeb("KPresenterShell_impl::~KPresenterShell_impl()\n");
  cleanUp();
  edeb("...KPresenterShell_impl::~KPresenterShell_impl()\n");
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
  
  OPParts::View_var view = m_rDoc->createView();
  view->setPartShell(this);
  setRootPart(view);
  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);

  return true;
}

/*========================== save document =======================*/
bool KPresenterShell_impl::saveDocument(const char *_filename,const char *_format)
{
  assert(!CORBA::is_nil(m_rDoc));

  if (_format == 0L || *_format == 0) _format = "kpr";
  
  return m_rDoc->saveAs(_filename,_format);
}

/*========================= file new ============================*/
void KPresenterShell_impl::fileNew()
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate(new KPresenterDocument_impl);
  
  m_vView = m_rDoc->createView();
  m_vView->setPartShell(this);
  setRootPart(m_vView);
  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);
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
