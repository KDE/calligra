/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTSHAPEONPATHWIDGET_H
#define ARTISTICTEXTSHAPEONPATHWIDGET_H

#include <QWidget>

namespace Ui
{
class ArtisticTextShapeOnPathWidget;
}

class ArtisticTextTool;

class ArtisticTextShapeOnPathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArtisticTextShapeOnPathWidget(ArtisticTextTool *tool, QWidget *parent = nullptr);
    ~ArtisticTextShapeOnPathWidget() override;

public Q_SLOTS:
    void updateWidget();

Q_SIGNALS:
    /// triggered whenever the start offset has changed
    void offsetChanged(int);

private:
    Ui::ArtisticTextShapeOnPathWidget *ui;
    ArtisticTextTool *m_textTool;
};

#endif // ARTISTICTEXTSHAPEONPATHWIDGET_H
