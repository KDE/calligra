/***************************************************************************
                          ktablesserver.cpp  -  description                              
                             -------------------                                         
    begin                : Thu Jul 8 1999                                           
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

#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <kdb.h>
#include <kapp.h>
#include <klocale.h>

#include "ktablesserver.h"

KtablesServer::KtablesServer(QWidget *p_parent, const char *p_name)
 : QWidget(p_parent,p_name)
{
	QLabel      *lab;
	QPushButton *but;
	int w;
	
	_base = new QLineEdit( this,"base_id" );
	_host = new QLineEdit( this,"host_id" );
	_user = new QLineEdit( this,"user_id" );
	lab   = new QLabel( this );
	lab->setBuddy( _base );
	lab->setText( i18n("Database:") );
	lab->setAlignment( AlignRight|AlignVCenter );
	lab->move( 10,10 );
	w = lab->width();
	
	lab = new QLabel( this );
	lab->setText( i18n("Server name:") );
	lab->setBuddy( _host );
	lab->setAlignment( AlignRight|AlignVCenter );
	lab->move( 10,35 );
	if ( lab->width() > w )
		w = lab->width();
	
	lab = new QLabel( this );
	lab->setBuddy( _user );
	lab->setText( i18n("Login ID:") );
	lab->setAlignment( AlignRight|AlignVCenter );
	lab->move( 10,60 );
	if ( lab->width() > w )
		w = lab->width();
	
	_base->setGeometry( 20+w,12,100,23 );
	_host->setGeometry( 20+w,37,100,23 );
	_user->setGeometry( 20+w,62,100,23 );
	resize( lab->width()+_base->width()+20,150 );
	
	but = new QPushButton( this,"ok_0" );
	but->setText( i18n("Connect") );
	but->setGeometry( 10,125,84,23 );
	connect( but,SIGNAL(clicked()),SLOT(tryConnect()));
	
	but = new QPushButton( this,"cancel_0" );
	but->setText( i18n("Cancel") );
	but->setGeometry( width()-94,125,84,23 );
	connect( but,SIGNAL(clicked()),SLOT(cancelMe()));
	
}

KtablesServer::~KtablesServer()
{
	// I'm assuming my children are automatically deleted by QWidget.
}

void
KtablesServer::tryConnect()
{
	QString host = _host->text();
	QString user = _user->text();
	QString base = _base->text();
	
	// We are ignoring the user at the moment, and relying on
	// Ident identification by postgres.
	try {
		Kdb::Open( Kdb::Postgres, base, host );
	}
	catch(const QString& s) {
		emit errorMessage( s );
	}
	catch(const char *s) {
		emit errorMessage( s );
	}
	if ( Kdb::isOpen() )
		emit serverSelected();
	hide();
}

void
KtablesServer::cancelMe()
{
	hide();
}

#include "ktablesserver.moc"

