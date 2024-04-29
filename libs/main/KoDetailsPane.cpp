/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDetailsPane.h"

#include <QKeyEvent>
#include <QStandardItemModel>

////////////////////////////////////
// class KoDetailsPane
///////////////////////////////////

class KoDetailsPanePrivate
{
public:
    KoDetailsPanePrivate()
    {
        m_model = new QStandardItemModel;
    }
    ~KoDetailsPanePrivate()
    {
        delete m_model;
    }

    QStandardItemModel *m_model;
};

KoDetailsPane::KoDetailsPane(QWidget *parent, const QString &header)
    : QWidget(parent)
    , Ui_KoDetailsPaneBase()
    , d(new KoDetailsPanePrivate)
{
    d->m_model->setHorizontalHeaderItem(0, new QStandardItem(header));

    setupUi(this);

    m_previewLabel->installEventFilter(this);
    m_documentList->installEventFilter(this);
    m_documentList->setIconSize(QSize(IconExtent, IconExtent));
    m_documentList->setModel(d->m_model);
    m_splitter->setSizes(QList<int>() << 2 << 1);

    changePalette();

    connect(m_documentList->selectionModel(), &QItemSelectionModel::currentChanged, this, &KoDetailsPane::selectionChanged);
    connect(m_documentList, &QTreeView::doubleClicked, this, QOverload<const QModelIndex &>::of(&KoDetailsPane::openFile));
    connect(m_openButton, &QPushButton::clicked, this, QOverload<>::of(&KoDetailsPane::openFile));
}

KoDetailsPane::~KoDetailsPane()
{
    delete d;
}

bool KoDetailsPane::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == m_previewLabel) {
        if (e->type() == QEvent::MouseButtonDblClick) {
            openFile();
        }
    }

    if (watched == m_documentList) {
        if ((e->type() == QEvent::Resize) && isVisible()) {
            Q_EMIT splitterResized(this, m_splitter->sizes());
        }

        if ((e->type() == QEvent::KeyPress)) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                openFile();
            }
        }
    }

    return false;
}

void KoDetailsPane::resizeSplitter(KoDetailsPane *sender, const QList<int> &sizes)
{
    if (sender == this)
        return;

    m_splitter->setSizes(sizes);
}

void KoDetailsPane::openFile()
{
    QModelIndex index = m_documentList->selectionModel()->currentIndex();
    openFile(index);
}

void KoDetailsPane::changePalette()
{
    QPalette p = palette();
    p.setBrush(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::Normal, QPalette::WindowText));
    m_detailsLabel->setPalette(p);
}

QStandardItemModel *KoDetailsPane::model() const
{
    return d->m_model;
}
