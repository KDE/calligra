/* This file is part of the KDE project
   Copyright 2000-2006 The KSpread Team <koffice-devel@kde.org>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CURRENCY
#define KSPREAD_CURRENCY

#include <koffice_export.h>

namespace KSpread
{

class KSPREAD_EXPORT Currency
{
public:
    enum currencyFormat { Native, Gnumeric, OpenCalc, ApplixSpread, GobeProductiveSpread, HancomSheet };

    Currency();
    Currency(int index);
    ~Currency();

    /**
     * If code doesn't fit to index the index gets ignored
     */
    Currency(int index, QString const & code);

    /**
     * code: e.g. EUR, USD,..
     * Looks up index, if code found more than once: saved without country info
     * currencyFormat: in Gnumeric the code is: [$EUR]
     *                 saves some work in the filter...
     */
    Currency(QString const & code, currencyFormat format = Native);
    Currency & operator=(int type);
    Currency & operator=(char const * code);
    bool operator==(Currency const & cur) const;
    bool operator==(int type) const;
    operator int() const;

    QString getCode() const;
    QString getCountry() const;
    QString getName() const;
    QString getDisplayCode() const;
    int     getIndex() const;

    static QString getChooseString(int type, bool & ok);
    static QString getDisplaySymbol(int type);
    static QString getCurrencyCode( int type);

    /**
     * Code for use in Gnumeric export filter
     */
    QString getExportCode(currencyFormat format) const;

private:
    int     m_type;
    QString m_code;
};

} // namespace KSpread

#endif
