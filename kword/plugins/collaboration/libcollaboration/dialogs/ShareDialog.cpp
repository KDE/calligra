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
#include "ShareDialog.h"
#include "ui_ShareDialog.h"
#include <QMessageBox>
#include <QUuid>
using namespace kcollaborate;

ShareDialog::ShareDialog( bool defaultUi, QWidget *parent, Qt::WindowFlags f )
        : QDialog( parent, f ), ui( NULL )
{
    if ( defaultUi ) {
        ui = new Ui::ShareDialog();
        ui->setupUi( this );
    }

    baseUrl = "collaborate://127.0.0.1:2131/text/";
    QUuid uuid = QUuid::createUuid();
    baseUrl.append( QString::number( uuid.data1 ) ).append( QString::number( uuid.data2 ) ).append( '/' );
}

ShareDialog::~ShareDialog()
{}

int ShareDialog::exec()
{
    //prepare to show
    ui->documentNameLineEdit->setText( tr( "My document" ) );
    on_documentNameLineEdit_textEdited( tr( "My document" ) );

    int r = QDialog::exec();
    if ( QDialog::Accepted == r ) {
        Url url( ui->codeLineEdit->text() );
        if ( url.broken() ) {
            QMessageBox::critical( this,
                                   tr( "Incorrect URL" ),
                                   tr( "Incorrect URL: could not parse given URL" ) );
        } else {
            emit share( url );
        }
    }
    return r;
}

void ShareDialog::on_documentNameLineEdit_textEdited( const QString &text )
{
    ui->codeLineEdit->setText( baseUrl + text );
}

#include "ShareDialog.moc"
