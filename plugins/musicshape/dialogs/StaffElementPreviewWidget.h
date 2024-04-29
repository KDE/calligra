/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef STAFFELEMENTPREVIEWWIDGET_H
#define STAFFELEMENTPREVIEWWIDGET_H

#include <QWidget>
class MusicStyle;
class MusicRenderer;
namespace MusicCore
{
class StaffElement;
class Staff;
class Clef;
}

class StaffElementPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StaffElementPreviewWidget(QWidget *parent = nullptr);
    ~StaffElementPreviewWidget() override;

    QSize sizeHint() const override;

    void setMusicStyle(MusicStyle *style);
    void setStaffElement(MusicCore::StaffElement *se);
    MusicCore::Staff *staff();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    MusicStyle *m_style;
    MusicRenderer *m_renderer;
    MusicCore::StaffElement *m_element;
    MusicCore::Staff *m_staff;
    MusicCore::Clef *m_clef;
};

#endif // STAFFELEMENTPREVIEWWIDGET_H
