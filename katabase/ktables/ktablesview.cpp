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
#include <qpainter.h>
#include <qheader.h>

// application specific includes
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>

#include <ktablesview.h>
#include "ktablesdoc.h"
#include "ktables.h"

KtablesView::KtablesView(kdbDataSet *p_set,OPDocumentIf *doc,QWidget *parent, const char* name)
 : kdbRecordView(p_set,parent,name), OPViewIf(doc)
{
	_doc = doc;
	setBackgroundMode( PaletteBase );
	_doc->registerView( this );
}

KtablesView::~KtablesView()
{
	_doc->unregisterView( this );
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
KtablesView::setTable(const QString& p_table)
{
	kdbDataSet *set;

	if ( p_table == QString::null || !Kdb::isOpen() || header()->count() > 0 )
		return;
	try {
		if ( set=Kdb::dataSet(p_table) ) {
			kdbRecordView::setTable( set );
			set->runQuery( false );
		}
	} catch( const char *s ) {
		emit signalMsg( QString("query error: %1").arg(s) );
	} catch( const QString& s ) {
		emit signalMsg( QString("query error: %1").arg(s) );
	}
}

KtablesDoc* KtablesView::getDocument() const
{
	KtablesApp* theApp=(KtablesApp*)parentWidget();
	return theApp->getDocument();
}

void KtablesView::print(QPrinter* m_pPrinter)
{
	QPainter printpainter;
	printpainter.begin(m_pPrinter);
	
	// TODO: add your printing code here
	
	printpainter.end();

}

#include "ktablesview.moc"

