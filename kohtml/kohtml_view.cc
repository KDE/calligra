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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ksimpleconfig.h>

#include "kohtml_view.h"
#include "kohtml_view.moc"

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koQueryTypes.h>

#include <qkeycode.h>
#include <qmsgbox.h>
#include <qclipboard.h>
#include <qfileinfo.h>
#include <kiconloader.h>
#include <kfm.h>
#include <kmimemagic.h>
#include <kmimetypes.h>
#include <kpixmapcache.h>
#include <kwm.h>
#include <kkeydialog.h>

#include <khtmlsavedpage.h>

#include "kohtml_shell.h"
#include "htmwidget.h"
#include "settingsdlg.h"
#include "edithtmldlg.h"
#include "openurldlg.h"
#include "kfileio.h"
#include <klocale.h>

KoHTMLFrame::KoHTMLFrame(KoHTMLView *_view, KoHTMLChild *_child)
:KoFrame(_view)
{
  m_pKoHTMLView = _view;
  m_pKoHTMLChild = _child;
}

KoHTMLView::KoHTMLView(QWidget *parent, const char *name, KoHTMLDoc *_doc)
: KMyHTMLView(_doc, parent, name), KoViewIf(_doc), OPViewIf(_doc), KoHTML::KoHTMLView_skel()
{
  setWidget(this);
  
  OPPartIf::setFocusPolicy(OpenParts::Part::ClickFocus);

  m_strCaptionText = "KOffice HTML Viewer";
  
  m_pDoc = _doc;
  
  m_lstFrames.setAutoDelete(true);
  
  QObject::connect(m_pDoc, SIGNAL(sig_insertObject(KoHTMLChild *)),
                   this, SLOT(slotInsertObject(KoHTMLChild *)));
  QObject::connect(m_pDoc, SIGNAL(sig_updateChildGeometry(KoHTMLChild *)),
                   this, SLOT(slotUpdateChildGeometry(KoHTMLChild *)));
		   
  m_bToolBarVisible = true;		   
  m_bStatusBarVisible = true;

  m_pAccel = new KAccel(this);

  m_pDoc->addHTMLView(this);

  QObject::connect(this, SIGNAL(setTitle(const char *)),
                   SLOT(slotSetCaption(const char *)));
  QObject::connect(this, SIGNAL(onURL(KHTMLView *, const char *)),
                   SLOT(slotShowURL(KHTMLView *, const char *)));
  QObject::connect(this, SIGNAL(URLSelected(KHTMLView *, const char *, int, const char *)),
                   SLOT(slotOpenURL(KHTMLView *, const char *, int, const char *)));
  QObject::connect(this, SIGNAL(popupMenu(KHTMLView *, const char *, const QPoint &)),
                   SLOT(slotURLPopup(KHTMLView *, const char *, const QPoint &)));

  m_pAccel->insertItem(i18n("History Back"), "Back", ALT+Key_Left);
  m_pAccel->insertItem(i18n("History Forward"), "Forward", ALT+Key_Right);
  m_pAccel->connectItem("Back", this, SLOT(slotBack2()));
  m_pAccel->connectItem("Forward", this, SLOT(slotForward2()));

  m_pAccel->readSettings();
  
  QObject::connect(m_pDoc, SIGNAL(contentChanged()),
                   this, SLOT(slotDocumentContentChanged()));

  KoHTML::KoHTMLDocument_var m_vDoc = KoHTML::KoHTMLDocument::_duplicate( m_pDoc );

  m_vDoc->connect("documentStarted", this, "slotDocumentStarted");  
  m_vDoc->connect("documentDone", this, "slotDocumentDone");

  m_idBookmarkId = 111;

  m_bStackLock = false;
  m_backStack.setAutoDelete(false);
  m_forwardStack.setAutoDelete(false);
  
  m_strTmpFile = "";
}

KoHTMLView::~KoHTMLView()
{
  delete m_pAccel;
  
  cleanUp();
}

void KoHTMLView::cleanUp()
{
  if (m_bIsClean) return;

  KoHTML::KoHTMLDocument_var m_vDoc = KoHTML::KoHTMLDocument::_duplicate( m_pDoc );
  
  m_vDoc->disconnect("documentDone", this, "slotDocumentDone");
  m_vDoc->disconnect("documentStarted", this, "slotDocumentStarted");

  QListIterator<KoHTMLFrame> it(m_lstFrames);
  
  for (; it.current(); ++it)
      {
        it.current()->detach();
      }

  OpenParts::MenuBarManager_var menuBarMan = m_vMainWindow->menuBarManager();
  
  if (!CORBA::is_nil(menuBarMan))
     menuBarMan->unregisterClient(id());
     
  OpenParts::ToolBarManager_var toolBarMan = m_vMainWindow->toolBarManager();
  
  if (!CORBA::is_nil(toolBarMan))
     toolBarMan->unregisterClient(id());

  OpenParts::StatusBarManager_var statusBarMan = m_vMainWindow->statusBarManager();
  
  if (!CORBA::is_nil(statusBarMan))
     statusBarMan->unregisterClient(id());

  m_pDoc->removeHTMLView(this);
  m_pDoc->removeView(this);

  map<int,QString*>::iterator it2 = m_mapBookmarks.begin();
  for( ; it2 != m_mapBookmarks.end(); ++it2 )
    delete it2->second;
  m_mapBookmarks.clear();
  
  m_backStack.setAutoDelete(true);
  m_backStack.clear();

  m_forwardStack.setAutoDelete(true);
  m_forwardStack.clear();

  if ( !m_strTmpFile.isEmpty() )
    unlink( m_strTmpFile.data() );
  
  KoViewIf::cleanUp();     
}

