/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_STENCIL_SPAWNER_SET_H
#define KIVIO_STENCIL_SPAWNER_SET_H

#include <qdom.h>
#include <qlist.h>
#include <qstring.h>

class KivioStencilSpawner;

class KivioStencilSpawnerSet
{
protected:
    QString m_dir;
    QString m_name;
    
    QList<KivioStencilSpawner> *m_pSpawners;
    KivioStencilSpawner *m_pSelected;
    
public:
    KivioStencilSpawnerSet(const QString& name=QString::null);
    virtual ~KivioStencilSpawnerSet();
    
    bool loadDir( const QString & );
    KivioStencilSpawner* loadFile( const QString & );
    
    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );
    
    QList<KivioStencilSpawner> *spawners() { return m_pSpawners; }
    QString dir() { return m_dir; }
    QString name() { return m_name; }
    KivioStencilSpawner *selected() { return m_pSelected; }
    
    KivioStencilSpawner *spawnerAt( int i ) { return m_pSpawners->at(i); }
    KivioStencilSpawner *find( const QString& );
    
    int count() { return m_pSpawners->count(); }

    static QString readDesc( const QString & );
    
};

#endif


