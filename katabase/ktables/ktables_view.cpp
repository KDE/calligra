/***************************************************************************
                          ktablesview.cpp  -  description
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

#include <iostream.h>

// include files for Qt
#include <qprinter.h>
#include <qprintdialog.h>
#include <qpainter.h>
#include <qheader.h>

#include <kiconloader.h>
#include <kstdaccel.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <komApplication.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

// application specific includes
#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>

#include "ktables_view.h"
#include "ktables_doc.h"
#include "ktables_shell.h"
#include "querydialog.h"
#include "tableselect.h"
#include "ktables_server.h"

KtablesView::KtablesView(kdbDataSet *p_set,KtablesDoc *doc,QWidget *parent, const char* name)
 : kdbRecordView(p_set,parent,name),
   KoViewIf(doc),
   OPViewIf(doc),
   Ktables::View_skel()
{
	kdebug(KDEBUG_INFO,0,"KtablesView::KtablesView()");
	ADD_INTERFACE( "IDL:Ktables/View:1.0" );
	
	setWidget( this );
	
	OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

	m_pDoc = doc;
	
	_queryTree    = new QueryDialog( 0,"Query_tree" );
	_queryTree->hide();
	
	_tableSelect  = new TableSelect( 0,"tables_select" );
	_tableSelect->hide();
	
	QObject::connect( _tableSelect,SIGNAL(tableSelectionOk()),this,SLOT(tablesSelectedOk()) );
	
	_selectServer = new KtablesServer( 0,"select_server" );
	_selectServer->hide();
	
	QObject::connect( _selectServer,SIGNAL(serverSelected()),this,SLOT(serverOpenedOk()) );
	
	setBackgroundMode( PaletteBase );
}

KtablesView::~KtablesView()
{
	m_pDoc->unregisterView( this );
	cleanUp();
}

void
KtablesView::selectServer()
{
	emit signalMsg( i18n("Selecting server connection...") );
	_selectServer->move( QCursor::pos() );
	_selectServer->show();
}

void
KtablesView::selectTables()
{
	emit signalMsg( i18n("Selecting tables to query...") );
	_tableSelect->move( QCursor::pos() );
	_tableSelect->show();
}

void
KtablesView::selectQuery()
{
	_queryTree->move( QCursor::pos() );
	_queryTree->show();
}

void
KtablesView::tablesSelectedOk()
{
	QString s;
	
	_queryTree->clear();
	for( uint i=0;(s=_tableSelect->tableName(i)) != "";i++ )
		_queryTree->addTable( s );
	if ( !CORBA::is_nil(m_vMenuQuery) ) {
		m_vMenuQuery->setItemEnabled( m_idMenuQuery_query,true  );
		m_vMenuQuery->setItemEnabled( m_idMenuQuery_tuples,true );
	}
	emit tableActive();
}

void
KtablesView::serverOpenedOk()
{
	kdebug( KDEBUG_INFO,0,"KtablesView::serverOpenedOk()" );
	_tableSelect->init();
	if ( !CORBA::is_nil(m_vMenuQuery) ) {
		m_vMenuQuery->setItemEnabled( m_idMenuQuery_server,false );
		m_vMenuQuery->setItemEnabled( m_idMenuQuery_tables,true );
	}
	emit serverOpened();
}

void
KtablesView::setupView()
{
	emit signalMsg( i18n("Building query...") );
	_queryTree->buildQuery();
	setTable( _tableSelect->tableName(0) );
}

void
KtablesView::insertRow()
{
	if ( _dataSet ) {
		for( uint i=0;i<_dataSet->fieldSet()->count();i++ )
			_dataSet->field( i ).setText( "" );
		_dataSet->insert();
	}
}

void
KtablesView::removeRow()
{
	if ( _dataSet )
		_dataSet->remove();
}

void
KtablesView::commit()
{
	if ( !Kdb::isOpen() || !_dataSet )
		return;
	kdbRecordView::commitEdits();
}

void
KtablesView::reScanTable()
{
	if ( !_dataSet )
		return;
	_dataSet->runQuery( false );
}

void
KtablesView::cutSelection()
{
}

void
KtablesView::copySelection()
{
}

void
KtablesView::paste()
{
}

void
KtablesView::helpUsing()
{
  kapp->invokeHTMLHelp( "kspread/ktables.html", QString::null );
}

void
KtablesView::setTable(const QString& p_table)
{
	kdbDataSet *set;

	if ( p_table == QString::null || !Kdb::isOpen() || header()->count() > 0 )
		return;
	emit signalMsg( i18n("Fetching tuples...") );
	try {
		if ( (set=Kdb::dataSet(p_table)) ) {
			kdbRecordView::setTable( set );
			set->runQuery( false );
			emit signalMsg( QString("%1 %2").arg(set->records()).arg(i18n("Records.")) );
			if ( !CORBA::is_nil(m_vMenuQuery) ) {
				m_vMenuQuery->setItemEnabled( m_idMenuQuery_tables,false );
				m_vMenuQuery->setItemEnabled( m_idMenuQuery_query,false  );
				m_vMenuQuery->setItemEnabled( m_idMenuQuery_tuples,false );
			}
			if ( !CORBA::is_nil(m_vMenuEdit) ) {
				m_vMenuEdit->setItemEnabled( m_idMenuEdit_insert,true  );
				m_vMenuEdit->setItemEnabled( m_idMenuEdit_remove,true  );
				m_vMenuEdit->setItemEnabled( m_idMenuEdit_commit,true  );
				m_vMenuEdit->setItemEnabled( m_idMenuEdit_discard,true );
			}
		}
	} catch( Kdb::ExceptionTypes e ) {
		QString t = Kdb::exceptionMsg( e );
		
		emit signalMsg( t );
	}
}

void
KtablesView::slotUpdateView()
{
}

KtablesDoc*
KtablesView::getDocument() const
{
	return m_pDoc;
}

void
KtablesView::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "void KtablesView::cleanUp() " );

  if ( m_bIsClean )
  {
    return;
  }

  kdebug( KDEBUG_INFO, 0, "1b) Unregistering menu and toolbar" );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pDoc->removeView( this );

  KoViewIf::cleanUp();
}

void
KtablesView::init()
{
  /******************************************************
   * Menu
   ******************************************************/

  kdebug( KDEBUG_INFO, 0, "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a menu bar manager" );

  /******************************************************
   * Toolbar
   ******************************************************/

  kdebug( KDEBUG_INFO, 0, "Registering toolbar as %li", id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a tool bar manager" );
}

