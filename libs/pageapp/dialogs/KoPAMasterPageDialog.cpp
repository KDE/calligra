/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAMasterPageDialog.h"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "KoPADocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPageThumbnailModel.h"

KoPAMasterPageDialog::KoPAMasterPageDialog(KoPADocument *document, KoPAMasterPage *activeMaster, QWidget *parent)
    : KoDialog(parent)
    , m_document(document)
{
    QSize iconSize(128, 128);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    if (m_document->pageType() == KoPageApp::Slide) {
        layout->addWidget(new QLabel(i18n("Select a master slide design:"), mainWidget));
        setCaption(i18n("Master Slide"));
    } else {
        layout->addWidget(new QLabel(i18n("Select a master page design:"), mainWidget));
        setCaption(i18n("Master Page"));
    }

    m_listView = new QListView;
    m_listView->setDragDropMode(QListView::NoDragDrop);
    m_listView->setIconSize(iconSize);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setFlow(QListView::LeftToRight);
    m_listView->setWrapping(true);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setMovement(QListView::Static);
    m_listView->setMinimumSize(320, 200);

    m_pageThumbnailModel = new KoPAPageThumbnailModel(m_document->pages(true), m_listView);
    m_pageThumbnailModel->setIconSize(iconSize);
    m_listView->setModel(m_pageThumbnailModel);
    layout->addWidget(m_listView);

    int row = m_document->pageIndex(activeMaster);
    QModelIndex index = m_pageThumbnailModel->index(row, 0);
    m_listView->setCurrentIndex(index);

    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &KoPAMasterPageDialog::selectionChanged);

    mainWidget->setLayout(layout);
    setMainWidget(mainWidget);
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
}

KoPAMasterPageDialog::~KoPAMasterPageDialog()
{
    // delete m_pageThumbnailModel;
}

KoPAMasterPage *KoPAMasterPageDialog::selectedMasterPage()
{
    QModelIndex index = m_listView->currentIndex();
    KoPAPageBase *page = static_cast<KoPAPageBase *>(index.internalPointer());
    KoPAMasterPage *masterPage = dynamic_cast<KoPAMasterPage *>(page);
    Q_ASSERT(masterPage);
    return masterPage;
}

void KoPAMasterPageDialog::selectionChanged()
{
    // TODO: user shouldn't be able to deselect any item
    enableButtonOk(m_listView->selectionModel()->hasSelection());
}
