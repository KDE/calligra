/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
