/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VectorShapeConfigWidget.h"

#include "VectorShape.h"
#include "VectorDebug.h"
// KF5
#include <kfilewidget.h>
#include <KIO/Job>
// Qt
#include <QVBoxLayout>
#include <QUrl>

void LoadWaiter::setImageData(KJob *job)
{
    if (m_vectorShape) {
        KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob*>(job);
        Q_ASSERT(transferJob);

        const QByteArray contents = transferJob->data();
        const VectorShape::VectorType vectorType = VectorShape::vectorType(contents);

        m_vectorShape->setCompressedContents(qCompress(contents), vectorType);
    }

    deleteLater();
}

// ---------------------------------------------------- //

VectorShapeConfigWidget::VectorShapeConfigWidget()
    : m_shape(0),
    m_fileWidget(0)
{
}

VectorShapeConfigWidget::~VectorShapeConfigWidget()
{
    delete m_fileWidget;
}

void VectorShapeConfigWidget::open(KoShape *shape)
{
    m_shape = dynamic_cast<VectorShape*>(shape);
    Q_ASSERT(m_shape);
    delete m_fileWidget;
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_fileWidget = new KFileWidget(QUrl(/*QT5TODO:"kfiledialog:///OpenDialog"*/), this);
    m_fileWidget->setOperationMode(KFileWidget::Opening);
    const QStringList mimetypes = QStringList()
        << QLatin1String("image/x-wmf")
        << QLatin1String("image/x-emf")
        << QLatin1String("image/x-svm")
        << QLatin1String("image/svg+xml");
    m_fileWidget->setMimeFilter(mimetypes);
    layout->addWidget(m_fileWidget);
    setLayout(layout);
    connect(m_fileWidget, &KFileWidget::accepted, this, &KoShapeConfigWidgetBase::accept);
}

void VectorShapeConfigWidget::save()
{
    if (!m_shape)
        return;
    m_fileWidget->accept();
    QUrl url = m_fileWidget->selectedUrl();
    if (!url.isEmpty()) {
        KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, {});
        LoadWaiter *waiter = new LoadWaiter(m_shape);
        connect(job, &KJob::result, waiter, &LoadWaiter::setImageData);
    }
}

bool VectorShapeConfigWidget::showOnShapeCreate()
{
    return true;
}

bool VectorShapeConfigWidget::showOnShapeSelect()
{
    return false;
}
