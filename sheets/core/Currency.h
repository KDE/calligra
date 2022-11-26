/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2006 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CURRENCY
#define CALLIGRA_SHEETS_CURRENCY

#include <QHash>
#include <QString>
#include <QMetaType>

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Style
 * \class Currency
 * Currency format information.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Currency
{
public:
    enum Format { Native, Gnumeric, OpenCalc, ApplixSpread, GobeProductiveSpread, HancomSheet };

    /**
     * Constructor.
     * Creates a currency corresponding to \p code .
     * \param code the code, e.g. EUR, USD,..
     * \param format the format, e.g. the code in Gnumeric format is [$EUR]
     */
    Currency(QString const & code = QString(), Format format = Native);

    /**
     * Destructor.
     */
    ~Currency();


    bool operator==(Currency const & other) const;
    inline bool operator!=(Currency const & other) const {
        return !operator==(other);
    }

    QString code(Format format = Native) const;
    QString name() const;
    QString symbol() const;

    static QList<QString> symbols();
    QString symbolToCode(const QString &code);
private:
    static void loadSymbols();
    QString m_code;
    static QMap<QString, QString> m_symbols;
};

} // namespace Sheets
} // namespace Calligra

uint qHash(const Calligra::Sheets::Currency& cur);

Q_DECLARE_METATYPE(Calligra::Sheets::Currency)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Currency, Q_MOVABLE_TYPE);


#endif
