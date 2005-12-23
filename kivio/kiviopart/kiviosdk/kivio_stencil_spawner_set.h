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
#ifndef KIVIO_STENCIL_SPAWNER_SET_H
#define KIVIO_STENCIL_SPAWNER_SET_H

#include <qptrlist.h>
#include <qdom.h>

class KivioStencilSpawner;
class QStringList;

class KivioStencilSpawnerSet
{
  protected:
    QString m_dir;
    QString m_name;
    QString m_id;

    QStringList m_files;

    QPtrList<KivioStencilSpawner> *m_pSpawners;
    KivioStencilSpawner *m_pSelected;

    bool m_hidden;

  public:
    KivioStencilSpawnerSet(const QString& name=QString::null);
    virtual ~KivioStencilSpawnerSet();

    bool loadDir( const QString & );
    KivioStencilSpawner* loadFile( const QString & );

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    QPtrList<KivioStencilSpawner> *spawners() { return m_pSpawners; }
    KivioStencilSpawner *selected() { return m_pSelected; }

    QString dir() { return m_dir; }
    QString name() { return m_name; }
    QString id() { return m_id; }

    void setDir( const QString &s ) { m_dir=s; }
    void setName( const QString &s ) { m_name=s; }
    void setId( const QString &s ) { m_id=s; }

    KivioStencilSpawner *spawnerAt( int i ) { return m_pSpawners->at(i); }
    KivioStencilSpawner *find( const QString& );

    int count() { return m_pSpawners->count(); }

    static QString readTitle( const QString & );
    static QString readId( const QString & );
    static QString readDescription(const QString&);

    QStringList files() const { return m_files; }

    void addSpawner(KivioStencilSpawner* spawner);

    void setHidden(bool hide) { m_hidden = hide; }
    bool hidden() const { return m_hidden; }
};

#endif


