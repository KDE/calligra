/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __koshell_window_h__
#define __koshell_window_h__

#include <koMainWindow.h>
#include <qlist.h>
#include <qvaluelist.h>

class QHBox;
class KoKoolBar;
class KoDocumentEntry;

class KoShellWindow : public KoMainWindow
{
  Q_OBJECT

public:

  // C++
  KoShellWindow();
  ~KoShellWindow();

  virtual void cleanUp();

  virtual bool newPage( KoDocumentEntry& _e );
  virtual bool closeApplication();
  virtual bool saveAllPages();
  virtual void releasePages();
  
protected slots:

  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotFilePrint();
  void slotFileClose();
  void slotFileQuit();

  void slotKoolBar( int _grp, int _item );
  
protected:

  struct Page
  {
    KOffice::Document_var m_vDoc;
    KOffice::View_var m_vView;
    int m_id;
  };

  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();

  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();

  QValueList<Page> m_lstPages;
  QValueList<Page>::Iterator m_activePage;

  KoKoolBar* m_pKoolBar;

  int m_grpFile;
  int m_grpDocuments;
  
  QValueList<KoDocumentEntry> m_lstComponents;
  QMap<int,KoDocumentEntry*> m_mapComponents;

  OpenParts::Part_var m_vKfm;
  
  QHBox *m_pLayout;
  
  static QList<KoShellWindow>* s_lstShells;
};

#endif // __koshell_window_h__

