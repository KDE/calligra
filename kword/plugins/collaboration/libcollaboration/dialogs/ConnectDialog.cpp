/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"
#include <QMessageBox>
using namespace kcollaborate;

ConnectDialog::ConnectDialog( bool defaultUi, QWidget *parent, Qt::WindowFlags f )
        : QDialog( parent, f ), ui( NULL )
{
    if ( defaultUi ) {
        ui = new Ui::ConnectDialog();
        ui->setupUi( this );
    }
}

ConnectDialog::~ConnectDialog()
{}

int ConnectDialog::exec()
{
    //prepare to show
    ui->codeLineEdit->setText( "" );

    int r = QDialog::exec();
    if ( QDialog::Accepted == r ) {
        QString urlText = ui->codeLineEdit->text();
        if ( !urlText.isEmpty() ) {
            Url url( urlText );
            if ( url.broken() ) {
                QMessageBox::critical( this,
                                       tr( "Incorrect URL" ),
                                       tr( "Incorrect URL: could not parse given URL" ) );
            } else {
                emit connect( url );
            }
        }
    }
    return r;
}

#include "ConnectDialog.moc"
