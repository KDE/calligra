/* This file is part of the KDE project
   Copyright (C) 2007 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <klocale.h>

#include "kexitextformatter.h"
#include <widget/utils/kexidatetimeformatter.h>
#include <kexidb/utils.h>

//! @internal
class KexiTextFormatter::Private
{
public:
    Private() : field(0), dateFormatter(0), timeFormatter(0) {
    }

    ~Private() {
        delete dateFormatter;
        delete timeFormatter;
    }

    KexiDB::Field* field;
    KexiDateFormatter *dateFormatter;
    KexiTimeFormatter *timeFormatter;
};

KexiTextFormatter::KexiTextFormatter()
        : d(new Private)
{
}

KexiTextFormatter::~KexiTextFormatter()
{
    delete d;
}

void KexiTextFormatter::setField(KexiDB::Field* field)
{
    d->field = field;
    if (!d->field)
        return;
    if (d->field->type() == KexiDB::Field::Date || d->field->type() == KexiDB::Field::DateTime)
        d->dateFormatter = new KexiDateFormatter();
    else {
        delete d->dateFormatter;
        d->dateFormatter = 0;
    }
    if (d->field->type() == KexiDB::Field::Time || d->field->type() == KexiDB::Field::DateTime)
        d->timeFormatter = new KexiTimeFormatter();
    else {
        delete d->timeFormatter;
        d->timeFormatter = 0;
    }
}

QString KexiTextFormatter::valueToText(const QVariant& value, const QString& add) const
{
    //cases, in order of expected frequency
    if (!d->field || d->field->isTextType())
        return value.toString() + add;
    else if (d->field->isIntegerType()) {
        if (value.toInt() == 0)
            return add; //eat 0
    } else if (d->field->isFPNumericType()) {
//! @todo precision!
//! @todo support 'g' format
        if (value.toDouble() == 0.0)
            return add.isEmpty() ? "0" : add; //eat 0
#if 0 //moved to KexiDB::formatNumberForVisibleDecimalPlaces()
        QString text(QString::number(value.toDouble(), 'f',
                                     QMAX(d->field->visibleDecimalPlaces(), 10)));  //!<-- 10 is quite good maximum for fractional digits
        //!< @todo add command line settings?
//! @todo (js): get decimal places settings here...
        QStringList sl = QStringList::split(".", text);
        //nothing
    } else if (sl.count() == 2) {
//    kdDebug() << "sl.count()=="<<sl.count()<< " " <<sl[0] << " | " << sl[1] << endl;
        const QString sl1 = sl[1];
        int pos = sl1.length() - 1;
        if (pos >= 1) {
            for (;pos >= 0 && sl1[pos] == '0';pos--)
                ;
            pos++;
        }
        if (pos > 0)
            text = sl[0] + m_decsym + sl1.left(pos);
        else
            text = sl[0]; //no decimal point
    }
#endif
    return KexiDB::formatNumberForVisibleDecimalPlaces(
               value.toDouble(), d->field->visibleDecimalPlaces()) + add;
}
else if (d->field->type() == KexiDB::Field::Boolean)
{
//! @todo temporary solution for booleans!
    const bool boolValue = value.isNull() ? QVariant(add).toBool() : value.toBool();
    return boolValue ? "1" : "0";
} else if (d->field->type() == KexiDB::Field::Date)
{
    return d->dateFormatter->dateToString(value.toString().isEmpty() ? QDate() : value.toDate());
} else if (d->field->type() == KexiDB::Field::Time)
{
    return d->timeFormatter->timeToString(
               //hack to avoid converting null variant to valid QTime(0,0,0)
               value.toString().isEmpty() ? value.toTime() : QTime(99, 0, 0));
} else if (d->field->type() == KexiDB::Field::DateTime)
{
    if (value.toString().isEmpty())
        return add;
    return d->dateFormatter->dateToString(value.toDateTime().date()) + " " +
           d->timeFormatter->timeToString(value.toDateTime().time());
} else if (d->field->type() == KexiDB::Field::BigInteger)
{
    if (value.toLongLong() == 0)
        return add; //eat 0
}
//default: text
return value.toString() + add;
}

QVariant KexiTextFormatter::textToValue(const QString& text) const
{
    if (!d->field)
        return QVariant();
    const KexiDB::Field::Type t = d->field->type();
    switch (t) {
    case KexiDB::Field::Text:
    case KexiDB::Field::LongText:
        return text;
    case KexiDB::Field::Byte:
    case KexiDB::Field::ShortInteger:
        return text.toShort();
//! @todo uint, etc?
    case KexiDB::Field::Integer:
        return text.toInt();
    case KexiDB::Field::BigInteger:
        return text.toLongLong();
    case KexiDB::Field::Boolean:
//! @todo temporary solution for booleans!
        return text == "1" ? QVariant(true, 1) : QVariant(false, 0);
    case KexiDB::Field::Date:
        return d->dateFormatter->stringToVariant(text);
    case KexiDB::Field::Time:
        return d->timeFormatter->stringToVariant(text);
    case KexiDB::Field::DateTime:
        return stringToDateTime(*d->dateFormatter, *d->timeFormatter, text);
    case KexiDB::Field::Float:
    case KexiDB::Field::Double: {
        // replace custom decimal symbol with '.' as required by to{Float|Double}()
        QString fixedText(text);
        fixedText.replace(KGlobal::locale()->decimalSymbol(), ".");
        if (t == KexiDB::Field::Double)
            return fixedText.toDouble();
        return fixedText.toFloat();
    }
    default:
        return text;
    }
//! @todo more data types!
}

bool KexiTextFormatter::valueIsEmpty(const QString& text) const
{
    if (text.isEmpty())
        return true;

    if (d->field) {
        const KexiDB::Field::Type t = d->field->type();
        if (t == KexiDB::Field::Date)
            return d->dateFormatter->isEmpty(text);
        else if (t == KexiDB::Field::Time)
            return d->timeFormatter->isEmpty(text);
        else if (t == KexiDB::Field::Time)
            return dateTimeIsEmpty(*d->dateFormatter, *d->timeFormatter, text);
    }

//! @todo
    return text.isEmpty();
}

bool KexiTextFormatter::valueIsValid(const QString& text) const
{
    if (!d->field)
        return true;
//! @todo fix for fields with "required" property = true
    if (valueIsEmpty(text)/*ok?*/)
        return true;

    const KexiDB::Field::Type t = d->field->type();
    if (t == KexiDB::Field::Date)
        return d->dateFormatter->stringToVariant(text).isValid();
    else if (t == KexiDB::Field::Time)
        return d->timeFormatter->stringToVariant(text).isValid();
    else if (t == KexiDB::Field::DateTime)
        return dateTimeIsValid(*d->dateFormatter, *d->timeFormatter, text);

//! @todo
    return true;
}

QString KexiTextFormatter::inputMask() const
{
    const KexiDB::Field::Type t = d->field->type();
    if (t == KexiDB::Field::Date) {
//! @todo use KDateWidget?
        return d->dateFormatter->inputMask();
    } else if (t == KexiDB::Field::Time) {
//! @todo use KTimeWidget
        d->timeFormatter->inputMask();
    } else if (t == KexiDB::Field::DateTime) {
        dateTimeInputMask(*d->dateFormatter, *d->timeFormatter);
    }
    return QString();
}

