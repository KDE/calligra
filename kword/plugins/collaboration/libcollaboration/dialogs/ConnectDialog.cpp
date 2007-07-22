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
#include <QDebug>
#include <QMessageBox>
#include <DNSSD/ServiceBrowser>
#include "RemoteServiceListWidgetItem.h"
using namespace kcollaborate;

ConnectDialog::ConnectDialog( bool defaultUi, QWidget *parent, Qt::WindowFlags f )
        : QDialog( parent, f ), ui( NULL ), browser( "_kcollaboration._tcp" )
{
    if ( defaultUi ) {
        ui = new Ui::ConnectDialog();
        ui->setupUi( this );

        qDebug() << "DNSSD::ServiceBrowser::isAvailable: " << ( int )DNSSD::ServiceBrowser::isAvailable();
        if ( DNSSD::ServiceBrowser::isAvailable() == DNSSD::ServiceBrowser::Working ) {
            ui->listWidget->takeItem( 0 );//remove 1 item from UI file
            ui->listWidget->setEnabled( true );

            connect( &browser, SIGNAL( serviceAdded( DNSSD::RemoteService::Ptr ) ),
                     this, SLOT( serviceAdded( DNSSD::RemoteService::Ptr ) ) );
            connect( &browser, SIGNAL( serviceRemoved( DNSSD::RemoteService::Ptr ) ),
                     this, SLOT( serviceRemoved( DNSSD::RemoteService::Ptr ) ) );
            browser.startBrowse();
        }
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
                emit connectUrl( url );
            }
        }
    }
    return r;
}

void ConnectDialog::serviceAdded( DNSSD::RemoteService::Ptr remoteService )
{
    ui->listWidget->addItem( new RemoteServiceListWidgetItem( remoteService, ui->listWidget ) );
}

void ConnectDialog::serviceRemoved( DNSSD::RemoteService::Ptr remoteService )
{
    QList<QListWidgetItem *> items = ui->listWidget->findItems( remoteService->serviceName(), Qt::MatchExactly );
    if ( items.size() == 1 ) {
        QListWidgetItem *item = items.at( 0 );
        ui->listWidget->takeItem( ui->listWidget->row( item ) );//TODO: it's ugly to delete item by label in current API
    } else {
        qWarning() << "[ConnectDialog::serviceRemoved] Remote service item '" << remoteService->serviceName() << "' not found in the list (" << items.size() << ")";
    }
}

#include "ConnectDialog.moc"
