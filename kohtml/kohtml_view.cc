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

#include <khtmlsavedpage.h>

#include "kohtml_shell.h"
#include "khtmlwidget_patched.h"
#include "settingsdlg.h"
#include "edithtmldlg.h"
#include "openurldlg.h"

KoHTMLFrame::KoHTMLFrame(KoHTMLView *_view, KoHTMLChild *_child)
:KoFrame(_view)
{
  m_pKoHTMLView = _view;
  m_pKoHTMLChild = _child;
}

KoHTMLView::KoHTMLView(QWidget *parent, const char *name, KoHTMLDoc *_doc)
: QWidget(parent, name), KoViewIf(_doc), OPViewIf(_doc), KoHTML::KoHTMLView_skel()
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

  KAccel *acc = new KAccel(this);

  m_pHTMLView = new KHTMLView_Patched(this, "m_pHTMLView");
  widget()->setFocusProxy(m_pHTMLView);
  widget()->setFocusPolicy(QWidget::StrongFocus);

  m_pDoc->addHTMLView(m_pHTMLView);

  QObject::connect(m_pHTMLView, SIGNAL(setTitle(const char *)),
                   this, SLOT(slotSetCaption(const char *)));
  QObject::connect(m_pHTMLView, SIGNAL(onURL(KHTMLView *, const char *)),
                   this, SLOT(slotShowURL(KHTMLView *, const char *)));
  QObject::connect(m_pHTMLView, SIGNAL(URLSelected(KHTMLView *, const char *, int, const char *)),
                   this, SLOT(slotOpenURL(KHTMLView *, const char *, int, const char *)));
  QObject::connect(m_pHTMLView, SIGNAL(popupMenu(KHTMLView *, const char *, const QPoint &)),
                   this, SLOT(slotURLPopup(KHTMLView *, const char *, const QPoint &)));

  acc->insertItem("Scroll Up", "Scroll Up", "Up");
  acc->insertItem("Scroll Down", "Scroll Down", "Down");
  
  acc->connectItem("Scroll Up", m_pHTMLView, SLOT(slotVertSubtractLine()));
  acc->connectItem("Scroll Down", m_pHTMLView, SLOT(slotVertAddLine()));
  acc->connectItem(KAccel::Prior, m_pHTMLView, SLOT(slotVertSubtractPage()));
  acc->connectItem(KAccel::Next, m_pHTMLView, SLOT(slotVertAddPage()));

  QObject::connect(m_pDoc, SIGNAL(contentChanged()),
                   this, SLOT(slotDocumentContentChanged()));

  KoHTML::KoHTMLDocument_var m_vDoc = KoHTML::KoHTMLDocument::_duplicate( m_pDoc );

  m_vDoc->connect("documentStarted", this, "slotDocumentStarted");  
  m_vDoc->connect("documentDone", this, "slotDocumentDone");

  m_idBookmarkId = 111;

  m_bStackLock = false;
  m_backStack.setAutoDelete(false);
  m_forwardStack.setAutoDelete(false);
}

KoHTMLView::~KoHTMLView()
{
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

  m_pDoc->removeHTMLView(m_pHTMLView);
  m_pDoc->removeView(this);

  delete m_pHTMLView;

  map<int,QString*>::iterator it2 = m_mapBookmarks.begin();
  for( ; it2 != m_mapBookmarks.end(); ++it2 )
    delete it2->second;
  m_mapBookmarks.clear();

  m_backStack.setAutoDelete(true);
  m_backStack.clear();

  m_forwardStack.setAutoDelete(true);
  m_forwardStack.clear();
  
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

  if ((bool)mode)
     m_pHTMLView->setMouseLock(true);
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

  m_idStatusBar_StatusMsg = m_vStatusBar->insertItem(i18n("Ready."), -1);
  m_idStatusBar_URLMsg = m_vStatusBar->insertItem("", -1);

  QListIterator<KoHTMLChild> it = m_pDoc->childIterator();
  for (; it.current(); ++it)
      slotInsertObject(it.current());
      
  m_vMainWindow->setPartCaption( id(), m_strCaptionText );      
  
  slotUpdateConfig();

  if (m_browserStart == 0) slotDocumentContentChanged();
  else if (m_browserStart == 1) m_pDoc->openURL(m_strHomePage);
}

