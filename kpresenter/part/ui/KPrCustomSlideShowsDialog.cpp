/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include <QDebug>
#include <QStringList>
#include <QListWidgetItem>
#include <QIcon>
#include <QListWidgetItem>

#include <KoPAPageBase.h>
#include "KPrCustomSlideShowsDialog.h"
#include <KPrCustomSlideShows.h>

KPrCustomSlideShowsDialog::KPrCustomSlideShowsDialog( QWidget *parent, KPrCustomSlideShows *slideShows,
                                                      QList<KoPAPageBase*> *allPages, KPrCustomSlideShows *newSlideShows )
: QDialog(parent)
, m_slideShows( new KPrCustomSlideShows(*newSlideShows) )
{
    m_uiWidget.setupUi( this );
    connect( m_uiWidget.addButton, SIGNAL( clicked() ), this, SLOT( addCustomSlideShow() ) );
    connect( m_uiWidget.okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_uiWidget.deleteButton, SIGNAL( clicked() ), this, SLOT( deleteCustomSlideShow() ) );

    QListWidgetItem * item;
    foreach( QString slideShowName, m_slideShows->names() ) {
        item = new QListWidgetItem( slideShowName, m_uiWidget.customSlideShowsList );
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        item->setData( SlideShowName, QVariant(slideShowName) );
    }

    connect( m_uiWidget.customSlideShowsList, SIGNAL( itemChanged( QListWidgetItem*) ),
            this, SLOT( renameCustomSlideShow(QListWidgetItem*) ) );

    int currentPage = 1;
    foreach( KoPAPageBase* page, *allPages ) {
        item = new QListWidgetItem( QIcon(), i18n("Slide %1", currentPage++), m_uiWidget.availableSlidesList );
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
    }
}

KPrCustomSlideShowsDialog::~KPrCustomSlideShowsDialog()
{
}

void KPrCustomSlideShowsDialog::addCustomSlideShow()
{
    static int newSlideShowsCount = 1;
    QListWidgetItem * item = new QListWidgetItem( i18n("New Slide Show %1", newSlideShowsCount++) );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    item->setData( SlideShowName, QVariant(i18n("New Slide Show %1", newSlideShowsCount)) );
    m_uiWidget.customSlideShowsList->addItem( item );
    m_slideShows->insert( i18n("New Slide Show %1", newSlideShowsCount), QList<KoPAPageBase*>() );

    m_uiWidget.customSlideShowsList->editItem( item );
}

void KPrCustomSlideShowsDialog::renameCustomSlideShow( QListWidgetItem *item )
{
    if(item->data( SlideShowName ).toString() != item->data( Qt::DisplayRole ).toString() ) {
        m_slideShows->rename( item->data( SlideShowName ).toString(), item->data( Qt::DisplayRole ).toString() );
        item->setData( SlideShowName, item->data( Qt::DisplayRole ) );
    }
}

void KPrCustomSlideShowsDialog::deleteCustomSlideShow()
{
    QListWidgetItem* item = m_uiWidget.customSlideShowsList->currentItem();
    if( !item ){
        m_slideShows->remove( item->data(SlideShowName).toString() );
        m_uiWidget.customSlideShowsList->removeItemWidget( item );
    }
}
#include "KPrCustomSlideShowsDialog.moc"
