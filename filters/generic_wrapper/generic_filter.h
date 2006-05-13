/* This file is part of the KDE project
   Copyright (C) 2002 Lukas Tinkl <lukas@kde.org>

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

#ifndef __GENERICFILTER_H__
#define __GENERICFILTER_H__

#include <q3cstring.h>
#include <QStringList>

#include <kprocess.h>
#include <KoFilter.h>
#include <KoStore.h>

class GenericFilter : public KoFilter
{
	Q_OBJECT

public:
    GenericFilter(QObject *parent, const QStringList&);
    virtual ~GenericFilter() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from,
                                                const QByteArray& to );
private:
    KoFilter::ConversionStatus doImport();
    KoFilter::ConversionStatus doExport();
    QString m_to, m_from, m_exec;
    KoStore* m_out;
};

#endif /* __GENERICFILTER_H__ */
