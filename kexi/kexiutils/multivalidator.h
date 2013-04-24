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

#ifndef KEXI_VALIDATOR_H
#define KEXI_VALIDATOR_H

#include "kexiutils_export.h"

#include <db/validator.h>

namespace KexiUtils
{

//! @short A validator groupping multiple QValidators
/*! MultiValidator behaves like normal KexiDB::Validator,
 but it allows to add define more than one different validator.
 Given validation is successful if every subvalidator accepted given value.

 - acceptsEmptyValue() is used globally here
   (no matter what is defined in subvalidators).

 - result of calling check() depends on value of check() returned by subvalidators:
   - Error is returned if at least one subvalidator returned Error;
   - Warning is returned if at least one subvalidator returned Warning and
     no validator returned error;
   - Ok is returned only if exactly all subvalidators returned Ok.
   - If there is no subvalidators defined, Error is always returned.
   - If a given subvalidator is not of class Validator but just QValidator,
     it's assumed it's check() method returned Ok.

 - result of calling validate() (a method implemented for QValidator)
   depends on value of validate() returned by subvalidators:
   - Invalid is returned if at least one subvalidator returned Invalid
   - Intermediate is returned if at least one subvalidator returned Intermediate
   - Acceptable is returned if exactly all subvalidators returned Acceptable.
   - If there is no subvalidators defined, Invalid is always returned.

 If there are no subvalidators, the multi validator always accepts the input.
*/
class KEXIUTILS_EXPORT MultiValidator : public KexiDB::Validator
{
public:
    /*! Constructs multivalidator with no subvalidators defined.
     You can add more validators with addSubvalidator(). */
    MultiValidator(QObject * parent = 0);

    /*! Constructs multivalidator with one validator \a validator.
     It will be owned if has no parent defined.
     You can add more validators with addSubvalidator(). */
    MultiValidator(QValidator *validator, QObject * parent = 0);

    ~MultiValidator();

    /*! Adds validator \a validator as another subvalidator.
     Subvalidator will be owned by this multivalidator if \a owned is true
     and its parent is NULL. */
    void addSubvalidator(QValidator* validator, bool owned = true);

    /*! Reimplemented to call validate() on subvalidators. */
    virtual QValidator::State validate(QString & input, int & pos) const;

    /*! Calls QValidator::fixup() on every subvalidator.
     This may be senseless to use this methog in certain cases
     (can return weir results), so think twice before.. */
    virtual void fixup(QString & input) const;

private:
    virtual Validator::Result internalCheck(
        const QString &valueName, const QVariant& v,
        QString &message, QString &details);

    class Private;
    Private* const d;
};

}

#endif
