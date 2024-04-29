/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CITATIONINSERTIONDIALOG_H
#define CITATIONINSERTIONDIALOG_H

#include "ui_CitationInsertionDialog.h"
#include <KoTextEditor.h>
#include <QTextBlock>

class KoInlineCite;

class CitationInsertionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CitationInsertionDialog(KoTextEditor *editor, QWidget *parent = nullptr);
    KoInlineCite *toCite(); // returns cite with values filled in form
    void fillValuesFrom(KoInlineCite *cite); // fills form with values in cite

public Q_SLOTS:
    void insert();
    void selectionChangedFromExistingCites();

private:
    Ui::CitationInsertionDialog dialog;
    bool m_blockSignals;
    KoTextEditor *m_editor;
    QMap<QString, KoInlineCite *> m_cites;
};

#endif // CITATIONBIBLIOGRAPHYDIALOG_H
