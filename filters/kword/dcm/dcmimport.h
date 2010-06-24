/* This file is part of the KOffice project
   Copyright (C) 2010 Arjun Asthana <arjun.kde@iiitd.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef DCMIMPORT_H
#define DCMIMPORT_H

#include <KoFilter.h>

class DCMImport : public KoFilter
{

    Q_OBJECT

public:
    DCMImport(QObject *parent, QStringList const&)
            : KoFilter(parent)
    {
    }

    ~DCMImport()
    {
    }

    KoFilter::ConversionStatus convert(QByteArray const& from, QByteArray const& to);

};

#endif // DCMIMPORT_H
