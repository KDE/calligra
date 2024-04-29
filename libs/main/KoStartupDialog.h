/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSTARTUPDIALOG_H
#define KOSTARTUPDIALOG_H

#include <QDialog>

/**
 * @brief The KoStartupDialog class shows the file selectioed, custom document
 * widgets and template lists. A bit like it was in KOffice 1.4...
 */
class KoStartupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KoStartupDialog(QWidget *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
};

#endif // KOSTARTUPDIALOG_H
