/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#ifndef __kchart_shell_h__
#define __kchart_shell_h__

class KChartShell_impl;

#include <koMainWindow.h>

class KChartDoc;
class KChartView;

#include <qlist.h>
#include <qtimer.h>
#include <qstring.h>

class KChartShell : public KoMainWindow
{
  Q_OBJECT
public:
  // C++
  KChartShell();
  ~KChartShell();

  // C++
  virtual void cleanUp();
  void setDocument( KChartDoc *_doc );

  // C++
  virtual bool newDocument();
  virtual bool openDocument( const char * _filename );
  virtual bool saveDocument();
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
  // C++
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  void releaseDocument();
  
  KChartDoc* m_pDoc;
  KChartView* m_pView;

  static QList<KChartShell>* s_lstShells;
};

#endif
