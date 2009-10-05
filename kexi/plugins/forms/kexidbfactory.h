/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBFACTORY_H
#define KEXIDBFACTORY_H

#include <formeditor/widgetfactory.h>

class KAction;

namespace KFormDesigner
{
class Form;
}

//! Kexi Factory (DB widgets + subform)
class KexiDBFactory : public KFormDesigner::WidgetFactory
{
    Q_OBJECT

public:
    KexiDBFactory(QObject *parent, const QVariantList &);
    virtual ~KexiDBFactory();

    virtual QWidget *createWidget(const QByteArray &classname, QWidget *parent, const char *name,
                                  KFormDesigner::Container *container,
                                  CreateWidgetOptions options = DefaultOptions);

    virtual void createCustomActions(KActionCollection* col);
    virtual bool createMenuActions(const QByteArray &classname, QWidget *w, QMenu *menu,
                                   KFormDesigner::Container *container);
    virtual bool startInlineEditing(InlineEditorCreationArguments& args);
    virtual bool previewWidget(const QByteArray &, QWidget *, KFormDesigner::Container *);
    virtual bool clearWidgetContent(const QByteArray &classname, QWidget *w);

    //virtual void  saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
    //QDomElement &parentNode, QDomDocument &parent) {}
    //virtual void            readSpecialProperty(const QCString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item) {}
    virtual QList<QByteArray> autoSaveProperties(const QByteArray &classname);

protected slots:
    void slotImageBoxIdChanged(long id); /*KexiBLOBBuffer::Id_t*/

protected:
    virtual bool changeInlineText(KFormDesigner::Form *form, QWidget *widget, const QString &text);
    virtual void resizeEditor(QWidget *editor, QWidget *widget, const QByteArray &classname);

    virtual bool isPropertyVisibleInternal(const QByteArray& classname, QWidget *w,
                                           const QByteArray& property, bool isTopLevel);

    //! Sometimes property sets should be reloaded when a given property value changed.
    virtual bool propertySetShouldBeReloadedAfterPropertyChange(const QByteArray& classname, QWidget *w,
            const QByteArray& property);

    KAction* m_assignAction;
};

#endif
