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
//
// $Id$
//

#ifndef __kohtml_shell_h__
#define __kohtml_shell_h__

class KoHTMLShell_impl;

#include <koMainWindow.h>

class KoHTMLDoc;
class KoHTMLView;

#include <qlist.h>
#include <qstring.h>

#include "kohtml_doc.h"

class KoHTMLShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KoHTMLShell();
  ~KoHTMLShell();

  virtual void cleanUp();
  void setDocument(KoHTMLDoc *_doc);    
  
  virtual bool newDocument();
  virtual bool openDocument(const char *filename, const char *format);
  virtual bool saveDocument(const char *filename, const char *format);
  virtual bool closeDocument();
  virtual bool closeAllDocuments();
  
protected slots:
  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotFilePrint();
  void slotFileClose();
  void slotFileQuit();
  
protected:
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();
  
  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();
  
  bool isModified();
  bool requestClose();
  
  void releaseDocument();
  
  KoHTMLDoc *m_pDoc;
  KoHTMLView * m_pView;
  
  static QList<KoHTMLShell> *s_lstShells;  
};

#endif
