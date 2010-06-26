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

#include <KoXmlNS.h>

#include <kdebug.h>

using namespace KSpread;

class CalculationSettings::Private
{
public:
    KLocale* locale;
    bool caseSensitiveComparisons : 1;
    bool precisionAsShown         : 1;
    bool wholeCellSearchCriteria  : 1;
    bool automaticFindLabels      : 1;
    bool useRegularExpressions    : 1;
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
    d->automaticCalculation     = true;
    d->refYear = 1930;
    d->refDate = QDate(1899, 12, 30);
    d->precision = 8;
}

CalculationSettings::~CalculationSettings()
{
    delete d->locale;
    delete d;
}

void CalculationSettings::loadOdf(const KoXmlElement& body)
{
    KoXmlNode settings = KoXml::namedItemNS(body, KoXmlNS::table, "calculation-settings");
    kDebug() << "Calculation settings found?" << !settings.isNull();
    if (!settings.isNull()) {
        KoXmlElement element = settings.toElement();
        if (element.hasAttributeNS(KoXmlNS::table,  "case-sensitive")) {
            d->caseSensitiveComparisons = true;
            QString value = element.attributeNS(KoXmlNS::table, "case-sensitive", "true");
            if (value == "false")
                d->caseSensitiveComparisons = false;
        } else if (element.hasAttributeNS(KoXmlNS::table, "precision-as-shown")) {
            d->precisionAsShown = false;
            QString value = element.attributeNS(KoXmlNS::table, "precision-as-shown", "false");
            if (value == "true")
                d->precisionAsShown = true;
        } else if (element.hasAttributeNS(KoXmlNS::table, "search-criteria-must-apply-to-whole-cell")) {
            d->wholeCellSearchCriteria = true;
            QString value = element.attributeNS(KoXmlNS::table, "search-criteria-must-apply-to-whole-cell", "true");
            if (value == "false")
                d->wholeCellSearchCriteria = false;
        } else if (element.hasAttributeNS(KoXmlNS::table, "automatic-find-labels")) {
            d->automaticFindLabels = true;
            QString value = element.attributeNS(KoXmlNS::table, "automatic-find-labels", "true");
            if (value == "false")
                d->automaticFindLabels = false;
        } else if (element.hasAttributeNS(KoXmlNS::table, "use-regular-expressions")) {
            d->useRegularExpressions = true;
            QString value = element.attributeNS(KoXmlNS::table, "use-regular-expressions", "true");
            if (value == "false")
                d->useRegularExpressions = false;
        } else if (element.hasAttributeNS(KoXmlNS::table, "null-year")) {
            d->refYear = 1930;
            QString value = element.attributeNS(KoXmlNS::table, "null-year", "1930");
            if (value == "false")
                d->refYear = false;
        }

        forEachElement(element, settings) {
            if (element.namespaceURI() != KoXmlNS::table)
                continue;
            else if (element.tagName() ==  "null-date") {
                d->refDate = QDate(1899, 12, 30);
                QString valueType = element.attributeNS(KoXmlNS::table, "value-type", "date");
                if (valueType == "date") {
                    QString value = element.attributeNS(KoXmlNS::table, "date-value", "1899-12-30");
                    QDate date = QDate::fromString(value, Qt::ISODate);
                    if (date.isValid())
                        d->refDate = date;
                } else {
                    kDebug() << "CalculationSettings: Error on loading null date."
                    << "Value type """ << valueType << """ not handled"
                    << ", falling back to default." << endl;
                    // NOTE Stefan: I don't know why different types are possible here!
                }
            } else if (element.tagName() ==  "iteration") {
                // TODO
            }
        }
    }
}

bool CalculationSettings::saveOdf(KoXmlWriter &/*settingsWriter*/) const
{
    return true;
}

KLocale* CalculationSettings::locale() const
{
    return d->locale;
}

void CalculationSettings::setReferenceYear(int year)
{
    if (year < 100)
        d->refYear = 1900 + year;
    else
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

void CalculationSettings::setDefaultDecimalPrecision(int precision)
{
    d->precision = (precision < 0) ? 8 : precision;
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
