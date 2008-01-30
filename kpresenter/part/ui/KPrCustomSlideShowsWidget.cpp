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

#include <QStringList>
#include <QListWidgetItem>
#include <QIcon>

#include <KoPAPageBase.h>
#include "KPrCustomSlideShowsWidget.h"
#include "KPrCustomSlideShows.h"

KPrCustomSlideShowsWidget::KPrCustomSlideShowsWidget( QWidget *parent, KPrCustomSlideShows *slideShows,
                                                      QList<KoPAPageBase*> *allPages )
: QDialog(parent)
, m_slideShows(slideShows)
{
    m_uiWidget.setupUi( this );
    connect( m_uiWidget.addButton, SIGNAL( clicked() ), this, SLOT( addCustomSlideShow() ) );

    m_uiWidget.customSlideShowsList->addItems( QStringList( m_slideShows->customSlideShowsNames() ));

    int currentPage = 1;
    foreach( KoPAPageBase* page, *allPages ) {
        new QListWidgetItem( QIcon(), i18n("Slide %1", currentPage++), m_uiWidget.availableSlidesList );
    }
}

KPrCustomSlideShowsWidget::~KPrCustomSlideShowsWidget()
{
}

void KPrCustomSlideShowsWidget::addCustomSlideShow()
{
    QListWidgetItem * item = new QListWidgetItem( i18n("New Slide Show") );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    m_uiWidget.customSlideShowsList->addItem( item );
    m_uiWidget.customSlideShowsList->editItem( item );
}

#include "KPrCustomSlideShowsWidget.moc"
