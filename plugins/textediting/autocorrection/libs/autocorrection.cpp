/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>
  code based on calligra autocorrection.

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autocorrection.h"

#include "AutoCorrectionDebug.h"
#include "import/importkmailautocorrection.h"
// #include "settings/pimcommonsettings.h"
#include <KColorScheme>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTextBlock>
#include <QTextDocument>
#include <QXmlStreamWriter>

AutoCorrection::AutoCorrection()
{
    mNonBreakingSpace = QChar(QChar::Nbsp);
    // default double quote open 0x201c
    // default double quote close 0x201d
    // default single quote open 0x2018
    // default single quote close 0x2019
    mTypographicSingleQuotes.begin = QChar(0x2018);
    mTypographicSingleQuotes.end = QChar(0x2019);
    mTypographicDoubleQuotes.begin = QChar(0x201c);
    mTypographicDoubleQuotes.end = QChar(0x201d);

    auto locale = QLocale::system();
    mAutoCorrectLang = locale.uiLanguages().value(0).replace(QLatin1Char('-'), QLatin1Char('_'));
    static QRegularExpression reg(QStringLiteral("@.*"));
    mAutoCorrectLang.remove(reg);

    mCacheNameOfDays.reserve(7);
    for (int i = 1; i <= 7; ++i) {
        mCacheNameOfDays.append(locale.dayName(i).toLower());
    }
    readConfig();
}

AutoCorrection::~AutoCorrection() = default;

void AutoCorrection::selectStringOnMaximumSearchString(QTextCursor &cursor, int cursorPosition)
{
    cursor.setPosition(cursorPosition);

    QTextBlock block = cursor.block();
    int pos = qMax(block.position(), cursorPosition - mMaxFindStringLenght);

    // TODO verify that pos == block.position() => it's a full line => not a piece of word
    // TODO if not => check if pos -1 is a space => not a piece of word
    // TODO otherwise move cursor until we detect a space
    // TODO otherwise we must not autoconvert it.
    if (pos != block.position()) {
        const QString text = block.text();
        const int currentPos = (pos - block.position());
        if (!text.at(currentPos - 1).isSpace()) {
            // qDebug() << " current Text " << text << " currentPos "<< currentPos << " pos " << pos;
            // qDebug() << "selected text " << text.right(text.length() - currentPos);
            // qDebug() << "  text after " << text.at(currentPos - 1);
            bool foundWord = false;
            const int textLength(text.length());
            for (int i = currentPos; i < textLength; ++i) {
                if (text.at(i).isSpace()) {
                    pos = qMin(cursorPosition, pos + 1 + block.position());
                    foundWord = true;
                    break;
                }
            }
            if (!foundWord) {
                pos = cursorPosition;
            }
        }
    }
    cursor.setPosition(pos);
    cursor.setPosition(cursorPosition, QTextCursor::KeepAnchor);
}

void AutoCorrection::selectPreviousWord(QTextCursor &cursor, int cursorPosition)
{
    cursor.setPosition(cursorPosition);
    QTextBlock block = cursor.block();
    cursor.setPosition(block.position());
    cursorPosition -= block.position();
    QString string = block.text();
    int pos = 0;
    bool space = false;
    QString::Iterator iter = string.begin();
    while (iter != string.end()) {
        if (iter->isSpace()) {
            if (space) {
                // double spaces belong to the previous word
            } else if (pos < cursorPosition) {
                cursor.setPosition(pos + block.position() + 1); // +1 because we don't want to set it on the space itself
            } else {
                space = true;
            }
        } else if (space) {
            break;
        }
        ++pos;
        ++iter;
    }
    cursor.setPosition(pos + block.position(), QTextCursor::KeepAnchor);
}

bool AutoCorrection::autocorrect(bool htmlMode, QTextDocument &document, int &position)
{
    if (mEnabled) {
        mCursor = QTextCursor(&document);
        mCursor.setPosition(position);

        // If we already have a space not necessary to look at other autocorrect feature.
        if (!singleSpaces()) {
            return false;
        }

        int oldPosition = position;
        selectPreviousWord(mCursor, position);
        mWord = mCursor.selectedText();
        if (mWord.isEmpty()) {
            return true;
        }
        mCursor.beginEditBlock();
        bool done = false;
        if (htmlMode) {
            done = autoFormatURLs();
            if (!done) {
                done = autoBoldUnderline();
                // We replace */- by format => remove cursor position by 2
                if (done) {
                    oldPosition -= 2;
                }
            }
            if (!done) {
                superscriptAppendix();
            }
        }
        if (!done) {
            done = autoFractions();
            // We replace three characters with 1
            if (done) {
                oldPosition -= 2;
            }
        }
        if (!done) {
            uppercaseFirstCharOfSentence();
            fixTwoUppercaseChars();
            capitalizeWeekDays();
            replaceTypographicQuotes();
            if (mWord.length() <= 2) {
                addNonBreakingSpace();
            }
        }

        if (mCursor.selectedText() != mWord) {
            mCursor.insertText(mWord);
        }
        position = oldPosition;

        if (!done) {
            selectStringOnMaximumSearchString(mCursor, position);
            mWord = mCursor.selectedText();
            if (!mWord.isEmpty()) {
                const int newPos = advancedAutocorrect();
                if (newPos != -1) {
                    if (mCursor.selectedText() != mWord) {
                        mCursor.insertText(mWord);
                    }
                    position = newPos;
                }
            }
        }
        mCursor.endEditBlock();
    }
    return true;
}

