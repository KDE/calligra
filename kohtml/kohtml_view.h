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

#ifndef __kohtml_view_h__
#define __kohtml_view_h__

class KoHTMLFrame;
class KoHTMLView;
class KoHTMLChild;
class KMyHTMLView;

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

#include "htmview.h"
#include "kohtmljob.h"

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

class KoHTMLView : public KMyHTMLView,
                   virtual public KoViewIf,
		   virtual public KoHTML::KoHTMLView_skel
{
  Q_OBJECT
public:
  KoHTMLView(QWidget *parent, const char *name, KoHTMLDoc *_doc);
  ~KoHTMLView();

  KoHTMLDoc *doc() { return m_pDoc; }

  virtual void cleanUp();

  virtual void slotNewWindow() {}
  virtual void slotClose() {}

  virtual void editCopy();
  virtual void editHTMLCode();
  virtual void editSettings();
  virtual void editKeys();

  virtual void viewToolBar();
  virtual void viewStatusBar();
//  virtual void toggleCache();

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
  virtual void slotOpenURLDlg();

  virtual void slotDocumentStarted();
  virtual void slotDocumentDone();

  virtual void setFocus(CORBA::Boolean mode);
  virtual CORBA::Boolean printDlg();

  virtual void scanBookmarks( OpenPartsUI::Menu_var menu, const char *path );

public slots:
  void slotInsertObject(KoHTMLChild *child);
  void slotUpdateChildGeometry(KoHTMLChild *child);

  void slotGeometryEnd(KoFrame *frame);
  void slotMoveEnd(KoFrame *frame);

  void slotBack2();
  void slotForward2();

  void slotDocumentContentChanged();

  void slotSetCaption(const char *title);
  void slotShowURL(KHTMLView *view, const char *url);
  void slotOpenURL(KHTMLView *view, const char *url, int button, const char *target);
  void slotOpenURL();
  void slotOpenURLInNewWindow();
  void slotURLPopup(KHTMLView *view, const char *url, const QPoint &coord);
  void slotCopyURLtoClipboard();

  void slotUpdateConfig();

  void eventOpenURL( const char *url, bool reload );

protected:
  virtual void init();
  virtual bool event(const char *event, const CORBA::Any &value);
  virtual bool mappingCreateMenuBar(OpenPartsUI::MenuBar_ptr menuBar);
  virtual bool mappingCreateToolBar(OpenPartsUI::ToolBarFactory_ptr factory);
  virtual bool mappingOpenURL( KoHTML::EventOpenURL event );

  virtual void pushURLToHistory();
  virtual void updateHistory(bool enableBack, bool enableForward);

  KAccel *m_pAccel;

  OpenPartsUI::MenuBar_var m_vMenuBar;
  OpenPartsUI::Menu_var m_vMenuFile;
  OpenPartsUI::Menu_var m_vMenuEdit;
  OpenPartsUI::Menu_var m_vMenuEdit_Insert;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_HTMLCode;
  CORBA::Long m_idMenuEdit_Insert_Object;

  OpenPartsUI::Menu_var m_vMenuBookmarks;
  CORBA::Long m_idMenuBookmarks_Add;
  CORBA::Long m_idMenuBookmarks_Edit;

  OpenPartsUI::Menu_var m_vMenuOptions;
  CORBA::Long m_idMenuOptions_Settings;
  CORBA::Long m_idMenuOptions_ConfigureKeys;
  CORBA::Long m_idMenuOptions_View_ToolBar;
  CORBA::Long m_idMenuOptions_View_StatusBar;

  OpenPartsUI::ToolBar_var m_vMainToolBar;
  CORBA::Long m_idBack;
  CORBA::Long m_idForward;
  CORBA::Long m_idHome;
  CORBA::Long m_idReload;
  CORBA::Long m_idStop;
  CORBA::Long m_idOpenURL;
  CORBA::Long m_idButton_Copy;
  CORBA::Long m_idConfigure;
  CORBA::Long m_idInsert_Object;
  CORBA::Long m_idEditHTMLCode;

  OpenPartsUI::ToolBar_var m_vLocationToolBar;
  CORBA::Long m_idLocation;

  OpenPartsUI::StatusBar_var m_vStatusBar;

  CORBA::Long m_idStatusBar_StatusMsg;
  CORBA::Long m_idStatusBar_URLMsg;

  bool m_bToolBarVisible;
  bool m_bStatusBarVisible;

  static const int ID_EDIT_COPY              = 1;
  static const int ID_EDIT_INSERT_OBJECT     = 2;
  static const int ID_EDIT_HTMLCODE          = 3;
  static const int ID_OPTIONS_SETTINGS       = 4;
  static const int ID_OPTIONS_CONFIGUREKEYS  = 5;
  static const int ID_OPTIONS_VIEW_TOOLBAR   = 6;
  static const int ID_OPTIONS_VIEW_STATUSBAR = 7;
  static const int ID_LOCATION               = 8;
  static const int ID_BOOKMARKS_ADD          = 9;
  static const int ID_BOOKMARKS_EDIT         = 10;
  static const int ID_BACK                   = 11;
  static const int ID_FORWARD                = 12;
  static const int ID_HOME                   = 13;
  static const int ID_RELOAD                 = 14;
  static const int ID_STOP                   = 15;
  static const int ID_OPENURL                = 16;
  static const int ID_NEWWINDOW              = 17;
  static const int ID_CLOSE                  = 18;

  KoHTMLDoc *m_pDoc;

  QString m_strTmpFile;

  int m_idBookmarkId;
  map<int,QString*> m_mapBookmarks;

  QString m_strCurrentURL;

  bool m_bStackLock;
  QStack<SavedPage> m_backStack;
  QStack<SavedPage> m_forwardStack;

  QList<KoHTMLFrame> m_lstFrames;

  QString m_strCaptionText;

  // config variables
  int m_browserStart;
  QString m_strHomePage;
  int m_fontSize;
  QFont m_standardFont;
  QFont m_fixedFont;
  QColor m_bgColor;
  QColor m_lnkColor;
  QColor m_txtColor;
  QColor m_vlnkColor;
};	

#endif
