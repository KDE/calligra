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

#include<KoPAPageBase.h>
#include"KPrCustomSlideShowsWidget.h"
#include"KPrCustomSlideShows.h"

KPrCustomSlideShowsWidget::KPrCustomSlideShowsWidget( QWidget *parent, KPrCustomSlideShows *slideShows,
                                                      QList<KoPAPageBase*> *allPages )
: QDialog(parent)
, m_slideShows(slideShows)
{
    m_uiWidget.setupUi( this );
    m_uiWidget.customSlideShowsList->addItems( QStringList( m_slideShows->customSlideShowsNames() ));
    const int pagesCount= allPages->count();
    if( m_slideShows->customSlideShowsNames().count() != 0 )
    {
        for( int i=0; i<pagesCount; ++i ) {
            QListWidgetItem( QIcon(), m_slideShows->customSlideShowsNames().at( i ), m_uiWidget.avaliableSlidesList );
        }
    }
}

KPrCustomSlideShowsWidget::~KPrCustomSlideShowsWidget()
{
}

#include "KPrCustomSlideShowsWidget.moc"
