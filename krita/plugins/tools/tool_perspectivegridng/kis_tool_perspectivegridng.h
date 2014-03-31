/*
 *  kis_tool_perspectivegridng.h - part of Krita
 *
 *  Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_TOOL_PERSPECTIVE_GRIDNG_H_
#define _KIS_TOOL_PERSPECTIVE_GRIDNG_H_

#include <kis_tool.h>
#include <KoToolFactoryBase.h>
#include "kis_perspective_gridng.h"
//#include "ui_PerspectiveGridNgToolsOptions.h"
#include <KoIcon.h>


class RulerDecoration;
class KisCanvas2;
class ConstraintSolver;
class KJob;

class KisPerspectiveGridNgTool : public KisTool
{
    Q_OBJECT
    enum PerspectiveGridNgEditionMode {
        MODE_CREATION, // This is the mode when there is not yet a perspective grid
        MODE_EDITING, // This is the mode when the grid has been created, and we are waiting for the user to click on a control box
        MODE_DRAGGING_NODE, // In this mode one node is translated
        MODE_DRAGGING_TRANSLATING_TWONODES // This mode is used when creating a new sub perspective grid
    };
public:
    KisPerspectiveGridNgTool(KoCanvasBase * canvas);
    virtual ~KisPerspectiveGridNgTool();

    virtual quint32 priority() {
        return 3;
    }
    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);

    //virtual QWidget *createOptionWidget();
private:
    void addAssistant();
    void removeAssistant(KisPerspectiveGridNg *assistant);
    bool mouseNear(const QPointF& mousep, const QPointF& point);
    KisPerspectiveGridNgHandleSP nodeNearPoint(KisPerspectiveGridNg* grid, QPointF point);
public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();
private slots:
    void removeAllAssistants();

    void saveAssistants();
    void loadAssistants();

    void saveFinish(KJob* job);
    void openFinish(KJob* job);
protected:

    virtual void paint(QPainter& gc, const KoViewConverter &converter);

protected:
    KisCanvas2* m_canvas;
    QList<KisPerspectiveGridNgHandleSP> m_handles, m_sideHandles,m_perspectiveHandles;
    KisPerspectiveGridNgHandleSP m_handleDrag;
    KisPerspectiveGridNgHandleSP m_handleCombine;
    KisPerspectiveGridNg* m_assistantDrag;
    KisPerspectiveGridNg* m_newAssistant;
    QPointF m_mousePosition;
    //Ui::PerspectiveGridNgToolOptions m_options;
    //QWidget* m_optionsWidget;
    QPointF m_dragEnd;

private:
    PerspectiveGridNgEditionMode m_internalMode;
    qint32 m_handleSize, m_handleHalfSize;
    KisPerspectiveGridNgHandleSP m_selectedNode1, m_selectedNode2, m_higlightedNode;
};


class KisPerspectiveGridNgToolFactory : public KoToolFactoryBase
{
public:
    KisPerspectiveGridNgToolFactory()
            : KoToolFactoryBase("KisPerspectiveGridNgTool") {
        qDebug()<<"Shiva: " << "KisPerspectiveGridNgToolFactory";
        setToolTip(i18n("Perspective Grid Editor"));
        setToolType(TOOL_TYPE_VIEW);
        setIconName(koIconNameCStr("krita_tool_perspectivegridng"));
        setPriority(0);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    };


    virtual ~KisPerspectiveGridNgToolFactory() {}

    virtual KoToolBase * createTool(KoCanvasBase * canvas) {
        return new KisPerspectiveGridNgTool(canvas);
    }

};


#endif

