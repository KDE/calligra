/*
 *  kimageshop_shell.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kimageshop_shell_h__
#define __kimageshop_shell_h__

#include <qlist.h>

#include <koMainWindow.h>

class KImageShopDoc;
class KImageShopView;

class KImageShopShell : public KoMainWindow
{
  Q_OBJECT

public:

  KImageShopShell();
  ~KImageShopShell();

  virtual void cleanUp();
  void setDocument(KImageShopDoc* _doc);

  virtual bool newDocument();
  virtual bool openDocument(const char* _filename);
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

  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();
  virtual bool printDlg();
  virtual void helpAbout();
  virtual int documentCount();

  bool isModified();
  bool requestClose();
  void releaseDocument();

  KImageShopDoc* m_pDoc;
  KImageShopView* m_pView;

  static QList<KImageShopShell>* s_lstShells;
};

#endif
