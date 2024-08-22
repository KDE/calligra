/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "Importabstractautocorrection.h"

ImportAbstractAutocorrection::ImportAbstractAutocorrection() = default;

ImportAbstractAutocorrection::~ImportAbstractAutocorrection() = default;

const QSet<QString> &ImportAbstractAutocorrection::upperCaseExceptions() const
{
    return mUpperCaseExceptions;
}

const QSet<QString> &ImportAbstractAutocorrection::twoUpperLetterExceptions() const
{
    return mTwoUpperLetterExceptions;
}

const QHash<QString, QString> &ImportAbstractAutocorrection::autocorrectEntries() const
{
    return mAutocorrectEntries;
}

const TypographicQuotes &ImportAbstractAutocorrection::typographicSingleQuotes() const
{
    return mTypographicSingleQuotes;
}

const TypographicQuotes &ImportAbstractAutocorrection::typographicDoubleQuotes() const
{
    return mTypographicDoubleQuotes;
}

int ImportAbstractAutocorrection::maxFindStringLenght() const
{
    return mMaxFindStringLenght;
}

int ImportAbstractAutocorrection::minFindStringLenght() const
{
    return mMinFindStringLenght;
}

const QHash<QString, QString> &ImportAbstractAutocorrection::superScriptEntries() const
{
    return mSuperScriptEntries;
}
