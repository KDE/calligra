/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_spawner_drag.h"

#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_icon_view.h"

#include <kdebug.h>

KivioSpawnerDrag::KivioSpawnerDrag( KivioIconView *view, QWidget *parent, const char *name )
    : QIconDrag( parent, name )
{
    m_pView = view;
}

KivioSpawnerDrag::~KivioSpawnerDrag()
{
    m_pView->clearCurrentDrag();
    kdDebug(43000) << "KivioSpawnerDrag - this destroyed" << endl;
}

const char *KivioSpawnerDrag::format( int i ) const
{
    if( i==0 )
        return "application/x-qiconlist";
    else if( i==1 )
        return "kivio/stencilSpawner";
    else
        return 0L;
}

QByteArray KivioSpawnerDrag::encodedData( const char *mime ) const
{
    QByteArray a;

    if( QString(mime) == "application/x-qiconlist" )
    {
        a = QIconDrag::encodedData(mime);
    }
    else if( QString(mime) == "kivio/stencilSpawner" )
    {
        QString s = m_spawners.join("\r\n");
        a.resize(s.length());
        memcpy( a.data(), s.latin1(), s.length() );
    }

    return a;
}

bool KivioSpawnerDrag::canDecode( QMimeSource *e )
{
    return e->provides( "application/x-qiconlist" ) ||
           e->provides( "kivio/stencilSpawner" );
}

void KivioSpawnerDrag::append( const QIconDragItem &item, const QRect &pr,
            const QRect &tr, KivioStencilSpawner &spawner )
{
    QIconDrag::append( item, pr, tr );

    QString full = spawner.set()->dir() + "/" + spawner.info()->title();
    
    kdDebug(43000) << "KivioSpawnerDrag::append() - Adding " << full << endl;

    m_spawners << full;
}
