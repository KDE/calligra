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

#ifndef CONTAINERFACTORY_H
#define CONTAINERFACTORY_H

#include "commands.h"
#include "widgetfactory.h"
#include "utils.h"
#include "FormWidgetInterface.h"

#include <QGroupBox>
#include <QMenu>

class QPaintEvent;

namespace KFormDesigner
{
class Form;
class Container;
}

//! Helper widget (used when using 'Lay out horizontally')
class HBox : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    HBox(QWidget *parent);
    virtual ~HBox();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out vertically')
class VBox : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    VBox(QWidget *parent);
    virtual ~VBox();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out in a grid')
class Grid : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    Grid(QWidget *parent);
    virtual ~Grid();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class HFlow : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    HFlow(QWidget *parent);
    virtual ~HFlow();
    virtual void paintEvent(QPaintEvent *ev);
};

//! Helper widget (used when using 'Lay out with horizontal flow')
class VFlow : public QFrame, public KFormDesigner::FormWidgetInterface
{
public:
    VFlow(QWidget *parent);
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

//! Action of adding tab to a tab widget
//! Keeps context expressed using container and receiver widget
class AddTabAction : public KAction
{
Q_OBJECT
public:
    AddTabAction(KFormDesigner::Container *container,
                 TabWidgetBase *receiver, QObject *parent);
public slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action of removing tab from a tab widget
//! Keeps context expressed using container and receiver widget
class RemoveTabAction : public KAction
{
Q_OBJECT
public:
    RemoveTabAction(KFormDesigner::Container *container,
                    TabWidgetBase *receiver, QObject *parent);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action renaming tab widget's tab
//! Keeps context expressed using container and receiver widget
class RenameTabAction : public KAction
{
Q_OBJECT
public:
    RenameTabAction(KFormDesigner::Container *container,
                    TabWidgetBase *receiver, QObject *parent);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    TabWidgetBase *m_receiver;
};

//! Action of adding page to a stacked widget
//! Keeps context expressed using container and receiver widget
class AddStackPageAction : public KAction
{
Q_OBJECT
public:
    AddStackPageAction(KFormDesigner::Container *container,
                       QWidget *receiver, QObject *parent);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
};

//! Action of removing page from a stacked widget
//! Keeps context expressed using container and receiver widget
class RemoveStackPageAction : public KAction
{
Q_OBJECT
public:
    RemoveStackPageAction(KFormDesigner::Container *container,
                          QWidget *receiver, QObject *parent);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
};

//! Action of moving between pages of a stacked widget
//! Keeps context expressed using container and receiver widget
class GoToStackPageAction : public KAction
{
Q_OBJECT
public:
    enum Direction {
        Previous,
        Next
    };
    GoToStackPageAction(Direction direction, KFormDesigner::Container *container,
                        QWidget *receiver, QObject *parent);
protected slots:
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
    
signals:
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

signals:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);
};

//! @todo SubForm
#if 0
//! A form embedded as a widget inside other form
class SubForm : public Q3ScrollView
{
    Q_OBJECT
    Q_PROPERTY(QString formName READ formName WRITE setFormName)

public:
    SubForm(KFormDesigner::Form *parentForm, QWidget *parent);
    ~SubForm();

    //! \return the name of the subform inside the db
    QString   formName() const {
        return m_formName;
    }
    void      setFormName(const QString &name);

private:
    KFormDesigner::Form   *m_form;
    KFormDesigner::Form   *m_parentForm;
    QWidget  *m_widget;
    QString   m_formName;
};
#endif //0

//! Standard Factory for all container widgets
class ContainerFactory : public KFormDesigner::WidgetFactory
{
    Q_OBJECT

public:
    ContainerFactory(QObject *parent, const QVariantList &args);
    virtual ~ContainerFactory();

    virtual QWidget* createWidget(const QByteArray &classname, QWidget *parent, const char *name,
                                  KFormDesigner::Container *container,
                                  CreateWidgetOptions options = DefaultOptions);
    virtual bool createMenuActions(const QByteArray& classname, QWidget *w,
                                   QMenu *menu, KFormDesigner::Container *container);
    virtual bool startInlineEditing(InlineEditorCreationArguments& args);
    virtual bool previewWidget(const QByteArray& classname, QWidget *widget,
                               KFormDesigner::Container *container);
    virtual bool saveSpecialProperty(const QByteArray& classname, const QString &name,
                                     const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent);
    virtual bool readSpecialProperty(const QByteArray& classname, QDomElement &node, QWidget *w,
                                     KFormDesigner::ObjectTreeItem *item);

    //! Reimplemented for pages of QTabWidget.
    virtual KFormDesigner::ObjectTreeItem* selectableItem(KFormDesigner::ObjectTreeItem* item);

protected:
    virtual bool isPropertyVisibleInternal(const QByteArray &classname, QWidget *w,
                                           const QByteArray &property, bool isTopLevel);
    virtual bool changeInlineText(KFormDesigner::Form *form, QWidget *widget,
                                  const QString &text, QString &oldText);
    virtual void resizeEditor(QWidget *editor, QWidget *widget, const QByteArray &classname);

public slots:
//moved to internal AddTabAction       void addTabPage();
//moved to internal RenameTabAction    void renameTabPage();
//moved to internal RemoveTabAction    void removeTabPage();
//moved to internal AddStackPageAction void addStackPage();
//moved to internal RemoveStackPageAction void removeStackPage();
//moved to internal GoToStackPageAction   void prevStackPage();
//moved to internal GoToStackPageAction   void nextStackPage();
    void reorderTabs(int oldpos, int newpos);
};

#endif