void KoHTMLView::newView()
{
  assert((m_pDoc != 0L));

  KoHTMLShell *shell = new KoHTMLShell;
  shell->show();
  shell->setDocument(m_pDoc);
}

void KoHTMLView::insertObject()
{
  KoDocumentEntry de = KoPartSelectDia::selectPart();

  if (de.name.isEmpty()) return;
  
  KRect tmp;
  tmp.setLeft(0);
  tmp.setTop(0);
  tmp.setRight(100);
  tmp.setBottom(100);
  
  m_pDoc->insertObject(tmp, de);
}
  
void KoHTMLView::setFocus(CORBA::Boolean mode)
{
  CORBA::Boolean old = m_bFocus;
  
  KoViewIf::setFocus( mode );
  
  if ( old == m_bFocus )
    return;

  if ( KoViewIf::mode() != KOffice::View::RootMode )
    resizeEvent( 0L );
}

CORBA::Boolean KoHTMLView::printDlg()
{
  return true;
}

void KoHTMLView::init()
{
  OpenParts::MenuBarManager_var menuBarMan = m_vMainWindow->menuBarManager();
  
  if (!CORBA::is_nil(menuBarMan))
     menuBarMan->registerClient(id(), this);
     
  OpenParts::ToolBarManager_var toolBarMan = m_vMainWindow->toolBarManager();
  
  if (!CORBA::is_nil(toolBarMan))
     toolBarMan->registerClient(id(), this);     

  OpenParts::StatusBarManager_var statusBarMan = m_vMainWindow->statusBarManager();

  if (!CORBA::is_nil(statusBarMan))
     m_vStatusBar = statusBarMan->registerClient(id());

  CORBA::WString_var item = Q2C( i18n("Ready.") );
  m_idStatusBar_StatusMsg = m_vStatusBar->insertItem(item, -1);
  m_idStatusBar_URLMsg = m_vStatusBar->insertItem(0L, -1);

  QListIterator<KoHTMLChild> it = m_pDoc->childIterator();
  for (; it.current(); ++it)
      slotInsertObject(it.current());
      
  CORBA::WString_var caption = Q2C( m_strCaptionText );
  m_vMainWindow->setPartCaption( id(), caption );      
  
  slotUpdateConfig();

  if (m_browserStart == 1) eventOpenURL( m_strHomePage, false );
  //m_pDoc->openURL(m_strHomePage, false);
}

bool KoHTMLView::event(const char *event, const CORBA::Any &value)
{

  EVENT_MAPPER(event, value);
  
  MAPPING(OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenuBar);
  MAPPING(OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolBar);
  MAPPING(KoHTML::eventOpenURL, KoHTML::EventOpenURL, mappingOpenURL);
  
  END_EVENT_MAPPER;
  
  return false;
}

