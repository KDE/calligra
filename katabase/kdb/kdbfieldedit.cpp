/***************************************************************************
                          kdbFieldEdit.cpp  -  description
                             -------------------                                         
    begin                : Fri Jun 11 1999                                           
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

#include <qevent.h>
#include <qfontmetrics.h>
#include <qobjectlist.h>
#include <qmultilinedit.h>
#include <qlayout.h>

#include <kdbdataset.h>
#include <kdbfieldset.h>
#include <kdbdatafield.h>
#include <kdbrelation.h>

#include "kdbfieldedit.h"
#include "kdbcalendar.h"
#include "kdbmoneyedit.h"

kdbFieldEdit::kdbFieldEdit(QWidget *p_par, const char *p_nam, kdbDataSet *p_set, const char *p_fld)
 : QWidget(p_par, p_nam)
{
	if ( !p_set || !p_set->fieldSet()->has(p_fld) )
		throw QString("set doesn't have field %1").arg(p_fld);
	_fieldName     = p_fld;
	_acceptSignals = true;
	init(p_set, p_fld);
}

void
kdbFieldEdit::init(kdbDataSet *p_set, const QString& p_field)
{
	QFontMetrics fm(font());
	int x=0,y=0,length,height;
	kdbRelation *rel = 0;

	_set    = p_set;
	_field  = &p_set->field(p_field);
	_label  = "";
	_l      = 0;
	_column = 0;
	connect(_field,SIGNAL(postChange(const QString&)),SLOT(postChange(const QString&)));
	length  = _field->length();
	height  = fm.height()+6;
	if ( (rel = Kdb::relation(p_set,p_field)) ) {
		_w = new QComboBox(this);
		if (length < 10)
			length = 10;
		else if (length > 30)
			length = 30;
		((QComboBox *)_w)->setListBox( rel->listBox() );
	} else if (_field->type() == "char" || _field->type() == "bpchar") {
		if (length == -1)
		  length = 40;
		_w = new QLineEdit(this);
	} else if ( _field->type() == "name")
		_w = new QLineEdit(this);
	else if (_field->type() == "bool") {
		_w = new QCheckBox(this);
		length = 2;
	} else if (_field->type() == "money") {
		_w = new kdbMoneyEdit( 0,this );
		length = 12;
	} else if (_field->type() == "float4" || _field->type() == "float8") {
		_w = new QLineEdit(this);
		length = 12;
		((QLineEdit *)_w)->setValidator(new QDoubleValidator(_w));
	} else if (_field->type() == "int2" || _field->type() == "int4") {
		_w = new QLineEdit(this);
		length = 8;
		((QLineEdit *)_w)->setValidator(new QIntValidator(_w));
	} else if (_field->type() == "date")
		_w = new kdbCalendar( Kdb::dateFormat(),0,this );
	else
		throw QString("Don't know about '%1' field types").arg(_field->type());
	if ( _w->isA("QLineEdit") || _w->isA("kdbMoneyEdit") )
		connect(_w, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));
	else if ( _w->isA("QCheckBox") )
		connect(_w, SIGNAL(toggled(bool)), SLOT(toggled(bool)));
	else if ( _w->isA("QComboBox") && !rel )
		connect(_w, SIGNAL(activated(const QString&)), SLOT(textChanged(const QString&)));
	else if ( _w->isA("kdbCalendar") )
		connect(_w,SIGNAL(dateChanged(const QDate&)),SLOT(dateChanged(const QDate&)));
	_field->setLength(length);
	setLabel( _field->label() );
	_w->setGeometry(_l->width()+2,(_l->height()-fm.height()-6)/2,fm.width("0")*length,height);
	QWidget::setGeometry(x,y,_w->width()+_l->width()+20,_l->height()>height?_l->height():height);
	QLayout *lout = this->parentWidget()->layout();
	if (lout != 0) {
		if (lout->isA("QVBoxLayout"))
			((QVBoxLayout *)lout)->addWidget( this, 0, AlignLeft );
		else
			lout->add( this );
	}
}

kdbFieldEdit::~kdbFieldEdit()
{
	delete _w;
	if (_l != 0)
		delete _l;
}

QSize
kdbFieldEdit::sizeHint() const
{
	return QSize(width(), height());
}

QSizePolicy
kdbFieldEdit::sizePolicy() const
{
	if (_w->isA("kdbCalendar"))
		return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

bool
kdbFieldEdit::event(QEvent *p_ev)
{
	bool rv = QWidget::event(p_ev);
	
	if (p_ev->type() == QEvent::Show)
		postChange( _field->text() );
	return rv;
}

void
kdbFieldEdit::setLabel(const QString& p_lab)
{
	QFontMetrics fm(font());
	
	if (_l == 0)
		_l = new QLabel(p_lab+":", this);
	else
		_l->setText(p_lab + ":");
	_label = p_lab;
	_l->setAlignment(AlignVCenter|AlignRight);
}

void
kdbFieldEdit::setGeometry(int p_x, int p_y)
{
	if (p_x > _l->width())
		p_x -= _l->width();
	QWidget::setGeometry(p_x, p_y,width(),height());
}

void
kdbFieldEdit::setText(const QString& p_str)
{
	if (_w->isA("QLineEdit"))
		((QLineEdit *)_w)->setText(p_str);
	else if (_w->isA("QMultiLineEdit"))
		((QMultiLineEdit *)_w)->setText(p_str);
	else if (_w->isA("kdbMoneyEdit"))
		((kdbMoneyEdit *)_w)->setText(p_str);
	else if (_w->isA("kdbCalendar"))
		((kdbCalendar *)_w)->setText(p_str);
	else if (_w->isA("QCheckBox"))
		((QCheckBox *)_w)->setChecked(p_str == "t");
	else if (_w->isA("QComboBox"))
		return;
	_field->set(p_str);
}

const QString&
kdbFieldEdit::text()
{
	return _field->text();
}

const QString&
kdbFieldEdit::label()
{
	return _label;
}

int
kdbFieldEdit::labelWidth()
{
	if (_l == 0)
		return 0;
	return _l->width();
}

int
kdbFieldEdit::column()
{
	return _column;
}

void
kdbFieldEdit::setColumn(int p_col)
{
	_column = p_col;
}

void
kdbFieldEdit::setLabelWidth(int p_width)
{
	_l->setGeometry(_l->x(),_l->y(),p_width,_l->height());
}

void
kdbFieldEdit::textChanged(const QString& p_str)
{
	bool state = _acceptSignals;
	
	_acceptSignals = false;
	if ( state ) {
		_field->set( p_str );
		if ( _set )
			_set->update();
	}
	_acceptSignals = state;
}

void
kdbFieldEdit::toggled(bool p_val)
{
	const QString& s = (p_val?"t":"f");

	textChanged( s );	
}

void
kdbFieldEdit::dateChanged(const QDate& p_date)
{
	bool state = _acceptSignals;
	
	_acceptSignals = false;
	if ( state ) {
		_field->set( p_date );
		if ( _set )
			_set->update();
	}
	_acceptSignals = state;
}

void
kdbFieldEdit::postChange(const QString& p_str)
{
	bool state = _acceptSignals;
	
	_acceptSignals = false;
	if ( state ) {
		if (_w->isA("QLineEdit"))
			((QLineEdit *)_w)->setText( p_str );
		else if (_w->isA("QMultiLineEdit"))
			((QMultiLineEdit *)_w)->setText( p_str );
		else if (_w->isA("kdbMoneyEdit"))
			((kdbMoneyEdit *)_w)->setText( p_str );
		else if (_w->isA("kdbCalendar"))
			((kdbCalendar *)_w)->setText( p_str );
		else if (_w->isA("QCheckBox"))
			((QCheckBox *)_w)->setChecked( p_str == "t" );
		else if (_w->isA("QComboBox")) {
			QComboBox *box = (QComboBox *)_w;
			kdbRelation *rel = Kdb::relation( _set,_field->name() );
		
			if ( rel )
				box->setCurrentItem( rel->index(p_str) );
			else
				for( int i=0;i<box->count();i++ )
					if ( p_str == box->text(i) )
						box->setCurrentItem( i );
		}
	}
	_acceptSignals = true;
}

#include "kdbfieldedit.moc"

