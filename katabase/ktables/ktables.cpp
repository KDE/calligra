/***************************************************************************
                          ktables.cpp  -  description                              
                             -------------------                                         
    begin                : Mið Júl  7 17:04:49 CEST 1999
                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


// include files for QT
#include <qdir.h>
#include <qstrlist.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qheader.h>

// include files for KDE
#include <kiconloader.h>
#include <kmsgbox.h>
#include <kfiledialog.h>

// application specific includes
#include <ktables.h>
#include "ktablesview.h"
#include "ktablesdoc.h"
#include "ktablesserver.h"
#include "tableselect.h"
#include "resource.h"


KtablesApp::KtablesApp()
{
	config=kapp->getConfig();
	
	tableSelect  = new TableSelect( 0,"tables_select" );
	tableSelect->hide();
	
	connect( tableSelect,SIGNAL(tableSelectionOk()),this,SLOT(tableActive()) );
	connect( tableSelect,SIGNAL(signalMsg(const char *)),this,SLOT(slotStatusMsg(const char *)) );
	
	selectServer = new KtablesServer( 0,"select_server" );
	selectServer->hide();
	
	connect( selectServer,SIGNAL(serverSelected()),this,SLOT(serverOpened()) );
	connect( selectServer,SIGNAL(errorMessage(const char *)),this,SLOT(slotStatusMsg(const char *)) );
	
  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initMenuBar();
  initToolBar();
  initStatusBar();
  initKeyAccel();
  initDocument();
  initView();
	
	readOptions();

  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar items at startup
	disableCommand(ID_FILE_SAVE);
	disableCommand(ID_FILE_SAVE_AS);
	disableCommand(ID_FILE_PRINT);
	
	disableCommand(ID_EDIT_CUT);
	disableCommand(ID_EDIT_COPY);
	disableCommand(ID_EDIT_PASTE);
	disableCommand(ID_EDIT_DELETE);
	disableCommand(ID_EDIT_INSERT);
	disableCommand(ID_EDIT_COMMIT);
	disableCommand(ID_EDIT_DISCARD);
	
	disableCommand(ID_QUERY_TABLES);
	disableCommand(ID_QUERY_COLUMNS);
	disableCommand(ID_QUERY_CRITERIA);
	disableCommand(ID_QUERY_SORT);
	disableCommand(ID_QUERY_FETCH);
}

KtablesApp::~KtablesApp()
{

}

void KtablesApp::initKeyAccel()
{
	key_accel = new KAccel( this );
	
	// file_menu accelerators
	key_accel->connectItem( KAccel::New, this, SLOT( slotFileNew() ) );
	key_accel->connectItem( KAccel::Open, this, SLOT( slotFileOpen() ) );
	key_accel->connectItem( KAccel::Save, this, SLOT( slotFileSave() ) );
	key_accel->connectItem( KAccel::Close, this, SLOT( slotFileClose() ) );
	key_accel->connectItem( KAccel::Print, this, SLOT( slotFilePrint() ) );
	key_accel->connectItem( KAccel::Quit, this, SLOT( slotFileQuit() ) );
	// edit_menu accelerators
	key_accel->connectItem( KAccel::Cut, this, SLOT( slotEditCut() ) );
	key_accel->connectItem( KAccel::Copy, this, SLOT( slotEditCopy() ) );
	key_accel->connectItem( KAccel::Paste, this, SLOT( slotEditPaste() ) );

	key_accel->connectItem( KAccel::Help, kapp, SLOT( appHelpActivated() ) );
			
	key_accel->changeMenuAccel(	file_menu, ID_FILE_NEW, KAccel::New );
	key_accel->changeMenuAccel(	file_menu, ID_FILE_OPEN, KAccel::Open );
	key_accel->changeMenuAccel(	file_menu, ID_FILE_SAVE, KAccel::Save );
	key_accel->changeMenuAccel( file_menu, ID_FILE_CLOSE, KAccel::Close );
	key_accel->changeMenuAccel(	file_menu, ID_FILE_PRINT, KAccel::Print );
	key_accel->changeMenuAccel(	file_menu, ID_FILE_QUIT, KAccel::Quit );

  key_accel->changeMenuAccel(	edit_menu, ID_EDIT_CUT, KAccel::Cut );
  key_accel->changeMenuAccel(	edit_menu, ID_EDIT_COPY, KAccel::Copy );
  key_accel->changeMenuAccel(	edit_menu, ID_EDIT_PASTE, KAccel::Paste );

	key_accel->readSettings();	
}

void KtablesApp::initMenuBar()
{

  ///////////////////////////////////////////////////////////////////
  // MENUBAR  

	recent_files_menu = new OPMenu();
  connect( recent_files_menu, SIGNAL(activated(int)), SLOT(slotFileOpenRecent(int)) );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry file_menu
  file_menu = new OPMenu();
  file_menu->insertItem(Icon("mini/ktablesapp.xpm"), i18n("New &Window"), ID_FILE_NEW_WINDOW );
  file_menu->insertSeparator();
  file_menu->insertItem(Icon("filenew.xpm"), i18n("&New"), ID_FILE_NEW );
  file_menu->insertItem(Icon("fileopen.xpm"), i18n("&Open..."), ID_FILE_OPEN );
	file_menu->insertItem(i18n("Open &recent"), recent_files_menu, ID_FILE_OPEN_RECENT );

  file_menu->insertItem(i18n("&Close"), ID_FILE_CLOSE );
  file_menu->insertSeparator();
  file_menu->insertItem(Icon("filefloppy.xpm") ,i18n("&Save"), ID_FILE_SAVE );
  file_menu->insertItem(i18n("Save &As..."), ID_FILE_SAVE_AS );
  file_menu->insertSeparator();
  file_menu->insertItem(Icon("fileprint.xpm"), i18n("&Print..."), ID_FILE_PRINT );
  file_menu->insertSeparator();
  file_menu->insertItem(i18n("E&xit"), ID_FILE_QUIT );

	
  ///////////////////////////////////////////////////////////////////
  // menuBar entry edit_menu
  edit_menu = new OPMenu();
  edit_menu->insertItem(Icon("editcut.xpm"), i18n("Cu&t"), ID_EDIT_CUT );
  edit_menu->insertItem(Icon("editcopy.xpm"), i18n("&Copy"), ID_EDIT_COPY );
  edit_menu->insertItem(Icon("editpaste.xpm"), i18n("&Paste"), ID_EDIT_PASTE );
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("&Delete"), ID_EDIT_DELETE);
  edit_menu->insertItem(i18n("&Insert"), ID_EDIT_INSERT);
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("Co&mmit changes"), ID_EDIT_COMMIT);
  edit_menu->insertItem(i18n("Di&scard changes"), ID_EDIT_DISCARD);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry view_menu
  view_menu = new OPMenu();
  view_menu->setCheckable(true);
  view_menu->insertItem(i18n("&Toolbar"), ID_VIEW_TOOLBAR);
  view_menu->insertItem(i18n("&Statusbar"), ID_VIEW_STATUSBAR );

	///////////////////////////////////////////////////////////////////
	// menuBar entry query_menu
  query_menu = new OPMenu();
  query_menu->insertItem(i18n("Ser&ver connection"), ID_QUERY_SERVER);
  query_menu->insertSeparator();
  query_menu->insertItem(i18n("&Select tables..."), ID_QUERY_TABLES);
  query_menu->insertItem(i18n("Select &columns..."), ID_QUERY_COLUMNS);
  query_menu->insertItem(i18n("Search cr&iteria"), ID_QUERY_CRITERIA);
  query_menu->insertSeparator();
  query_menu->insertItem(i18n("S&ort"), ID_QUERY_SORT);
  query_menu->insertSeparator();
  query_menu->insertItem(i18n("Fetch rows"), ID_QUERY_FETCH);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry help_menu
  QString aboutstring=QString("%1 %2\n\n").arg(kapp->name()).arg(VERSION);

  help_menu = new OPMenu();
  help_menu = (OPMenu *)komapp->getHelpMenu( true, aboutstring );

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  // insert your popup menus with the according menubar entries in the order
  // they will appear later from left to right
  opMenuBar()->insertItem(i18n("&File"), file_menu);
  opMenuBar()->insertItem(i18n("&Edit"), edit_menu);
  opMenuBar()->insertItem(i18n("&View"), view_menu);
  opMenuBar()->insertItem(i18n("&Query"), query_menu);

  opMenuBar()->insertSeparator();
  opMenuBar()->insertItem(i18n("&Help"), help_menu);

  ///////////////////////////////////////////////////////////////////
  // CONNECT THE MENU SLOTS WITH SIGNALS
  // for execution slots and statusbar messages

  connect(file_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(file_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(edit_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(edit_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(view_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(view_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(query_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(query_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

}

void KtablesApp::initToolBar()
{

  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  opToolBar()->insertButton(Icon("filenew.xpm"), ID_FILE_NEW, true, i18n("New File") );
  opToolBar()->insertButton(Icon("fileopen.xpm"), ID_FILE_OPEN, true, i18n("Open File") );
  opToolBar()->insertButton(Icon("filefloppy.xpm"), ID_FILE_SAVE, true, i18n("Save File") );
  opToolBar()->insertButton(Icon("fileprint.xpm"), ID_FILE_PRINT, true, i18n("Print") );
  opToolBar()->insertSeparator();
  opToolBar()->insertButton(Icon("editcut.xpm"), ID_EDIT_CUT, true, i18n("Cut") );
  opToolBar()->insertButton(Icon("editcopy.xpm"), ID_EDIT_COPY, true, i18n("Copy") );
  opToolBar()->insertButton(Icon("editpaste.xpm"), ID_EDIT_PASTE, true, i18n("Paste") );
  opToolBar()->insertSeparator();
  opToolBar()->insertButton(Icon("network_settings.xpm"), ID_QUERY_SERVER, SIGNAL(clicked()),
          this, SLOT( openServer() ),true,i18n("Server"));
  opToolBar()->insertButton(Icon("help.xpm"), ID_HELP_CONTENTS, SIGNAL(clicked() ),
  				kapp, SLOT( appHelpActivated() ), true,i18n("Help"));

  ///////////////////////////////////////////////////////////////////
  // INSERT YOUR APPLICATION SPECIFIC TOOLBARS HERE WITH toolBar(n)


  ///////////////////////////////////////////////////////////////////
  // CONNECT THE TOOLBAR SLOTS WITH SIGNALS - add new created toolbars by their according number
	// connect for invoking the slot actions
  connect(opToolBar(), SIGNAL(clicked(int)), SLOT(commandCallback(int)));
	// connect for the status help on holing icons pressed with the mouse button
  connect(opToolBar(), SIGNAL(pressed(int)), SLOT(statusCallback(int)));

}

void KtablesApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
	// TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG );
}

void KtablesApp::initDocument()
{
  doc = new KtablesDoc(this);
  doc->newDocument();
}

void KtablesApp::initView()
{
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

 	OpenParts::View_var v = doc->createView();
 	view = doc->currentView();
 	setView( view );
  setCaption( doc->getTitle() );

}

void
KtablesApp::setCaption(const QString& p_str)
{
	KTMainWindow::setCaption(QString("%1: %2").arg(kapp->name()).arg(p_str));
}

void KtablesApp::enableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // enable menu and toolbar functions by their ID's
  opMenuBar()->setItemEnabled(id_,true);
  opToolBar()->setItemEnabled(id_,true);
}

void KtablesApp::disableCommand(int id_)
{
  ///////////////////////////////////////////////////////////////////
  // disable menu and toolbar functions by their ID's
  opMenuBar()->setItemEnabled(id_,false);
  opToolBar()->setItemEnabled(id_,false);
}

void KtablesApp::addRecentFile(const char* file)
{
  if(recent_files.find(file) == -1){
    if( recent_files.count() < 5)
      recent_files.insert(0,file);
    else{
      recent_files.remove(4);
      recent_files.insert(0,file);
    }
    recent_files_menu->clear();
    for ( int i =0 ; i < (int)recent_files.count(); i++){
      recent_files_menu->insertItem(recent_files.at(i));
    }
	}
}

void KtablesApp::openDocumentFile(const char* _cmdl)
{
  slotStatusMsg(i18n("Opening file..."));
	doc->openDocument(_cmdl);
  slotStatusMsg(i18n("Ready."));
}


KtablesDoc* KtablesApp::getDocument() const
{
	return doc;
}

void KtablesApp::saveOptions()
{	
	config->setGroup("General Options");
	config->writeEntry("Geometry", size() );
  config->writeEntry("Show Toolbar", opToolBar()->isVisible());
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
  config->writeEntry("MenuBarPos", (int)menuBar()->menuBarPos());
  config->writeEntry("ToolBarPos",  (int)opToolBar()->barPos());
	config->writeEntry("Recent Files", recent_files);
}


void KtablesApp::readOptions()
{
	
	config->setGroup("General Options");

	// bar status settings
	bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
	view_menu->setItemChecked(ID_VIEW_TOOLBAR, bViewToolbar);
	if(!bViewToolbar)
		enableToolBar(KToolBar::Hide);
	
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
	view_menu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
	if(!bViewStatusbar)
		enableStatusBar(KStatusBar::Hide);
	
	// bar position settings
	KMenuBar::menuPosition menu_bar_pos;
	menu_bar_pos=(KMenuBar::menuPosition)config->readNumEntry("MenuBarPos", KMenuBar::Top);

  KToolBar::BarPosition tool_bar_pos;
  tool_bar_pos=(KToolBar::BarPosition)config->readNumEntry("ToolBarPos", KToolBar::Top);

	menuBar()->setMenuBarPos(menu_bar_pos);
  toolBar()->setBarPos(tool_bar_pos);
	
  // initialize the recent file list
	recent_files.setAutoDelete(TRUE);
	config->readListEntry("Recent Files",recent_files);
	
	uint i;
	for ( i =0 ; i < recent_files.count(); i++){
    recent_files_menu->insertItem(recent_files.at(i));
  }

  QSize size=config->readSizeEntry("Geometry");
	if(!size.isEmpty())
		resize(size);
}

void KtablesApp::saveProperties(KConfig* )
{
	if( doc->getTitle() != i18n("Untitled") && !doc->isModified()){
		return;
	}
	else{
		QString filename=doc->getPathName()+doc->getTitle();	
    config->writeEntry("filename",filename);
    config->writeEntry("modified",doc->isModified());
		
		const char* tempname = kapp->tempSaveName(filename);
		doc->saveDocument(tempname);
	}
}


void KtablesApp::readProperties(KConfig*)
{
	QString filename = config->readEntry("filename","");
	bool modified = config->readBoolEntry("modified",false);
  if( modified ){
  	bool b_canRecover;
		QString tempname = kapp->checkRecoverFile(filename,b_canRecover);
  	
  	if(b_canRecover){
   			doc->openDocument(tempname);
   			doc->setModified();
   			QFileInfo info(filename);
   			doc->pathName(info.absFilePath());
   			doc->title(info.fileName());
   			QFile::remove(tempname);
		}
	}
 	else if(!filename.isEmpty()){
		doc->openDocument(filename);
	}
  setCaption(doc->getTitle());
}		

bool KtablesApp::queryClose()
{
	return doc->saveModified();
}

bool KtablesApp::queryExit()
{
	saveOptions();
	return true;
}

/////////////////////////////////////////////////////////////////////
// Open a server connection
/////////////////////////////////////////////////////////////////////
void
KtablesApp::openServer()
{
	slotStatusMsg(i18n("Selecting server..."));
	selectServer->show();
}

void
KtablesApp::serverOpened()
{
	slotStatusMsg(i18n("Initializing table list..."));
	enableCommand(ID_QUERY_TABLES);
	disableCommand(ID_QUERY_SERVER);
	tableSelect->init();
}

/////////////////////////////////////////////////////////////////////
// Table selection made
/////////////////////////////////////////////////////////////////////
void
KtablesApp::tableActive()
{
	enableCommand(ID_QUERY_CRITERIA);
	enableCommand(ID_QUERY_SORT);
	enableCommand(ID_QUERY_FETCH);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KtablesApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new Application window..."));
	
	KtablesApp* new_window= new KtablesApp();
	new_window->show();
	
  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileNew()
{
  slotStatusMsg(i18n("Creating new document..."));
	
	if(!doc->saveModified())
		return;
	
	doc->newDocument();		
	setCaption(doc->getTitle());

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));
	
	if(!doc->saveModified())
		return;
	
	QString fileToOpen=KFileDialog::getOpenFileName(QDir::homeDirPath(), "", this, i18n("Open File..."));
	if(!fileToOpen.isEmpty()){
		doc->openDocument(fileToOpen);
		setCaption(doc->getTitle());
		addRecentFile(fileToOpen);
	}

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileOpenRecent(int id_)
{
  slotStatusMsg(i18n("Opening file..."));
	
	if(!doc->saveModified())
		return;

  doc->openDocument(recent_files.at(id_));
	setCaption(doc->getTitle());

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
	
	doc->saveDocument(doc->getPathName()+doc->getTitle());

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file under new filename..."));

	QString newName=KFileDialog::getSaveFileName(QDir::currentDirPath(), "", this, i18n("Save As..."));
	if(!newName.isEmpty()){
		QFileInfo saveAsInfo(newName);
		doc->title(saveAsInfo.fileName());
		doc->pathName(saveAsInfo.absFilePath());
		doc->saveDocument(newName);
		addRecentFile(newName);
		setCaption(doc->getTitle());
	}

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."));
	
	close();
}

void KtablesApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  QPrinter printer;
  if (printer.setup(this)){
			view->print(&printer);
	}

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotFileQuit()
{
	saveOptions();
	// close the first window, the list makes the next one the first again.
	// This ensures that queryClose() is called on each window to ask for closing
	KTMainWindow* w;
	if(memberList){
		for(w=memberList->first(); w; w=memberList->first()){
			// only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
			// the window and the application stay open.
			if(!w->close())
				break;
		}
	}	
}

void KtablesApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to Clipboard..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting Clipboard contents..."));

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotViewToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
	if( view_menu->isItemChecked(ID_VIEW_TOOLBAR))
	  view_menu->setItemChecked(ID_VIEW_TOOLBAR, false);
	else
		view_menu->setItemChecked(ID_VIEW_TOOLBAR, true);
		
  enableToolBar();

  slotStatusMsg(i18n("Ready."));
}

void KtablesApp::slotViewStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
	if( view_menu->isItemChecked(ID_VIEW_STATUSBAR))
	  view_menu->setItemChecked(ID_VIEW_STATUSBAR, false);
	else
		view_menu->setItemChecked(ID_VIEW_STATUSBAR, true);

  enableStatusBar();

  slotStatusMsg(i18n("Ready."));
}


void KtablesApp::slotStatusMsg(const char *text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG );
}


void KtablesApp::slotStatusHelpMsg(const char *text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}



void KtablesApp::commandCallback(int id_){
  switch (id_){
    case ID_FILE_NEW_WINDOW:
    	slotFileNewWindow();
    	break;
    case ID_FILE_NEW:
    	slotFileNew();
    	break;
    case ID_FILE_OPEN:
    	slotFileOpen();
    	break;
    case ID_FILE_SAVE:
    	slotFileSave();
    	break;
    case ID_FILE_SAVE_AS:
    	slotFileSaveAs();
    	break;
    case ID_FILE_CLOSE:
    	slotFileClose();
    	break;
    case ID_FILE_PRINT:
    	slotFilePrint();
    	break;
    case ID_FILE_QUIT:
    	slotFileQuit();
    	break;

    case ID_EDIT_CUT:
    	slotEditCut();
    	break;
    case ID_EDIT_COPY:
    	slotEditCopy();
    	break;
    case ID_EDIT_PASTE:
    	slotEditPaste();
    	break;
    case ID_EDIT_DELETE:
    	doc->removeRow();
    	break;
    case ID_EDIT_INSERT:
    	doc->insertRow();
    	break;
    case ID_EDIT_COMMIT:
    	doc->commitChanges();
    	break;
    case ID_EDIT_DISCARD:
    	doc->discardChanges();
    	break;
  
    case ID_VIEW_TOOLBAR:
    	slotViewToolBar();
    	break;
    case ID_VIEW_STATUSBAR:
    	slotViewStatusBar();
    	break;

    case ID_QUERY_SERVER:
    	openServer();
    	break;
    case ID_QUERY_TABLES:
  		tableSelect->show();
  		break;
  	case ID_QUERY_FETCH:
  		view->setTable( tableSelect->tableName(0) );
  		if ( tableSelect->tableName(0) != QString::null )
	  		doc->title( tableSelect->tableName(0) );
	  	if ( view->childCount() ) {
	  		enableCommand( ID_EDIT_INSERT );
	  		enableCommand( ID_EDIT_DELETE );
	  		enableCommand( ID_EDIT_COMMIT );
	  		enableCommand( ID_EDIT_DISCARD );
	  		disableCommand( ID_QUERY_TABLES );
	  		disableCommand( ID_QUERY_CRITERIA );
	  		disableCommand( ID_QUERY_SORT );
	  		disableCommand( ID_QUERY_FETCH );
	  	}
	  	setCaption( doc->getTitle() );
  		break;

		default:
    	break;
  }
}

void KtablesApp::statusCallback(int id_){
  switch (id_){
  	case ID_QUERY_TABLES:
  		slotStatusHelpMsg(i18n("Displays a dialog to select tables to query"));
  		break;
    case ID_FILE_NEW_WINDOW:
    	slotStatusHelpMsg(i18n("Opens a new application window"));
    	break;
    case ID_FILE_NEW:
 	  	slotStatusHelpMsg(i18n("Creates a new document"));
 	  	break;
    case ID_FILE_OPEN:
 	  	slotStatusHelpMsg(i18n("Opens an existing document"));
 	  	break;
    case ID_FILE_OPEN_RECENT:
 	  	slotStatusHelpMsg(i18n("Opens a recently used file"));
 	  	break;
    case ID_FILE_SAVE:
 	  	slotStatusHelpMsg(i18n("Save the actual document"));
 	  	break;
    case ID_FILE_SAVE_AS:
 	  	slotStatusHelpMsg(i18n("Save the document as..."));
 	  	break;
    case ID_FILE_CLOSE:
    	slotStatusHelpMsg(i18n("Closes the actual file"));
    	break;
    case ID_FILE_PRINT:
    	slotStatusHelpMsg(i18n("Prints the current document"));
    	break;
    case ID_FILE_QUIT:
    	slotStatusHelpMsg(i18n("Exits ") + kapp->name());
    	break;

    case ID_EDIT_CUT:
    	slotStatusHelpMsg(i18n("Cuts the selected section and puts it to the clipboard"));
    	break;
    case ID_EDIT_COPY:
    	slotStatusHelpMsg(i18n("Copys the selected section to the clipboard"));
    	break;
    case ID_EDIT_PASTE:
    	slotStatusHelpMsg(i18n("Pastes the clipboard contents to actual position"));
    	break;

    case ID_VIEW_TOOLBAR:
    	slotStatusHelpMsg(i18n("Enables / disables the actual Toolbar"));
    	break;
    case ID_VIEW_STATUSBAR:
    	slotStatusHelpMsg(i18n("Enables / disables the actual Statusbar"));
    	break;
		default:
			break;
  }
}

#include "ktables.moc"