bool KoHTMLView::mappingCreateToolBar(OpenPartsUI::ToolBarFactory_ptr factory)
{
  OpenPartsUI::Pixmap_var pix;

  if (CORBA::is_nil(factory))
     {
       m_vMainToolBar->disconnect("highlighted", this, "statusCallback");
     
       m_vMainToolBar = 0L;
       m_vLocationToolBar = 0L;
       
       return true;
     }

  m_vMainToolBar = factory->create(OpenPartsUI::ToolBarFactory::Transient);

  m_vMainToolBar->setFullWidth(false);

  m_vMainToolBar->connect("highlighted", this, "statusCallback");

  pix = OPUIUtils::convertPixmap(ICON("back.xpm"));
  CORBA::WString_var toolTip = Q2C( i18n("Back") );
  m_idBack = m_vMainToolBar->insertButton2(pix, ID_BACK, SIGNAL(clicked()), this, "slotBack", !m_backStack.isEmpty(), toolTip, -1);

  pix = OPUIUtils::convertPixmap(ICON("forward.xpm"));
  toolTip = Q2C( i18n("Forward") );
  m_idForward = m_vMainToolBar->insertButton2(pix, ID_FORWARD, SIGNAL(clicked()), this, "slotForward", !m_forwardStack.isEmpty(), toolTip, -1);

  pix = OPUIUtils::convertPixmap(ICON("home.xpm"));
  toolTip = Q2C( i18n("Home") );
  m_idHome = m_vMainToolBar->insertButton2(pix, ID_HOME, SIGNAL(clicked()), this, "slotHome", true, toolTip, -1);

  pix = OPUIUtils::convertPixmap(ICON("reload.xpm"));
  toolTip = Q2C( i18n("Reload Page") );
  m_idReload = m_vMainToolBar->insertButton2(pix, ID_RELOAD, SIGNAL(clicked()), this, "slotReload", true, toolTip, -1);

  //grmpfl... there's already a stop icon in koffice/pics/toolbar, but I
  //don't want to use it, I want the original kfm icon
  pix = OPUIUtils::convertPixmap(ICON( kapp->kde_toolbardir() + "/stop.xpm" ));
  toolTip = Q2C( i18n("Stop") );
  m_idStop = m_vMainToolBar->insertButton2(pix, ID_STOP, SIGNAL(clicked()), this, "slotStop", m_pDoc->documentLoading(), toolTip, -1);

  pix = OPUIUtils::convertPixmap(ICON("go-url3.xpm"));
  toolTip = Q2C( i18n("Open URL") );
  m_idOpenURL = m_vMainToolBar->insertButton2(pix, ID_OPENURL, SIGNAL(clicked()), this, "slotOpenURLDlg", true, toolTip, -1);
    
  m_vMainToolBar->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("editcopy.xpm"));
  toolTip = Q2C( i18n("Copy") );
  m_idButton_Copy = m_vMainToolBar->insertButton2(pix, ID_EDIT_COPY, SIGNAL(clicked()), this, "editCopy", true, toolTip, -1);

  m_vMainToolBar->insertSeparator(-1);

  pix = OPUIUtils::convertPixmap(ICON("configure.xpm"));
  toolTip = Q2C( i18n("Settings") );
  m_idConfigure = m_vMainToolBar->insertButton2(pix, ID_OPTIONS_SETTINGS, SIGNAL(clicked()), this, "editSettings", true, toolTip, -1);

  m_vMainToolBar->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("parts.xpm"));
  toolTip = Q2C( i18n("Insert Object") );
  m_idInsert_Object = m_vMainToolBar->insertButton2(pix, ID_EDIT_INSERT_OBJECT, SIGNAL(clicked()), this, "insertObject", true, toolTip, -1);

  pix = OPUIUtils::convertPixmap(ICON("edit-html.xpm"));
  toolTip = Q2C( i18n("Edit current HTML code") );
  m_idEditHTMLCode = m_vMainToolBar->insertButton2(pix, ID_EDIT_HTMLCODE, SIGNAL(clicked()), this, "editHTMLCode", true, toolTip, -1);
  
  m_vLocationToolBar = factory->create(OpenPartsUI::ToolBarFactory::Transient);

  m_vLocationToolBar->setFullWidth(true);

  CORBA::WString_var labelText = Q2C( i18n("Location : ") );
  m_vLocationToolBar->insertTextLabel(labelText, -1, -1);
  
  CORBA::String_var htmlURL = m_pDoc->htmlURL();
  QString u = htmlURL.in();
  CORBA::WString_var wu = Q2C( u );
  toolTip = Q2C( i18n("Current Location") );
  m_idLocation = m_vLocationToolBar->insertLined(wu, ID_LOCATION, SIGNAL(returnPressed()), this, "slotURLEntered", true, toolTip, 70, -1);
  m_vLocationToolBar->setItemAutoSized(ID_LOCATION, true);

  if (m_bToolBarVisible) 
     {
       m_vMainToolBar->enable(OpenPartsUI::Show);
       m_vLocationToolBar->enable(OpenPartsUI::Show);
     }  
  else 
     {
       m_vMainToolBar->enable(OpenPartsUI::Hide);
       m_vLocationToolBar->enable(OpenPartsUI::Hide);
     }  

  return true;
}

