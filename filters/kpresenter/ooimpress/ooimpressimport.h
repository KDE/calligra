/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef OoImpress_IMPORT_H__
#define OoImpress_IMPORT_H__

#include <koFilter.h>

#include <qdict.h>
#include <qdom.h>

class OoImpressImport : public KoFilter
{
    Q_OBJECT
public:
    OoImpressImport( KoFilter * parent, const char * name, const QStringList & );
    virtual ~OoImpressImport();

    virtual KoFilter::ConversionStatus convert( QCString const & from, QCString const & to );

private:
    void createDocumentInfo( QDomDocument &docinfo );
    KoFilter::ConversionStatus openFile();

    QDomDocument    m_content;
    QDomDocument    m_meta;
    QDomDocument    m_settings;

};

#endif

