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

#ifndef __ko_main_window_h__
#define __ko_main_window_h__

#include <opMainWindow.h>
#include <opMainWindowIf.h>
#include <openparts.h>
#include <koffice.h>

class OPMenu;
class OPMenuBar;
class KoFrame;
class KoMainWindowIf;
class KoDocument;
class KoViewIf;

class KoMainWindow : public OPMainWindow
{
  Q_OBJECT
public:
  KoMainWindow( const char *_name = 0L );
  ~KoMainWindow();

  virtual OPMainWindowIf* interface();
  virtual KoMainWindowIf* koInterface();

  virtual void createFileMenu( OPMenuBar* );
  virtual void createHelpMenu( OPMenuBar* );

  virtual void setRootPart( unsigned long _part_id );
  virtual void setRootPart( KoViewIf* _view );
  
  virtual void cleanUp();

  virtual KOffice::Document_ptr document() = 0L;
  virtual KOffice::View_ptr view() = 0L;

  KoFrame *getFrame() { return m_pFrame; }

  virtual bool newDocument() { return false; };
  virtual bool openDocument( const char* _filename, const char* _format ) { return false; };
  
protected slots:
  virtual void slotActivePartChanged( unsigned long _new_part_id, unsigned long _old_opart_id );

  virtual void slotFileNew();
  virtual void slotFileOpen();
  virtual void slotFileSave();
  virtual void slotFileSaveAs();
  virtual void slotFilePrint();
  virtual void slotFileClose();
  virtual void slotFileQuit();
  virtual void slotHelpAbout();

protected:
  OPMenu* m_pFileMenu;
  OPMenu* m_pHelpMenu;

  int m_idMenuFile_New;
  int m_idMenuFile_Open;
  int m_idMenuFile_Save;
  int m_idMenuFile_SaveAs;
  int m_idMenuFile_Print;
  int m_idMenuFile_Close;
  int m_idMenuFile_Quit;
  int m_idMenuHelp_About;

  enum { TOOLBAR_NEW, TOOLBAR_OPEN, TOOLBAR_SAVE, TOOLBAR_PRINT };

  KoFrame* m_pFrame;
  KoMainWindowIf* m_pKoInterface;
};

class KoMainWindowIf : virtual public OPMainWindowIf,
		       virtual public KOffice::MainWindow_skel
{
public:
  KoMainWindowIf( KoMainWindow* _main );
  ~KoMainWindowIf();

  // IDL
  virtual void setMarkedPart( OpenParts::Id id );		
  virtual KOffice::Document_ptr document();
  virtual KOffice::View_ptr view();
  virtual CORBA::Boolean partClicked( OpenParts::Id _part_id, CORBA::Long _button );
  
protected:
  void unmarkPart();

  KoMainWindow* m_pKoMainWindow;

  OpenParts::Id m_iMarkedPart;
};

#endif
