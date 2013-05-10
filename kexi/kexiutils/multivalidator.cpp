/* This file is part of the KDE project
   Copyright (C) 2004, 2006 Jarosław Staniek <staniek@kde.org>

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

#include "multivalidator.h"

namespace KexiUtils
{
class MultiValidator::Private
{
public:
    Private() {
    }
    ~Private() {
        qDeleteAll(ownedSubValidators);
        ownedSubValidators.clear();
    }

    QList<QValidator*> ownedSubValidators;
    QList<QValidator*> subValidators;
};
}

//-----------------------------------------------------------

using namespace KexiUtils;

//-----------------------------------------------------------

MultiValidator::MultiValidator(QObject* parent)
        : KexiDB::Validator(parent)
        , d(new Private)
{
}

MultiValidator::MultiValidator(QValidator *validator, QObject * parent)
        : KexiDB::Validator(parent)
        , d(new Private)
{
    addSubvalidator(validator);
}

MultiValidator::~MultiValidator()
{
    delete d;
}

void MultiValidator::addSubvalidator(QValidator* validator, bool owned)
{
    if (!validator)
        return;
    d->subValidators.append(validator);
    if (owned && !validator->parent())
        d->ownedSubValidators.append(validator);
}

QValidator::State MultiValidator::validate(QString & input, int & pos) const
{
    State s;
    foreach(QValidator* validator, d->subValidators) {
        s = validator->validate(input, pos);
        if (s == Intermediate || s == Invalid)
            return s;
    }
    return Acceptable;
}

void MultiValidator::fixup(QString & input) const
{
    foreach(QValidator* validator, d->subValidators) {
        validator->fixup(input);
    }
}

KexiDB::Validator::Result MultiValidator::internalCheck(
    const QString &valueName, const QVariant& v,
    QString &message, QString &details)
{
    Result r;
    bool warning = false;
    foreach(QValidator* validator, d->subValidators) {
        if (dynamic_cast<KexiDB::Validator*>(validator))
            r = dynamic_cast<KexiDB::Validator*>(validator)->internalCheck(valueName, v, message, details);
        else
            r = Ok; //ignore
        if (r == Error)
            return Error;
        else if (r == Warning)
            warning = true;
    }
    return warning ? Warning : Ok;
}
