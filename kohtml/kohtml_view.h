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
#ifndef __kohtml_view_h__
#define __kohtml_view_h__

class KoHTMLFrame;
class KoHTMLView;
class KoHTMLChild;
class KHTMLView_Patched;

#include <kom.h>
#include <komBase.h>
#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <opStatusBar.h>
#include <openparts_ui.h>

#include <map>
#include <qlist.h>
#include <qstack.h>
#include <kaccel.h>

#include "kohtml_doc.h"
#include "kohtml.h"

#include <khtml.h>
#include <khtmlview.h>
#include "khtmlview_patched.h"

class KoHTMLFrame : public KoFrame
{
public:  
  KoHTMLFrame(KoHTMLView *, KoHTMLChild *);
  
  KoHTMLChild *getChild() { return m_pKoHTMLChild; }
  KoHTMLView *getView() { return m_pKoHTMLView; }
  
protected:
  KoHTMLChild *m_pKoHTMLChild;
  KoHTMLView *m_pKoHTMLView;  
};

class KoHTMLView : public QWidget,
                   virtual public KoViewIf,
		   virtual public KoHTML::KoHTMLView_skel
{
  Q_OBJECT
public:
  KoHTMLView(QWidget *parent, const char *name, KoHTMLDoc *_doc);
  ~KoHTMLView();
  
  KoHTMLDoc *doc() { return m_pDoc; }

  virtual void cleanUp();

  virtual void editCopy();
  virtual void editPreferences();
  
  virtual void viewToolBar();
  virtual void viewStatusBar();

  virtual void newView();
  virtual void insertObject();
  
  virtual void slotStatusMsg(const char *text);
  virtual void statusCallback(CORBA::Long ID);

  virtual void slotURLEntered();

  virtual void addBookmark();
  virtual void editBookmarks();
  virtual void slotBookmarkSelected( CORBA::Long ID );

  virtual void slotBack();
  virtual void slotForward();
  virtual void slotHome();
  virtual void slotReload();
  virtual void slotStop();
  
  virtual void setFocus(CORBA::Boolean mode);
  virtual CORBA::Boolean printDlg();

  virtual void scanBookmarks( OpenPartsUI::Menu_var menu, const char *path );
  
public slots:
  void slotInsertObject(KoHTMLChild *child);
  void slotUpdateChildGeometry(KoHTMLChild *child);

  void slotGeometryEnd(KoFrame *frame);  
  void slotMoveEnd(KoFrame *frame);

  void slotDocumentContentChanged();
  
  void slotSetCaption(const char *title);
  void slotShowURL(KHTMLView *view, const char *url);
  void slotOpenURL(KHTMLView *view, const char *url, int button, const char *target);
  void slotOpenURL();
  void slotURLPopup(KHTMLView *view, const char *url, const QPoint &coord);  
  void slotCopyURLtoClipboard();

protected:
  virtual void init();
  virtual bool event(const char *event, const CORBA::Any &value);
  virtual bool mappingCreateMenuBar(OpenPartsUI::MenuBar_ptr menuBar);
  virtual bool mappingCreateToolBar(OpenPartsUI::ToolBarFactory_ptr factory);

  virtual void resizeEvent(QResizeEvent *ev);

  virtual void pushURLToHistory();
  virtual void updateHistory(bool enableBack, bool enableForward);

  OpenPartsUI::MenuBar_var m_vMenuBar;
  OpenPartsUI::Menu_var m_vMenuEdit;
  OpenPartsUI::Menu_var m_vMenuEdit_Insert;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Insert_Object;
  CORBA::Long m_idMenuEdit_Preferences;

  OpenPartsUI::Menu_var m_vMenuBookmarks;
  CORBA::Long m_idMenuBookmarks_Add;
  CORBA::Long m_idMenuBookmarks_Edit;

  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_ToolBar;
  CORBA::Long m_idMenuView_StatusBar;

  OpenPartsUI::ToolBar_var m_vMainToolBar;
  CORBA::Long m_idBack;
  CORBA::Long m_idForward;
  CORBA::Long m_idHome;
  CORBA::Long m_idReload;
  CORBA::Long m_idStop;
  CORBA::Long m_idButton_Copy;
  CORBA::Long m_idConfigure;
  CORBA::Long m_idButton_Insert_Object;
  
  OpenPartsUI::ToolBar_var m_vLocationToolBar;
  CORBA::Long m_idLocation;

  OpenPartsUI::StatusBar_var m_vStatusBar;
  
  CORBA::Long m_idStatusBar_StatusMsg;
  CORBA::Long m_idStatusBar_URLMsg;

  bool m_bToolBarVisible;
  bool m_bStatusBarVisible;

  static const int ID_EDIT_COPY = 1;
  static const int ID_EDIT_INSERT_OBJECT = 2;
  static const int ID_EDIT_PREFERENCES = 3;
  static const int ID_VIEW_TOOLBAR = 4;
  static const int ID_VIEW_STATUSBAR = 5;
  static const int ID_LOCATION = 6;
  static const int ID_BOOKMARKS_ADD = 7;
  static const int ID_BOOKMARKS_EDIT = 8;
  static const int ID_BACK = 9;
  static const int ID_FORWARD = 10;
  static const int ID_HOME = 11;
  static const int ID_RELOAD = 12;
  static const int ID_STOP = 13;

  KoHTMLDoc *m_pDoc;  
  
  KHTMLView_Patched *m_pHTMLView;

  int bookmarkId;
  map<int,QString*> m_mapBookmarks;
  
  QString m_vCurrentURL;
  
  bool m_bStackLock;
  QStack<SavedPage> m_vBackStack;
  QStack<SavedPage> m_vForwardStack;
 
  QList<KoHTMLFrame> m_lstFrames;
  
};	

#endif