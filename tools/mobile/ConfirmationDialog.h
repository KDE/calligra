/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CONFIRMATIONDIALOG_H
#define CONFIRMATIONDIALOG_H

#include <QDialog>

class QPushButton;

//! Dialog with "Document is modified, do you want to save it before closing?" message
class ConfirmationDialog : public QDialog
{
    Q_OBJECT
public:
    ConfirmationDialog(QWidget * parent = 0);
    ~ConfirmationDialog();

private slots:
    void buttonClicked();

private:
    QPushButton *m_yes;
    QPushButton *m_no;
    QPushButton *m_cancel;
};

#endif
