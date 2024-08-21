/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>
   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "VectorTool.h"
#include "ChangeVectorDataCommand.h"
#include "VectorShape.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QToolButton>
#include <QUrl>

#include <KIO/StoredTransferJob>
#include <KLocalizedString>

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoImageCollection.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

VectorTool::VectorTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_shape(nullptr)
{
}

void VectorTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_shape = dynamic_cast<VectorShape *>(shape);
        if (m_shape)
            break;
    }
    if (!m_shape) {
        Q_EMIT done();
        return;
    }
    useCursor(Qt::ArrowCursor);
}

void VectorTool::deactivate()
{
    m_shape = nullptr;
}

QWidget *VectorTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(optionWidget);

    QToolButton *button = nullptr;

    button = new QToolButton(optionWidget);
    button->setIcon(koIcon("document-open"));
    button->setToolTip(i18n("Open Vector Image (EMF/WMF/SVM)"));
    layout->addWidget(button, 0, 0);
    connect(button, &QAbstractButton::clicked, this, &VectorTool::changeUrlPressed);

    return optionWidget;
}

void VectorTool::changeUrlPressed()
{
    if (m_shape == nullptr)
        return;
    const QUrl url = QFileDialog::getOpenFileUrl(/*QT5TODO: QLatin1String("image/x-emf image/x-wmf image/x-svm image/svg+xml")*/);
    if (!url.isEmpty()) {
        // TODO move this to an action in the libs, with a nice dialog or something.
        KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, {});
        connect(job, &KJob::result, this, &VectorTool::setImageData);
    }
}

void VectorTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    if (canvas()->shapeManager()->shapeAt(event->point) != m_shape) {
        event->ignore(); // allow the event to be used by another
        return;
    }
    changeUrlPressed();
}

void VectorTool::setImageData(KJob *job)
{
    if (m_shape == nullptr) {
        return;
    }
    KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob *>(job);
    Q_ASSERT(transferJob);

    const QByteArray newData = transferJob->data();
    const VectorShape::VectorType vectorType = VectorShape::vectorType(newData);
    ChangeVectorDataCommand *cmd = new ChangeVectorDataCommand(m_shape, qCompress(newData), vectorType);

    canvas()->addCommand(cmd);
}
