/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPRESENTERVIEWTOOLWIDGET
#define KPRPRESENTERVIEWTOOLWIDGET

#include <QElapsedTimer>
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
    explicit KPrPresenterViewToolWidget(QWidget *parent = nullptr);
    void toggleSlideThumbnails(bool toggle);

Q_SIGNALS:
    void slideThumbnailsToggled(bool toggle);
    void previousSlideClicked();
    void nextSlideClicked();

private Q_SLOTS:
    void updateClock();

private:
    QToolButton *m_slidesToolButton;
    QLabel *m_clockLabel;
    QLabel *m_timerLabel;

    QElapsedTimer m_currentTime;
    QTimer *m_clockTimer;
};

#endif