void AutoCorrection::readConfiguration()
{
}

void AutoCorrection::readConfig()
{
    readConfiguration();
}

void AutoCorrection::writeConfig()
{
    writeConfiguration();
}

void AutoCorrection::writeConfiguration()
{
}

bool AutoCorrection::addAutoCorrect(const QString &currentWord, const QString &replaceWord)
{
    if (!mAutocorrectEntries.contains(currentWord)) {
        mAutocorrectEntries.insert(currentWord, replaceWord);
        writeAutoCorrectionXmlFile();
        return true;
    } else {
        return false;
    }
}

void AutoCorrection::setUpperCaseExceptions(const QSet<QString> &exceptions)
{
    mUpperCaseExceptions = exceptions;
}

void AutoCorrection::setTwoUpperLetterExceptions(const QSet<QString> &exceptions)
{
    mTwoUpperLetterExceptions = exceptions;
}

void AutoCorrection::setAutocorrectEntries(const QHash<QString, QString> &entries)
{
    mMaxFindStringLenght = 0;
    mMinFindStringLenght = 0;
    QHashIterator<QString, QString> i(entries);
    while (i.hasNext()) {
        i.next();
        const int findStringLenght(i.key().length());
        mMaxFindStringLenght = qMax(mMaxFindStringLenght, findStringLenght);
        mMinFindStringLenght = qMin(mMinFindStringLenght, findStringLenght);
    }
    mAutocorrectEntries = entries;
}

void AutoCorrection::setAutoFormatUrl(bool b)
{
    mAutoFormatUrl = b;
}

void AutoCorrection::setAutoBoldUnderline(bool b)
{
    mAutoBoldUnderline = b;
}

void AutoCorrection::setSuperScript(bool b)
{
    mSuperScriptAppendix = b;
}

void AutoCorrection::setAddNonBreakingSpace(bool b)
{
    mAddNonBreakingSpace = b;
}

bool AutoCorrection::isEnabledAutoCorrection() const
{
    return mEnabled;
}

bool AutoCorrection::isUppercaseFirstCharOfSentence() const
{
    return mUppercaseFirstCharOfSentence;
}

bool AutoCorrection::isFixTwoUppercaseChars() const
{
    return mFixTwoUppercaseChars;
}

bool AutoCorrection::isSingleSpaces() const
{
    return mSingleSpaces;
}

bool AutoCorrection::isAutoFractions() const
{
    return mAutoFractions;
}

bool AutoCorrection::isCapitalizeWeekDays() const
{
    return mCapitalizeWeekDays;
}

bool AutoCorrection::isReplaceDoubleQuotes() const
{
    return mReplaceDoubleQuotes;
}

bool AutoCorrection::isReplaceSingleQuotes() const
{
    return mReplaceSingleQuotes;
}

bool AutoCorrection::isAdvancedAutocorrect() const
{
    return mAdvancedAutocorrect;
}

bool AutoCorrection::isAutoFormatUrl() const
{
    return mAutoFormatUrl;
}

bool AutoCorrection::isAutoBoldUnderline() const
{
    return mAutoBoldUnderline;
}

bool AutoCorrection::isSuperScript() const
{
    return mSuperScriptAppendix;
}

bool AutoCorrection::isAddNonBreakingSpace() const
{
    return mAddNonBreakingSpace;
}

AutoCorrection::TypographicQuotes AutoCorrection::typographicDefaultSingleQuotes() const
{
    AutoCorrection::TypographicQuotes quote;
    quote.begin = QChar(0x2018);
    quote.end = QChar(0x2019);
    return quote;
}

AutoCorrection::TypographicQuotes AutoCorrection::typographicDefaultDoubleQuotes() const
{
    AutoCorrection::TypographicQuotes quote;
    quote.begin = QChar(0x201c);
    quote.end = QChar(0x201d);
    return quote;
}

QSet<QString> AutoCorrection::upperCaseExceptions() const
{
    return mUpperCaseExceptions;
}

QSet<QString> AutoCorrection::twoUpperLetterExceptions() const
{
    return mTwoUpperLetterExceptions;
}

QHash<QString, QString> AutoCorrection::autocorrectEntries() const
{
    return mAutocorrectEntries;
}

