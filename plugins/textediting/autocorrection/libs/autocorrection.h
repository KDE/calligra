/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "autocorrection_export.h"
#include <QHash>
#include <QSet>
#include <QTextCursor>

/**
 * @brief The AutoCorrection class
 * @author Laurent Montel <montel@kde.org>
 */
class AUTOCORRECTION_EXPORT AutoCorrection
{
public:
    struct TypographicQuotes {
        QChar begin;
        QChar end;
    };

    explicit AutoCorrection();
    virtual ~AutoCorrection();

    void setLanguage(const QString &lang, bool forceGlobal = false);
    void setEnabledAutoCorrection(bool b);
    void setUppercaseFirstCharOfSentence(bool b);
    void setFixTwoUppercaseChars(bool b);
    void setSingleSpaces(bool b);
    void setAutoFractions(bool b);
    void setCapitalizeWeekDays(bool b);
    void setReplaceDoubleQuotes(bool b);
    void setReplaceSingleQuotes(bool b);
    void setAdvancedAutocorrect(bool b);
    void setTypographicSingleQuotes(TypographicQuotes singleQuote);
    void setTypographicDoubleQuotes(TypographicQuotes doubleQuote);
    void setUpperCaseExceptions(const QSet<QString> &exceptions);
    void setTwoUpperLetterExceptions(const QSet<QString> &exceptions);
    void setAutocorrectEntries(const QHash<QString, QString> &entries);
    void setAutoFormatUrl(bool b);
    void setAutoBoldUnderline(bool b);
    void setSuperScript(bool b);
    void setAddNonBreakingSpace(bool b);

    Q_REQUIRED_RESULT bool isEnabledAutoCorrection() const;
    Q_REQUIRED_RESULT bool isUppercaseFirstCharOfSentence() const;
    Q_REQUIRED_RESULT bool isFixTwoUppercaseChars() const;
    Q_REQUIRED_RESULT bool isSingleSpaces() const;
    Q_REQUIRED_RESULT bool isAutoFractions() const;
    Q_REQUIRED_RESULT bool isCapitalizeWeekDays() const;
    Q_REQUIRED_RESULT bool isReplaceDoubleQuotes() const;
    Q_REQUIRED_RESULT bool isReplaceSingleQuotes() const;
    Q_REQUIRED_RESULT bool isAdvancedAutocorrect() const;
    Q_REQUIRED_RESULT bool isAutoFormatUrl() const;
    Q_REQUIRED_RESULT bool isAutoBoldUnderline() const;
    Q_REQUIRED_RESULT bool isSuperScript() const;

    Q_REQUIRED_RESULT bool isAddNonBreakingSpace() const;

    Q_REQUIRED_RESULT QString language() const;
    Q_REQUIRED_RESULT TypographicQuotes typographicSingleQuotes() const;
    Q_REQUIRED_RESULT TypographicQuotes typographicDoubleQuotes() const;
    Q_REQUIRED_RESULT TypographicQuotes typographicDefaultSingleQuotes() const;
    Q_REQUIRED_RESULT TypographicQuotes typographicDefaultDoubleQuotes() const;
    Q_REQUIRED_RESULT QSet<QString> upperCaseExceptions() const;
    Q_REQUIRED_RESULT QSet<QString> twoUpperLetterExceptions() const;
    Q_REQUIRED_RESULT QHash<QString, QString> autocorrectEntries() const;

    bool autocorrect(bool htmlMode, QTextDocument &document, int &position);

    /// emojis has the format :keyword: eg. :pine: wich shows a green tree.
    int replaceEmoji(QTextDocument &document, int position);

    Q_REQUIRED_RESULT bool addAutoCorrect(const QString &currentWord, const QString &replaceWord);

    void writeAutoCorrectionXmlFile();
    void writeAutoCorrectionXmlFile(const QString &fileName);

    void setNonBreakingSpace(QChar nonBreakingSpace);

    void loadGlobalFileName(const QString &fname, bool forceGlobal);
    void loadLocalFileName(const QString &localFileName);

    /// Re-implent this to read your configuration
    virtual void readConfiguration();
    /// Re-implent this to write your configuration
    virtual void writeConfiguration();

    void readConfig();
    void writeConfig();

protected:
    Q_REQUIRED_RESULT bool isFrenchLanguage() const;

    void fixTwoUppercaseChars();
    Q_REQUIRED_RESULT bool singleSpaces();
    void capitalizeWeekDays();
    Q_REQUIRED_RESULT bool autoFractions();
    void uppercaseFirstCharOfSentence();
    Q_REQUIRED_RESULT int advancedAutocorrect();
    void replaceTypographicQuotes();
    void superscriptAppendix();
    void addNonBreakingSpace();

    void selectPreviousWord(QTextCursor &cursor, int cursorPosition);
    void selectStringOnMaximumSearchString(QTextCursor &cursor, int cursorPosition);

    Q_REQUIRED_RESULT bool autoFormatURLs();
    Q_REQUIRED_RESULT bool autoBoldUnderline();

    Q_REQUIRED_RESULT QString autoDetectURL(const QString &_word) const;
    void readAutoCorrectionXmlFile(bool forceGlobal = false);
    Q_REQUIRED_RESULT bool excludeToUppercase(const QString &word) const;
    Q_REQUIRED_RESULT QColor linkColor();

private:
    bool mSingleSpaces = true; // suppress double spaces.
    bool mUppercaseFirstCharOfSentence = false; // convert first letter of a sentence automatically to uppercase
    bool mFixTwoUppercaseChars = false; // convert two uppercase characters to one upper and one lowercase.
    bool mAutoFractions = true; // replace 1/2 with ½
    bool mCapitalizeWeekDays = false;
    bool mAdvancedAutocorrect = false; // autocorrection from a list of entries

    bool mReplaceDoubleQuotes = false; // replace double quotes with typographical quotes
    bool mReplaceSingleQuotes = false; // replace single quotes with typographical quotes

    bool mAutoFormatUrl = false;
    bool mAutoBoldUnderline = false;
    bool mEnabled = false;
    bool mSuperScriptAppendix = false;

    bool mAddNonBreakingSpace = false;
    int mMaxFindStringLenght = 0;
    int mMinFindStringLenght = 0;
    QString mWord;
    QTextCursor mCursor;
    QChar mNonBreakingSpace;

    QString mAutoCorrectLang;
    QStringList mCacheNameOfDays;
    QSet<QString> mUpperCaseExceptions;
    QSet<QString> mTwoUpperLetterExceptions;
    QHash<QString, QString> mAutocorrectEntries;
    QHash<QString, QString> mSuperScriptEntries;
    TypographicQuotes mTypographicSingleQuotes;
    TypographicQuotes mTypographicDoubleQuotes;
    QColor mLinkColor;
};
