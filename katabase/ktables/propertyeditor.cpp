/***************************************************************************
                          propertyeditor.cpp  -  description                              
                             -------------------                                         
    begin                : Sun Jul 11 1999                                           
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

#include <qscrollview.h>
#include <qobjectlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

#include <klocale.h>

#include <kdb.h>
#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>

#include "propertyeditor.h"
#include "propertyitem.h"
#include "propertyeditor.moc"

PropertyEditor::PropertyEditor(QWidget *p_par, const char *p_nam)
 : QDialog(p_par,p_nam)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *but;
	
	_item  = 0;
	_count = 0;
	setCaption( i18n("Properties") );
	resize( 250,300 );
	_items = new QScrollView( this );
	_items->resize( 250,300 );
	
	vbox = new QVBoxLayout( this,0,-1,"vbox" );
	vbox->setMargin( 15 );
	vbox->addWidget( _items );
	
	hbox = new QHBoxLayout( -1,"hbox" );
	hbox->setMargin( 15 );
	
	but = new QPushButton( this,"ok_b" );
	but->setText( i18n("OK") );
	connect( but,SIGNAL(clicked()),SLOT(done()) );
	
	hbox->addWidget( but );
	hbox->addSpacing( 10 );
	
	but = new QPushButton( this,"cancel_b" );
	but->setText( i18n("Cancel") );
	connect( but,SIGNAL(clicked()),SLOT(canceled()) );
	
	hbox->addWidget( but );
	
	vbox->addSpacing( 10 );
	vbox->addLayout( hbox );
	
	_grid = new QGridLayout( _items->viewport(),8,3,1,-1,"grid" );
	_grid->setMargin( 15 );
	_grid->addColSpacing( 1,5 );
		
}

PropertyEditor::~PropertyEditor()
{
}

QLabel *
PropertyEditor::makeLabel(const QString& p_name, QWidget *p_buddy)
{
	QLabel *lab;
	
	lab = new QLabel( _items->viewport(),p_name );
	lab->setText( p_name );
	lab->setBuddy( p_buddy );
	_grid->addWidget( lab,_count,0 );
	
	return lab;
}

void
PropertyEditor::set(PropertyItem *p_itm)
{
	_item = p_itm;
}

void
PropertyEditor::set(const QString& p_name, const QString& p_value)
{
	QLineEdit *ed;;
	
	if ( _count > 0)
		_grid->addRowSpacing( _count++,1 );
	
	ed = new QLineEdit( _items->viewport() );
	ed->setText( p_value );
	makeLabel( p_name, ed );
	_grid->addWidget( ed,_count,2 );
	_count += 1;
}

void
PropertyEditor::set(const QString& p_name, const QStringList& p_sel)
{
	QComboBox *cb;
	
	if ( _count > 0 )
		_grid->addRowSpacing( _count++,1 );
		
	cb = new QComboBox( _items->viewport() );
	cb->insertStringList( p_sel );
	makeLabel( p_name,cb );
	_grid->addWidget( cb,_count,2 );
	_count += 1;
}

void
PropertyEditor::set(const QString& p_name, bool p_set)
{
	QCheckBox *cb;
	
	if ( _count > 0 )
		_grid->addRowSpacing( _count++,1 );
	cb = new QCheckBox( _items->viewport() );
	cb->setChecked( p_set );
	makeLabel( p_name,cb );
	_grid->addWidget( cb,_count,2 );
	_count += 1;
}

QString
PropertyEditor::setting(const QString& p_name)
{
	QLabel *lab;
	QWidget *w;
	
	QObjectList *list = _items->viewport()->queryList( "QLabel",p_name );
	QObjectListIt it( *list );
	lab = (QLabel *)it.current();
	if ( !lab )
		return "";
	w = lab->buddy();
	if ( w ) {
		if ( w->isA("QLineEdit") )
			return ((QLineEdit *)w)->text();
		if ( w->isA("QCheckBox") )
			return ((QCheckBox *)w)->isChecked()?"t":"f";
		if ( w->isA("QComboBox") )
			return ((QComboBox *)w)->currentText();
	}
	return "";
}

void
PropertyEditor::done()
{
	hide();
}

void
PropertyEditor::canceled()
{
	hide();
}












