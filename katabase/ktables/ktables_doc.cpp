/***************************************************************************
                          ktablesdoc.cpp  -  description                              
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

// include files for Qt
#include <qdir.h>
#include <qfileinfo.h>
#include <qwidget.h>

// include files for KDE
#include <kapp.h>
#include <kmessagebox.h>
#include <opView.h>

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

#include "ktables_doc.h"
#include "ktables_shell.h"
#include "ktables_view.h"

KtablesDoc::KtablesDoc(QWidget *parent, const char* name)
 : QObject(parent, name),
   KoDocument(),
   KoPrintExt(),
   Ktables::Document_skel()
{
	kdebug(KDEBUG_INFO,0,"KtablesDoc()");
	b_modified = false;
	QObject::connect( this,SIGNAL(signalMsg(const char *)),parent,SLOT(slotStatusMsg(const char *)) );
}

KtablesDoc::~KtablesDoc()
{
	cleanUp();
}

void
KtablesDoc::cleanUp()
{
  if ( m_bIsClean )
    return;

  ASSERT( m_lstViews.count() == 0 );

  KoDocument::cleanUp();
}

CORBA::Boolean
KtablesDoc::initDoc()
{
  return true;
}

KtablesView *
KtablesDoc::createTableView( QWidget *p_parent )
{
	KtablesView *p = new KtablesView( 0,this,p_parent );
	
	addView( p );
	return p;
}

OpenParts::View_ptr
KtablesDoc::createView()
{
	return OpenParts::View::_duplicate( createTableView() );
}

/*KtablesView *
KtablesDoc::currentView()
{
	if ( m_lstViews.count() == 0 )
		return 0;
	return (KtablesView *)m_lstViews.current();
}*/

KOffice::MainWindow_ptr
KtablesDoc::createMainWindow()
{
  KtablesApp *app = new KtablesApp;
  app->show();
  app->setDocument( this );

  kdebug( KDEBUG_INFO, 0, "KtablesDoc::createMainWindow()" );
  return KOffice::MainWindow::_duplicate( app->koInterface() );
}

void
KtablesDoc::addView(KtablesView *p_view)
{
	registerView( p_view );
	QObject::connect( p_view,SIGNAL(signalMsg(const char *)),this,SIGNAL(signalMsg(const char *)) );
}

void
KtablesDoc::removeView(KtablesView *p_view)
{
	unregisterView( p_view );
	QObject::disconnect( p_view );
}

const QString& KtablesDoc::getPathName() const
{
	return m_path;
}

void KtablesDoc::pathName( const char* path_name)
{
	m_path=path_name;
}

void KtablesDoc::title( const char* title)
{
	m_title=title;
}

const QString& KtablesDoc::getTitle() const
{
	return m_title;
}

bool KtablesDoc::openDocument(const char* filename, const char* format)
{
	QFileInfo fileInfo(filename);
	m_title=fileInfo.fileName();
	m_path=fileInfo.absFilePath();	
	/////////////////////////////////////////////////
	// TODO: Add your document opening code here
	/////////////////////////////////////////////////
	
	b_modified = false;
	return true;
}

bool KtablesDoc::saveDocument(const char* filename, const char* format)
{

	/////////////////////////////////////////////////
	// TODO: Add your document saving code here
	/////////////////////////////////////////////////

	b_modified = false;
	return true;
}

void KtablesDoc::deleteContents()
{
	/////////////////////////////////////////////////
	// TODO: Add implementation to delete the document contents
	/////////////////////////////////////////////////

}

void
KtablesDoc::slotUpdateAllViews(KtablesView *p_sender)
{
}

void
KtablesDoc::viewList(OpenParts::Document::ViewList*& _list )
{
  _list->length( m_lstViews.count() );

  int i = 0;
  QListIterator<OPViewIf> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KtablesDoc::viewCount()
{
  return m_lstViews.count();
}

bool
KtablesDoc::isEmpty()
{
  return false;
}

#include "ktables_doc.moc"

