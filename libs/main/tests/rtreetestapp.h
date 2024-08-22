/*
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef RTREETESTAPP_H
#define RTREETESTAPP_H

#define CALLIGRA_RTREE_DEBUG

#include <QFile>
#include <QList>
#include <QMainWindow>

#include "KoRTree.h"
#include "Tool.h"

class QAction;
class QActionGroup;
class QMenu;

class Data
{
public:
    Data(QRectF rect)
        : m_rect(rect)
    {
    }

    QRectF boundingBox() const
    {
        return m_rect;
    }
    void paint(QPainter &p)
    {
        p.save();
        QPen pen(Qt::black);
        p.setPen(pen);
        p.drawRect(m_rect);
        p.restore();
    }

private:
    QRectF m_rect;
};

class Canvas : public QWidget
{
    Q_OBJECT

public:
    Canvas();
    ~Canvas() override = default;

    void updateCanvas();
    void insert(QRectF &rect);
    void select(QRectF &rect);
    void remove(QRectF &rect);

public Q_SLOTS:
    void selectInsertTool();
    void selectSelectTool();
    void selectRemoveTool();
    void clear();

    void replay();
    void debug();
    void replayStep();
    void paintTree(bool paintTree);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void paintEvent(QPaintEvent *e) override;

private:
    qreal m_zoom;
    QSet<Data *> m_rects;
    QList<Data *> m_found;
    QRectF m_insertRect;
    bool m_buttonPressed;
    KoRTree<Data *> m_rtree;
    Tool *m_tool;
    CreateTool m_createTool;
    SelectTool m_selectTool;
    RemoveTool m_removeTool;
    QFile m_file;
    QTextStream m_out;
    QStringList m_list;
    int m_listId;
    bool m_paintTree;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private Q_SLOTS:
    void about();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    Canvas *m_canvas;

    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_helpMenu;

    QAction *m_aboutAct;
    QAction *m_aboutQtAct;
    QAction *m_quitAct;

    QAction *m_insertAct;
    QAction *m_selectAct;
    QAction *m_removeAct;
    QAction *m_clearAct;
    QActionGroup *m_toolAct;

    QAction *m_replayAct;
    QAction *m_debugAct;
    QAction *m_paintTreeAct;
};

#endif
