/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2006-2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef CONTAINERFACTORY_H
#define CONTAINERFACTORY_H

#include <k3command.h>

#include "widgetfactory.h"
#include "../utils.h"
#include <QGroupBox>
#include <QMenu>
#include <QPaintEvent>

namespace KFormDesigner
{
class Form;
class FormManager;
class Container;
}

class InsertPageCommand : public K3Command
{
public:
    InsertPageCommand(KFormDesigner::Container *container, QWidget *widget);

    virtual void execute();
    virtual void unexecute();
    virtual QString name() const;

protected:
    KFormDesigner::Form *m_form;
    QString  m_containername;
    QString  m_name;
    QString  m_parentname;
};

//! Helper widget (used when using 'Lay out horizontally')
class HBox : public QFrame
{
    Q_OBJECT

public:
    HBox(QWidget *parent);
    virtual ~HBox();
    void setPreviewMode() {
        m_preview = true;
    }
    virtual void paintEvent(QPaintEvent *ev);

protected:
    bool  m_preview;
};

//! Helper widget (used when using 'Lay out vertically')
class VBox : public QFrame
{
    Q_OBJECT

public:
    VBox(QWidget *parent);
    virtual ~VBox();
    void setPreviewMode() {
        m_preview = true;
    }
    virtual void paintEvent(QPaintEvent *ev);

protected:
    bool  m_preview;
};

//! Helper widget (used when using 'Lay out in a grid')
class Grid : public QFrame
{
    Q_OBJECT

public:
    Grid(QWidget *parent);
    virtual ~Grid();
    void setPreviewMode() {
        m_preview = true;
    }
    virtual void paintEvent(QPaintEvent *ev);

protected:
    bool  m_preview;
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class HFlow : public QFrame
{
    Q_OBJECT

public:
    HFlow(QWidget *parent);
    virtual ~HFlow();
    void setPreviewMode() {
        m_preview = true;
    }
    virtual void paintEvent(QPaintEvent *ev);

protected:
    bool  m_preview;
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class VFlow : public QFrame
{
    Q_OBJECT

public:
    VFlow(QWidget *parent);
    virtual ~VFlow();
    void setPreviewMode() {
        m_preview = true;
    }
    virtual void paintEvent(QPaintEvent *ev);
    virtual QSize  sizeHint() const;

protected:
    bool  m_preview;
};

//! A simple container widget
class ContainerWidget : public QWidget
{
    Q_OBJECT

    friend class KFDTabWidget;

public:
    ContainerWidget(QWidget *parent);
    virtual ~ContainerWidget();

    virtual QSize sizeHint() const;

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

signals:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
};

//! A tab widget
class KFDTabWidget : public KFormDesigner::TabWidget
{
    Q_OBJECT

public:
    KFDTabWidget(QWidget *parent);
    virtual ~KFDTabWidget();

    virtual QSize sizeHint() const;

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

signals:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
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

signals:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
};

//! A form embedded as a widget inside other form
class SubForm : public Q3ScrollView
{
    Q_OBJECT
    Q_PROPERTY(QString formName READ formName WRITE setFormName DESIGNABLE true)

public:
    SubForm(QWidget *parent);
    ~SubForm();

    //! \return the name of the subform inside the db
    QString   formName() const {
        return m_formName;
    }
    void      setFormName(const QString &name);

private:
//  KFormDesigner::FormManager *m_manager;
    KFormDesigner::Form   *m_form;
    QWidget  *m_widget;
    QString   m_formName;
};

//! Standard Factory for all container widgets
class ContainerFactory : public KFormDesigner::WidgetFactory
{
    Q_OBJECT

public:
    ContainerFactory(QObject *parent, const QStringList &args);
    virtual ~ContainerFactory();

    virtual QWidget *createWidget(const QByteArray& classname, 
                                  QWidget *parent, const char *name, KFormDesigner::Container *container,
                                  int options = DefaultOptions);
    virtual bool createMenuActions(const QByteArray& classname, QWidget *w,
                                   QMenu *menu, KFormDesigner::Container *container);
    virtual bool startEditing(const QByteArray& classname, QWidget *w,
                              KFormDesigner::Container *container);
    virtual bool previewWidget(const QByteArray& classname, QWidget *widget,
                               KFormDesigner::Container *container);
    virtual bool saveSpecialProperty(const QByteArray& classname, const QString &name,
                                     const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent);
    virtual bool readSpecialProperty(const QByteArray& classname, QDomElement &node, QWidget *w,
                                     KFormDesigner::ObjectTreeItem *item);
    virtual QList<QByteArray> autoSaveProperties(const QByteArray &classname);

protected:
    virtual bool isPropertyVisibleInternal(const QByteArray &classname, QWidget *w,
                                           const QByteArray &property, bool isTopLevel);
    virtual bool changeText(const QString &newText);
    virtual void resizeEditor(QWidget *editor, QWidget *widget, const QByteArray &classname);

public slots:
    void addTabPage();
    void addStackPage();
    void renameTabPage();
    void removeTabPage();
    void removeStackPage();
    void prevStackPage();
    void nextStackPage();
    void reorderTabs(int oldpos, int newpos);

private:
//  QWidget *m_widget;
//  KFormDesigner::Container *m_container;
//  KFormDesigner::FormManager  *m_manager;
};

#endif
