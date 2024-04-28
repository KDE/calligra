/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "Autocorrection.h"

#include "importkmailautocorrection.h"
#include "importlibreofficeautocorrection.h"

#include <QDebug>
#include <QDir>
#include <QXmlStreamWriter>

void AutoCorrection::updateAutoCorrections(const QString &libreoffice, const QString &calligra)
{
    const QString prefix = QStringLiteral("acor_");
    QDir lo_dir(libreoffice);
    QDir calligra_dir(calligra);
    const auto files = lo_dir.entryList(QStringList() << QStringLiteral("%1*.dat").arg(prefix));
    // qInfo()<<Q_FUNC_INFO<<files;
    for (const auto &file : files) {
        clear();
        language = file;
        language.replace(prefix, QLatin1String()).replace(QStringLiteral(".dat"), QLatin1String());
        language.replace(QLatin1Char('-'), QLatin1Char('_'));
        bool onlyWordExceptions = false;
        bool load = calligra_dir.cd(language);
        if (!load) {
            auto subs = language.left(language.lastIndexOf(QLatin1Char('_')));
            qDebug() << Q_FUNC_INFO << "calligra:" << language << "does not exist, trying" << subs;
            load = calligra_dir.cd(subs);
            onlyWordExceptions = load;
        }
        if (load) {
            m_type = Calligra;
            const auto list = calligra_dir.entryList(QStringList() << QStringLiteral("%1.xml").arg(language));
            if (!list.isEmpty()) {
                QString fileName = calligra_dir.canonicalPath() + QLatin1Char('/') + list.first();
                if (!importAutoCorrection(m_type, fileName)) {
                    qDebug() << Q_FUNC_INFO << "failed to load:" << list.first() << fileName;
                } else {
                    // qInfo()<<Q_FUNC_INFO<<"loaded calligra:"<<fileName;
                    if (onlyWordExceptions) {
                        auto we = mTwoUpperLetterExceptions;
                        clear();
                        mTwoUpperLetterExceptions = we;
                        // qInfo()<<Q_FUNC_INFO<<"use only word exceptions:"<<language<<mTwoUpperLetterExceptions;
                    }
                }
                calligra_dir.cd(QStringLiteral(".."));
            } else {
                qDebug() << Q_FUNC_INFO << "No xml file for language:" << language;
            }
        }
        QString fileName = lo_dir.canonicalPath() + QLatin1Char('/') + file;
        m_type = LibreOffice;
        if (!importAutoCorrection(m_type, fileName)) {
            qDebug() << Q_FUNC_INFO << "failed to load:" << fileName;
            continue;
        }
        if (newLanguages.contains(language) || modifiedLanguages.contains(language)) {
            QString saveUrl = calligra_dir.canonicalPath() + QLatin1Char('/') + language + QLatin1Char('/') + language + QLatin1String(".xml");
            // qInfo()<<Q_FUNC_INFO<<"saving:"<<saveUrl;
            writeAutoCorrectionXmlFile(saveUrl);
        }
    }
}

bool AutoCorrection::importAutoCorrection(AutoCorrection::ImportFileType type, const QString &fileName)
{
    // qInfo()<<Q_FUNC_INFO<<type<<fileName;
    if (!fileName.isEmpty()) {
        ImportAbstractAutocorrection *importAutoCorrection = nullptr;
        switch (type) {
        case AutoCorrection::LibreOffice:
            importAutoCorrection = new ImportLibreOfficeAutocorrection();
            break;
        case AutoCorrection::Calligra:
            importAutoCorrection = new ImportKMailAutocorrection;
            break;
        default:
            return false;
        }
        QString messageError;
        if (importAutoCorrection->import(fileName, messageError, ImportAbstractAutocorrection::All)) {
            // qInfo()<<Q_FUNC_INFO<<"updated:"<<type<<fileName;
            addAutoCorrectEntries(importAutoCorrection);
        } else {
            qDebug() << Q_FUNC_INFO << messageError;
            delete importAutoCorrection;
            return false;
        }
        delete importAutoCorrection;
    }
    return true;
}

void AutoCorrection::addAutoCorrectEntries(const ImportAbstractAutocorrection *importAutoCorrection)
{
    reportchanges(mTypographicSingleQuotes != importAutoCorrection->typographicSingleQuotes(), 1);
    mTypographicSingleQuotes = importAutoCorrection->typographicSingleQuotes();

    reportchanges(mTypographicDoubleQuotes != importAutoCorrection->typographicDoubleQuotes(), 2);
    mTypographicDoubleQuotes = importAutoCorrection->typographicDoubleQuotes();

    for (const auto &s : importAutoCorrection->upperCaseExceptions()) {
        if (!mUpperCaseExceptions.contains(s)) {
            mUpperCaseExceptions.insert(s);
            reportchanges(true, 3);
        }
    }
    for (const auto &s : importAutoCorrection->twoUpperLetterExceptions()) {
        if (!mTwoUpperLetterExceptions.contains(s)) {
            mTwoUpperLetterExceptions.insert(s);
            reportchanges(true, 4);
        }
    }
    if (!importAutoCorrection->superScriptEntries().isEmpty()) {
        QHash<QString, QString>::const_iterator it = importAutoCorrection->superScriptEntries().constBegin();
        for (; it != importAutoCorrection->superScriptEntries().constEnd(); ++it) {
            reportchanges(!mSuperScriptEntries.contains(it.key()), 5);
            mSuperScriptEntries.insert(it.key(), it.value());
        }
    }
    if (!importAutoCorrection->autocorrectEntries().isEmpty()) {
        QHash<QString, QString>::const_iterator it = importAutoCorrection->autocorrectEntries().constBegin();
        for (; it != importAutoCorrection->autocorrectEntries().constEnd(); ++it) {
            reportchanges(!mAutocorrectEntries.contains(it.key()), 6);
            mAutocorrectEntries.insert(it.key(), it.value());
        }
    }
}

