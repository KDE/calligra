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
/* Module: KPresenter Shell (header)                              */
/******************************************************************/

#ifndef __kpresenter_shell_h__
#define __kpresenter_shell_h__

class KPresenterShell_impl;

#include <default_shell_impl.h>
#include <document_impl.h>

#include <qstring.h>
#include <qfileinf.h>

#include "kpresenter.h"
#include "kpresenter_doc.h"

/******************************************************************/
/* class KPresenterShell_impl                                     */
/******************************************************************/
class KPresenterShell_impl : public DefaultShell_impl
{
  Q_OBJECT

public:

  // ------ C++ ------
  // constructor - destructor
  KPresenterShell_impl();
  ~KPresenterShell_impl();
  
  // set a document
  virtual void setDocument(KPresenterDocument_impl *_doc);
  
  // open - save document
  virtual bool openDocument(const char *_filename);
  virtual bool saveDocument(const char *_file,const char *_format);
  virtual void fileSave();

  // new document
  virtual void fileNew();

  // clean
  virtual void cleanUp();

  virtual void helpAbout();
  
protected:

  // reference to a document
  Document_ref m_rDoc;
  OPParts::View_var m_vView;
  const char *filename,*format;

};

#endif
