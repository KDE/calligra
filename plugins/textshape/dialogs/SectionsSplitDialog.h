/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SECTIONSSPLITDIALOG_H
#define SECTIONSSPLITDIALOG_H

#include <KoDialog.h>

class KoTextEditor;
class KoSection;
class KoSectionModel;

#include <ui_SectionsSplitDialog.h>
class SectionsSplitDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit SectionsSplitDialog(QWidget *parent, KoTextEditor *editor);

private Q_SLOTS:
    void beforeListSelection();
    void afterListSelection();

    void slotOkClicked();

private:
    Ui::SectionsSplitDialog m_widget;
    KoTextEditor *m_editor;
};

#endif // SECTIONSSPLITDIALOG_H