void AutoCorrection::createCMake(const QFileInfo &fileInfo, const QString &xmlFile)
{
    auto cmake = QStringLiteral("%1/%2").arg(fileInfo.absolutePath(), QStringLiteral("CMakeLists.txt"));
    if (QFile::exists(cmake)) {
        return;
    }
    newLanguages << language;
    if (modifiedLanguages.contains(language)) {
        modifiedLanguages.removeAll(language);
    }
    QFile file(cmake);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << Q_FUNC_INFO << "failed to open CMakeLists file:" << cmake;
        return;
    }
    // qInfo()<<Q_FUNC_INFO<<cmake<<xmlFile;
    QString data = QStringLiteral("install(FILES %1 DESTINATION ${KDE_INSTALL_DATADIR}/calligra/autocorrect)\n").arg(xmlFile);
    file.write(data.toUtf8());
    file.close();
    auto dir = fileInfo.absoluteDir();
    bool ok = dir.cdUp();
    Q_ASSERT_X(ok, "createCMake:", "Eeek, something seriously wrong");
    Q_UNUSED(ok)
    cmake = QStringLiteral("%1/%2").arg(dir.absolutePath(), QStringLiteral("CMakeLists.txt"));
    file.setFileName(cmake);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << Q_FUNC_INFO << "failed to open parent CMakeLists file:" << cmake;
        return;
    }
    data = QStringLiteral("add_subdirectory(%1)\n").arg(xmlFile.split(QLatin1Char('.')).first());
    file.write(data.toUtf8());
    file.close();
    // qInfo()<<Q_FUNC_INFO<<"updated:"<<file.fileName();
}

void AutoCorrection::writeAutoCorrectionXmlFile(const QString &filename)
{
    // qInfo()<<Q_FUNC_INFO<<filename;
    if (filename.isEmpty()) {
        qDebug() << "We can't save to empty filename";
        return;
    }
    const QString fname = filename;
    QFileInfo fileInfo(fname);
    QDir().mkpath(fileInfo.absolutePath());
    createCMake(fileInfo, fname.right(fname.length() - fname.lastIndexOf(QLatin1Char('/')) - 1));
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "We can't save in file :" << fname;
        return;
    }
    QXmlStreamWriter streamWriter(&file);

    streamWriter.setAutoFormatting(true);
    streamWriter.setAutoFormattingIndent(2);
    streamWriter.writeStartDocument();

    streamWriter.writeDTD(QStringLiteral("<!DOCTYPE autocorrection>"));

    streamWriter.writeStartElement(QStringLiteral("Word"));

    streamWriter.writeStartElement(QStringLiteral("items"));
    QHashIterator<QString, QString> i(mAutocorrectEntries);
    while (i.hasNext()) {
        i.next();
        streamWriter.writeStartElement(QStringLiteral("item"));
        streamWriter.writeAttribute(QStringLiteral("find"), i.key());
        streamWriter.writeAttribute(QStringLiteral("replace"), i.value());
        streamWriter.writeEndElement();
    }
    streamWriter.writeEndElement();

    streamWriter.writeStartElement(QStringLiteral("UpperCaseExceptions"));
    QSet<QString>::const_iterator upper = mUpperCaseExceptions.constBegin();
    while (upper != mUpperCaseExceptions.constEnd()) {
        streamWriter.writeStartElement(QStringLiteral("word"));
        streamWriter.writeAttribute(QStringLiteral("exception"), *upper);
        ++upper;
        streamWriter.writeEndElement();
    }
    streamWriter.writeEndElement();

    streamWriter.writeStartElement(QStringLiteral("TwoUpperLetterExceptions"));
    QSet<QString>::const_iterator twoUpper = mTwoUpperLetterExceptions.constBegin();
    while (twoUpper != mTwoUpperLetterExceptions.constEnd()) {
        streamWriter.writeStartElement(QStringLiteral("word"));
        streamWriter.writeAttribute(QStringLiteral("exception"), *twoUpper);
        ++twoUpper;
        streamWriter.writeEndElement();
    }
    streamWriter.writeEndElement();

    streamWriter.writeStartElement(QStringLiteral("DoubleQuote"));
    streamWriter.writeStartElement(QStringLiteral("doublequote"));
    streamWriter.writeAttribute(QStringLiteral("begin"), mTypographicDoubleQuotes.begin);
    streamWriter.writeAttribute(QStringLiteral("end"), mTypographicDoubleQuotes.end);
    streamWriter.writeEndElement();
    streamWriter.writeEndElement();

    streamWriter.writeStartElement(QStringLiteral("SimpleQuote"));
    streamWriter.writeStartElement(QStringLiteral("simplequote"));
    streamWriter.writeAttribute(QStringLiteral("begin"), mTypographicSingleQuotes.begin);
    streamWriter.writeAttribute(QStringLiteral("end"), mTypographicSingleQuotes.end);
    streamWriter.writeEndElement();
    streamWriter.writeEndElement();

    streamWriter.writeEndDocument();
}

void AutoCorrection::reportchanges(bool change, int type)
{
    if (!change || m_type != LibreOffice || modifiedLanguages.contains(language)) {
        return;
    }
    qInfo() << Q_FUNC_INFO << language << "type:" << type;
    modifiedLanguages << language;
}
