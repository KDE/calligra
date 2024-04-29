/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SNAPGUIDECONFIGWIDGET_H
#define SNAPGUIDECONFIGWIDGET_H

#include "ui_SnapGuideConfigWidget.h"

#include <QWidget>

class KoSnapGuide;
class QShowEvent;

class SnapGuideConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SnapGuideConfigWidget(KoSnapGuide *snapGuide, QWidget *parent = nullptr);
    ~SnapGuideConfigWidget() override;

private Q_SLOTS:
    void snappingEnabled(bool isEnabled);
    void strategyChanged();
    void distanceChanged(int distance);
    void updateControls();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui_SnapGuideConfigWidget widget;
    KoSnapGuide *m_snapGuide;
};

#endif // SNAPGUIDECONFIGWIDGET_H
