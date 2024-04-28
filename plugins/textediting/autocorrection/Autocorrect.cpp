/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Autocorrect.h"
#include "AutocorrectDebug.h"

#include <autocorrectiondialog.h>

#include <QAction>
#include <QFile>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

Autocorrect::Autocorrect()
    : AutoCorrection()
{
    /* setup actions for this plugin */
    QAction *configureAction = new QAction(i18n("Configure &Autocorrection..."), this);
    connect(configureAction, &QAction::triggered, this, &Autocorrect::configureAutocorrect);
    addAction("configure_autocorrection", configureAction);

    auto a = new QAction(i18n("Enable Autocorrection"), this);
    a->setCheckable(true);
    connect(a, &QAction::toggled, this, &Autocorrect::setEnabled);
    addAction("enable_autocorrection", a);

    m_autoFormatBulletList = false;
    m_trimParagraphs = true;
    m_autoNumbering = false;

    readConfigurationInternal();
}

Autocorrect::~Autocorrect()
{
    writeConfigurationInternal();
}

void Autocorrect::finishedWord(QTextDocument *document, int cursorPosition)
{
    auto mCursor = QTextCursor(document);
    mCursor.setPosition(cursorPosition);
    selectPreviousWord(mCursor, cursorPosition);
    debugAutocorrect << cursorPosition << mCursor.selectedText() << ':' << document->toPlainText();
    if (!isEnabledAutoCorrection())
        return;

    auto cursor = QTextCursor(document);
    // If you enter a word (no space after) and delete characters,
    // we get called after the 2nd delete.
    // This should be caught elsewere but for now
    // we check for a valid cursorPosiotion here,
    cursor.setPosition(cursorPosition);
    if (cursor.position() == 0) {
        warnAutocorrect << "Invalid cursor position" << cursorPosition;
        return;
    }
    selectWord(cursor, cursorPosition);
    auto m_word = cursor.selectedText();
    if (m_word.isEmpty()) {
        return;
    }
    bool supportsHtmlMode = true;
    if (!autocorrect(supportsHtmlMode, *document, cursorPosition)) {
        return;
    }
    bool done = autoNumbering();
    if (!done) {
        done = autoFormatBulletList();
    }
    if (!done) {
        done = autoTrimParagraph();
    }
    Q_UNUSED(done);
}

void Autocorrect::finishedParagraph(QTextDocument *document, int cursorPosition)
{
    Q_UNUSED(document);
    Q_UNUSED(cursorPosition);
}

void Autocorrect::startingSimpleEdit(QTextDocument *document, int cursorPosition)
{
    Q_UNUSED(document);
    Q_UNUSED(cursorPosition);
}

int Autocorrect::characterInserted(QTextDocument *document, int cursorPosition)
{
    auto mCursor = QTextCursor(document);
    mCursor.setPosition(cursorPosition);
    selectPreviousWord(mCursor, cursorPosition);
    int pos = replaceEmoji(*document, cursorPosition);
    debugAutocorrect << pos;
    return pos;
}

void Autocorrect::setEnabled(bool enable)
{
    setEnabledAutoCorrection(enable);
}

void Autocorrect::configureAutocorrect()
{
    AutoCorrectionDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto a = actions().value(QStringLiteral("enable_autocorrection"));
        if (a) {
            a->setChecked(isEnabledAutoCorrection());
        }
    }
}

bool Autocorrect::autoNumbering()
{
    if (!m_autoNumbering) {
        return false;
    }
    // TODO
    return false;
}

bool Autocorrect::autoFormatBulletList()
{
    if (!m_autoFormatBulletList) {
        return false;
    }
    // TODO
    return false;
}

bool Autocorrect::autoTrimParagraph()
{
    if (!m_trimParagraphs) {
        return false;
    }
    // TODO
    return false;
}

void Autocorrect::readConfiguration()
{
    readConfigurationInternal();
}

