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

#include <QMetaType>

#include "kspread_export.h"

namespace KSpread
{

/**
 * \ingroup Style
 * \class Currency
 * Currency format information.
 */
class KSPREAD_EXPORT Currency
{
public:
    enum Format { Native, Gnumeric, OpenCalc, ApplixSpread, GobeProductiveSpread, HancomSheet };

    /**
     * Constructor.
     * Creates a currency corresponding to the given currency table index.
     * If \p index is omitted or zero, a currency with the locale default
     * currency unit is created.
     */
    explicit Currency(int index = 0);

    /**
     * Constructor.
     * Creates a currency corresponding to \p code .
     * Looks up the index.
     * If the code is found more than once: saved without country info.
     * If the code is not found, \p code is handled as custom unit.
     * \p code e.g. EUR, USD,..
     * \param format in Gnumeric the code is: [$EUR]
     */
    explicit Currency(QString const & code, Format format = Native);

    /**
     * Destructor.
     */
    ~Currency();


    bool operator==(Currency const & other) const;
    inline bool operator!=(Currency const & other) const {
        return !operator==(other);
    }

    QString code(Format format = Native) const;
    QString country() const;
    QString name() const;
    QString symbol() const;
    int     index() const;

    static QString chooseString(int type, bool & ok);

private:
    int     m_index;
    QString m_code;
};

} // namespace KSpread

Q_DECLARE_METATYPE(KSpread::Currency)

#endif
