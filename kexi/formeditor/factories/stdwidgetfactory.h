/* This file is part of the KDE project
   Copyright (C) 2003 by Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#include <QFrame>
#include <QPixmap>
#include <QLabel>
#include <QList>
#include <QMenu>

#include "widgetfactory.h"
#include "container.h"
#include "FormWidgetInterface.h"

class QTreeWidgetItem;
class QTreeWidget;

namespace KoProperty
{
class Set;
}

//! A picture label widget for use within forms
class KexiPictureLabel : public QLabel, public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT

public:
    KexiPictureLabel(const QPixmap &pix, QWidget *parent);
    virtual ~KexiPictureLabel();

    virtual bool setProperty(const char *name, const QVariant &value);
};

//! A line widget for use within forms
class Line : public QFrame, public KFormDesigner::FormWidgetInterface
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
    StdWidgetFactory(QObject *parent, const QVariantList &args);
    ~StdWidgetFactory();

    virtual QWidget* createWidget(const QByteArray &classname, QWidget *parent, const char *name,
                                  KFormDesigner::Container *container,
                                  CreateWidgetOptions options = DefaultOptions);

    virtual bool createMenuActions(const QByteArray &classname, QWidget *w,
                                   QMenu *menu, KFormDesigner::Container *container);
    virtual bool startInlineEditing(InlineEditorCreationArguments& args);
    virtual bool previewWidget(const QByteArray &classname, QWidget *widget,
                               KFormDesigner::Container *container);
    virtual bool clearWidgetContent(const QByteArray &classname, QWidget *w);

    virtual bool saveSpecialProperty(const QByteArray &classname,
                                     const QString &name, const QVariant &value, QWidget *w,
                                     QDomElement &parentNode, QDomDocument &parent);
    virtual bool readSpecialProperty(const QByteArray &classname, QDomElement &node,
                                     QWidget *w, KFormDesigner::ObjectTreeItem *item);
    virtual QList<QByteArray> autoSaveProperties(const QByteArray &classname);

    virtual void setPropertyOptions(KoProperty::Set& set, const KFormDesigner::WidgetInfo& info, QWidget *w);

    //! Moved into public for EditRichTextAction
    bool editRichText(QWidget *w, QString &text) const { return KFormDesigner::WidgetFactory::editRichText(w, text); }

    //! Moved into public for EditRichTextAction
    void changeProperty(KFormDesigner::Form *form, QWidget *widget, const char *name,
        const QVariant &value) { KFormDesigner::WidgetFactory::changeProperty(form, widget, name, value); }

public slots:
// moved to EditRichTextAction
//    void  editText();
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    void  editListContents();
#endif

protected:
    virtual bool isPropertyVisibleInternal(const QByteArray &classname, QWidget *w,
                                           const QByteArray &property, bool isTopLevel);
    virtual bool changeInlineText(KFormDesigner::Form *form, QWidget *widget,
                                  const QString &text, QString &oldText);
    virtual void resizeEditor(QWidget *editor, QWidget *widget, const QByteArray &classname);
#ifndef KEXI_FORMS_NO_LIST_WIDGET
    void saveTreeItem(QTreeWidgetItem *item, QDomNode &parentNode, QDomDocument &domDoc);
    void readTreeItem(QDomElement &node, QTreeWidgetItem *parent, QTreeWidget *treewidget);
#endif

private:
//  KFormDesigner::Container *m_container;
//  QWidget *m_widget;
};

#endif
