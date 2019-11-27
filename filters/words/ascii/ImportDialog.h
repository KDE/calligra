/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _IMPORT_DIALOG_H
#define _IMPORT_DIALOG_H

#include <QWidget>
#include <QButtonGroup>

#include <KoDialog.h>
#include <ui_ImportDialogUI.h>

class AsciiImportDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit AsciiImportDialog(const QString &encoding, QWidget *parent = 0);
    ~AsciiImportDialog() override;
    QTextCodec *getCodec() const;
    int getParagraphStrategy() const;

private:
    Ui::ImportDialogUI m_ui;
    QButtonGroup m_radioGroup;
};

#endif /* _IMPORT_DIALOG_H */
