/* This file is part of the KDE project
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "qtsqlopeneditor.h"
#include "qtsqlopeneditor.moc"
#include <kcombobox.h>
#include <klineedit.h>
#include <kdebug.h>
#include <qlayout.h>

/******************************************************************
 *
 * Class: KWQTSQLSerialLetterOpen
 *
 ******************************************************************/

KWQTSQLSerialLetterOpen::KWQTSQLSerialLetterOpen( QWidget *parent, KWQTSQLSerialDataSourceBase *db_ )
        :KDialogBase( Plain, i18n( "Serial Letter - Setup database connection" ), Ok | Cancel, Ok, parent, "", true ), db( db_ ){
        (new QVBoxLayout(plainPage()))->setAutoAdd(true);
        setMainWidget(widget=new KWQTSQLOpenWidget(plainPage()));
        widget->drivers->insertStringList(QSqlDatabase::drivers());
        widget->hostname->setText(db->hostname);
        widget->username->setText(db->username);
        widget->port->setText(db->port);
        widget->databasename->setText(db->databasename);
        connect(this,SIGNAL(okClicked()),this,SLOT(handleOk()));
}

KWQTSQLSerialLetterOpen::~KWQTSQLSerialLetterOpen(){;}

void KWQTSQLSerialLetterOpen::handleOk()
{
        db->hostname=widget->hostname->text();
        db->username=widget->username->text();
        db->port=widget->port->text();
        db->databasename=widget->databasename->text();
        db->driver=widget->drivers->currentText();
}