void AutoCorrection::superscriptAppendix()
{
    if (!mSuperScriptAppendix) {
        return;
    }

    const QString trimmed = mWord.trimmed();
    int startPos = -1;
    int endPos = -1;
    const int trimmedLenght(trimmed.length());

    QHash<QString, QString>::const_iterator i = mSuperScriptEntries.constBegin();
    while (i != mSuperScriptEntries.constEnd()) {
        if (i.key() == trimmed) {
            startPos = mCursor.selectionStart() + 1;
            endPos = startPos - 1 + trimmedLenght;
            break;
        } else if (i.key() == QLatin1String("othernb")) {
            const int pos = trimmed.indexOf(i.value());
            if (pos > 0) {
                QString number = trimmed.left(pos);
                QString::ConstIterator constIter = number.constBegin();
                bool found = true;
                // don't apply superscript to 1th, 2th and 3th
                const int numberLength(number.length());
                if (numberLength == 1 && (*constIter == QLatin1Char('1') || *constIter == QLatin1Char('2') || *constIter == QLatin1Char('3'))) {
                    found = false;
                }
                if (found) {
                    while (constIter != number.constEnd()) {
                        if (!constIter->isNumber()) {
                            found = false;
                            break;
                        }
                        ++constIter;
                    }
                }
                if (found && numberLength + i.value().length() == trimmedLenght) {
                    startPos = mCursor.selectionStart() + pos;
                    endPos = startPos - pos + trimmedLenght;
                    break;
                }
            }
        }
        ++i;
    }

    if (startPos != -1 && endPos != -1) {
        QTextCursor cursor(mCursor);
        cursor.setPosition(startPos);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);

        QTextCharFormat format;
        format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        cursor.mergeCharFormat(format);
    }
}

void AutoCorrection::addNonBreakingSpace()
{
    if (mAddNonBreakingSpace && isFrenchLanguage()) {
        const QTextBlock block = mCursor.block();
        const QString text = block.text();
        const QChar lastChar = text.at(mCursor.position() - 1 - block.position());

        if (lastChar == QLatin1Char(':') || lastChar == QLatin1Char(';') || lastChar == QLatin1Char('!') || lastChar == QLatin1Char('?')
            || lastChar == QLatin1Char('%')) {
            const int pos = mCursor.position() - 2 - block.position();
            if (pos >= 0) {
                const QChar previousChar = text.at(pos);
                if (previousChar.isSpace()) {
                    QTextCursor cursor(mCursor);
                    cursor.setPosition(pos);
                    cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
                    cursor.deleteChar();
                    mCursor.insertText(mNonBreakingSpace);
                }
            }
        } else {
            // °C (degrees)
            const int pos = mCursor.position() - 2 - block.position();
            if (pos >= 0) {
                const QChar previousChar = text.at(pos);

                if (lastChar == QLatin1Char('C') && previousChar == QChar(0x000B0)) {
                    const int pos = mCursor.position() - 3 - block.position();
                    if (pos >= 0) {
                        const QChar previousChar = text.at(pos);
                        if (previousChar.isSpace()) {
                            QTextCursor cursor(mCursor);
                            cursor.setPosition(pos);
                            cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
                            cursor.deleteChar();
                            mCursor.insertText(mNonBreakingSpace);
                        }
                    }
                }
            }
        }
    }
}

bool AutoCorrection::autoBoldUnderline()
{
    if (!mAutoBoldUnderline) {
        return false;
    }
    const QString trimmed = mWord.trimmed();

    if (trimmed.length() < 3) {
        return false;
    }

    const QChar trimmedFirstChar(trimmed.at(0));
    const QChar trimmedLastChar(trimmed.at(trimmed.length() - 1));
    const bool underline = (trimmedFirstChar == QLatin1Char('_') && trimmedLastChar == QLatin1Char('_'));
    const bool bold = (trimmedFirstChar == QLatin1Char('*') && trimmedLastChar == QLatin1Char('*'));
    const bool strikeOut = (trimmedFirstChar == QLatin1Char('-') && trimmedLastChar == QLatin1Char('-'));
    if (underline || bold || strikeOut) {
        const int startPos = mCursor.selectionStart();
        const QString replacement = trimmed.mid(1, trimmed.length() - 2);
        bool foundLetterNumber = false;

        QString::ConstIterator constIter = replacement.constBegin();
        while (constIter != replacement.constEnd()) {
            if (constIter->isLetterOrNumber()) {
                foundLetterNumber = true;
                break;
            }
            ++constIter;
        }

        // if no letter/number found, don't apply autocorrection like in OOo 2.x
        if (!foundLetterNumber) {
            return false;
        }
        mCursor.setPosition(startPos);
        mCursor.setPosition(startPos + trimmed.length(), QTextCursor::KeepAnchor);
        mCursor.insertText(replacement);
        mCursor.setPosition(startPos);
        mCursor.setPosition(startPos + replacement.length(), QTextCursor::KeepAnchor);

        QTextCharFormat format;
        format.setFontUnderline(underline ? true : mCursor.charFormat().fontUnderline());
        format.setFontWeight(bold ? QFont::Bold : mCursor.charFormat().fontWeight());
        format.setFontStrikeOut(strikeOut ? true : mCursor.charFormat().fontStrikeOut());
        mCursor.mergeCharFormat(format);

        // to avoid the selection being replaced by mWord
        mWord = mCursor.selectedText();
        return true;
    } else {
        return false;
    }

    return true;
}

