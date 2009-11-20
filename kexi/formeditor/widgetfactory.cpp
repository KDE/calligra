/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include "widgetfactory.h"

#include <kdebug.h>
#include <klocale.h>
//#ifdef KEXI_KTEXTEDIT
#include <ktextedit.h>
//#else
#include <klineedit.h>
//#endif
#include <kdialog.h>
#include <keditlistbox.h>
//2.0#include <kxmlguiclient.h>
#include <kactioncollection.h>

#include "richtextdialog.h"
#ifndef KEXI_FORMS_NO_LIST_WIDGET
# include "editlistviewdialog.h"
#endif
#include "resizehandle.h"
//#include "formmanager.h"
#include "form.h"
#include "container.h"
#include "objecttree.h"
#include "widgetlibrary.h"
#include "WidgetInfo.h"
#include "utils.h"
//removed #include "widgetpropertyset.h"
#include "widgetwithsubpropertiesinterface.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <kexiutils/utils.h>

using namespace KFormDesigner;

InternalPropertyHandlerInterface::InternalPropertyHandlerInterface()
{
}

InternalPropertyHandlerInterface::~InternalPropertyHandlerInterface()
{
}

///// InlineEditorCreationArguments //////////////////////////

WidgetFactory::InlineEditorCreationArguments::InlineEditorCreationArguments(
    const QByteArray& _classname, QWidget *_widget, Container *_container)
    : classname(_classname), widget(_widget), container(_container), 
      geometry(_widget ? _widget->geometry() : QRect()),
      alignment( Qt::AlignLeft ),
      useFrame( false ), multiLine( false ), execute( true ), transparentBackground( false )
{
}

///// Widget Factory //////////////////////////

WidgetFactory::WidgetFactory(QObject *parent, const char *name)
        : QObject(parent)
{
    setObjectName(QString("kformdesigner_") + name);
    m_showAdvancedProperties = true;
    m_hiddenClasses = 0;
//2.0    m_guiClient = 0;
}

WidgetFactory::~WidgetFactory()
{
    qDeleteAll(m_classesByName);
    delete m_hiddenClasses;
}

void WidgetFactory::addClass(WidgetInfo *w)
{
    WidgetInfo *oldw = m_classesByName.value(w->className());
    if (oldw == w)
        return;
    if (oldw) {
        kWarning() << "class with name '"
            << w->className()
            << "' already exists for factory '" << objectName() << "'";
        return;
    }
    m_classesByName.insert(w->className(), w);
}

void WidgetFactory::hideClass(const char *classname)
{
    if (!m_hiddenClasses)
        m_hiddenClasses = new QSet<QByteArray>;
    m_hiddenClasses->insert(QByteArray(classname).toLower());
}

const WidgetInfoHash& WidgetFactory::classes() const
{
    return m_classesByName;
}

void WidgetFactory::disableFilter(QWidget *w, Container *container)
{
    container->form()->disableFilter(w, container);
}

bool WidgetFactory::editList(QWidget *w, QStringList &list) const
{
    KDialog dialog(w->topLevelWidget());
    dialog.setObjectName("stringlist_dialog");
    dialog.setModal(true);
    dialog.setWindowTitle(i18n("Edit List of Items"));
    dialog.setButtons(KDialog::Ok | KDialog::Cancel);

    KEditListBox *edit = new KEditListBox(
        i18n("Contents of %1", w->objectName()), &dialog);
    edit->setObjectName("editlist");
    dialog.setMainWidget(edit);
    edit->insertStringList(list);

    if (dialog.exec() == QDialog::Accepted) {
        list = edit->items();
        return true;
    }
    return false;
}

bool WidgetFactory::editRichText(QWidget *w, QString &text) const
{
    RichTextDialog dlg(w, text);
    if (dlg.exec() == QDialog::Accepted) {
        text = dlg.text();
        return true;
    }
    return false;
}

#ifndef KEXI_FORMS_NO_LIST_WIDGET
void
WidgetFactory::editListWidget(QListWidget *listwidget) const
{
    EditListViewDialog dlg(((QWidget*)listwidget)->topLevelWidget());
//! @todo
    //dlg.exec(listview);
}
#endif

void WidgetFactory::changeProperty(Form *form, QWidget *widget, const char *name, const QVariant &value)
{
    if (form->selectedWidget()) { // single selection
        form->propertySet().changePropertyIfExists(name, value);
        widget->setProperty(name, value);
    }
    else {
        // If eg multiple labels are selected, 
        // we only want to change the text of one of them (the one the user cliked on)
        if (widget) {
            widget->setProperty(name, value);
        }
        else {
            form->selectedWidgets()->first()->setProperty(name, value);
        }
    }
}

