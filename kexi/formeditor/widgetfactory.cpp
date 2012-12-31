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

#include <kdeversion.h>
#include <kdebug.h>
#include <klocale.h>

#include <ktextedit.h>

#include <klineedit.h>

#include <kdialog.h>
#if KDE_IS_VERSION(4,5,90)
# include <keditlistwidget.h>
#else
# include <keditlistbox.h>
#endif

#include <kactioncollection.h>

#include "richtextdialog.h"
#ifndef KEXI_FORMS_NO_LIST_WIDGET
# include "editlistviewdialog.h"
#endif
#include "resizehandle.h"

#include "form.h"
#include "container.h"
#include "objecttree.h"
#include "widgetlibrary.h"
#include "WidgetInfo.h"
#include "utils.h"

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

class WidgetFactory::Private
{
public:
    Private();
    ~Private();

    WidgetLibrary *library;

    WidgetInfoHash classesByName;
    QSet<QByteArray>* hiddenClasses;

    //! i18n stuff
    QHash<QByteArray, QString> propDesc;
    QHash<QByteArray, QString> propValDesc;
    //! internal properties
    QHash<QByteArray, QVariant> internalProp;

    /*! flag useful to decide whether to hide some properties.
     It's value is inherited from WidgetLibrary. */
    bool advancedPropertiesVisible;
};

WidgetFactory::Private::Private()
    : hiddenClasses(0), advancedPropertiesVisible(true)
{

}

WidgetFactory::Private::~Private()
{
    qDeleteAll(classesByName);
    delete hiddenClasses;
}

WidgetFactory::WidgetFactory(QObject *parent, const char *name)
    : QObject(parent), d(new Private())
{
    setObjectName(QString("kformdesigner_") + name);
}

WidgetFactory::~WidgetFactory()
{
    delete d;
}

void WidgetFactory::addClass(WidgetInfo *w)
{
    WidgetInfo *oldw = d->classesByName.value(w->className());
    if (oldw == w)
        return;
    if (oldw) {
        kWarning() << "class with name '"
            << w->className()
            << "' already exists for factory '" << objectName() << "'";
        return;
    }
    d->classesByName.insert(w->className(), w);
}

void WidgetFactory::hideClass(const char *classname)
{
    if (!d->hiddenClasses)
        d->hiddenClasses = new QSet<QByteArray>;
    d->hiddenClasses->insert(QByteArray(classname).toLower());
}

const WidgetInfoHash& WidgetFactory::classes() const
{
    return d->classesByName;
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
    dialog.setWindowTitle(i18n("Edit Contents of %1", w->objectName()));
    dialog.setButtons(KDialog::Ok | KDialog::Cancel);

#if KDE_IS_VERSION(4,5,90)
    KEditListWidget *edit = new KEditListWidget(&dialog);
#else
    KEditListBox *edit = new KEditListBox(&dialog);
#endif
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
    if (property == "acceptDrops" || property == "inputMethodHints")
        return false;

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
    foreach (WidgetInfo *winfo, d->classesByName) {
        if (!winfo->parentFactoryName().isEmpty())
            return true;
    }
    return false;
}

void WidgetFactory::setPropertyOptions(KoProperty::Set& set, const WidgetInfo& info, QWidget *w)
{
    Q_UNUSED(set)
    Q_UNUSED(info)
    Q_UNUSED(w)
    //nothing
}

ObjectTreeItem* WidgetFactory::selectableItem(ObjectTreeItem* item)
{
    return item;
}

void WidgetFactory::setInternalProperty(const QByteArray &classname, const QByteArray &property, const QVariant &value)
{
    d->internalProp.insert(classname + ":" + property, value);
}

QVariant WidgetFactory::internalProperty(const QByteArray &classname, const QByteArray &property) const
{
    return d->internalProp.value(classname + ":" + property);
}

QString WidgetFactory::propertyDescription(const char* name) const
{
    return d->propDesc.value(name);
}

QString WidgetFactory::valueDescription(const char* name) const
{
    return d->propValDesc.value(name);
}

WidgetInfo* WidgetFactory::widgetInfoForClassName(const char* classname)
{
    return d->classesByName.value(classname);
}

const QSet<QByteArray> *WidgetFactory::hiddenClasses() const
{
    return d->hiddenClasses;
}

WidgetLibrary* WidgetFactory::library()
{
    return d->library;
}

bool WidgetFactory::advancedPropertiesVisible() const
{
    return d->advancedPropertiesVisible;
}

void WidgetFactory::setLibrary(WidgetLibrary* library)
{
    d->library = library;
}

void WidgetFactory::setAdvancedPropertiesVisible(bool set)
{
    d->advancedPropertiesVisible = set;
}

void WidgetFactory::setPropertyDescription(const char* property, const QString &description)
{
    d->propDesc.insert(property, description);
}

void WidgetFactory::setValueDescription(const char *valueName, const QString &description)
{
    d->propValDesc.insert(valueName, description);
}

#include "widgetfactory.moc"