QColor AutoCorrection::linkColor()
{
    if (mLinkColor.isValid()) {
        return mLinkColor;
    }
    mLinkColor = KColorScheme(QPalette::Active, KColorScheme::View).foreground(KColorScheme::LinkText).color();
    return mLinkColor;
}

bool AutoCorrection::autoFormatURLs()
{
    if (!mAutoFormatUrl) {
        return false;
    }

    const QString link = autoDetectURL(mWord);
    if (link.isNull()) {
        return false;
    }

    const QString trimmed = mWord.trimmed();
    const int startPos = mCursor.selectionStart();
    mCursor.setPosition(startPos);
    mCursor.setPosition(startPos + trimmed.length(), QTextCursor::KeepAnchor);

    QTextCharFormat format;
    format.setAnchorHref(link);
    format.setFontItalic(true);
    format.setAnchor(true);
    format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    format.setUnderlineColor(linkColor());
    format.setForeground(linkColor());
    mCursor.mergeCharFormat(format);

    mWord = mCursor.selectedText();
    return true;
}

QString AutoCorrection::autoDetectURL(const QString &_word) const
{
    QString word = _word;

    /* this method is ported from lib/kotext/KoAutoFormat.cpp KoAutoFormat::doAutoDetectUrl
     * from Calligra 1.x branch */
    // qCDebug(AUTOCORRECTION_LOG) <<"link:" << word;

    bool secure = false;
    int link_type = 0;
    int pos = word.indexOf(QLatin1String("http://"));
    int tmp_pos = word.indexOf(QLatin1String("https://"));

    if (tmp_pos != -1 && pos == -1) {
        secure = true;
    }

    if (tmp_pos < pos && tmp_pos != -1) {
        pos = tmp_pos;
    }

    tmp_pos = word.indexOf(QLatin1String("mailto:/"));
    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1) {
        pos = tmp_pos;
    }
    tmp_pos = word.indexOf(QLatin1String("ftp://"));
    const int secureftp = word.indexOf(QLatin1String("ftps://"));
    if (secureftp != -1 && tmp_pos == -1) {
        secure = true;
    }

    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1) {
        pos = tmp_pos;
    }
    tmp_pos = word.indexOf(QLatin1String("ftp."));

    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1) {
        pos = tmp_pos;
        link_type = 3;
    }
    tmp_pos = word.indexOf(QLatin1String("file:/"));
    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1) {
        pos = tmp_pos;
    }
    tmp_pos = word.indexOf(QLatin1String("news:"));
    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1) {
        pos = tmp_pos;
    }
    tmp_pos = word.indexOf(QLatin1String("www."));

    if ((tmp_pos < pos || pos == -1) && tmp_pos != -1 && word.indexOf(QLatin1Char('.'), tmp_pos + 4) != -1) {
        pos = tmp_pos;
        link_type = 2;
    }
    tmp_pos = word.indexOf(QLatin1Char('@'));
    if (pos == -1 && tmp_pos != -1) {
        pos = tmp_pos - 1;
        QChar c;

        while (pos >= 0) {
            c = word.at(pos);
            if (c.isPunct() && c != QLatin1Char('.') && c != QLatin1Char('_')) {
                break;
            } else {
                --pos;
            }
        }
        if (pos == tmp_pos - 1) { // not a valid address
            pos = -1;
        } else {
            ++pos;
        }
        link_type = 1;
    }

    if (pos != -1) {
        // A URL inside e.g. quotes (like "http://www.calligra.org" with the quotes) shouldn't include the quote in the URL.
        while (!word.at(word.length() - 1).isLetter() && !word.at(word.length() - 1).isDigit() && word.at(word.length() - 1) != QLatin1Char('/')) {
            word.chop(1);
        }
        word.remove(0, pos);
        QString newWord = word;

        switch (link_type) {
        case 1:
            newWord = QLatin1String("mailto:") + word;
            break;
        case 2:
            newWord = (secure ? QStringLiteral("https://") : QStringLiteral("http://")) + word;
            break;
        case 3:
            newWord = (secure ? QStringLiteral("ftps://") : QStringLiteral("ftp://")) + word;
            break;
        }
        // qCDebug(AUTOCORRECTION_LOG) <<"newWord:" << newWord;
        return newWord;
    }

    return {};
}

void AutoCorrection::fixTwoUppercaseChars()
{
    if (!mFixTwoUppercaseChars) {
        return;
    }
    if (mWord.length() <= 2) {
        return;
    }

    if (mTwoUpperLetterExceptions.contains(mWord.trimmed())) {
        return;
    }

    const QChar firstChar = mWord.at(0);
    const QChar secondChar = mWord.at(1);

    if (secondChar.isUpper() && firstChar.isUpper()) {
        const QChar thirdChar = mWord.at(2);

        if (thirdChar.isLower()) {
            mWord.replace(1, 1, secondChar.toLower());
        }
    }
}

