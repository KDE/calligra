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

#ifndef KPRPRESENTERVIEWTOOLWIDGET
#define KPRPRESENTERVIEWTOOLWIDGET

#include <QTime>
#include <QFrame>

class QLabel;
class QTimer;
class QToolButton;

/**
 * KPrPresenterViewToolWidget
 * This widget shows all navigation functions (previous and next slide) together
 * with clock and timer
 */
class KPrPresenterViewToolWidget : public QFrame
{
    Q_OBJECT
public:
    explicit KPrPresenterViewToolWidget(QWidget *parent = 0);
    void toggleSlideThumbnails( bool toggle );

Q_SIGNALS:
    void slideThumbnailsToggled( bool toggle );
    void previousSlideClicked();
    void nextSlideClicked();

private Q_SLOTS:
    void updateClock();

private:
    QToolButton *m_slidesToolButton;
    QLabel *m_clockLabel;
    QLabel *m_timerLabel;

    QTime m_currentTime;
    QTimer *m_clockTimer;
};

#endif

