/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kexiqueryparameters.h"
#include <kexi_global.h>
#include "utils/kexidatetimeformatter.h"

#include <KDbQuerySchemaParameter>
#include <KDbUtils>

#include <kinputdialog.h>
#include <knumvalidator.h>
#include <KLocalizedString>

#include <QDebug>

//static
QList<QVariant> KexiQueryParameters::getParameters(QWidget *parent,
        const KDbDriver &driver, KDbQuerySchema& querySchema, bool *ok)
{
    Q_ASSERT(ok);
    Q_UNUSED(driver);
    *ok = false;
    const KDbQuerySchemaParameterList params(querySchema.parameters());
    QList<QVariant> values;
    const QString caption(xi18nc("Enter Query Parameter Value", "Enter Parameter Value"));
    for (KDbQuerySchemaParameterListConstIterator it = params.constBegin();
            it != params.constEnd(); ++it) {
        switch ((*it).type) {
        case KDbField::Byte:
        case KDbField::ShortInteger:
        case KDbField::Integer:
        case KDbField::BigInteger: {
//! @todo problem for ranges in case of BigInteger - will disappear when we remove use of KInputDialog
            int minValue, maxValue;
//! @todo add support for unsigned parameter here
            KDb::getLimitsForType((*it).type, &minValue, &maxValue);
            const int result = KInputDialog::getInteger(
                                   caption, (*it).message, 0, minValue, maxValue, 1/*step*/, 10/*base*/, ok, parent);
            if (!*ok)
                return QList<QVariant>(); //cancelled
            values.append(result);
            break;
        }
        case KDbField::Boolean: {
            QStringList list;
            list << xi18nc("Boolean True - Yes", "Yes") << xi18nc("Boolean False - No", "No");
            const QString result = KInputDialog::getItem(
                                       caption, (*it).message, list, 0/*current*/, false /*!editable*/, ok, parent);
            if (!*ok || result.isEmpty())
                return QList<QVariant>(); //cancelled
            values.append(result == list.first());
            break;
        }
        case KDbField::Date: {
                KexiDateFormatter df;
                const QString result = KInputDialog::getText(
                                           caption, (*it).message, QString(), ok, parent, 0/*name*/,
//! @todo add validator
                                           0/*validator*/, df.inputMask());
                if (!*ok)
                    return QList<QVariant>(); //cancelled
                values.append(df.fromString(result));
                break;
            }
        case KDbField::DateTime: {
                KexiDateFormatter df;
                KexiTimeFormatter tf;
                const QString result = KInputDialog::getText(
                                           caption, (*it).message, QString(), ok, parent, 0/*name*/,
//! @todo add validator
                                           0/*validator*/, KexiDateTimeFormatter::inputMask(df, tf));
                if (!*ok)
                    return QList<QVariant>(); //cancelled
                values.append(KexiDateTimeFormatter::fromString(df, tf, result));
                break;
            }
        case KDbField::Time: {
                KexiTimeFormatter tf;
                const QString result = KInputDialog::getText(
                                           caption, (*it).message, QString(), ok, parent, 0/*name*/,
//! @todo add validator
                                           0/*validator*/, tf.inputMask());
                if (!*ok)
                    return QList<QVariant>(); //cancelled
                values.append(tf.fromString(result));
                break;
            }
        case KDbField::Float:
        case KDbField::Double: {
            // KInputDialog::getDouble() does not work well, use getText and double validator
            KDoubleValidator validator(0);
            const QString textResult(
                KInputDialog::getText(caption, (*it).message, QString(),
                                      ok, parent, &validator));
            if (!*ok || textResult.isEmpty())
                return QList<QVariant>(); //cancelled
//! @todo this value will be still rounded: consider storing them as a decimal type
//!    (e.g. using a special qint64+decimalplace class)
            const double result = textResult.toDouble(ok); //this is also good for float (to avoid rounding)
            if (!*ok)
                return QList<QVariant>();
            values.append(result);
            break;
        }
        case KDbField::Text:
        case KDbField::LongText: {
            const QString result = KInputDialog::getText(
                                       caption, (*it).message, QString(), ok, parent);
            if (!*ok)
                return QList<QVariant>(); //cancelled
            values.append(result);
            break;
        }
        case KDbField::BLOB: {
//! @todo BLOB input unsupported
            values.append(QByteArray());
        }
        default:
            qWarning() << "unsupported type " << KDbField::typeName((*it).type)
                << "for parameter \"" << (*it).message << "\" - aborting query execution!";
            return QList<QVariant>();
        }
    }
    *ok = true;
    return values;
}

