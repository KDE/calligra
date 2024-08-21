/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoShapeConfigWidget.h"

#include "VideoDebug.h"
#include "VideoShape.h"

#include <SelectVideoWidget.h>
#include <VideoCollection.h>
#include <VideoData.h>

#include <QGridLayout>

VideoShapeConfigWidget::VideoShapeConfigWidget()
    : KoShapeConfigWidgetBase()
    , m_shape(nullptr)
    , m_fileSelectionWidget(nullptr)
{
}

VideoShapeConfigWidget::~VideoShapeConfigWidget()
{
    delete m_fileSelectionWidget;
}

void VideoShapeConfigWidget::open(KoShape *shape)
{
    m_shape = dynamic_cast<VideoShape *>(shape);
    Q_ASSERT(m_shape);
    if (!m_fileSelectionWidget) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        m_fileSelectionWidget = new SelectVideoWidget(this);
        layout->addWidget(m_fileSelectionWidget);
        setLayout(layout);
    }
}

void VideoShapeConfigWidget::save()
{
    if (!m_shape)
        return;
    m_fileSelectionWidget->accept();
    VideoData *data = m_shape->videoCollection()->createExternalVideoData(m_fileSelectionWidget->selectedUrl(), m_fileSelectionWidget->saveEmbedded());
    m_shape->setUserData(data);
}

bool VideoShapeConfigWidget::showOnShapeCreate()
{
    return true;
}

bool VideoShapeConfigWidget::showOnShapeSelect()
{
    return false;
}
