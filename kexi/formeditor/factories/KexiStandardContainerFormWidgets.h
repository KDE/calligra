/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2006-2010 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISTANDARDCONTAINERWIDGETS_H
#define KEXISTANDARDCONTAINERWIDGETS_H

#include "FormWidgetInterface.h"
#include "utils.h"

#include <QAction>
#include <QGroupBox>

class QPaintEvent;

namespace KFormDesigner
{
class Container;
}

//! Helper widget (used when using 'Lay out horizontally')
class HBox : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    explicit HBox(QWidget *parent);
    virtual ~HBox();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out vertically')
class VBox : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    explicit VBox(QWidget *parent);
    virtual ~VBox();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out in a grid')
class Grid : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    explicit Grid(QWidget *parent);
    virtual ~Grid();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class HFlow : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    explicit HFlow(QWidget *parent);
    virtual ~HFlow();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class VFlow : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    explicit VFlow(QWidget *parent);
    virtual ~VFlow();
    virtual void paintEvent(QPaintEvent *ev);
    virtual QSize sizeHint() const;
};

//! A simple container widget
class ContainerWidget : public QWidget
{
    Q_OBJECT

    friend class KFDTabWidget;

public:
    explicit ContainerWidget(QWidget *parent);
    virtual ~ContainerWidget();

    virtual QSize sizeHint() const;

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

Q_SIGNALS:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
};

//! Action of adding tab to a tab widget
//! Keeps context expressed using container and receiver widget
class AddTabAction : public QAction
{
Q_OBJECT
public:
    AddTabAction(KFormDesigner::Container *container,
                 TabWidgetBase *receiver, QObject *parent);
public Q_SLOTS:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action of removing tab from a tab widget
//! Keeps context expressed using container and receiver widget
class RemoveTabAction : public QAction
{
Q_OBJECT
public:
    RemoveTabAction(KFormDesigner::Container *container,
                    TabWidgetBase *receiver, QObject *parent);
protected Q_SLOTS:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action renaming tab widget's tab
//! Keeps context expressed using container and receiver widget
class RenameTabAction : public QAction
{
Q_OBJECT
public:
    RenameTabAction(KFormDesigner::Container *container,
                    TabWidgetBase *receiver, QObject *parent);
protected Q_SLOTS:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action of adding page to a stacked widget
//! Keeps context expressed using container and receiver widget
class AddStackPageAction : public QAction
{
Q_OBJECT
public:
    AddStackPageAction(KFormDesigner::Container *container,
                       QWidget *receiver, QObject *parent);
protected Q_SLOTS:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
};

//! Action of removing page from a stacked widget
//! Keeps context expressed using container and receiver widget
class RemoveStackPageAction : public QAction
{
Q_OBJECT
public:
    RemoveStackPageAction(KFormDesigner::Container *container,
                          QWidget *receiver, QObject *parent);
protected Q_SLOTS:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
};

//! Action of moving between pages of a stacked widget
//! Keeps context expressed using container and receiver widget
class GoToStackPageAction : public QAction
{
Q_OBJECT
public:
    enum Direction {
        Previous,
        Next
    };
    GoToStackPageAction(Direction direction, KFormDesigner::Container *container,
                        QWidget *receiver, QObject *parent);
protected Q_SLOTS:
    void slotTriggered();
private:
    int nextWidgetIndex() const;

    Direction m_direction;
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
};

//! A tab widget
class KFDTabWidget : public KFormDesigner::TabWidget
{
    Q_OBJECT

public:
    KFDTabWidget(KFormDesigner::Container *container, QWidget *parent);
    virtual ~KFDTabWidget();

    virtual QSize sizeHint() const;

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

    KFormDesigner::Container *container() const { return m_container; }

Q_SIGNALS:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);

private:
    KFormDesigner::Container *m_container;
};

//! A group box widget
class GroupBox : public QGroupBox
{
    Q_OBJECT

public:
    GroupBox(const QString & title, QWidget *parent);
    virtual ~GroupBox();

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

    virtual QSize sizeHint() const;

Q_SIGNALS:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
};

#endif
