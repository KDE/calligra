/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SECTIONFORMATDIALOG_H
#define SECTIONFORMATDIALOG_H

#include <KoDialog.h>

class KoTextEditor;
class KoSection;
class KoSectionModel;

#include <ui_SectionFormatDialog.h>
class SectionFormatDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit SectionFormatDialog(QWidget *parent, KoTextEditor *editor);

private Q_SLOTS:
    void sectionSelected(const QModelIndex &idx);
    void sectionNameChanged();
    void updateTreeState();

private:
    class ProxyModel;
    class SectionNameValidator;

    Ui::SectionFormatDialog m_widget;
    KoTextEditor *m_editor;
    QModelIndex m_curIdx;
    KoSectionModel *m_sectionModel;

    KoSection *sectionFromModel(const QModelIndex &idx);
};

#endif // SECTIONFORMATDIALOG_H
