/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresenterViewToolWidget.h"

#include <KoIcon.h>
#include <QBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QToolButton>

KPrPresenterViewToolWidget::KPrPresenterViewToolWidget(QWidget *parent)
    : QFrame(parent)
{
    QSize iconSize(32, 32);
    QHBoxLayout *mainLayout = new QHBoxLayout;

    QHBoxLayout *hLayout = new QHBoxLayout;
    QToolButton *toolButton = new QToolButton;
    toolButton->setIcon(koIcon("go-previous"));
    toolButton->setIconSize(iconSize);
    connect(toolButton, &QAbstractButton::clicked, this, &KPrPresenterViewToolWidget::previousSlideClicked);
    hLayout->addWidget(toolButton);
    toolButton = new QToolButton;
    toolButton->setIcon(koIcon("go-next"));
    toolButton->setIconSize(iconSize);
    connect(toolButton, &QAbstractButton::clicked, this, &KPrPresenterViewToolWidget::nextSlideClicked);
    hLayout->addWidget(toolButton);

    mainLayout->addLayout(hLayout);
    mainLayout->addSpacing(5);
    QFrame *frame = new QFrame;
    frame->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    mainLayout->addWidget(frame);
    mainLayout->addSpacing(5);

    m_slidesToolButton = new QToolButton;
    m_slidesToolButton->setCheckable(true);
    m_slidesToolButton->setIcon(koIcon("view-list-icons"));
    m_slidesToolButton->setIconSize(iconSize);
    connect(m_slidesToolButton, &QAbstractButton::toggled, this, &KPrPresenterViewToolWidget::slideThumbnailsToggled);
    mainLayout->addWidget(m_slidesToolButton);

    mainLayout->addSpacing(5);
    frame = new QFrame;
    frame->setFrameStyle(QFrame::VLine | QFrame::Raised);
    mainLayout->addWidget(frame);
    mainLayout->addSpacing(5);

    hLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(koIcon("user-away").pixmap(iconSize));
    hLayout->addWidget(iconLabel);
    m_clockLabel = new QLabel(QTime::currentTime().toString("hh:mm:ss ap"));
    m_clockLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget(m_clockLabel);
    mainLayout->addLayout(hLayout);

    mainLayout->addSpacing(5);
    frame = new QFrame;
    frame->setFrameStyle(QFrame::VLine | QFrame::Plain);
    mainLayout->addWidget(frame);
    mainLayout->addSpacing(5);

    hLayout = new QHBoxLayout;
    iconLabel = new QLabel;
    iconLabel->setPixmap(koIcon("chronometer").pixmap(iconSize));
    hLayout->addWidget(iconLabel);
    m_timerLabel = new QLabel("00:00:00");
    m_timerLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget(m_timerLabel);
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    m_currentTime.start();
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &KPrPresenterViewToolWidget::updateClock);
    m_clockTimer->start(1000);
}

void KPrPresenterViewToolWidget::toggleSlideThumbnails(bool toggle)
{
    m_slidesToolButton->setChecked(toggle);
}

void KPrPresenterViewToolWidget::updateClock()
{
    QTime time = QTime::currentTime();
    m_clockLabel->setText(time.toString("hh:mm:ss a"));
    int sec = m_currentTime.elapsed() / 1000;

    int hour = sec / 3600;
    sec -= hour * 3600;
    int min = sec / 60;
    sec -= min * 60;

    // display the timer, with 0 appended if only 1 digit
    m_timerLabel->setText(QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0')));
}