bool KoHTMLView::event(const char *event, const CORBA::Any &value)
{

  EVENT_MAPPER(event, value);
  
  MAPPING(OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenuBar);
  MAPPING(OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolBar);
  
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
  m_idBack = m_vMainToolBar->insertButton2(pix, ID_BACK, SIGNAL(clicked()), this, "slotBack", !m_backStack.isEmpty(), i18n("Back"), -1);

  pix = OPUIUtils::convertPixmap(ICON("forward.xpm"));
  m_idForward = m_vMainToolBar->insertButton2(pix, ID_FORWARD, SIGNAL(clicked()), this, "slotForward", !m_forwardStack.isEmpty(), i18n("Forward"), -1);

  pix = OPUIUtils::convertPixmap(ICON("home.xpm"));
  m_idHome = m_vMainToolBar->insertButton2(pix, ID_HOME, SIGNAL(clicked()), this, "slotHome", true, i18n("Home"), -1);

  pix = OPUIUtils::convertPixmap(ICON("reload.xpm"));
  m_idReload = m_vMainToolBar->insertButton2(pix, ID_RELOAD, SIGNAL(clicked()), this, "slotReload", true, i18n("Reload Page"), -1);

  //grmpfl... there's already a stop icon in koffice/pics/toolbar, but I
  //don't want to use it, I want the original kfm icon
  pix = OPUIUtils::convertPixmap(ICON( kapp->kde_toolbardir() + "/stop.xpm" ));
  m_idStop = m_vMainToolBar->insertButton2(pix, ID_STOP, SIGNAL(clicked()), this, "slotStop", m_pDoc->documentLoading(), i18n("Stop"), -1);

  pix = OPUIUtils::convertPixmap(ICON("go-url3.xpm"));
  m_idOpenURL = m_vMainToolBar->insertButton2(pix, ID_OPENURL, SIGNAL(clicked()), this, "slotOpenURLDlg", true, i18n("Open URL"), -1);
    
  m_vMainToolBar->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("editcopy.xpm"));
  m_idButton_Copy = m_vMainToolBar->insertButton2(pix, ID_EDIT_COPY, SIGNAL(clicked()), this, "editCopy", true, i18n("Copy"), -1);

  m_vMainToolBar->insertSeparator(-1);

  pix = OPUIUtils::convertPixmap(ICON("configure.xpm"));
  m_idConfigure = m_vMainToolBar->insertButton2(pix, ID_OPTIONS_SETTINGS, SIGNAL(clicked()), this, "editSettings", true, i18n("Settings"), -1);

  m_vMainToolBar->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("parts.xpm"));
  m_idInsert_Object = m_vMainToolBar->insertButton2(pix, ID_EDIT_INSERT_OBJECT, SIGNAL(clicked()), this, "insertObject", true, i18n("Insert Object"), -1);

  pix = OPUIUtils::convertPixmap(ICON("edit-html.xpm"));
  m_idEditHTMLCode = m_vMainToolBar->insertButton2(pix, ID_EDIT_HTMLCODE, SIGNAL(clicked()), this, "editHTMLCode", true, i18n("Edit current HTML code"), -1);
  
  m_vLocationToolBar = factory->create(OpenPartsUI::ToolBarFactory::Transient);

  m_vLocationToolBar->setFullWidth(true);

  m_vLocationToolBar->insertTextLabel(i18n("Location : "), -1, -1);
  
  m_idLocation = m_vLocationToolBar->insertLined(m_pDoc->htmlURL(), ID_LOCATION, SIGNAL(returnPressed()), this, "slotURLEntered", true, i18n("Current Location"), 70, -1);
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
  
  menuBar->insertMenu(i18n("&Edit"), m_vMenuEdit, -1, -1);

  pix = OPUIUtils::convertPixmap(ICON("editcopy.xpm"));
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6(pix, i18n("&Copy"), this, "editCopy", CTRL + Key_C, ID_EDIT_COPY, -1);

  m_vMenuEdit->insertSeparator(-1);
  
  m_vMenuEdit->insertItem8(i18n("&Insert"), m_vMenuEdit_Insert, -1, -1);
  
  pix = OPUIUtils::convertPixmap(ICON("parts.xpm"));  
  m_idMenuEdit_Insert_Object = m_vMenuEdit_Insert->insertItem6(pix, i18n("&Object..."), this, "insertObject", 0, ID_EDIT_INSERT_OBJECT, -1);

  m_vMenuEdit->insertSeparator(-1);
  
  pix = OPUIUtils::convertPixmap(ICON("edit-html.xpm"));
  m_vMenuEdit->insertItem6(pix, i18n("Edit HTML code"), this, "editHTMLCode", 0, ID_EDIT_HTMLCODE, -1);

  menuBar->insertMenu(i18n("Bookmarks"), m_vMenuBookmarks, -1, -1);
  
  m_vMenuBar->connect("activated", this, "slotBookmarkSelected");

  m_vMenuBookmarks->insertItem4(i18n("Add Bookmark"), this, "addBookmark", 0, ID_BOOKMARKS_ADD, -1);
  m_vMenuBookmarks->insertItem4(i18n("Edit Bookmarks"), this, "editBookmarks", 0, ID_BOOKMARKS_EDIT, -1);
  m_vMenuBookmarks->insertSeparator(-1);
  
  QString bdir(kapp->localkdedir().data());
  bdir += "/share/apps/kfm/bookmarks";
  scanBookmarks( m_vMenuBookmarks, bdir );  

  menuBar->insertMenu(i18n("&Options"), m_vMenuOptions, -1, -1);

  pix = OPUIUtils::convertPixmap(ICON("configure.xpm"));
  m_idMenuOptions_Settings = m_vMenuOptions->insertItem6(pix, i18n("Settings..."), this, "editSettings", 0, ID_OPTIONS_SETTINGS, -1);

  m_vMenuOptions->insertSeparator(-1);

  m_idMenuOptions_View_ToolBar = m_vMenuOptions->insertItem4(i18n("Show/Hide Tool&bar"), this, "viewToolBar", 0, ID_OPTIONS_VIEW_TOOLBAR, -1);
  m_idMenuOptions_View_StatusBar = m_vMenuOptions->insertItem4(i18n("Show/Hide &Statusbar"), this, "viewStatusBar", 0, ID_OPTIONS_VIEW_STATUSBAR, -1);

  m_vMenuOptions->setItemChecked(m_idMenuOptions_View_ToolBar, m_bToolBarVisible);
  m_vMenuOptions->setItemChecked(m_idMenuOptions_View_StatusBar, m_bStatusBarVisible);

  return true;
}

