/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shell                                                  */
/******************************************************************/

#include "kword_shell.h"
#include "kword_shell.moc"

/******************************************************************/
/* Class: KWordShell_impl                                         */
/******************************************************************/

/*================================================================*/
KWordShell_impl::KWordShell_impl()
{
}

/*================================================================*/
KWordShell_impl::~KWordShell_impl()
{
  sdeb("KWordShell_impl::~KWordShell_impl()\n");
  cleanUp();
  edeb("...KWordShell_impl::~KWordShell_impl()\n");
}

/*================================================================*/
void KWordShell_impl::cleanUp()
{
  if (m_bIsClean) return;

  DefaultShell_impl::cleanUp();

  mdeb("========DOC=======================\n");
  m_rDoc = 0L;
}

/*================================================================*/
void KWordShell_impl::setDocument(KWordDocument_impl *_doc)
{
  m_rDoc = OPParts::Document::_duplicate(_doc);

  m_vView = _doc->createView();  
  m_vView->setPartShell(this);
  setRootPart(m_vView);
}

/*================================================================*/
bool KWordShell_impl::openDocument(const char *_filename)
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate(new KWordDocument_impl);
  if (!m_rDoc->open(_filename)) return false;
  
  m_vView = m_rDoc->createView();
  m_vView->setPartShell(this);
  setRootPart(m_vView);

  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);

  return true;
}

/*================================================================*/
bool KWordShell_impl::saveDocument(const char *_filename,const char *_format)
{
  assert(!CORBA::is_nil(m_rDoc));

  if (_format == 0L || *_format == 0)
    _format = "kwd";
  
  return m_rDoc->saveAs(_filename,_format);
}

/*================================================================*/
void KWordShell_impl::fileNew()
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate(new KWordDocument_impl);
  if (!m_rDoc->init())
    {
      QMessageBox::critical(this,i18n("KPresenter Error"),i18n("Could not init"),i18n("OK"));
      return;
    }

  m_vView = m_rDoc->createView();
  m_vView->setPartShell(this);
  setRootPart(m_vView);
  m_rMenuBar->setItemEnabled(m_idMenuFile_SaveAs,true);
  m_rMenuBar->setItemEnabled(m_idMenuFile_Save,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Print,true);
  m_rToolBarFile->setItemEnabled(m_idButtonFile_Save,true);
  m_rToolBarFile->setFullWidth(false);
}
