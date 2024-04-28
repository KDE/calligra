/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autocorrectionlanguage.h"
#include <QDebug>
#include <QLocale>
#include <QSet>

static bool stripCountryCode(QString *languageCode)
{
    const int idx = languageCode->indexOf(QLatin1Char('_'));
    if (idx != -1) {
        *languageCode = languageCode->left(idx);
        return true;
    }
    return false;
}

AutoCorrectionLanguage::AutoCorrectionLanguage(QWidget *parent)
    : QComboBox(parent)
{
    QLocale cLocale(QLocale::C);
    QSet<QString> insertedLanguages;
    const QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for (const QLocale &lang : allLocales) {
        QString languageCode = lang.name();
        if (lang != cLocale) {
            const QString nativeName = lang.nativeLanguageName();
            // For some languages the native name might be empty.
            // In this case use the non native language name as fallback.
            // See: QTBUG-51323
            QString languageName = nativeName.isEmpty() ? QLocale::languageToString(lang.language()) : nativeName;
            languageName = languageName.toLower();
            if (!insertedLanguages.contains(languageName)) {
                addItem(languageName, languageCode);
                insertedLanguages << languageName;
            } else if (stripCountryCode(&languageCode)) {
                if (!insertedLanguages.contains(languageName)) {
                    addItem(languageName, languageCode);
                    insertedLanguages << languageName;
                }
            }
            qInfo() << Q_FUNC_INFO << languageName << languageCode;
        }
    }

    QString defaultLang;
    if (!QLocale::system().uiLanguages().isEmpty()) {
        defaultLang = QLocale::system().uiLanguages().at(0);
        defaultLang.replace(QLatin1Char('-'), QLatin1Char('_'));
        if (defaultLang == QLatin1Char('C')) {
            defaultLang = QStringLiteral("en_US");
        }
    }
    const int index = findData(defaultLang);
    setCurrentIndex(index);
    model()->sort(0);
    qInfo() << Q_FUNC_INFO << QLocale::system().uiLanguages() << ':' << index << defaultLang;
    //    for (int i = 0; i < count(); ++i) {
    //        qInfo()<<i<<':'<<itemText(i)<<itemData(i).toString();
    //    }
}

AutoCorrectionLanguage::~AutoCorrectionLanguage() = default;

QString AutoCorrectionLanguage::language() const
{
    return itemData(currentIndex()).toString();
}

void AutoCorrectionLanguage::setLanguage(const QString &language)
{
    const int index = findData(language);
    setCurrentIndex(index);
}