void KoHTMLView::resizeEvent(QResizeEvent *ev)
{
  m_pHTMLView->setGeometry(0, 0, width(), height());
}

void KoHTMLView::pushURLToHistory()
{
  if (m_bStackLock) return;
  
  SavedPage *p = m_pHTMLView->saveYourself();
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
  
  if (!m_pHTMLView->isTextSelected()) return;
  
  m_pHTMLView->getSelectedText(text);
  
  clip->setText(text);
}

void KoHTMLView::editHTMLCode()
{
  HTMLEditDlg htmlEditDlg(QString(m_pDoc->htmlData()));
  
  if (htmlEditDlg.exec() == QDialog::Accepted)
     m_pDoc->feedData(i18n("meta:/(manually edited code)"), htmlEditDlg.getText());
}

void KoHTMLView::editSettings()
{
  SettingsDlg settingsDlg;
  
  QObject::connect(&settingsDlg, SIGNAL(configChanged()),
                   this, SLOT(slotUpdateConfig()));

  settingsDlg.exec();
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

void KoHTMLView::slotStatusMsg(const char *text)
{
  if (m_vStatusBar != 0L)
     {
       m_vStatusBar->clear();
       m_vStatusBar->message(text, m_idStatusBar_StatusMsg);
     }
}

void KoHTMLView::statusCallback(CORBA::Long ID)
{
  switch (ID)
    {
      case ID_EDIT_COPY              : slotStatusMsg(i18n("Copies the selected section to the clipboard")); break;
      case ID_EDIT_INSERT_OBJECT     : slotStatusMsg(i18n("Inserts an embedded object into the document")); break;
      case ID_EDIT_HTMLCODE          : slotStatusMsg(i18n("Edit the current html code")); break;
      case ID_OPTIONS_SETTINGS       : slotStatusMsg(i18n("Change user settings")); break;
      case ID_OPTIONS_VIEW_TOOLBAR   : slotStatusMsg(i18n("Enables / disables the toolbar")); break;
      case ID_OPTIONS_VIEW_STATUSBAR : slotStatusMsg(i18n("Enables / disables the statusbar")); break;
      case ID_BOOKMARKS_ADD          : slotStatusMsg(i18n("Add the current url to the bookmark list")); break;
      case ID_BOOKMARKS_EDIT         : slotStatusMsg(i18n("Edit the bookmark list")); break;
      case ID_BACK		     : slotStatusMsg(i18n("Load the previous document in the history")); break;
      case ID_FORWARD                : slotStatusMsg(i18n("Load the next document in the history")); break;
      case ID_HOME		     : slotStatusMsg(i18n("Load the start document")); break;
      case ID_RELOAD		     : slotStatusMsg(i18n("Reload the current document")); break;      
      case ID_STOP		     : slotStatusMsg(i18n("Stop loading the current document")); break;
      case ID_OPENURL                : slotStatusMsg(i18n("Open an URL dialog")); break;
      default                        : slotStatusMsg(i18n("Ready."));
    }
}

void KoHTMLView::slotDocumentContentChanged()
{
  QString url = m_pDoc->htmlURL();
  QString data = m_pDoc->htmlData();

  KURL u(url);  

  m_pHTMLView->begin(u.url());  
  m_pHTMLView->parse();
  m_pHTMLView->write(data);
  m_pHTMLView->end();
  m_pHTMLView->show();

  if (m_vLocationToolBar) m_vLocationToolBar->setLinedText(ID_LOCATION, u.url());
} 

void KoHTMLView::slotURLEntered()
{
  QString url = m_vLocationToolBar->linedText(ID_LOCATION);
  
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
       tmp += m_vLocationToolBar->linedText(ID_LOCATION);
       QMessageBox::critical((QWidget *)0L, i18n("KoHTML Error"), tmp);
       return;
     }     
     
  pushURLToHistory();       
  m_pDoc->openURL(url);
}

