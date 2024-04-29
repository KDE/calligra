/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _TOOL_DOCKER_H
#define _TOOL_DOCKER_H

#include <QDockWidget>
#include <QPointer>

class QWidget;

/**
 * The tool docker shows the tool option widget associated with the
 * current tool and the current canvas.
 */
class ToolDocker : public QDockWidget
{
    Q_OBJECT
public:
    explicit ToolDocker(QWidget *parent = nullptr);
    ~ToolDocker();

protected:
    virtual void resizeEvent(QResizeEvent *event); ///< reimplemented from QWidget
public Q_SLOTS:
    /**
     * Update the option widgets to the argument one, removing the currently set widget.
     */
    void setOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList);

    /**
     * Returns whether the docker has an optionwidget attached
     */
    bool hasOptionWidget();

private:
    Q_PRIVATE_SLOT(d, void toggleLock())
    Q_PRIVATE_SLOT(d, void toggleTab())
    Q_PRIVATE_SLOT(d, void locationChanged(Qt::DockWidgetArea area))

    class Private;
    Private *const d;
};

#endif
