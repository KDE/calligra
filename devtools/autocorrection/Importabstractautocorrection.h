/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QHash>
#include <QSet>
#include <QString>

struct TypographicQuotes {
    QChar begin;
    QChar end;
    bool operator!=(const TypographicQuotes &q) const
    {
        return begin != q.begin || end != q.end;
    }
};

class ImportAbstractAutocorrection
{
public:
    enum LoadAttribute {
        All = 0,
        SuperScript,
    };

    ImportAbstractAutocorrection();
    virtual ~ImportAbstractAutocorrection();

    virtual bool import(const QString &fileName, QString &errorMessage, ImportAbstractAutocorrection::LoadAttribute loadAttribute = All) = 0;

    const QSet<QString> &upperCaseExceptions() const;
    const QSet<QString> &twoUpperLetterExceptions() const;
    const QHash<QString, QString> &autocorrectEntries() const;
    const QHash<QString, QString> &superScriptEntries() const;

    const TypographicQuotes &typographicSingleQuotes() const;
    const TypographicQuotes &typographicDoubleQuotes() const;

    int maxFindStringLenght() const;

    int minFindStringLenght() const;

protected:
    int mMaxFindStringLenght = 0;
    int mMinFindStringLenght = 0;
    QSet<QString> mUpperCaseExceptions;
    QSet<QString> mTwoUpperLetterExceptions;
    QHash<QString, QString> mAutocorrectEntries;
    QHash<QString, QString> mSuperScriptEntries;
    TypographicQuotes mTypographicSingleQuotes;
    TypographicQuotes mTypographicDoubleQuotes;
};
