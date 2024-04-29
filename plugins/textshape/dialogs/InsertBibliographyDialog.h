/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef INSERTBIBLIOGRAPHYDIALOG_H
#define INSERTBIBLIOGRAPHYDIALOG_H

#include "ui_InsertBibliographyDialog.h"

#include <QDialog>
#include <QTextBlock>

#include <KoTextEditor.h>

class KoBibliographyInfo;
class QListWidgetItem;

class InsertBibliographyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InsertBibliographyDialog(KoTextEditor *editor, QWidget *parent = nullptr);
    QString bibliographyType();

public Q_SLOTS:
    void insert();
    void updateFields();
    void addField();
    void removeField();
    void addSpan();
    void insertTabStop();
    void removeTabStop();
    void spanChanged(QListWidgetItem *);

private:
    Ui::InsertBibliographyDialog dialog;
    bool m_blockSignals;
    KoTextEditor *m_editor;
    KoBibliographyInfo *m_bibInfo;
};

#endif // INSERTBIBLIOGRAPHYDIALOG_H