/*
void
WidgetFactory::addPropertyDescription(Container *container, const char *prop, const QString &desc)
{
  WidgetPropertySet *buff = container->form()->manager()->buffer();
  buff->addPropertyDescription(prop, desc);
}

void
WidgetFactory::addValueDescription(Container *container, const char *value, const QString &desc)
{
  WidgetPropertySet *buff = container->form()->manager()->buffer();
  buff->addValueDescription(value, desc);
}*/

bool
WidgetFactory::isPropertyVisible(const QByteArray &classname, QWidget *w,
                                 const QByteArray &property, bool multiple, bool isTopLevel)
{
    if (multiple) {
        return property == "font" || property == "paletteBackgroundColor" || property == "enabled"
               || property == "paletteForegroundColor" || property == "cursor" 
               || property == "paletteBackgroundPixmap";
    }

    return isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

bool
WidgetFactory::isPropertyVisibleInternal(const QByteArray &, QWidget *w,
        const QByteArray &property, bool isTopLevel)
{
    Q_UNUSED(w);

#ifdef KEXI_NO_CURSOR_PROPERTY
//! @todo temporary unless cursor works properly in the Designer
    if (property == "cursor")
        return false;
#endif

    if (!isTopLevel
            && (property == "windowTitle" || property == "windowIcon" || property == "sizeIncrement" || property == "windowIconText")) {
        // don't show these properties for a non-toplevel widget
        return false;
    }
    return true;
}

bool
WidgetFactory::propertySetShouldBeReloadedAfterPropertyChange(
    const QByteArray& classname, QWidget *w, const QByteArray& property)
{
    Q_UNUSED(classname)
    Q_UNUSED(w)
    Q_UNUSED(property)
    return false;
}

void
WidgetFactory::resizeEditor(QWidget *, QWidget *, const QByteArray&)
{
}

bool
WidgetFactory::clearWidgetContent(const QByteArray &, QWidget *)
{
    return false;
}

bool WidgetFactory::changeInlineText(Form *form, QWidget *widget,
                                     const QString& text, QString &oldText)
{
    oldText = widget->property("text").toString();
    changeProperty(form, widget, "text", text);
    return true;
}

bool
WidgetFactory::readSpecialProperty(const QByteArray &, QDomElement &, QWidget *, ObjectTreeItem *)
{
    return false;
}

bool
WidgetFactory::saveSpecialProperty(const QByteArray &, const QString &, const QVariant&, QWidget *, QDomElement &,  QDomDocument &)
{
    return false;
}

bool WidgetFactory::inheritsFactories()
{
    foreach (WidgetInfo *winfo, m_classesByName) {
        if (!winfo->parentFactoryName().isEmpty())
            return true;
    }
    return false;
}

#if 0 // 2.0
void WidgetFactory::setEditor(QWidget *widget, QWidget *editor)
{
    if (!widget)
        return;
    WidgetInfo *winfo = m_classesByName.value(widget->metaObject()->className());
    if (!winfo || winfo->parentFactoryName().isEmpty()) {
        m_editor = editor;
    } else {
        WidgetFactory *f = m_library->factory(winfo->parentFactoryName());
        if (f != this)
            f->setEditor(widget, editor);
        m_editor = editor; //keep a copy
    }
}
#endif

#if 0 // 2.0
QWidget *WidgetFactory::editor(QWidget *widget) const
{
    if (!widget)
        return 0;
    WidgetInfo *winfo = m_classesByName.value(widget->metaObject()->className());
    if (!winfo || winfo->parentFactoryName().isEmpty()) {
        return m_editor;
    } else {
        WidgetFactory *f = m_library->factoryForClassName(
                               widget->metaObject()->className());
        if (f != this)
            return f->editor(widget);
        return m_editor;
    }
}
#endif

#if 0 // 2.0
void WidgetFactory::setWidget(QWidget *widget, Container* container)
{
    WidgetInfo *winfo = widget
        ? m_classesByName.value(widget->metaObject()->className()) : 0;
    if (winfo && !winfo->parentFactoryName().isEmpty()) {
        WidgetFactory *f = m_library->factory(winfo->parentFactoryName());
        if (f != this)
            f->setWidget(widget, container);
    }
    m_widget = widget; //keep a copy
    m_container = container;
}
#endif

#if 0 // 2.0
QWidget *WidgetFactory::widget() const
{
    return m_widget;
}
#endif

void WidgetFactory::setPropertyOptions(KoProperty::Set& set, const WidgetInfo& info, QWidget *w)
{
    Q_UNUSED(set)
    Q_UNUSED(info)
    Q_UNUSED(w)
    //nothing
}

#include "widgetfactory.moc"
