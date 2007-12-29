/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPatternChooser.h"
#include "KarbonPatternItem.h"

#include <KoResourceServerProvider.h>

#include <kfiledialog.h>
#include <klocale.h>

KarbonPatternChooser::KarbonPatternChooser( QWidget *parent )
    : KoResourceItemChooser( parent )
{
    m_resourceAdapter = new KoResourceServerAdapter<KoPattern>( 0 );

    connect( m_resourceAdapter, SIGNAL(resourceAdded(KoResource*)),
             this, SLOT(addPattern(KoResource*)));

    m_resourceAdapter->setResourceServer( KoResourceServerProvider::instance()->patternServer() );

    connect( this, SIGNAL( importClicked() ), this, SLOT( importPattern() ) );
    connect( this, SIGNAL( deleteClicked() ), this, SLOT( deletePattern() ) );
}

KarbonPatternChooser::~KarbonPatternChooser()
{
    delete m_resourceAdapter;
}

void KarbonPatternChooser::addPattern(KoResource* resource)
{
    KoPattern * pattern = dynamic_cast<KoPattern*>( resource );
    if( pattern && pattern->valid() )
        addItem( new KarbonPatternItem( pattern ) );
}

void KarbonPatternChooser::removePattern(KoResource* resource)
{
}

void KarbonPatternChooser::importPattern()
{
    QString filter( "*.jpg *.gif *.png *.tif *.xpm *.bmp" );
    QString filename = KFileDialog::getOpenFileName( KUrl(), filter, 0, i18n( "Choose Pattern to Add" ) );

    m_resourceAdapter->importResource( filename );
}

void KarbonPatternChooser::deletePattern()
{
    if(! currentItem() )
        return;

    KoPattern * pattern = static_cast<KarbonPatternItem*>( currentItem() )->pattern();

    if( m_resourceAdapter->removeResource( pattern ) )
        removeItem( static_cast<KoResourceItem*>( currentItem() ) );
}

#include "KarbonPatternChooser.moc"
