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
#include <kdebug.h>

#include "ktables_server.h"

KtablesServer::KtablesServer(QWidget *p_parent, const char *p_name)
 : QWidget(p_parent,p_name)
{
	QLabel      *lab;
	QPushButton *but;
	QGridLayout *grid;;
	
	grid = new QGridLayout( this,5,3,5,5 );
	_base = new QLineEdit( this,"base_id" );
	_host = new QLineEdit( this,"host_id" );
	_user = new QLineEdit( this,"user_id" );
	lab   = new QLabel( this );
	lab->setBuddy( _base );
	lab->setText( i18n("&Database:") );
	lab->setAlignment( AlignRight|AlignVCenter );
	grid->addWidget( lab,0,0 );
	grid->addWidget( _base,0,2 );	
	
	lab = new QLabel( this );
	lab->setText( i18n("&Server name:") );
	lab->setBuddy( _host );
	lab->setAlignment( AlignRight|AlignVCenter );
	grid->addWidget( lab,1,0 );
	grid->addWidget( _host,1,2 );
		
	lab = new QLabel( this );
	lab->setBuddy( _user );
	lab->setText( i18n("&Login ID:") );
	lab->setAlignment( AlignRight|AlignVCenter );
	grid->addWidget( lab,2,0 );
	grid->addWidget( _user,2,2 );
		
	grid->addRowSpacing( 3,10 );
	grid->addColSpacing( 1,2 );
	
	but = new QPushButton( this,"ok_0" );
	but->setText( i18n("C&onnect") );
	but->setGeometry( 10,125,84,23 );
	connect( but,SIGNAL(clicked()),SLOT(tryConnect()));
	grid->addWidget( but,4,0 );
		
	but = new QPushButton( this,"cancel_0" );
	but->setText( i18n("&Cancel") );
	but->setGeometry( width()-94,125,84,23 );
	connect( but,SIGNAL(clicked()),SLOT(cancelMe()));
	grid->addWidget( but,4,2 );
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
	
	kdebug( KDEBUG_INFO,0,"KtablesServer::tryConnect(%s,%s)", base.data(), host.data() );
	// We are ignoring the user at the moment, and relying on
	// Ident identification by postgres.
	try {
		Kdb::Open( Kdb::Postgres, base, host );
	}
	catch(Kdb::ExceptionTypes t) {
		QString s = Kdb::exceptionMsg( t );
		emit errorMessage( s );
	}
	kdebug( KDEBUG_INFO,0,"KtablesServer::tryConnect()" );
	if ( Kdb::isOpen() )
		emit serverSelected();
	hide();
}

void
KtablesServer::cancelMe()
{
	hide();
}

#include "ktables_server.moc"