void KoHTMLView::addBookmark()
{
  QString url = m_pDoc->htmlURL();
  QString title = m_strCaptionText;
  K2URL u(url);
  
  KMimeType::initStatic();
  KMimeMagic::initStatic();  

  QFileInfo icon(KPixmapCache::pixmapFileForURL(url, 0, u.isLocalFile(), false));
  QFileInfo miniIcon(KPixmapCache::pixmapFileForURL(url, 0, u.isLocalFile(), true));
  
  
  if (title.isEmpty())
     title = u.filename().c_str();

  QString p = kapp->localkdedir().data();
  
  p += "/share/apps/kfm/bookmarks/";
  p += title;
     
  KSimpleConfig c(p, true);
  
  c.setGroup("KDE Desktop Entry");
  c.writeEntry("Type", "Link");
  c.writeEntry("URL", url);
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
       m_pDoc->openURL(it->second->data());
     }  
}

void KoHTMLView::slotBack()
{
  if (m_backStack.isEmpty()) return;
  
  SavedPage *s = m_backStack.pop();
  SavedPage *p = m_pHTMLView->saveYourself();
  
  m_forwardStack.push(p);
  
  updateHistory(!m_backStack.isEmpty(), true);

  m_bStackLock = true; 
  m_pHTMLView->restore(s); 
  m_bStackLock = false;
  
  delete s;
}

