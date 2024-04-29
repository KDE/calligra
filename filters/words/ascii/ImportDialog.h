/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _IMPORT_DIALOG_H
#define _IMPORT_DIALOG_H

#include <QButtonGroup>
#include <QWidget>

#include <KoDialog.h>
#include <ui_ImportDialogUI.h>

class AsciiImportDialog : public KoDialog
{
    Q_OBJECT
public:
    enum ParagraphStrategy {
        EndOfLine,
        EndOfSentence,
        EmptyLine,
    };
    explicit AsciiImportDialog(QWidget *parent = nullptr);
    ~AsciiImportDialog() override;
    ParagraphStrategy getParagraphStrategy() const;

private:
    Ui::ImportDialogUI m_ui;
    QButtonGroup m_radioGroup;
};

#endif /* _IMPORT_DIALOG_H */
