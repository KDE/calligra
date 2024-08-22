/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
                 2000, 2001 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <goutte@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoFilterManager_p.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QMimeDatabase>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <ksqueezedtextlabel.h>

#include <unistd.h>

KoFilterChooser::KoFilterChooser(QWidget *parent, const QStringList &mimeTypes, const QString & /*nativeFormat*/, const QUrl &url)
    : KoDialog(parent)
    , m_mimeTypes(mimeTypes)
{
    setObjectName("kofilterchooser");
    setInitialSize(QSize(300, 350));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    setDefaultButton(KoDialog::Ok);
    setCaption(i18n("Choose Filter"));
    setModal(true);

    QWidget *page = new QWidget(this);
    setMainWidget(page);

    QVBoxLayout *layout = new QVBoxLayout(page);
    if (url.isValid()) {
        KSqueezedTextLabel *l = new KSqueezedTextLabel(url.path(), page);
        layout->addWidget(l);
    }
    m_filterList = new QListWidget(page);
    layout->addWidget(m_filterList);
    page->setLayout(layout);

    Q_ASSERT(!m_mimeTypes.isEmpty());
    QMimeDatabase db;
    for (QStringList::ConstIterator it = m_mimeTypes.constBegin(); it != m_mimeTypes.constEnd(); ++it) {
        QMimeType mime = db.mimeTypeForName(*it);
        const QString name = mime.isValid() ? mime.comment() : *it;
        if (!name.isEmpty()) {
            QListWidgetItem *item = new QListWidgetItem(name, m_filterList);
            item->setData(32, *it);
        }
    }

    m_filterList->sortItems();

    if (m_filterList->currentRow() == -1)
        m_filterList->setCurrentRow(0);

    m_filterList->setFocus();

    connect(m_filterList, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
    resize(QSize(520, 400)); //.expandedTo(minimumSizeHint()));
}

KoFilterChooser::~KoFilterChooser() = default;

QString KoFilterChooser::filterSelected()
{
    QListWidgetItem *item = m_filterList->currentItem();
    return item->data(32).toString();
}
