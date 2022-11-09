/*
  SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

class ImportKMailAutocorrectionTest : public QObject
{
    Q_OBJECT
public:
    explicit ImportKMailAutocorrectionTest(QObject *parent = nullptr);
    ~ImportKMailAutocorrectionTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldLoadFile();
};