bool KtablesView::event( const QCString & _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

bool KtablesView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr p_factory )
{
  kdebug( KDEBUG_INFO, 0, "bool KtablesView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" );

  if ( CORBA::is_nil( p_factory ) )
  {
    kdebug( KDEBUG_INFO,0,"Setting to nil" );
    m_vToolBarQuery = 0L;
    return true;
  }

  QString wstr;

  m_vToolBarQuery = p_factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarQuery->setFullWidth(false);

  OpenPartsUI::Pixmap_var pix;
  pix = OPICON( "network_settings" );
  m_idButtonQuery_server = m_vToolBarQuery->insertButton2( pix,1,SIGNAL(clicked()),this,"selectServer",true,(wstr = i18n( "Server")),-1 );

  return true;
}

bool KtablesView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr p_mbar )
{
  KStdAccel stdAccel;
  kdebug( KDEBUG_INFO, 0, "bool KtablesView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )" );

  if( CORBA::is_nil( p_mbar ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    m_vMenuQuery = 0L;
    return true;
  }

  QString text;
  OpenPartsUI::Pixmap_var pix;

  // Edit menu
  text = i18n( "&Edit" ) ;
  p_mbar->insertMenu( text, m_vMenuEdit, -1, -1 );

  text = i18n( "Cu&t" );
  pix = OPICON( "editcut" );
  m_idMenuEdit_cut = m_vMenuEdit->insertItem6( pix, text, this, "cutSelection", stdAccel.cut(), -1, -1 );

  text = i18n( "&Copy" ) ;
  pix = OPICON( "editcopy" );
  m_idMenuEdit_copy = m_vMenuEdit->insertItem6( pix, text,this, "copySelection", stdAccel.copy(), -1, -1 );

  text = i18n( "&Paste" ) ;
  pix = OPICON( "editpaste" );
  m_idMenuEdit_paste = m_vMenuEdit->insertItem6( pix, text, this, "paste", stdAccel.paste(), -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = i18n("&Insert row");
  m_idMenuEdit_insert = m_vMenuEdit->insertItem4( text, this, "insertRow", 0, -1, -1 );

  text = i18n("&Delete row");
  m_idMenuEdit_remove = m_vMenuEdit->insertItem4( text, this, "removeRow", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = i18n("Co&mmit edits");
  m_idMenuEdit_commit = m_vMenuEdit->insertItem4( text, this, "commit", 0, -1, -1 );

  text = i18n( "Di&scard edits" ) ;
  m_idMenuEdit_discard = m_vMenuEdit->insertItem4( text, this, "reScanTable", 0, -1, -1 );

  // Query menu
  text = i18n( "&Query" ) ;
  p_mbar->insertMenu( text, m_vMenuQuery, -1, -1 );
	
  text = i18n( "&Connect..." ) ;
  m_idMenuQuery_server = m_vMenuQuery->insertItem4( text, this, "selectServer", 0, -1, -1 );

  m_vMenuQuery->insertSeparator( -1 );

  text = i18n( "Select &Tables" ) ;
  m_idMenuQuery_tables = m_vMenuQuery->insertItem4( text, this, "selectTables", 0, -1, -1 );

  text = i18n( "Set &Criteria" ) ;
  m_idMenuQuery_query = m_vMenuQuery->insertItem4( text, this, "selectQuery", 0, -1, -1 );

  m_vMenuQuery->insertSeparator( -1 );

  text = i18n( "&Fetch tuples" ) ;
  m_idMenuQuery_tuples = m_vMenuQuery->insertItem4( text, this, "setupView", 0, -1, -1 );

  m_vMenuEdit->setItemEnabled( m_idMenuEdit_insert, false  );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_remove, false  );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_commit, false  );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_discard, false );
	
  m_vMenuQuery->setItemEnabled( m_idMenuQuery_tables, false );
  m_vMenuQuery->setItemEnabled( m_idMenuQuery_query, false  );
  m_vMenuQuery->setItemEnabled( m_idMenuQuery_tuples, false );
	
  return true;
}

bool
KtablesView::printDlg()
{
  QPrinter prt;

  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    //m_pDoc->print( &prt );
  }
  return true;
}

void
KtablesView::newView()
{
  ASSERT( m_pDoc != 0L );

  kdebug( KDEBUG_INFO,0,"KtablesView::newView()" );
  KtablesApp *app = new KtablesApp;
  app->show();
  app->setDocument( m_pDoc );
}

#include "ktables_view.moc"

