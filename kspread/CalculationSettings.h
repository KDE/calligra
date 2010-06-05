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

#ifndef KSPREAD_CALCULATION_SETTINGS
#define KSPREAD_CALCULATION_SETTINGS

#include "kspread_export.h"

#include <KoXmlReader.h>

#include <QDate>

class KLocale;

class KoXmlWriter;

namespace KSpread
{

/**
 * Calculation settings
 */
class KSPREAD_EXPORT CalculationSettings
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
     * \ingroup OpenDocument
     */
    void loadOdf(const KoXmlElement& body);

    /**
     * \ingroup OpenDocument
     */
    bool saveOdf(KoXmlWriter &settingsWriter) const;

    /**
     * A document could use a different localization as the KDE default.
     * @return the KLocale associated with this document
     */
    KLocale *locale() const;

    /**
     * Sets the reference year.
     * All two-digit-year dates are stored as numbers relative to a reference year.
     *
     * \param year the new reference year
     */
    void setReferenceYear(int year);

    /**
     * Returns the reference year all two-digit-year dates are stored relative to.
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

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_CALCULATION_SETTINGS
