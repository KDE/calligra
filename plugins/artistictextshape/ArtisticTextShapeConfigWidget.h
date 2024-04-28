/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTSHAPECONFIGWIDGET_H
#define ARTISTICTEXTSHAPECONFIGWIDGET_H

#include <ui_ArtisticTextShapeConfigWidget.h>

#include <KoShapeConfigWidgetBase.h>

class ArtisticTextTool;

class ArtisticTextShapeConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArtisticTextShapeConfigWidget(ArtisticTextTool *textTool);

public Q_SLOTS:
    /// updates the widget form the current artistic text shape
    void updateWidget();

Q_SIGNALS:
    /// Triggered whenever the font family has changed
    void fontFamilyChanged(const QFont &);

    /// Triggered whenever the font size has changed
    void fontSizeChanged(int);

private:
    void blockChildSignals(bool block);
    Ui::ArtisticTextShapeConfigWidget widget;
    ArtisticTextTool *m_textTool;
};

#endif // ARTISTICTEXTSHAPECONFIGWIDGET_H
