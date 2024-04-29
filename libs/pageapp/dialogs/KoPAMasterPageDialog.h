/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAMASTERPAGEDIALOG_H
#define KOPAMASTERPAGEDIALOG_H

#include <KoDialog.h>

class QListView;

class KoPADocument;
class KoPAMasterPage;
class KoPAPageThumbnailModel;

class KoPAMasterPageDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KoPAMasterPageDialog(KoPADocument *document, KoPAMasterPage *activeMaster, QWidget *parent = nullptr);
    ~KoPAMasterPageDialog() override;

    KoPAMasterPage *selectedMasterPage();

private Q_SLOTS:
    void selectionChanged();

private:
    QListView *m_listView;
    KoPADocument *m_document;
    KoPAPageThumbnailModel *m_pageThumbnailModel;
};

#endif
