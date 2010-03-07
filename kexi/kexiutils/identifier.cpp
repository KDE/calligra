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

#include "identifier.h"
#include "transliteration_table.h"

using namespace KexiUtils;

bool KexiUtils::isIdentifier(const QString& s)
{
    uint i;
    const uint sLength = s.length();
    for (i = 0; i < sLength; i++) {
        QChar c = s.at(i).toLower();
        if (!(c == '_' || (c >= 'a' && c <= 'z') || (i > 0 && c >= '0' && c <= '9')))
            break;
    }
    return i > 0 && i == sLength;
}

QString KexiUtils::string2FileName(const QString &s)
{
    QString fn = s.simplified();
    fn.replace(' ', "_"); fn.replace('$', "_");
    fn.replace('\\', "-"); fn.replace('/', "-");
    fn.replace(':', "-"); fn.replace('*', "-");
    return fn;
}

inline QString char2Identifier(const QChar& c)
{
    if (c.unicode() >= TRANSLITERATION_TABLE_SIZE)
        return QString(QChar('_'));
    const char *const s = transliteration_table()[c.unicode()];
    return s ? QString::fromLatin1(s) : QString(QChar('_'));
}

QString KexiUtils::string2Identifier(const QString &s)
{
    if (s.isEmpty())
        return QString();
    QString r, id = s.simplified();
    if (id.isEmpty())
        return QString();
    r.reserve(id.length());
    id.replace(' ', "_");
    QChar c = id[0];
    QString add;
    bool wasUnderscore = false;

    if (c >= '0' && c <= '9') {
        r += '_';
        r += c;
    } else {
        add = char2Identifier(c);
        r += add;
        wasUnderscore = add == "_";
    }

    const uint idLength = id.length();
    for (uint i = 1; i < idLength; i++) {
        add = char2Identifier(id.at(i));
        if (wasUnderscore && add == "_")
            continue;
        wasUnderscore = add == "_";
        r += add;
    }
    return r;
}

//--------------------------------------------------------------------------------

QString KexiUtils::identifierExpectedMessage(const QString &valueName, const QVariant& v)
{
    return "<p>" + i18n("Value of \"%1\" column must be an identifier.", valueName)
           + "</p><p>" + i18n("\"%1\" is not a valid identifier.", v.toString()) + "</p>";
}

//--------------------------------------------------------------------------------

IdentifierValidator::IdentifierValidator(QObject * parent)
        : Validator(parent)
{
}

IdentifierValidator::~IdentifierValidator()
{
}

QValidator::State IdentifierValidator::validate(QString& input, int& pos) const
{
    uint i;
    for (i = 0; (int)i < input.length() && input.at(i) == ' '; i++)
        ;
    pos -= i; //i chars will be removed from beginning
    if ((int)i < input.length() && input.at(i) >= '0' && input.at(i) <= '9')
        pos++; //_ will be added at the beginning
    bool addspace = (input.right(1) == " ");
    input = string2Identifier(input);
    if (addspace)
        input += "_";
    if (pos > input.length())
        pos = input.length();
    return input.isEmpty() ? QValidator::Intermediate : Acceptable;
}

Validator::Result IdentifierValidator::internalCheck(
    const QString &valueName, const QVariant& v,
    QString &message, QString & /*details*/)
{
    if (isIdentifier(v.toString()))
        return Validator::Ok;
    message = identifierExpectedMessage(valueName, v);
    return Validator::Error;
}

