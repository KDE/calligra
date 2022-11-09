/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KSharedConfig>
#include <QObject>
class AutoCorrectionTest : public QObject
{
    Q_OBJECT
public:
    AutoCorrectionTest();
    ~AutoCorrectionTest() override;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldRestoreValue();
    void shouldUpperCaseFirstCharOfSentence();
    void shouldFixTwoUpperCaseChars();
    void shouldReplaceSingleQuote();
    void shouldReplaceDoubleQuote();
    void shouldNotReplaceUppercaseLetter();
    void shouldReplaceToTextFormat();
    void shouldReplaceAutoFraction();
    void shouldNotAddSpaceWhenWeAlreadyHaveASpace();
    void shouldAutocorrectWord();
    void shouldNotUpperCaseFirstCharOfSentence();

    void shouldAutocorrectMultiWord_data();
    void shouldAutocorrectMultiWord();

    void shouldAddNonBreakingSpace_data();
    void shouldAddNonBreakingSpace();

    void shouldReplaceWithMultiOption_data();
    void shouldReplaceWithMultiOption();

    void shouldAddNonBreakingSpaceBeforeAfterQuote();

    void shouldLoadSaveAutocorrection_data();
    void shouldLoadSaveAutocorrection();

private:
    KSharedConfig::Ptr mConfig;
};
