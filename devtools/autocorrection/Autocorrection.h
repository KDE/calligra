/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "Importabstractautocorrection.h"

#include <QHash>
#include <QSet>
#include <QString>

/**
 * @brief The AutoCorrection class
 * @author Laurent Montel <montel@kde.org>
 */

class ImportAbstractAutocorrection;
class QFileInfo;

class AutoCorrection
{
public:
    AutoCorrection() = default;
    ~AutoCorrection() = default;

    enum ImportFileType {
        LibreOffice,
        Calligra,
    };

    void updateAutoCorrections(const QString &libreoffice, const QString &calligra);

private:
    bool importAutoCorrection(AutoCorrection::ImportFileType type, const QString &fileName);
    void addAutoCorrectEntries(const ImportAbstractAutocorrection *importAutoCorrection);
    void writeAutoCorrectionXmlFile(const QString &filename);
    void createCMake(const QFileInfo &fileInfo, const QString &xmlFile);

    void clear()
    {
        mUpperCaseExceptions.clear();
        mTwoUpperLetterExceptions.clear();
        mAutocorrectEntries.clear();
        mSuperScriptEntries.clear();
        mTypographicSingleQuotes = TypographicQuotes();
        mTypographicDoubleQuotes = TypographicQuotes();
    }

    QSet<QString> mUpperCaseExceptions;
    QSet<QString> mTwoUpperLetterExceptions;
    QHash<QString, QString> mAutocorrectEntries;
    QHash<QString, QString> mSuperScriptEntries;
    TypographicQuotes mTypographicSingleQuotes;
    TypographicQuotes mTypographicDoubleQuotes;

    void reportchanges(bool change, int type);
    QString language;
    ImportFileType m_type;

public:
    QStringList newLanguages;
    QStringList modifiedLanguages;
};
