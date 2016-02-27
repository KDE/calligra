/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
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

// Local
#include "CalculationSettings.h"

#include "Localization.h"
#include "SheetsDebug.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CalculationSettings::Private
{
public:
    KLocale* locale;
    bool caseSensitiveComparisons : 1;
    bool precisionAsShown         : 1;
    bool wholeCellSearchCriteria  : 1;
    bool automaticFindLabels      : 1;
    bool useRegularExpressions    : 1;
    bool useWildcards             : 1;
    bool automaticCalculation     : 1;
    int refYear; // the reference year two-digit years are relative to
    QDate refDate; // the reference date all dates are relative to
    // The precision used for decimal numbers, if the default cell style's
    // precision is set to arbitrary.
    int precision;
    QString fileName;
};

/*****************************************************************************
 *
 * CalculationSettings
 *
 *****************************************************************************/

CalculationSettings::CalculationSettings()
        : d(new Private)
{
    d->locale = new Localization();
    d->caseSensitiveComparisons = true;
    d->precisionAsShown         = false;
    d->wholeCellSearchCriteria  = true;
    d->automaticFindLabels      = true;
    d->useRegularExpressions    = true;
    d->useWildcards             = false;
    d->automaticCalculation     = true;
    d->refYear = 1930;
    d->refDate = QDate(1899, 12, 30);
    d->precision = -1;
}

CalculationSettings::~CalculationSettings()
{
    delete d->locale;
    delete d;
}

KLocale* CalculationSettings::locale() const
{
    return d->locale;
}

void CalculationSettings::setReferenceYear(int year)
{
    d->refYear = year;
}

int CalculationSettings::referenceYear() const
{
    return d->refYear;
}

void CalculationSettings::setReferenceDate(const QDate& date)
{
    if (!date.isValid()) return;
    d->refDate.setDate(date.year(), date.month(), date.day());
}

QDate CalculationSettings::referenceDate() const
{
    return d->refDate;
}

void CalculationSettings::setPrecisionAsShown(bool enable)
{
    d->precisionAsShown = enable;
}

bool CalculationSettings::precisionAsShown() const
{
    return d->precisionAsShown;
}

void CalculationSettings::setDefaultDecimalPrecision(int precision)
{
    d->precision = precision;
}

int CalculationSettings::defaultDecimalPrecision() const
{
    return d->precision;
}

void CalculationSettings::setFileName(const QString& fileName)
{
    d->fileName = fileName;
}

const QString& CalculationSettings::fileName() const
{
    return d->fileName;
}

void CalculationSettings::setAutoCalculationEnabled(bool enable)
{
    d->automaticCalculation = enable;
}

bool CalculationSettings::isAutoCalculationEnabled() const
{
    return d->automaticCalculation;
}

void CalculationSettings::setAutomaticFindLabels(bool enabled)
{
    d->automaticFindLabels = enabled;
}

bool CalculationSettings::automaticFindLabels() const
{
    return d->automaticFindLabels;
}

void CalculationSettings::setCaseSensitiveComparisons(Qt::CaseSensitivity caseSensitive)
{
    d->caseSensitiveComparisons = caseSensitive == Qt::CaseSensitive;
}

Qt::CaseSensitivity CalculationSettings::caseSensitiveComparisons() const
{
    return d->caseSensitiveComparisons ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

void CalculationSettings::setWholeCellSearchCriteria(bool enabled)
{
    d->wholeCellSearchCriteria = enabled;
}

bool CalculationSettings::wholeCellSearchCriteria() const
{
    return d->wholeCellSearchCriteria;
}

void CalculationSettings::setUseRegularExpressions(bool enabled)
{
    d->useRegularExpressions = enabled;
}

bool CalculationSettings::useRegularExpressions() const
{
    return d->useRegularExpressions;
}

void CalculationSettings::setUseWildcards(bool enabled)
{
    d->useWildcards = enabled;
}

bool CalculationSettings::useWildcards() const
{
    return d->useWildcards;
}
