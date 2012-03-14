/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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

#ifndef KEXIUTILS_IDENTIFIER_H
#define KEXIUTILS_IDENTIFIER_H

#include "validator.h"
#include <QString>

namespace KexiUtils
{

/*! \return true if \a s is a valid identifier, ie. starts with a letter or '_' character
 and contains only letters, numbers and '_' character. */
KEXIUTILS_EXPORT bool isIdentifier(const QString& s);

/*! \return valid identifier based on \a s.
 Non-alphanumeric characters (or spaces) are replaced with '_'.
 If a number is at the beginning, '_' is added at start.
 Empty strings are not changed. Case remains unchanged. */
KEXIUTILS_EXPORT QString string2Identifier(const QString &s);

/*! \return useful message "Value of "valueName" column must be an identifier.
  "v" is not a valid identifier.". It is also used by IdentifierValidator.  */
KEXIUTILS_EXPORT QString identifierExpectedMessage(const QString &valueName,
        const QVariant& v);

//! \return Valid filename based on \a s
KEXIUTILS_EXPORT QString string2FileName(const QString &s);

//! Validates input for identifier name.
class KEXIUTILS_EXPORT IdentifierValidator : public Validator
{
public:
    IdentifierValidator(QObject * parent = 0);
    virtual ~IdentifierValidator();
    virtual State validate(QString & input, int & pos) const;

protected:
    virtual Validator::Result internalCheck(const QString &valueName, const QVariant& v,
                                            QString &message, QString &details);
};
}

#endif
