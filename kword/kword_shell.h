/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Shell (header)                                         */
/******************************************************************/

#ifndef kword_shell_h
#define kword_shell_h

class ImageShell_impl;

#include "kword.h"
#include "kword_doc.h"

#include <default_shell_impl.h>
#include <document_impl.h>

#include <qmessagebox.h>

/******************************************************************/
/* Class: KWordShell_impl                                         */
/******************************************************************/

class KWordShell_impl : public DefaultShell_impl
{
  Q_OBJECT

public:
  KWordShell_impl();
  ~KWordShell_impl();
  
  virtual void setDocument(KWordDocument_impl *_doc);
  
  virtual bool openDocument(const char *_filename);
  virtual bool saveDocument(const char *_file,const char *_format);

  virtual void fileNew();
  virtual bool printDlg();

  virtual void cleanUp();
  
protected:
  Document_ref m_rDoc;
  OPParts::View_var m_vView;

};

#endif
