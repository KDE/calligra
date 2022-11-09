/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class LineEditWithAutocorrectionTest : public QObject
{
    Q_OBJECT
public:
    LineEditWithAutocorrectionTest();

private Q_SLOTS:
    void shouldNotAutocorrectWhenDisabled();
    void shouldReplaceWordWhenExactText();
    void shouldNotReplaceWordWhenInexactText();
    void shouldNotAddTwoSpace();
};
