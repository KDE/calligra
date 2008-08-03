/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef STDWIDGETFACTORY_H
#define STDWIDGETFACTORY_H

#include <Q3Frame>
#include <QPixmap>
#include <Q3CString>
#include <QLabel>
#include <Q3ValueList>
#include <QMenu>
#include <K3ListViewItem>
#include <K3ListView>

#include "widgetfactory.h"
#include "container.h"

class KexiPictureLabel : public QLabel
{
    Q_OBJECT

public:
    KexiPictureLabel(const QPixmap &pix, QWidget *parent);
    virtual ~KexiPictureLabel();

    virtual bool setProperty(const char *name, const QVariant &value);
};

class Line : public Q3Frame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    Line(Qt::Orientation orient, QWidget *parent);
    virtual ~Line();

    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const;
};

//! Factory for all basic widgets, including Spring (not containers)
class StdWidgetFactory : public KFormDesigner::WidgetFactory
{
    Q_OBJECT

public:
    StdWidgetFactory(QObject *parent, const QStringList &args);
    ~StdWidgetFactory();

    virtual QWidget *createWidget(const Q3CString &c, QWidget *p, const char *n,
                                  KFormDesigner::Container *container, int options = DefaultOptions);

    virtual bool createMenuActions(const Q3CString &classname, QWidget *w,
                                   QMenu *menu, KFormDesigner::Container *container);
    virtual bool startEditing(const Q3CString &classname, QWidget *w,
                              KFormDesigner::Container *container);
    virtual bool previewWidget(const Q3CString &classname, QWidget *widget,
                               KFormDesigner::Container *container);
    virtual bool clearWidgetContent(const Q3CString &classname, QWidget *w);

    virtual bool saveSpecialProperty(const Q3CString &classname,
                                     const QString &name, const QVariant &value, QWidget *w,
                                     QDomElement &parentNode, QDomDocument &parent);
    virtual bool readSpecialProperty(const Q3CString &classname, QDomElement &node,
                                     QWidget *w, KFormDesigner::ObjectTreeItem *item);
    virtual Q3ValueList<Q3CString> autoSaveProperties(const Q3CString &classname);

    virtual void setPropertyOptions(KFormDesigner::WidgetPropertySet& buf,
                                    const KFormDesigner::WidgetInfo& info, QWidget *w);

public slots:
    void  editText();
    void  editListContents();

protected:
    virtual bool isPropertyVisibleInternal(const Q3CString &classname, QWidget *w,
                                           const Q3CString &property, bool isTopLevel);
    virtual bool changeText(const QString &newText);
    virtual void resizeEditor(QWidget *editor, QWidget *widget, const Q3CString &classname);
    void saveListItem(Q3ListViewItem *item, QDomNode &parentNode, QDomDocument &domDoc);
    void readListItem(QDomElement &node, Q3ListViewItem *parent, K3ListView *listview);

private:
//  KFormDesigner::Container *m_container;
//  QWidget *m_widget;
};

#endif
