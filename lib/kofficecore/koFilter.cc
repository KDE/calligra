/* This file is part of the KDE libraries
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koFilter.h>

#include <qfile.h>

#include <ktempfile.h>
#include <koFilterManager.h>


KoFilter::KoFilter() : QObject( 0, 0 ), m_chain( 0 )
{
}

KoFilter::~KoFilter()
{
}


KoEmbeddingFilter::~KoEmbeddingFilter()
{
}

int KoEmbeddingFilter::currentPart() const
{
    return m_currentPart;
}

KoEmbeddingFilter::KoEmbeddingFilter() : KoFilter(), m_currentPart( 1 )
{
}

int KoEmbeddingFilter::embedPart( const QCString& from, QCString& to,
                                  KoFilter::ConversionStatus& status )
{
    KTempFile tempIn;
    tempIn.setAutoDelete( true );
    savePartContents( tempIn.file() );
    tempIn.file()->close();

    KoFilterManager *manager = new KoFilterManager( tempIn.name(), from, m_chain );
    status = manager->exp0rt( QString::null, to );
    delete manager;

    // ###### Where do we get the map entry from?
    // ...

    return m_currentPart++;
}

void KoEmbeddingFilter::savePartContents( QIODevice* )
{
}

#include <koFilter.moc>
