/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2005 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef CALLIGRA_SHEETS_CALCULATION_SETTINGS
#define CALLIGRA_SHEETS_CALCULATION_SETTINGS

#include "sheets_odf_export.h"

#include <QDate>

class KLocale;

namespace Calligra
{
namespace Sheets
{

/**
 * Calculation settings
 * \ingroup Value
 */
class CALLIGRA_SHEETS_ODF_EXPORT CalculationSettings
{
public:
    /**
     * Constructor.
     */
    CalculationSettings();

    /**
     * Destructor.
     */
    ~CalculationSettings();

    /**
     * A document could use a different localization as the KDE default.
     * @return the KLocale associated with this document
     */
    KLocale *locale() const;

    /**
     * Sets the reference year.
     *
     * \param year the new reference year
     */
    void setReferenceYear(int year);

    /**
     * Returns the reference year all two-digit-year dates are stored relative to.
     *
     * This defines how to convert a two-digit year into a four-digit year. All
     * two-digit year values are interpreted as a year that equals or follows this year.
     *
     * \return the reference year (default: 1930)
     */
    int referenceYear() const;

    /**
     * Sets the reference date.
     * All dates are stored as numbers relative to a reference date.
     *
     * \param date the new reference date
     */
    void setReferenceDate(const QDate& date);

    /**
     * Returns the reference date all date are stored relative to.
     *
     * \return the reference date (default: 1899-12-30)
     */
    QDate referenceDate() const;

    void setPrecisionAsShown(bool enable);
    bool precisionAsShown() const;

    /**
     * Sets the default decimal precision.
     * It is used to format decimal numbers, if the cell style does not define
     * one.
     *
     * \param precision the default decimal precision
     */
    void setDefaultDecimalPrecision(int precision);

    /**
     * Returns the default decimal precision, which is used, if the cell style
     * does not define one.
     *
     * \return the default decimal precision
     */
    int defaultDecimalPrecision() const;

    /**
     * Sets the file name used in the FILENAME function.
     */
    void setFileName(const QString& fileName);

    /**
     * \return The file name used in the FILENAME function.
     */
    const QString& fileName() const;

    /**
     * Sets the activation state of the active sheet's automatic recalculation
     * setting.
     * Used in the INFO function.
     */
    void setAutoCalculationEnabled(bool enable);

    /**
     * Returns the activation state of the active sheet's automatic
     * recalculation setting.
     * Used in the INFO function.
     *
     * \return the activation state
     */
    bool isAutoCalculationEnabled() const;

    void setAutomaticFindLabels(bool enabled);
    bool automaticFindLabels() const;

    /**
     * Sets the comparisons on this document to be case sensitive or not.
     */
    void setCaseSensitiveComparisons(Qt::CaseSensitivity caseSensitive);

    /**
     * Returns whether comparisons in this document are case sensitive.
     */
    Qt::CaseSensitivity caseSensitiveComparisons() const;

    void setWholeCellSearchCriteria(bool enabled);
    bool wholeCellSearchCriteria() const;

    /**
     * If true, regular expressions are used for character string
     * comparisons and when searching.
     *
     * This option is mutually exclusive with \a useWildcards but
     * calling this method will not call \a setUseWildcards and
     * its also not guaranteed that both are not set to true when
     * calling \a loadOdf and the ODF defines both to be true.
     *
     * This is the default character string comparisons mode in ODF.
     */
    void setUseRegularExpressions(bool enabled);
    bool useRegularExpressions() const;

    /**
     * If true, wildcards question mark '?' and asterisk '*' are used for
     * character string comparisons and when searching. Wildcards may be
     * escaped with a tilde '~' character.
     *
     * This is the only comparison mode supported by Excel.
     */
    void setUseWildcards(bool enabled);
    bool useWildcards() const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CALCULATION_SETTINGS