// Return true if we can add space
bool AutoCorrection::singleSpaces()
{
    if (!mSingleSpaces) {
        return true;
    }
    if (!mCursor.atBlockStart()) {
        // then when the prev char is also a space, don't insert one.
        const QTextBlock block = mCursor.block();
        const QString text = block.text();
        if (text.at(mCursor.position() - 1 - block.position()) == QLatin1Char(' ')) {
            return false;
        }
    }
    return true;
}

void AutoCorrection::capitalizeWeekDays()
{
    if (!mCapitalizeWeekDays) {
        return;
    }

    const QString trimmed = mWord.trimmed();
    for (const QString &name : std::as_const(mCacheNameOfDays)) {
        if (trimmed == name) {
            const int pos = mWord.indexOf(name);
            mWord.replace(pos, 1, name.at(0).toUpper());
            return;
        }
    }
}

bool AutoCorrection::excludeToUppercase(const QString &word) const
{
    if (word.startsWith(QLatin1String("http://")) || word.startsWith(QLatin1String("www.")) || word.startsWith(QLatin1String("mailto:"))
        || word.startsWith(QLatin1String("ftp://")) || word.startsWith(QLatin1String("https://")) || word.startsWith(QLatin1String("ftps://"))) {
        return true;
    }
    return false;
}

void AutoCorrection::setNonBreakingSpace(QChar nonBreakingSpace)
{
    mNonBreakingSpace = nonBreakingSpace;
}

void AutoCorrection::uppercaseFirstCharOfSentence()
{
    if (!mUppercaseFirstCharOfSentence) {
        return;
    }

    int startPos = mCursor.selectionStart();
    QTextBlock block = mCursor.block();

    mCursor.setPosition(block.position());
    mCursor.setPosition(startPos, QTextCursor::KeepAnchor);

    int position = mCursor.selectionEnd();

    const QString text = mCursor.selectedText();

    if (text.isEmpty()) { // start of a paragraph
        if (!excludeToUppercase(mWord)) {
            mWord.replace(0, 1, mWord.at(0).toUpper());
        }
    } else {
        QString::ConstIterator constIter = text.constEnd();
        --constIter;

        while (constIter != text.constBegin()) {
            while (constIter != text.begin() && constIter->isSpace()) {
                --constIter;
                --position;
            }

            if (constIter != text.constBegin() && (*constIter == QLatin1Char('.') || *constIter == QLatin1Char('!') || *constIter == QLatin1Char('?'))) {
                constIter--;
                while (constIter != text.constBegin() && !(constIter->isLetter())) {
                    --position;
                    --constIter;
                }
                selectPreviousWord(mCursor, --position);
                const QString prevWord = mCursor.selectedText();

                // search for exception
                if (mUpperCaseExceptions.contains(prevWord.trimmed())) {
                    break;
                }
                if (excludeToUppercase(mWord)) {
                    break;
                }

                mWord.replace(0, 1, mWord.at(0).toUpper());
                break;
            } else {
                break;
            }
        }
    }

    mCursor.setPosition(startPos);
    mCursor.setPosition(startPos + mWord.length(), QTextCursor::KeepAnchor);
}

bool AutoCorrection::autoFractions()
{
    if (!mAutoFractions) {
        return false;
    }

    const QString trimmed = mWord.trimmed();
    if (trimmed.length() > 3) {
        const QChar x = trimmed.at(3);
        const uchar xunicode = x.unicode();
        if (!(xunicode == '.' || xunicode == ',' || xunicode == '?' || xunicode == '!' || xunicode == ':' || xunicode == ';')) {
            return false;
        }
    } else if (trimmed.length() < 3) {
        return false;
    }

    if (trimmed.startsWith(QLatin1String("1/2"))) {
        mWord.replace(0, 3, QStringLiteral("½"));
    } else if (trimmed.startsWith(QLatin1String("1/4"))) {
        mWord.replace(0, 3, QStringLiteral("¼"));
    } else if (trimmed.startsWith(QLatin1String("3/4"))) {
        mWord.replace(0, 3, QStringLiteral("¾"));
    } else {
        return false;
    }

    return true;
}

