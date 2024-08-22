/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoZoomToolWidget.h"
#include "KoZoomTool.h"
#include <KoIcon.h>
#include <QMouseEvent>
#include <QPainter>

KoZoomToolWidget::KoZoomToolWidget(KoZoomTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    setupUi(this);
    m_dirtyThumbnail = true;
    birdEyeLabel->installEventFilter(this);
    birdEyeLabel->hide(); // remove this when coding on the birdEyeLabel

    zoomInButton->setIcon(koIcon("zoom-in"));
    zoomOutButton->setIcon(koIcon("zoom-out"));

    connect(zoomInButton, &QAbstractButton::toggled, this, &KoZoomToolWidget::changeZoomMode);
    connect(zoomOutButton, &QAbstractButton::toggled, this, &KoZoomToolWidget::changeZoomMode);

    zoomInButton->click();
}

KoZoomToolWidget::~KoZoomToolWidget() = default;

void KoZoomToolWidget::paintBirdEye()
{
    QPainter p;
    if (m_dirtyThumbnail) {
        m_thumbnail = QPixmap(birdEyeLabel->size());
        //        m_thumbnail.fill(birdEyeLabel->palette().dark().color());
        p.begin(&m_thumbnail);
        // TODO fill in code to paint a thumbnail of the current document
        p.end();
        m_dirtyThumbnail = false;
    }

    p.begin(birdEyeLabel);
    p.drawPixmap(0, 0, m_thumbnail);
    //    p.drawRect(m_birdEyeRect);
    p.end();
}

bool KoZoomToolWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == birdEyeLabel) {
        if (event->type() == QEvent::Paint) {
            paintBirdEye();
            return true;
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() | Qt::LeftButton) {
                // m_tool->canvasController()->pan
                // TODO implement panning
            }
            return true;
        } else if (event->type() == QEvent::Resize) {
            m_dirtyThumbnail = true;
        } else
            return false;
    }
    return QWidget::eventFilter(object, event);
}

void KoZoomToolWidget::changeZoomMode()
{
    m_tool->setZoomInMode(zoomInButton->isChecked());
}