bool KoHTMLView::mappingCreateMenuBar(OpenPartsUI::MenuBar_ptr menuBar)
{
  OpenPartsUI::Pixmap_var pix;

  if (CORBA::is_nil(menuBar))
     { 
       m_vMenuBar->disconnect("highlighted", this, "statusCallback");
       m_vMenuBar->disconnect("activated", this, "slotBookmarkSelected");

       m_vMenuBar = 0L;
       m_vMenuEdit = 0L;
       m_vMenuEdit_Insert = 0L;
       m_vMenuOptions = 0L;
     
       return true;
     }

  m_vMenuBar = OpenPartsUI::MenuBar::_duplicate(menuBar);
  menuBar->connect("highlighted", this, "statusCallback");

  CORBA::WString_var text = Q2C( i18n("&File") );
  menuBar->setFileMenu( menuBar->insertMenu(text, m_vMenuFile, -1, -1) );

  pix = OPUIUtils::convertPixmap(ICON("go-url3.xpm"));
  text = Q2C( i18n("Open &URL...") );
  m_vMenuFile->insertItem6( pix, text, this, "slotOpenURLDlg", CTRL + Key_U, ID_OPENURL, -1 );
  
  text = Q2C( i18n("&Edit") );
  menuBar->insertMenu(text, m_vMenuEdit, -1, -1);

  pix = OPUIUtils::convertPixmap(ICON("editcopy.xpm"));
  text = Q2C( i18n("&Copy") );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6(pix, text, this, "editCopy", CTRL + Key_C, ID_EDIT_COPY, -1);

  m_vMenuEdit->insertSeparator(-1);
  
  text = Q2C( i18n("&Insert") );
  m_vMenuEdit->insertItem8(text, m_vMenuEdit_Insert, -1, -1);
  
  pix = OPUIUtils::convertPixmap(ICON("parts.xpm"));  
  text = Q2C( i18n("&Object...") );
  m_idMenuEdit_Insert_Object = m_vMenuEdit_Insert->insertItem6(pix, text, this, "insertObject", 0, ID_EDIT_INSERT_OBJECT, -1);

  m_vMenuEdit->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("edit-html.xpm"));
  text = Q2C( i18n("Edit HTML code") );
  m_vMenuEdit->insertItem6(pix, text, this, "editHTMLCode", 0, ID_EDIT_HTMLCODE, -1);

  text = Q2C( i18n("Bookmarks") );
  menuBar->insertMenu(text, m_vMenuBookmarks, -1, -1);
  
  m_vMenuBar->connect("activated", this, "slotBookmarkSelected");
  
  text = Q2C( i18n("Add Bookmark") );
  m_vMenuBookmarks->insertItem4(text, this, "addBookmark", 0, ID_BOOKMARKS_ADD, -1);
  text = Q2C( i18n("Edit Bookmarks") );
  m_vMenuBookmarks->insertItem4(text, this, "editBookmarks", 0, ID_BOOKMARKS_EDIT, -1);
  m_vMenuBookmarks->insertSeparator(-1);
  
  QString bdir(kapp->localkdedir().data());
  bdir += "/share/apps/kfm/bookmarks";
  scanBookmarks( m_vMenuBookmarks, bdir );  

  text = Q2C( i18n("&Options") );
  menuBar->insertMenu(text, m_vMenuOptions, -1, -1);

  pix = OPUIUtils::convertPixmap(ICON("configure.xpm"));
  text = Q2C( i18n("Settings...") );
  m_idMenuOptions_Settings = m_vMenuOptions->insertItem6(pix, text, this, "editSettings", 0, ID_OPTIONS_SETTINGS, -1);

  text = Q2C( i18n("Configure &keys") );
  m_idMenuOptions_ConfigureKeys = m_vMenuOptions->insertItem4(text, this, "editKeys", 0, ID_OPTIONS_CONFIGUREKEYS, -1);
  
  m_vMenuOptions->insertSeparator(-1);

  text = Q2C( i18n("Show/Hide Tool&bar") );
  m_idMenuOptions_View_ToolBar = m_vMenuOptions->insertItem4(text, this, "viewToolBar", 0, ID_OPTIONS_VIEW_TOOLBAR, -1);
  text = Q2C( i18n("Show/Hide &Statusbar") );
  m_idMenuOptions_View_StatusBar = m_vMenuOptions->insertItem4(text, this, "viewStatusBar", 0, ID_OPTIONS_VIEW_STATUSBAR, -1);

  m_vMenuOptions->setItemChecked(m_idMenuOptions_View_ToolBar, m_bToolBarVisible);
  m_vMenuOptions->setItemChecked(m_idMenuOptions_View_StatusBar, m_bStatusBarVisible);

  return true;
}

bool KoHTMLView::mappingOpenURL( KoHTML::EventOpenURL event )
{
  m_pDoc->openURL( event.url, (bool)event.reload );
  return true;
}

void KoHTMLView::pushURLToHistory()
{
  if (m_bStackLock) return;
  
  SavedPage *p = saveYourself();
  if (!p) return;
  
  m_backStack.push(p);
  
  m_forwardStack.setAutoDelete(true);
  m_forwardStack.clear();
  m_forwardStack.setAutoDelete(false);
  
  updateHistory(true, false);
}

void KoHTMLView::updateHistory(bool enableBack, bool enableForward)
{
  if (m_vMainToolBar)
     {
       m_vMainToolBar->setItemEnabled(ID_BACK, enableBack);
       m_vMainToolBar->setItemEnabled(ID_FORWARD, enableForward);
     }
}

void KoHTMLView::slotInsertObject(KoHTMLChild *child)
{
  OpenParts::View_var v;
  
  try
  {
    v = child->createView(m_vKoMainWindow);
  }
  catch (OpenParts::Document::MultipleViewsNotSupported &_ex)
  {
    printf("HMPFL, could not create view :(\n");
    exit(1);
  }
  
  if (CORBA::is_nil(v))
     {
       printf("ARGHL, view is nil :(\n");
       exit(1);
     }
     
  KoHTMLFrame *f = new KoHTMLFrame(this, child);
  f->setGeometry(child->geometry());
  
  m_lstFrames.append(f);
  
  KOffice::View_var koV = KOffice::View::_narrow(v);
  
  assert(!CORBA::is_nil(koV));     
  
  koV->setMode(KOffice::View::ChildMode);
  f->attachView(koV);
  
  KOffice::View::EventNewPart event;
  event.view = KOffice::View::_duplicate( koV );
  EMIT_EVENT( this, KOffice::View::eventNewPart, event );
  
  f->show();
  
  QObject::connect(f, SIGNAL(sig_geometryEnd(KoFrame *)),
                   this, SLOT(slotGeometryEnd(KoFrame *)));
  QObject::connect(f, SIGNAL(sig_moveEnd(KoFrame *)),
                   this, SLOT(slotMoveEnd(KoFrame *)));
}