int AutoCorrection::replaceEmoji(QTextDocument &document, int position)
{
    if (!mEnabled) {
        return position;
    }
    if (!mAdvancedAutocorrect) { }
    if (mAutocorrectEntries.isEmpty()) {
        return position;
    }
    const auto colon = QLatin1Char(':');
    if (document.characterAt(position) != colon) {
        qInfo() << Q_FUNC_INFO << "not colon" << document.characterAt(position);
        return position;
    }
    mCursor = QTextCursor(&document);
    mCursor.setPosition(position + 1); // include the colon in selection
    int prev = -1;
    QTextBlock block = mCursor.block();
    const auto string = block.text();
    qInfo() << Q_FUNC_INFO << "check:" << string;
    for (int pos = string.length() - 2; pos >= 0; --pos) {
        if (string.at(pos) == colon) {
            prev = block.position() + pos;
            break;
        }
    }
    if (prev < 0) {
        qInfo() << Q_FUNC_INFO << "no emoji";
        return position;
    }
    mCursor.setPosition(prev, QTextCursor::KeepAnchor);
    mWord = mCursor.selectedText();
    if (mWord.length() < 3) {
        qInfo() << Q_FUNC_INFO << prev << "too short" << mWord;
        return position;
    }

    QString actualWord = mWord;
    int colons = actualWord.count(colon);
    if (colons != 2) {
        qInfo() << Q_FUNC_INFO << "to few colons" << mWord;
        return position;
    }
    auto replacement = mAutocorrectEntries.value(mWord);
    qCDebug(AUTOCORRECTION_LOG) << "search for key:" << mWord << "replace with:" << replacement;
    if (replacement.isEmpty()) {
        return position;
    }
    mCursor.insertText(replacement);
    mCursor.setPosition(mCursor.selectionStart() + replacement.length());
    return mCursor.position();
}

int AutoCorrection::advancedAutocorrect()
{
    if (!mAdvancedAutocorrect) {
        return -1;
    }
    if (mAutocorrectEntries.isEmpty()) {
        return -1;
    }

    const QString trimmedWord = mWord.trimmed();
    if (trimmedWord.isEmpty()) {
        return -1;
    }

    QString actualWord = trimmedWord;

    const int actualWordLength(actualWord.length());
    if (actualWordLength < mMinFindStringLenght) {
        return -1;
    }
    if (actualWordLength > mMaxFindStringLenght) {
        return -1;
    }

    const int startPos = mCursor.selectionStart();
    const int length = mWord.length();
    // If the last char is punctuation, drop it for now
    bool hasPunctuation = false;
    const QChar lastChar = actualWord.at(actualWord.length() - 1);
    const ushort charUnicode = lastChar.unicode();
    if (charUnicode == '.' || charUnicode == ',' || charUnicode == '?' || charUnicode == '!' || charUnicode == ':' || charUnicode == ';') {
        hasPunctuation = true;
        actualWord.chop(1);
    }

    QString actualWordWithFirstUpperCase = actualWord;
    actualWordWithFirstUpperCase[0] = actualWordWithFirstUpperCase[0].toUpper();
    QHashIterator<QString, QString> i(mAutocorrectEntries);
    while (i.hasNext()) {
        i.next();
        if (i.key().length() > actualWordLength) {
            continue;
        }
        if (actualWord.endsWith(i.key()) || actualWord.endsWith(i.key(), Qt::CaseInsensitive) || actualWordWithFirstUpperCase.endsWith(i.key())) {
            int pos = mWord.lastIndexOf(i.key());
            if (pos == -1) {
                pos = actualWord.toLower().lastIndexOf(i.key());
            }
            if (pos == -1) {
                pos = actualWordWithFirstUpperCase.lastIndexOf(i.key());
            }
            if (pos == -1) {
                continue;
            }
            QString replacement = i.value();

            // Keep capitalized words capitalized.
            // (Necessary to make sure the first letters match???)
            const QChar actualWordFirstChar = actualWord.at(pos);
            const QChar replacementFirstChar = replacement[0];
            if (actualWordFirstChar.isUpper() && replacementFirstChar.isLower()) {
                replacement[0] = replacementFirstChar.toUpper();
            } else if (actualWordFirstChar.isLower() && replacementFirstChar.isUpper()) {
                replacement[0] = replacementFirstChar.toLower();
            }

            // If a punctuation mark was on the end originally, add it back on
            if (hasPunctuation) {
                replacement.append(lastChar);
            }

            mWord.replace(pos, pos + trimmedWord.length(), replacement);

            // We do replacement here, since the length of new word might be different from length of
            // the old world. Length difference might affect other type of autocorrection
            mCursor.setPosition(startPos);
            mCursor.setPosition(startPos + length, QTextCursor::KeepAnchor);
            mCursor.insertText(mWord);
            mCursor.setPosition(startPos); // also restore the selection
            const int newPosition = startPos + mWord.length();
            mCursor.setPosition(newPosition, QTextCursor::KeepAnchor);
            return newPosition;
        }
    }
    return -1;
}

