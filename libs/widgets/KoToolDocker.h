/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_TOOL_DOCKER_H
#define KO_TOOL_DOCKER_H

#include <QDockWidget>
#include <QPointer>

class QWidget;

#include <KoCanvasObserverBase.h>
#include <kowidgets_export.h>
/**
 * The tool docker shows the tool option widget associated with the
 * current tool and the current canvas.
 */
class KOWIDGETS_EXPORT KoToolDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KoToolDocker(QWidget *parent = nullptr);
    ~KoToolDocker() override;

    void resetWidgets();

    /// reimplemented
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

protected:
    void resizeEvent(QResizeEvent *event) override; ///< reimplemented from QWidget
public Q_SLOTS:
    /**
     * Update the option widgets to the argument one, removing the currently set widget.
     */
    void setOptionWidgets(const QList<QPointer<QWidget>> &optionWidgetList);

    /**
     * Returns whether the docker has an optionwidget attached
     */
    bool hasOptionWidget();

    /**
     * set the tab option
     */
    void setTabEnabled(bool enabled);

private:
    Q_PRIVATE_SLOT(d, void toggleTab())
    Q_PRIVATE_SLOT(d, void locationChanged(Qt::DockWidgetArea area))

    class Private;
    Private *const d;
};

#endif
