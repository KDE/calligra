/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "Importabstractautocorrection.h"

class QTemporaryDir;
class KZip;
class QDomDocument;
class QFile;
class KArchiveDirectory;

class ImportLibreOfficeAutocorrection : public ImportAbstractAutocorrection
{
public:
    ImportLibreOfficeAutocorrection();
    ~ImportLibreOfficeAutocorrection() override;

    Q_REQUIRED_RESULT bool import(const QString &fileName, QString &errorMessage, ImportAbstractAutocorrection::LoadAttribute loadAttribute = All) override;

private:
    enum Type {
        DOCUMENT,
        SENTENCE,
        WORD,
    };

    void importAutoCorrectionFile();
    void closeArchive();
    Q_REQUIRED_RESULT bool loadDomElement(QDomDocument &doc, QFile *file);
    Q_REQUIRED_RESULT bool importFile(Type type, const KArchiveDirectory *archiveDirectory);
    KZip *mArchive = nullptr;
    QTemporaryDir *mTempDir = nullptr;
};