void Autocorrect::readConfigurationInternal()
{
    KConfig cfg("calligrarc");
    KConfigGroup interface = cfg.group("Autocorrect");

    setEnabledAutoCorrection(interface.readEntry("enabled", isEnabledAutoCorrection()));
    setUppercaseFirstCharOfSentence(interface.readEntry("UppercaseFirstCharOfSentence", isUppercaseFirstCharOfSentence()));
    setAutoBoldUnderline(interface.readEntry("AutoBoldUnderline", isAutoBoldUnderline()));
    setAutoFormatUrl(interface.readEntry("AutoFormatURLs", false /*isAutoFormatURL()*/));
    setUppercaseFirstCharOfSentence(interface.readEntry("UppercaseFirstCharOfSentence", isUppercaseFirstCharOfSentence()));
    setFixTwoUppercaseChars(interface.readEntry("FixTwoUppercaseChars", isFixTwoUppercaseChars()));
    setSingleSpaces(interface.readEntry("SingleSpaces", isSingleSpaces()));
    setAutoFractions(interface.readEntry("AutoFractions", isAutoFractions()));
    setCapitalizeWeekDays(interface.readEntry("CapitalizeWeekDays", isCapitalizeWeekDays()));
    setAdvancedAutocorrect(interface.readEntry("AdvancedAutocorrect", isAdvancedAutocorrect()));
    setReplaceDoubleQuotes(interface.readEntry("ReplaceDoubleQuotes", isReplaceDoubleQuotes()));
    setReplaceSingleQuotes(interface.readEntry("ReplaceSingleQuotes", isReplaceSingleQuotes()));
    setSuperScript(interface.readEntry("SuperscriptAppendix", isSuperScript()));

    // mAddNonBreakingSpace = PimCommon::PimCommonSettings::self()->addNonBreakingSpaceInFrench();

    m_autoFormatBulletList = interface.readEntry("AutoFormatBulletList", m_autoFormatBulletList);
    m_autoNumbering = interface.readEntry("AutoNumbering", m_autoNumbering);
    m_trimParagraphs = interface.readEntry("TrimParagraphs", m_trimParagraphs);

    setLanguage(interface.readEntry("formatLanguage", QStringLiteral("en_US")));

    auto a = actions().value(QStringLiteral("enable_autocorrection"));
    if (a) {
        a->setChecked(isEnabledAutoCorrection());
    }
}

void Autocorrect::writeConfiguration()
{
    writeConfigurationInternal();
}

void Autocorrect::writeConfigurationInternal()
{
    KConfig cfg("calligrarc");
    KConfigGroup interface = cfg.group("Autocorrect");
    interface.writeEntry("enabled", isEnabledAutoCorrection());
    interface.writeEntry("UppercaseFirstCharOfSentence", isUppercaseFirstCharOfSentence());
    interface.writeEntry("FixTwoUppercaseChars", isFixTwoUppercaseChars());
    interface.writeEntry("AutoFormatURLs", isAutoFormatUrl());
    interface.writeEntry("SingleSpaces", isSingleSpaces());
    interface.writeEntry("TrimParagraphs", m_trimParagraphs);
    interface.writeEntry("AutoBoldUnderline", isAutoBoldUnderline());
    interface.writeEntry("AutoFractions", isAutoFractions());
    interface.writeEntry("AutoNumbering", m_autoNumbering);
    interface.writeEntry("SuperscriptAppendix", isSuperScript());
    interface.writeEntry("CapitalizeWeekDays", isCapitalizeWeekDays());
    interface.writeEntry("AutoFormatBulletList", m_autoFormatBulletList);
    interface.writeEntry("AdvancedAutocorrect", isAdvancedAutocorrect());

    interface.writeEntry("ReplaceDoubleQuotes", isReplaceDoubleQuotes());
    interface.writeEntry("ReplaceSingleQuotes", isReplaceSingleQuotes());

    interface.writeEntry("formatLanguage", language());
    writeAutoCorrectionXmlFile();
}
