/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoTool.h"

#include "ChangeVideoCommand.h"
#include "FullScreenPlayer.h"
#include "SelectVideoWidget.h"
#include "VideoCollection.h"
#include "VideoData.h"
#include "VideoShape.h"
#include "ui_VideoToolWidget.h"

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoViewConverter.h>

#include <KoDialog.h>

#include <QPainter>
#include <QPointer>
#include <QUrl>

class VideoToolUI : public QWidget, public Ui::VideoTool
{
public:
    VideoToolUI()
    {
        setupUi(this);
        btnPlay->setIcon(koIcon("media-playback-start"));
        btnPlay->setToolTip(i18n("Play"));
    }
};

VideoTool::VideoTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_videoToolUI(nullptr)
    , m_videoShape(nullptr)
{
}

VideoTool::~VideoTool() = default;

void VideoTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        if ((m_videoShape = dynamic_cast<VideoShape *>(shape)))
            break;
    }

    if (!m_videoShape) {
        Q_EMIT done();
        return;
    }

    useCursor(Qt::ArrowCursor);
}

void VideoTool::mousePressEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void VideoTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void VideoTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void VideoTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

QWidget *VideoTool::createOptionWidget()
{
    m_videoToolUI = new VideoToolUI();

    connect(m_videoToolUI->btnVideoFile, &QAbstractButton::clicked, this, &VideoTool::changeUrlPressed);
    connect(m_videoToolUI->btnPlay, &QAbstractButton::clicked, this, &VideoTool::play);

    return m_videoToolUI;
}

void VideoTool::changeUrlPressed()
{
    if (!m_videoShape) {
        return;
    }

    QPointer<KoDialog> diag = new KoDialog();
    SelectVideoWidget *fileSelectionWidget = new SelectVideoWidget(diag);
    diag->setMainWidget(fileSelectionWidget);

    if (diag->exec() == KoDialog::Accepted) {
        fileSelectionWidget->accept();
        VideoData *data = nullptr;
        data = m_videoShape->videoCollection()->createExternalVideoData(fileSelectionWidget->selectedUrl(), fileSelectionWidget->saveEmbedded());

        ChangeVideoCommand *command = new ChangeVideoCommand(m_videoShape, data);
        canvas()->addCommand(command);
    } else {
        fileSelectionWidget->cancel();
    }

    delete diag;
}

void VideoTool::play()
{
    VideoData *videoData = qobject_cast<VideoData *>(m_videoShape->userData());
    Q_ASSERT(videoData);
    new FullScreenPlayer(videoData->playableUrl());
}
