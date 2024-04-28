/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QListWidget>

class AutoCorrectionListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AutoCorrectionListWidget(QWidget *parent = nullptr);
    ~AutoCorrectionListWidget() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void deleteSelectedItems();
};