void KoHTMLView::slotUpdateChildGeometry(KoHTMLChild *child)
{
  KoHTMLFrame *f = 0L;
  
  QListIterator<KoHTMLFrame> it(m_lstFrames);
  for (; it.current(); ++it)
    if(it.current()->getChild() == child)
      f = it.current();
      
  assert(f != 0L);
  
  if (child->geometry() == f->partGeometry()) return;
  
  f->setPartGeometry(child->geometry());      
}

void KoHTMLView::slotGeometryEnd(KoFrame *frame)
{
  KoHTMLFrame *f = (KoHTMLFrame *)frame;
  
  m_pDoc->changeChildGeometry(f->getChild(), f->partGeometry());
}

void KoHTMLView::slotMoveEnd(KoFrame *frame)
{
  KoHTMLFrame *f = (KoHTMLFrame *)frame;
  
  m_pDoc->changeChildGeometry(f->getChild(), f->partGeometry());
}

void KoHTMLView::editCopy()
{
  QClipboard *clip = QApplication::clipboard();
  QString text;
  
  if (!isTextSelected()) return;
  
  getSelectedText(text);
  
  clip->setText(text);
}

void KoHTMLView::editHTMLCode()
{
  HTMLEditDlg htmlEditDlg( m_strDocument );
  
  int w = QApplication::desktop()->width() / 2;
  int h = QApplication::desktop()->height() / 2;

  htmlEditDlg.setGeometry( w / 2, h / 2, w, h );
  
  if (htmlEditDlg.exec() == QDialog::Accepted)
     {
       if ( !m_strTmpFile.isEmpty() )
         unlink( m_strTmpFile.data() );
	 
       m_strTmpFile = tmpnam(0);	 
	 
       kStringToFile( htmlEditDlg.getText(), m_strTmpFile, false, false, false );

       QString tmp = "file:" + m_strTmpFile;
       
       //don't use a) history b) openurl-event (for macros)
       m_pDoc->openURL( tmp.data(), true );
     }
}

void KoHTMLView::editSettings()
{
  SettingsDlg settingsDlg;
  
  QObject::connect(&settingsDlg, SIGNAL(configChanged()),
                   this, SLOT(slotUpdateConfig()));

  settingsDlg.exec();
}

void KoHTMLView::editKeys()
{
  KKeyDialog::configureKeys( m_pAccel );
}

void KoHTMLView::viewToolBar()
{
  m_bToolBarVisible = !m_bToolBarVisible;

  if (m_vMainToolBar != 0L)
     {
       if (m_bToolBarVisible) 
          {
	    m_vMainToolBar->enable(OpenPartsUI::Show);
	    m_vLocationToolBar->enable(OpenPartsUI::Show);
	  }
       else 
          {
	    m_vMainToolBar->enable(OpenPartsUI::Hide);
	    m_vLocationToolBar->enable(OpenPartsUI::Hide);
	  }
     }

  if (!CORBA::is_nil( m_vMenuOptions )) 
    m_vMenuOptions->setItemChecked(m_idMenuOptions_View_ToolBar, m_bToolBarVisible);
}

void KoHTMLView::viewStatusBar()
{
  m_bStatusBarVisible = !m_bStatusBarVisible;
  
  if (m_vStatusBar != 0L)
     {
       if (m_bStatusBarVisible) m_vStatusBar->enable(OpenPartsUI::Show);
       else m_vStatusBar->enable(OpenPartsUI::Hide);
     }

  if (!CORBA::is_nil( m_vMenuOptions )) 
    m_vMenuOptions->setItemChecked(m_idMenuOptions_View_StatusBar, m_bStatusBarVisible);
}     

void KoHTMLView::slotStatusMsg(CORBA::WChar *text)
{
  if (m_vStatusBar != 0L)
     {
       m_vStatusBar->clear();
       m_vStatusBar->message(text, m_idStatusBar_StatusMsg);
     }
}

void KoHTMLView::statusCallback(CORBA::Long ID)
{
#define STATUSMSG(s) \
{ \
  CORBA::WString_var msg = Q2C( s ); \
  slotStatusMsg( msg.out() ); \
} 

  switch (ID)
    {
      case ID_EDIT_COPY              : STATUSMSG(i18n("Copies the selected section to the clipboard")); break;
      case ID_EDIT_INSERT_OBJECT     : STATUSMSG(i18n("Inserts an embedded object into the document")); break;
      case ID_EDIT_HTMLCODE          : STATUSMSG(i18n("Edit the current html code")); break;
      case ID_OPTIONS_SETTINGS       : STATUSMSG(i18n("Change user settings")); break;
      case ID_OPTIONS_CONFIGUREKEYS  : STATUSMSG(i18n("Edit keybindings")); break;
      case ID_OPTIONS_VIEW_TOOLBAR   : STATUSMSG(i18n("Enables / disables the toolbar")); break;
      case ID_OPTIONS_VIEW_STATUSBAR : STATUSMSG(i18n("Enables / disables the statusbar")); break;
      case ID_BOOKMARKS_ADD          : STATUSMSG(i18n("Add the current url to the bookmark list")); break;
      case ID_BOOKMARKS_EDIT         : STATUSMSG(i18n("Edit the bookmark list")); break;
      case ID_BACK		     : STATUSMSG(i18n("Load the previous document in the history")); break;
      case ID_FORWARD                : STATUSMSG(i18n("Load the next document in the history")); break;
      case ID_HOME		     : STATUSMSG(i18n("Load the start document")); break;
      case ID_RELOAD		     : STATUSMSG(i18n("Reload the current document")); break;      
      case ID_STOP		     : STATUSMSG(i18n("Stop loading the current document")); break;
      case ID_OPENURL                : STATUSMSG(i18n("Open an URL dialog")); break;
      default                        : STATUSMSG(i18n("Ready."));
    }
#undef STATUSMSG    
}

