/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koDocument.h"
#include "KoDocumentIface.h"
#include "koView.h"
#include <kapp.h>
#include <dcopclient.h>
#include <kdcopactionproxy.h>
#include <kaction.h>
#include <kdebug.h>

KoDocumentIface::KoDocumentIface( KoDocument * doc, const char * name )
    : DCOPObject( name )
{
  m_pDoc = doc;
  m_actionProxy = new KDCOPActionProxy( doc->actionCollection(), this );
}

KoDocumentIface::~KoDocumentIface()
{
    delete m_actionProxy;
}

QString KoDocumentIface::url()
{
  return m_pDoc->url().url();
}

bool KoDocumentIface::isModified()
{
  return m_pDoc->isModified();
}

int KoDocumentIface::viewCount()
{
  return m_pDoc->viewCount();
}

DCOPRef KoDocumentIface::view( int idx )
{
  QList<KoView> views = m_pDoc->views();
  KoView *v = views.at( idx );
  if ( !v )
    return DCOPRef();

  DCOPObject *obj = v->dcopObject();

  if ( !obj )
    return DCOPRef();

  return DCOPRef( kapp->dcopClient()->appId(), obj->objId() );
}

DCOPRef KoDocumentIface::action( const QCString &name )
{
    return DCOPRef( kapp->dcopClient()->appId(), m_actionProxy->actionObjectId( name ) );
}

QCStringList KoDocumentIface::actions()
{
    QCStringList res;
    QValueList<KAction *> lst = m_actionProxy->actions();
    QValueList<KAction *>::ConstIterator it = lst.begin();
    QValueList<KAction *>::ConstIterator end = lst.end();
    for (; it != end; ++it )
        res.append( (*it)->name() );

    return res;
}

QMap<QCString,DCOPRef> KoDocumentIface::actionMap()
{
    return m_actionProxy->actionMap();
}

void KoDocumentIface::saveAs( const QString & url )
{
    m_pDoc->saveAs( KURL( url ) );
}

void KoDocumentIface::setOutputMimeType( const QCString & mimetype )
{
    m_pDoc->setOutputMimeType( mimetype );
}
