/*
 * Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (c) 2005-2008 Thomas Zander <zander@kde.org>
 * Copyright (c) 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 * Copyright (c) 2011 C. Boemann <cbo@boemann.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KO_MODEBOX_H
#define KO_MODEBOX_H

#include <KoCanvasObserverBase.h>

#include <QPointer>
#include <QList>
#include <QMap>
#include <QHash>
#include <QDockWidget>
#include <QQuickWidget>
#include <QScrollArea>
#include <memory>

#include <KoToolManager.h>

class KoCanvasControllerWidget;
class KoCanvasController;
class KoCanvasBase;
class KoShapeLayer;

class ScrollArea : public QScrollArea
{
    Q_OBJECT
protected:
    void showEvent(QShowEvent *) override;
};

/**
 * KoModeBox is housed in a dock widget that presents tools as headings in a QToolBox
 * according to type and priority.
 *
 * The ModeBox is a container for tool buttons which are themselves
 * divided into sections.
 *
 * @see KoToolManager
 */
class KoModeBox : public QQuickWidget {
    Q_OBJECT
public:
    /// constructor
    explicit KoModeBox(KoCanvasControllerWidget *canvas, const QString &applicationName);
    ~KoModeBox() override;

    /**
     * Should been called when the docker position has changed.
     * Organise widgets and icons and orientation of the tabs.
     *
     * @param area the new location area
     */
    void locationChanged(Qt::DockWidgetArea area);
public Q_SLOTS:
    /**
     * Using the buttongroup id passed in addButton() you can set the new active tool.
     * If the id does not resolve to a visible heading, this call is ignored.
     * @param canvas the currently active canvas.
     * @param id an id to identify the tool/heading to activate.
     */
    void setActiveTool(KoCanvasController *canvas, int id);

    /**
     * Show only the dynamic buttons that have a code from parameter codes.
     * The modebox allows buttons to be optionally registered with a visibilityCode. This code
     * can be passed here and all buttons that have that code are shown. All buttons that
     * have another visibility code registered are hidden.
     * @param codes a list of all the codes to show.
     */
    void updateShownTools(const QList<QString> &codes);

    void setOptionWidgets(const QList<QPointer<QWidget> > &optionWidgetList);

    /// set the canvas this docker should listen to for changes.
    void setCanvas(KoCanvasBase *canvas);
    void unsetCanvas();

private Q_SLOTS:
    void setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer* newLayer);

    /// add a tool post-initialization. The tool will also be activated.
    void toolAdded(KoToolAction *toolAction, KoCanvasController *canvas);

    /// slot for when a new item have been selected in the QToolBox
    void toolSelected(int index);

    /// slot for context menu of the tabbar
    void slotContextMenuRequested(const QPoint &pos);

    /// switch icon mode
    void switchIconMode(int);

    /// switch tabs side
    void switchTabsSide(int);

    /**
     * Add a tool to the modebox.
     * The tools should all be added before the first showing since adding will not really add
     * them to the UI until setup() is called.
     *
     * @param toolAction the action of the tool
     * @see setup()
     */
    void addToolAction(KoToolAction *toolAction);

public:
    static QString applicationName;

private:
    enum IconMode {
        IconAndText,
        IconOnly
    };

    enum VerticalTabsSide {
        TopSide,
        BottomSide
    };

    enum HorizontalTabsSide {
        LeftSide,
        RightSide
    };

    QIcon createTextIcon(KoToolAction *toolAction) const;
    QIcon createSimpleIcon(KoToolAction *toolAction) const;
    void addItem(KoToolAction *toolAction);
    void setIconSize() const;

private:
    class Private;
    std::unique_ptr<Private> const d;
};

#endif // _KO_TOOLBOX_H_