void KoHTMLView::slotBack2()
{
  slotBack();
}

void KoHTMLView::slotForward2()
{
  slotForward();
}

void KoHTMLView::slotDocumentContentChanged()
{
  CORBA::String_var htmlURL = m_pDoc->htmlURL();
  QString u = htmlURL.in();
  CORBA::WString_var wu = Q2C( u );
  if (m_vLocationToolBar) m_vLocationToolBar->setLinedText(ID_LOCATION, wu);
} 

void KoHTMLView::slotURLEntered()
{
  CORBA::WString_var wurl = m_vLocationToolBar->linedText(ID_LOCATION);
  QString url = C2Q( wurl.in() );
  
  if (url.isEmpty()) return;
  
  url = url.stripWhiteSpace();
  
  if (url.find("www") == 0)
     url.prepend("http://");
  else if (url.find("ftp.") == 0)
     url.prepend("ftp://");
     
  KURL u(url.data());
  
  if (u.isMalformed())
     {
       QString tmp;
       tmp = i18n("Malformed URL\n");
       tmp += C2Q( wurl.in() );
       QMessageBox::critical((QWidget *)0L, i18n("KoHTML Error"), tmp);
       return;
     }     
     
  pushURLToHistory();
//  m_pDoc->openURL(url, false);
  eventOpenURL( url, false );
}

void KoHTMLView::addBookmark()
{
  CORBA::String_var url = m_pDoc->htmlURL();
  QString title = m_strCaptionText;
  KURL u(url.in());
  
  QFileInfo icon(KPixmapCache::pixmapFileForURL(url.in(), 0, u.isLocalFile(), false));
  QFileInfo miniIcon(KPixmapCache::pixmapFileForURL(url.in(), 0, u.isLocalFile(), true));
  
  
  if (title.isEmpty())
     title = u.filename();

  QString p = kapp->localkdedir().data();
  
  p += "/share/apps/kfm/bookmarks/";
  p += title;
     
  KSimpleConfig c(p, true);
  
  c.setGroup("KDE Desktop Entry");
  c.writeEntry("Type", "Link");
  c.writeEntry("URL", url.in());
  c.writeEntry("Icon", icon.fileName());
  c.writeEntry("MiniIcon", miniIcon.fileName());
  c.writeEntry("Name", title);
  c.sync();
  KWM::sendKWMCommand("krootwm:refreshNew");
}

void KoHTMLView::editBookmarks()
{
  QString p = kapp->localkdedir().data();
  
  p += "/share/apps/kfm/bookmarks/";
  
  KFM kfm;
  kfm.openURL(p);
}

void KoHTMLView::slotBookmarkSelected( CORBA::Long ID )
{
  map<int,QString*>::iterator it = m_mapBookmarks.find(ID);
  
  if (it != m_mapBookmarks.end())
     {
       pushURLToHistory();  
       //m_pDoc->openURL(it->second->data(), false);
       eventOpenURL( it->second->data(), false );
     }
}

void KoHTMLView::slotBack()
{
  if (m_backStack.isEmpty()) return;
  
  SavedPage *s = m_backStack.pop();
  SavedPage *p = saveYourself();
  
  m_forwardStack.push(p);
  
  updateHistory(!m_backStack.isEmpty(), true);

  m_bStackLock = true; 
  restore(s);
  m_bStackLock = false;
  
  delete s;
}

void KoHTMLView::slotForward()
{
  if (m_forwardStack.isEmpty()) return;
  
  SavedPage *s = m_forwardStack.pop();
  SavedPage *p = saveYourself();
  
  m_backStack.push(p);
  
  updateHistory(true, !m_forwardStack.isEmpty());
  
  m_bStackLock = true;
  restore(s);
  m_bStackLock = false;
  
  delete s;
}

void KoHTMLView::slotHome()
{
//  m_pDoc->openURL(m_strHomePage, false);
  eventOpenURL( m_strHomePage, false );
}

void KoHTMLView::slotReload()
{
  cerr << "void KoHTMLView::slotReload()" << endl;

  cancelAllRequests();
  
  m_pDoc->stopLoading();

//  m_pDoc->openURL(m_pDoc->htmlURL(), true);
  CORBA::String_var htmlURL = m_pDoc->htmlURL();
  eventOpenURL( htmlURL.in(), true );
}

void KoHTMLView::slotStop()
{
  m_pDoc->stopLoading();
  slotDocumentDone();
}

void KoHTMLView::slotOpenURLDlg()
{
  OpenURLDlg openURLDlg;
  
  if (openURLDlg.exec() == QDialog::Accepted)
     {
       KURL url( openURLDlg.url() );
       
       if (!url.isMalformed())
          {
            pushURLToHistory();       
	    //m_pDoc->openURL( url.url(), false );
	    eventOpenURL( url.url(), false );
	  }
     }
}

