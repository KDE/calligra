/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PARTSWIDGET_H
#define PARTSWIDGET_H

#include <ui_PartsWidget.h>

#include <QWidget>

class MusicTool;
class MusicShape;
namespace MusicCore
{
class Sheet;
}

class PartsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartsWidget(MusicTool *tool, QWidget *parent = nullptr);

public Q_SLOTS:
    void setShape(MusicShape *shape);
private Q_SLOTS:
    void partDoubleClicked(const QModelIndex &index);
    void selectionChanged(const QModelIndex &current, const QModelIndex &prev);
    void addPart();
    void removePart();
    void editPart();

private:
    Ui::PartsWidget widget;
    MusicTool *m_tool;
    MusicShape *m_shape;
    MusicCore::Sheet *m_sheet;
};

#endif // PARTSWIDGET_H
