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
#include <qmessagebox.h>

// include files for KDE
#include <kapp.h>
#include <kmsgbox.h>
#include <opView.h>

// application specific includes
#include <kdb.h>
#include <kdbdataset.h>

#include <ktablesdoc.h>
#include "ktables.h"
#include "ktablesview.h"

KtablesDoc::KtablesDoc(QWidget *parent, const char* name)
 : QObject(parent, name), OPDocumentIf()
{
	QObject::connect( this,SIGNAL(signalMsg(const char *)),parent,SLOT(slotStatusMsg(const char *)) );
}

KtablesDoc::~KtablesDoc()
{
}

void
KtablesDoc::insertRow()
{
	KtablesView *v = currentView();
	
	if ( v )
		v->insertRow();
}

void
KtablesDoc::removeRow()
{
	KtablesView *v = currentView();
	
	if ( v )
		v->removeRow();
}

void
KtablesDoc::commitChanges()
{
	KtablesView *v = currentView();
	
	if ( v )
		v->commit();
}

void
KtablesDoc::discardChanges()
{
	KtablesView *v = currentView();
	
	if ( v )
		v->reScanTable();
}

OpenParts::View_ptr
KtablesDoc::createView()
{
	KtablesView *p = new KtablesView( 0,this,(QWidget *)parent() );
	
	addView( p );
	return OpenParts::View::_duplicate( p );
}

KtablesView *
KtablesDoc::currentView()
{
	if ( m_lstViews.count() == 0 )
		return 0;
	return (KtablesView *)m_lstViews.current();
}

void
KtablesDoc::addView(KtablesView *p_view)
{
	m_lstViews.append( p_view );
	QObject::connect( p_view,SIGNAL(signalMsg(const char *)),this,SIGNAL(signalMsg(const char *)) );
}

void
KtablesDoc::removeView(KtablesView *p_view)
{
	m_lstViews.setAutoDelete( false );
	m_lstViews.removeRef( p_view );
	m_lstViews.setAutoDelete( true );
}

const QString& KtablesDoc::getPathName() const
{
	return m_path;
}

void KtablesDoc::slotUpdateAllViews(KtablesView* pSender)
{
	OPViewIf* w;
		
	for( w = m_lstViews.first(); w; w = m_lstViews.next() )
		if( ((KtablesView *)w) != pSender)
			((KtablesView *)w)->repaint();
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

bool KtablesDoc::saveModified()
{
  if(b_modified)
  {
    KtablesApp* win=(KtablesApp*) parent();
/*
    int want_save = KMsgBox::yesNoCancel( win,
      i18n( "Warning" ), i18n( "The current file has been modified.\nDo you want to save it?" ) );
*/
    int want_save = QMessageBox::warning( 0, i18n( "Warning" ),
                    i18n( "The current file has been modified.\nDo you want to save it?" ),
                    i18n( "Yes" ), i18n( "No" ), i18n( "Cancel" ), 0, 2 );

    switch(want_save)
    {
      case 1:
        if(m_title == "Untitled")
    	  win->slotFileSaveAs();
    	else
	  saveDocument(getPathName()+getTitle());
       	
       	deleteContents();
        return true;
        break;
      case 2:
    	setModified(false);
      	deleteContents();
  	return true;
  	break;	
      case 3:
  	return false;
  	break;
      default:
  	return false;
  	break;
    }
  }
  else
    return true;
}

void KtablesDoc::closeDocument()
{
	deleteContents();
}

bool KtablesDoc::newDocument()
{
	
	/////////////////////////////////////////////////
	// TODO: Add your document initialization code here
	/////////////////////////////////////////////////
	b_modified=false;
	m_path=QDir::homeDirPath();
	m_title="Untitled";
	return true;
}

bool KtablesDoc::openDocument(const char* filename, const char* format)
{
	QFileInfo fileInfo(filename);
	m_title=fileInfo.fileName();
	m_path=fileInfo.absFilePath();	
	/////////////////////////////////////////////////
	// TODO: Add your document opening code here
	/////////////////////////////////////////////////
	
	b_modified=false;
	return true;
}

bool KtablesDoc::saveDocument(const char* filename, const char* format)
{

	/////////////////////////////////////////////////
	// TODO: Add your document saving code here
	/////////////////////////////////////////////////

	b_modified=false;
	return true;
}

void KtablesDoc::deleteContents()
{
	/////////////////////////////////////////////////
	// TODO: Add implementation to delete the document contents
	/////////////////////////////////////////////////

}

#include "ktablesdoc.moc"

