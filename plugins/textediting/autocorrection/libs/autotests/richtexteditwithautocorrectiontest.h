/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <QObject>

class RichTextEditWithAutoCorrectionTest : public QObject
{
    Q_OBJECT
public:
    RichTextEditWithAutoCorrectionTest();
private Q_SLOTS:
    void shouldNotAutocorrectWhenDisabled();
    void shouldReplaceWordWhenExactText();
    void shouldNotReplaceWordWhenInexactText();
    void shouldReplaceWhenPressEnter();
    void shouldReplaceWhenPressReturn();
};