void AutoCorrection::replaceTypographicQuotes()
{
    /* this method is ported from lib/kotext/KoAutoFormat.cpp KoAutoFormat::doTypographicQuotes
     * from Calligra 1.x branch */

    if (!(mReplaceDoubleQuotes && mWord.contains(QLatin1Char('"'))) && !(mReplaceSingleQuotes && mWord.contains(QLatin1Char('\'')))) {
        return;
    }

    const bool addNonBreakingSpace = (isFrenchLanguage() && isAddNonBreakingSpace());

    // Need to determine if we want a starting or ending quote.
    // we use a starting quote in three cases:
    //  1. if the previous character is a space
    //  2. if the previous character is some kind of opening punctuation (e.g., "(", "[", or "{")
    //     a. and the character before that is not an opening quote (so that we get quotations of single characters
    //        right)
    //  3. if the previous character is an opening quote (so that we get nested quotations right)
    //     a. and the character before that is not an opening quote (so that we get quotations of single characters
    //         right)
    //     b. and the previous quote of a different kind (so that we get empty quotations right)

    bool ending = true;
    for (int i = mWord.length(); i > 1; --i) {
        const QChar c = mWord.at(i - 1);
        if (c == QLatin1Char('"') || c == QLatin1Char('\'')) {
            const bool doubleQuotes = (c == QLatin1Char('"'));
            if (i > 2) {
                QChar::Category c1 = mWord.at(i - 1).category();

                // case 1 and 2
                if (c1 == QChar::Separator_Space || c1 == QChar::Separator_Line || c1 == QChar::Separator_Paragraph || c1 == QChar::Punctuation_Open
                    || c1 == QChar::Other_Control) {
                    ending = false;
                }

                // case 3
                if (c1 == QChar::Punctuation_InitialQuote) {
                    QChar openingQuote;

                    if (doubleQuotes) {
                        openingQuote = mTypographicDoubleQuotes.begin;
                    } else {
                        openingQuote = mTypographicSingleQuotes.begin;
                    }

                    // case 3b
                    if (mWord.at(i - 1) != openingQuote) {
                        ending = false;
                    }
                }
            }
            // case 2a and 3a
            if (i > 3 && !ending) {
                const QChar::Category c2 = (mWord.at(i - 2)).category();
                ending = (c2 == QChar::Punctuation_InitialQuote);
            }

            if (doubleQuotes && mReplaceDoubleQuotes) {
                if (ending) {
                    if (addNonBreakingSpace) {
                        mWord.replace(i - 1, 2, QString(mNonBreakingSpace + mTypographicDoubleQuotes.end));
                    } else {
                        mWord[i - 1] = mTypographicDoubleQuotes.end;
                    }
                } else {
                    if (addNonBreakingSpace) {
                        mWord.replace(i - 1, 2, QString(mNonBreakingSpace + mTypographicDoubleQuotes.begin));
                    } else {
                        mWord[i - 1] = mTypographicDoubleQuotes.begin;
                    }
                }
            } else if (mReplaceSingleQuotes) {
                if (ending) {
                    if (addNonBreakingSpace) {
                        mWord.replace(i - 1, 2, QString(mNonBreakingSpace + mTypographicSingleQuotes.end));
                    } else {
                        mWord[i - 1] = mTypographicSingleQuotes.end;
                    }
                } else {
                    if (addNonBreakingSpace) {
                        mWord.replace(i - 1, 2, QString(mNonBreakingSpace + mTypographicSingleQuotes.begin));
                    } else {
                        mWord[i - 1] = mTypographicSingleQuotes.begin;
                    }
                }
            }
        }
    }

    // first character
    if (mWord.at(0) == QLatin1Char('"') && mReplaceDoubleQuotes) {
        mWord[0] = mTypographicDoubleQuotes.begin;
        if (addNonBreakingSpace) {
            mWord.insert(1, mNonBreakingSpace);
        }
    } else if (mWord.at(0) == QLatin1Char('\'') && mReplaceSingleQuotes) {
        mWord[0] = mTypographicSingleQuotes.begin;
        if (addNonBreakingSpace) {
            mWord.insert(1, mNonBreakingSpace);
        }
    }
}

void AutoCorrection::readAutoCorrectionXmlFile(bool forceGlobal)
{
    mMaxFindStringLenght = 0;
    mMinFindStringLenght = 0;
    mUpperCaseExceptions.clear();
    mAutocorrectEntries.clear();
    mTwoUpperLetterExceptions.clear();
    mSuperScriptEntries.clear();
    if (mAutoCorrectLang.isEmpty()) {
        return;
    }
    bool localExists = false;
    QString localFileName;
    // Look at local file:
    if (!forceGlobal) {
        localFileName =
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("calligra/autocorrect/") + mAutoCorrectLang + QLatin1String(".xml"));
        localExists = QFile::exists(localFileName);
        if (localExists) {
            loadLocalFileName(localFileName);
        }
    }
    if (forceGlobal || !localExists) {
        QString fname;
        fname = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("calligra/autocorrect/") + mAutoCorrectLang + QLatin1String(".xml"));
        loadGlobalFileName(fname, forceGlobal);
    }
}

