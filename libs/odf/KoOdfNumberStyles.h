/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KOODFNUMBERSTYLES_H
#define KOODFNUMBERSTYLES_H

#include "koodf_export.h"
#include "KoXmlReaderForward.h"

#include <QPair>
#include <QString>
#include <QList>

class KoGenStyles;
class KoGenStyle;

/**
 * Loading and saving of number styles
 */
namespace KoOdfNumberStyles
{
    enum Format {
        Number,
        Scientific,
        Fraction,
        Currency,
        Percentage,
        Date,
        Time,
        Boolean,
        Text
    };
    /// Prefix and suffix are always included into formatStr. Having them as separate fields simply
    /// allows to extract them from formatStr, to display them in separate widgets.
    struct NumericStyleData {
        Format type;
        QString formatStr;
        QString prefix;
        QString suffix;
        int precision;
        QString currencySymbol;
        bool thousandsSep;
        bool localeFormat;
        QString language;
        QString country;
        QString script;
        QList<QPair<QString,QString> > styleMaps; // conditional formatting, first=condition, second=applyStyleName
        NumericStyleData() : type(Text), precision(-1), thousandsSep(false), localeFormat(false) {}
    };

    KOODF_EXPORT QString format(const QString &value, const NumericStyleData &format);

    KOODF_EXPORT QString formatNumber(qreal value, const QString &format, int precision = -1);
    KOODF_EXPORT QString formatBoolean(const QString &value, const QString &format);
    KOODF_EXPORT QString formatDate(int value, const QString &format);
    KOODF_EXPORT QString formatTime(qreal value, const QString &format);
    KOODF_EXPORT QString formatCurrency(qreal value, const QString &format, const QString& currencySymbol, int precision = -1);
    KOODF_EXPORT QString formatScientific(qreal value, const QString &format, int precision = -1);
    KOODF_EXPORT QString formatFraction(qreal value, const QString &format);
    KOODF_EXPORT QString formatPercent(const QString &value, const QString &format, int precision = -1);

    KOODF_EXPORT QPair<QString, NumericStyleData> loadOdfNumericStyle(const KoXmlElement &parent);
    KOODF_EXPORT QString saveOdfNumericStyle(KoGenStyles &mainStyles, const NumericStyleData &data);

    KOODF_EXPORT QString saveOdfDateStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfTimeStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfFractionStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfScientificStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfNumberStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfBooleanStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfPercentageStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfCurrencyStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
    KOODF_EXPORT QString saveOdfTextStyle(KoGenStyles &mainStyles, const NumericStyleData &data = NumericStyleData());
}

KOODF_EXPORT QDebug operator<<(QDebug dbg, const KoOdfNumberStyles::NumericStyleData &data);

#endif // KOODFNUMBERSTYLES_H
