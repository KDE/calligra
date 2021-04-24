/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2006 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CURRENCY
#define CALLIGRA_SHEETS_CURRENCY

#include <QHash>
#include <QMetaType>
#include <QString>

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Style
 * \class Currency
 * Currency format information.
 */
class CALLIGRA_SHEETS_ODF_EXPORT Currency
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
     * \param code the code, e.g. EUR, USD,..
     * \param format the format, e.g. the code in Gnumeric format is [$EUR]
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

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Currency)

static inline uint qHash(const Calligra::Sheets::Currency& cur) {
    return ::qHash(cur.code());
}

#endif
