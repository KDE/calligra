/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kupluakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWDEBUGWIDGET_H
#define KWDEBUGWIDGET_H

#include <KWCanvas.h>
#include <QWidget>

#include <QLabel>
#include <QPushButton>

/** KWDebugWidget shows some debug info.
 */

class KWDebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KWDebugWidget(QWidget *parent = nullptr);
    ~KWDebugWidget() override;

    friend class KWDebugDocker;

    void setCanvas(KWCanvas *canvas);

    void unsetCanvas();

private Q_SLOTS:
    void updateData();
    void doSetMagic();
    void doGetMagic();

private:
    void initUi();
    void initLayout();

    void updateDataUi();

private:
    QLabel *m_label;
    QPushButton *m_buttonSet;
    QPushButton *m_buttonGet;

    KWCanvas *m_canvas;
};

#endif // KWDEBUGWIDGET_H
