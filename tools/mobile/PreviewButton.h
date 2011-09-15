/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef PREVIEWBUTTON_H
#define PREVIEWBUTTON_H

#include <QPushButton>
#include <QTimer>

class PreviewButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PreviewButton(const QIcon & icon, const QString & text, QWidget * parent = 0);
    void setSlideNumber(int num);
    int getSlideNumber();

signals:
    /*!
     * signal for moving slide option
     */
    void moveSlide();
    /*!
     * signal for deleting a slide option
     */
    void deleteSlide();
    /*!
     * signal for adding new slide
     */
    void newSlide();

public slots:
    /*!
     * slot to service a right click on button in the case of freoffice running on n900 its a deep press
     */
    void rightButtonClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    /*!
     * timer used for implementing the right click on the QPushButton
     */
    QTimer *timer;
    QPoint pos;
    int slideNumber;
};

#endif // PREVIEWBUTTON_H