void KoHTMLView::slotDocumentStarted()
{
  if (!CORBA::is_nil(m_vMainToolBar))
    m_vMainToolBar->setItemEnabled(ID_STOP, true);
}

void KoHTMLView::slotDocumentDone()
{
  if (!CORBA::is_nil(m_vMainToolBar))
    m_vMainToolBar->setItemEnabled(ID_STOP, false);  
}

//----------------------------------------------
//
// Bookmark code taken from KFM
// (c) Torben Weis, weis@kde.org
//
//----------------------------------------------

void KoHTMLView::scanBookmarks( OpenPartsUI::Menu_var menu, const char * path )
{
  struct stat buff;
  DIR *dp;
  struct dirent *ep;
  dp = opendir( path );
  if ( dp == 0L )
    return;

  // Loop thru all directory entries
  while ( ( ep = readdir( dp ) ) != 0L )
  {
    if ( strcmp( ep->d_name, "." ) != 0 && strcmp( ep->d_name, ".." ) != 0 )
    {
      // QString name = ep->d_name;	

      QString file = path;
      file += "/";
      file += ep->d_name;
      stat( file.data(), &buff );
      if ( S_ISDIR( buff.st_mode ) )
      {
	OpenPartsUI::Pixmap_var pix;
	QString f = kapp->kde_icondir().data();
	f += "/mini/folder.xpm";
	pix = OPUIUtils::loadPixmap(f);

        OpenPartsUI::Menu_var pop;
	CORBA::WString_var text = Q2C( QString( ep->d_name ) );
	menu->insertItem12(pix, text, pop, -1, -1);
	
	scanBookmarks( pop, file );
      }
      else
      {
	KSimpleConfig cfg( file, true );
	cfg.setGroup( "KDE Desktop Entry" );
	QString type = cfg.readEntry( "Type" );	
	if ( type == "Link" )
	{
	  QString url = cfg.readEntry( "URL" );
	  QString icon = cfg.readEntry( "Icon", "unknown.xpm" );
	  QString miniicon = cfg.readEntry( "MiniIcon", icon );
	  if ( !url.isEmpty() && !miniicon.isEmpty() )
	  {
	    OpenPartsUI::Pixmap_var pix;
	    CORBA::WString_var text;
	    QString name = cfg.readEntry( "Name" );
	    if ( name.isEmpty() )
	    {
	      name = ep->d_name;

	      int i = 0;
	      while ( ( i = name.find( "%%", i ) ) != -1 )
	      {
		name.replace( i, 2, "%");
		i++;
	      }

	      while ( ( i = name.find( "%2f" ) ) != -1 )
		name.replace( i, 3, "/");
	      while ( ( i = name.find( "%2F" ) ) != -1 )
		name.replace( i, 3, "/");
	
	      if ( name.length() > 7 && name.right( 7 ) == ".kdelnk" )
		name.truncate( name.length() - 7 );
	    }

	    QString f( kapp->localkdedir().data() );
	    f += "/share/icons/mini/";
	    f += miniicon;
	    if ( access( f, R_OK ) >= 0 )
	      pix = OPUIUtils::loadPixmap(f);
	    else
	    {
	      f = kapp->kde_icondir().data();
	      f += "/mini/";
	      f += miniicon;
	      if ( access( f, R_OK ) >= 0 )
		pix = OPUIUtils::loadPixmap(f);
	      else
	      {
		f = kapp->kde_icondir().data();
		f += "/mini/unknown.xpm";
		pix = OPUIUtils::loadPixmap(f);
	      }
	    }
	    text = Q2C( name );
	    menu->insertItem11( pix, text, m_idBookmarkId, -1);
	    m_mapBookmarks[ m_idBookmarkId++ ] = new QString( url );
	  }
	}
      }
    }
  }
}

void KoHTMLView::slotSetCaption(const char *title)
{
  m_strCaptionText = i18n("KOffice HTML Viewer");
    
  if (title)
    {
      if (!(strlen(title) == 1 && *title == ':'))
        m_strCaptionText = QString(title).prepend("KoHTML : ");
    }  

  CORBA::WString_var caption = Q2C( m_strCaptionText );
  if (!CORBA::is_nil( m_vMainWindow ))
    m_vMainWindow->setPartCaption( id(), caption );
}

void KoHTMLView::slotShowURL(KHTMLView *view, const char *url)
{
  cerr << "on url : " << url << endl;
  
  if ( !url )
   return;

  QString StatusMsg = url;
  
  if (!strnicmp(StatusMsg, "mailto:", 7))
     StatusMsg.remove(0, 7);

  CORBA::WString_var WStatusMsg = Q2C( StatusMsg );
  slotStatusMsg(WStatusMsg);     
}

