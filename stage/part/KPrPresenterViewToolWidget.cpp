/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrPresenterViewToolWidget.h"

#include <KoIcon.h>
#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QTimer>


KPrPresenterViewToolWidget::KPrPresenterViewToolWidget(QWidget *parent)
    : QFrame(parent)
{
    QSize iconSize( 32, 32 );
    QHBoxLayout *mainLayout = new QHBoxLayout;

    QHBoxLayout *hLayout = new QHBoxLayout;
    QToolButton *toolButton = new QToolButton;
    toolButton->setIcon(koIcon("go-previous"));
    toolButton->setIconSize( iconSize );
    connect( toolButton, SIGNAL(clicked()), this, SIGNAL(previousSlideClicked()) );
    hLayout->addWidget(toolButton);
    toolButton = new QToolButton;
    toolButton->setIcon(koIcon("go-next"));
    toolButton->setIconSize( iconSize );
    connect( toolButton, SIGNAL(clicked()), this, SIGNAL(nextSlideClicked()) );
    hLayout->addWidget( toolButton );

    mainLayout->addLayout(hLayout);
    mainLayout->addSpacing( 5 );
    QFrame *frame = new QFrame;
    frame->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    mainLayout->addWidget(frame);
    mainLayout->addSpacing( 5 );

    m_slidesToolButton = new QToolButton;
    m_slidesToolButton->setCheckable( true );
    m_slidesToolButton->setIcon(koIcon("view-list-icons"));
    m_slidesToolButton->setIconSize( iconSize );
    connect( m_slidesToolButton, SIGNAL(toggled(bool)), this, SIGNAL(slideThumbnailsToggled(bool)) );
    mainLayout->addWidget( m_slidesToolButton );

    mainLayout->addSpacing( 5 );
    frame = new QFrame;
    frame->setFrameStyle( QFrame::VLine | QFrame::Raised );
    mainLayout->addWidget( frame );
    mainLayout->addSpacing( 5 );

    hLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(koIcon("user-away").pixmap(iconSize));
    hLayout->addWidget( iconLabel );
    m_clockLabel = new QLabel( QTime::currentTime().toString( "hh:mm:ss ap" ) );
    m_clockLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget( m_clockLabel );
    mainLayout->addLayout( hLayout );

    mainLayout->addSpacing( 5 );
    frame = new QFrame;
    frame->setFrameStyle( QFrame::VLine | QFrame::Plain );
    mainLayout->addWidget(frame);
    mainLayout->addSpacing(5);

    hLayout = new QHBoxLayout;
    iconLabel = new QLabel;
    iconLabel->setPixmap(koIcon("chronometer").pixmap(iconSize));
    hLayout->addWidget(iconLabel);
    m_timerLabel = new QLabel( "00:00:00");
    m_timerLabel->setStyleSheet("QLabel { font-size: 24px }");
    hLayout->addWidget( m_timerLabel );
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    m_currentTime.start();
    m_clockTimer = new QTimer( this );
    connect( m_clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()) );
    m_clockTimer->start( 1000 );
}

void KPrPresenterViewToolWidget::toggleSlideThumbnails( bool toggle )
{
    m_slidesToolButton->setChecked( toggle );
}

void KPrPresenterViewToolWidget::updateClock()
{
    QTime time = QTime::currentTime();
    m_clockLabel->setText( time.toString( "hh:mm:ss a" ) );
    int sec = m_currentTime.elapsed() / 1000;

    int hour = sec / 3600;
    sec -= hour * 3600;
    int min = sec / 60;
    sec -= min * 60;

    // display the timer, with 0 appended if only 1 digit
    m_timerLabel->setText( QString( "%1:%2:%3").arg( hour, 2, 10, QChar( '0' ) )
            .arg( min, 2, 10, QChar( '0' ) ).arg( sec, 2, 10, QChar( '0' ) ) );
}