void AutoCorrection::loadGlobalFileName(const QString &fname, bool forceGlobal)
{
    qInfo() << Q_FUNC_INFO << mAutoCorrectLang << fname;
    if (fname.isEmpty()) {
        mTypographicSingleQuotes = typographicDefaultSingleQuotes();
        mTypographicDoubleQuotes = typographicDefaultDoubleQuotes();
    } else {
        ImportKMailAutocorrection import;
        QString messageError;
        if (import.import(fname, messageError, ImportAbstractAutocorrection::All)) {
            mUpperCaseExceptions = import.upperCaseExceptions();
            mTwoUpperLetterExceptions = import.twoUpperLetterExceptions();
            mAutocorrectEntries = import.autocorrectEntries();
            mTypographicSingleQuotes = import.typographicSingleQuotes();
            mTypographicDoubleQuotes = import.typographicDoubleQuotes();
            mSuperScriptEntries = import.superScriptEntries();
            if (forceGlobal) {
                mTypographicSingleQuotes = typographicDefaultSingleQuotes();
                mTypographicDoubleQuotes = typographicDefaultDoubleQuotes();
            }
            mMaxFindStringLenght = std::max(mMaxFindStringLenght, import.maxFindStringLenght());
            mMinFindStringLenght = std::max(mMinFindStringLenght, import.minFindStringLenght());
        }
    }
}

void AutoCorrection::loadLocalFileName(const QString &localFileName)
{
    qInfo() << Q_FUNC_INFO << mAutoCorrectLang << localFileName;
    ImportKMailAutocorrection import;
    QString messageError;
    if (import.import(localFileName, messageError, ImportAbstractAutocorrection::All)) {
        mUpperCaseExceptions = import.upperCaseExceptions();
        mTwoUpperLetterExceptions = import.twoUpperLetterExceptions();
        mAutocorrectEntries = import.autocorrectEntries();
        mTypographicSingleQuotes = import.typographicSingleQuotes();
        mTypographicDoubleQuotes = import.typographicDoubleQuotes();
        // Don't import it in local Why?
        mSuperScriptEntries = import.superScriptEntries();
    }
    mMaxFindStringLenght = std::max(mMaxFindStringLenght, import.maxFindStringLenght());
    mMinFindStringLenght = std::max(mMinFindStringLenght, import.minFindStringLenght());
}

void AutoCorrection::writeAutoCorrectionXmlFile()
{
    if (mAutoCorrectLang.isEmpty()) {
        qCDebug(AUTOCORRECTION_LOG) << "language not set";
        return;
    }
    const QString fname = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/calligra/autocorrect/") + mAutoCorrectLang
        + QLatin1String(".xml");
    writeAutoCorrectionXmlFile(fname);
}

void AutoCorrection::writeAutoCorrectionXmlFile(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QDir().mkpath(fileInfo.absolutePath());
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCDebug(AUTOCORRECTION_LOG) << "Failed to open:" << fileName;
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

QString AutoCorrection::language() const
{
    return mAutoCorrectLang;
}

AutoCorrection::TypographicQuotes AutoCorrection::typographicSingleQuotes() const
{
    return mTypographicSingleQuotes;
}

AutoCorrection::TypographicQuotes AutoCorrection::typographicDoubleQuotes() const
{
    return mTypographicDoubleQuotes;
}

void AutoCorrection::setLanguage(const QString &lang, bool forceGlobal)
{
    qInfo() << Q_FUNC_INFO << lang << forceGlobal;
    if (mAutoCorrectLang != lang || forceGlobal) {
        mAutoCorrectLang = lang;
        // Re-read xml file
        readAutoCorrectionXmlFile(forceGlobal);
    }
}

void AutoCorrection::setEnabledAutoCorrection(bool b)
{
    mEnabled = b;
    qInfo() << Q_FUNC_INFO << mEnabled;
}

void AutoCorrection::setUppercaseFirstCharOfSentence(bool b)
{
    mUppercaseFirstCharOfSentence = b;
}

void AutoCorrection::setFixTwoUppercaseChars(bool b)
{
    mFixTwoUppercaseChars = b;
}

void AutoCorrection::setSingleSpaces(bool b)
{
    mSingleSpaces = b;
}

void AutoCorrection::setAutoFractions(bool b)
{
    mAutoFractions = b;
}

void AutoCorrection::setCapitalizeWeekDays(bool b)
{
    mCapitalizeWeekDays = b;
}

void AutoCorrection::setReplaceDoubleQuotes(bool b)
{
    mReplaceDoubleQuotes = b;
}

void AutoCorrection::setReplaceSingleQuotes(bool b)
{
    mReplaceSingleQuotes = b;
}

void AutoCorrection::setAdvancedAutocorrect(bool b)
{
    mAdvancedAutocorrect = b;
}

void AutoCorrection::setTypographicSingleQuotes(AutoCorrection::TypographicQuotes singleQuote)
{
    mTypographicSingleQuotes = singleQuote;
}

void AutoCorrection::setTypographicDoubleQuotes(AutoCorrection::TypographicQuotes doubleQuote)
{
    mTypographicDoubleQuotes = doubleQuote;
}

bool AutoCorrection::isFrenchLanguage() const
{
    return mAutoCorrectLang == QLatin1String("FR_fr") || mAutoCorrectLang == QLatin1String("fr");
}