void KoHTMLView::slotOpenURL(KHTMLView *view, const char *url, int button, const char *target)
{
  if (!url) return;

  if (button != LeftButton) return;

  if (!strnicmp(url, "mailto:", 7))
     {
       system("kmail " + QString(url));
       return;
     }  
     
  if (target != 0L && target[0] != 0)
     {
      
        QString tmp = "searching view:\n";
	tmp.sprintf("searching view:\n%s\nurl is :\n%s", target, url);
	
	QMessageBox::critical(0L, "KoHTML", tmp);
     
        KHTMLView *targetView1 = view->findView(target);
	KHTMLView *targetView2 = findView(target);

        tmp.sprintf("targetView1 = %p\ntargetView2 = %p", targetView1, targetView2);
	QMessageBox::critical(0L, "KoHTML", tmp);
	
        if ((!targetView1) && (!targetView2))
          {
	    KoHTMLDoc *doc = new KoHTMLDoc();

	    if (!doc->init()) return;
	    
	    doc->openURL(url, false);
	    
	    KoHTMLShell *shell = new KoHTMLShell();
	    shell->show();
	    shell->setDocument(doc);
	    return;
	  }

        if (!targetView1)
	   targetView1 = targetView2;

	cerr << "getFrameName() = " << getFrameName() << endl;
	cerr << "this = " << this << endl;
	   
        if (targetView1 != this)
	   {
             pushURLToHistory();  
	     cerr << "targetView1->openURL(url);" << endl;
	     targetView1->openURL(url);
	     return;
	   }     

     }

  cerr << "m_pDoc->openURL(url);" << endl;
  pushURLToHistory();
//  m_pDoc->openURL(url, false);
  eventOpenURL( url, false );
}

void KoHTMLView::slotOpenURL()
{
  slotOpenURL(0, m_strCurrentURL, LeftButton, 0);
}

void KoHTMLView::slotOpenURLInNewWindow()
{
  assert( m_pDoc != 0L );

  KoHTMLDoc *doc = new KoHTMLDoc();

  if (!doc->init()) return;
	    
  KoHTMLShell *shell = new KoHTMLShell();
  shell->show();
  shell->setDocument(doc);
  
  doc->openURL(m_strCurrentURL, false);
}

void KoHTMLView::slotURLPopup(KHTMLView *view, const char *url, const QPoint &coord)
{
  QPopupMenu *menu = new QPopupMenu;
  
  if ( url ) 
     {
       m_strCurrentURL = url;

       menu->insertItem(i18n("Open URL..."), this, SLOT(slotOpenURL()));
       menu->insertItem(i18n("Open URL in new window..."), this, SLOT(slotOpenURLInNewWindow()));
       menu->insertItem(i18n("Copy URL to clipboard"), this, SLOT(slotCopyURLtoClipboard()));
       menu->insertSeparator(-1);
     }
       
  menu->setItemEnabled( menu->insertItem(i18n("Back"), this, SLOT(slotBack2())), !m_backStack.isEmpty() );
  menu->setItemEnabled( menu->insertItem(i18n("Forward"), this, SLOT(slotForward2())), !m_forwardStack.isEmpty() );
  
  menu->exec(coord);
  
  delete menu;  
}

void KoHTMLView::slotCopyURLtoClipboard()
{
  QClipboard *clip = QApplication::clipboard();
  QString text = m_strCurrentURL;
  
  if (!strnicmp(text, "mailto:", 7))
     text.remove(0, 7);
     
  clip->setText(text);     
}

void KoHTMLView::slotUpdateConfig()
{
  KConfig *config = kapp->getConfig();
  
  config->setGroup("Personal Settings");
  
  m_browserStart = config->readNumEntry("BrowserStart", 0);
  m_strHomePage = config->readEntry("HomePage", "http://www.kde.org/");

  config->writeEntry("BrowserStart", m_browserStart);
  config->writeEntry("HomePage", m_strHomePage);
  
  config->setGroup("Fonts");
  
  QFont helvetica("helvetica");
  QFont courier("courier");
  
  m_fontSize = config->readNumEntry("FontSize", 0);
  m_standardFont = config->readFontEntry("StandardFont", &helvetica);
  m_fixedFont = config->readFontEntry("FixedFont", &courier);

  config->writeEntry("FontSize", m_fontSize);
  config->writeEntry("StandardFont", m_standardFont);
  config->writeEntry("FixedFont", m_fixedFont);    
  
  config->setGroup("Colors");
  
  m_bgColor = config->readColorEntry("BackgroundColor", &white);
  m_lnkColor = config->readColorEntry("LinkColor", &red);
  m_txtColor = config->readColorEntry("TextColor", &black);
  m_vlnkColor = config->readColorEntry("VLinkColor", &magenta);

  config->writeEntry("BackgroundColor", m_bgColor);
  config->writeEntry("LinkColor", m_lnkColor);
  config->writeEntry("TextColor", m_txtColor);
  config->writeEntry("VLinkColor", m_vlnkColor);
  
  getKHTMLWidget()->setDefaultBGColor(m_bgColor);
  getKHTMLWidget()->setDefaultTextColors(m_txtColor, m_lnkColor, m_vlnkColor);
  getKHTMLWidget()->setStandardFont(m_standardFont.family());
  getKHTMLWidget()->setFixedFont(m_fixedFont.family());
  
  config->sync();
}

void KoHTMLView::eventOpenURL( const char *url, bool reload )
{
  KoHTML::EventOpenURL event;
  
  event.url = CORBA::string_dup( url );
  event.reload = (CORBA::Boolean)reload;
  EMIT_EVENT( this, KoHTML::eventOpenURL, event );
}