void KoHTMLView::slotForward()
{
  if (m_forwardStack.isEmpty()) return;
  
  SavedPage *s = m_forwardStack.pop();
  SavedPage *p = m_pHTMLView->saveYourself();
  
  m_backStack.push(p);
  
  updateHistory(true, !m_forwardStack.isEmpty());
  
  m_bStackLock = true;
  m_pHTMLView->restore(s);
  m_bStackLock = false;
  
  delete s;
}

void KoHTMLView::slotHome()
{
  m_pDoc->openURL(m_strHomePage);
}

void KoHTMLView::slotReload()
{
  cerr << "void KoHTMLView::slotReload()" << endl;

  m_pHTMLView->cancelAllRequests();
  
  m_pDoc->stopLoading();

  m_pDoc->openURL(m_pDoc->htmlURL());
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
       K2URL url( openURLDlg.url() );
       
       if (!url.isMalformed())
          {
	    m_pDoc->openURL( url.url().c_str() );
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
	menu->insertItem12(pix, ep->d_name, pop, -1, -1);
	
	//ohoh, this will lead to trouble when activating/deactivating
//	pop->connect("activated", this, "slotBookmarkSelected");

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
	    menu->insertItem11( pix, name, m_idBookmarkId, -1);
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

  if (!CORBA::is_nil( m_vMainWindow ))
    m_vMainWindow->setPartCaption( id(), m_strCaptionText );
}

void KoHTMLView::slotShowURL(KHTMLView *view, const char *url)
{
  QString StatusMsg = url;
  
  StatusMsg.detach();
  
  if (!strnicmp(StatusMsg, "mailto:", 7))
     StatusMsg.remove(0, 7);

  slotStatusMsg(StatusMsg);     
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
	KHTMLView *targetView2 = m_pHTMLView->findView(target);

        tmp.sprintf("targetView1 = %p\ntargetView2 = %p", targetView1, targetView2);
	QMessageBox::critical(0L, "KoHTML", tmp);
	
        if ((!targetView1) && (!targetView2))
          {
	    KoHTMLDoc *doc = new KoHTMLDoc();

	    if (!doc->init()) return;
	    
	    doc->openURL(url);
	    
	    KoHTMLShell *shell = new KoHTMLShell();
	    shell->show();
	    shell->setDocument(doc);
	    return;
	  }

        if (!targetView1)
	   targetView1 = targetView2;

        if (targetView1 != m_pHTMLView)
	   {
             pushURLToHistory();  
	     targetView1->openURL(url);   
	     return;
	   }     

     }

  pushURLToHistory();  
  m_pDoc->openURL(url);     
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
  
  doc->openURL(m_strCurrentURL);
}

void KoHTMLView::slotURLPopup(KHTMLView *view, const char *url, const QPoint &coord)
{
  QPopupMenu *menu = new QPopupMenu;
  
  if (!url) return;
 
  m_strCurrentURL = url;

  menu->insertItem(i18n("Open URL..."), this, SLOT(slotOpenURL()));
  menu->insertItem(i18n("Open URL in new window..."), this, SLOT(slotOpenURLInNewWindow()));
  menu->insertItem(i18n("Copy URL to clipboard"), this, SLOT(slotCopyURLtoClipboard()));
  
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
  
  m_pHTMLView->getKHTMLWidget()->setDefaultBGColor(m_bgColor);
  m_pHTMLView->getKHTMLWidget()->setDefaultTextColors(m_txtColor, m_lnkColor, m_vlnkColor);
  m_pHTMLView->getKHTMLWidget()->setStandardFont(m_standardFont.family());
  m_pHTMLView->getKHTMLWidget()->setFixedFont(m_fixedFont.family());
  
  config->sync();
}