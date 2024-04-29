/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2014
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOFILEDIALOGTESTER_H
#define KOFILEDIALOGTESTER_H

#include <QWidget>

namespace Ui
{
class KoFileDialogTester;
}

class KoFileDialogTester : public QWidget
{
    Q_OBJECT

public:
    explicit KoFileDialogTester(QWidget *parent = nullptr);
    ~KoFileDialogTester() override;

private Q_SLOTS:

    void testOpenFile();
    void testOpenFiles();
    void testOpenDirectory();
    void testImportFile();
    void testImportFiles();
    void testImportDirectory();
    void testSaveFile();

private:
    Ui::KoFileDialogTester *ui;

    QStringList m_nameFilters;
    QStringList m_mimeFilter;
};

#endif // KOFILEDIALOGTESTER_H
