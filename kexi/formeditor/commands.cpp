/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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
#include <qdom.h>
#include <QLayout>
#include <QLabel>
#include <QSplitter>
#include <QMetaObject>
#include <QClipboard>
#include <QApplication>

#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kacceleratormanager.h>

#include "formIO.h"
#include "container.h"
#include "objecttree.h"
//unused #include "formmanager.h"
#include "form.h"
#include "widgetlibrary.h"
#include "events.h"
#include "utils.h"
//removed 2.0 #include "widgetpropertyset.h"
#include "widgetwithsubpropertiesinterface.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <kexiutils/utils.h>

#include "commands.h"

#include <memory>
#include <limits.h>

using namespace KFormDesigner;

// Command

Command::Command()
        : K3Command()
{
}

Command::~Command()
{
}

// PropertyCommand

namespace KFormDesigner
{
class PropertyCommand::Private
{
public:
    Private()
    {
    }

    //KoProperty::Set *propertySet;
    Form *form;
    QVariant value;
    QHash<QByteArray, QVariant> oldValues;
    QByteArray propertyName;
};
}

PropertyCommand::PropertyCommand(Form& form, const QByteArray &wname,
                                 const QVariant &oldValue, const QVariant &value, const QByteArray &propertyName)
        : Command(), d( new Private )
{
    d->form = &form;
    d->value = value;
    d->propertyName = propertyName;
    d->oldValues.insert(wname, oldValue);
}

PropertyCommand::PropertyCommand(Form& form, const QHash<QByteArray, QVariant> &oldValues,
                                 const QVariant &value, const QByteArray &propertyName)
        : Command(), d( new Private )
{
    d->form = &form;
    d->value = value;
    d->propertyName = propertyName;
    d->oldValues = oldValues;
}

PropertyCommand::~PropertyCommand()
{
    delete d;
}

QVariant PropertyCommand::value() const
{
    return d->value;
}

void PropertyCommand::setValue(const QVariant &value)
{
    d->value = value;
//moved to Form::slotPropertyChanged():    emit d->form->modified();
}

void PropertyCommand::execute()
{
    d->form->selectFormWidget();
    d->form->setUndoing(true);

    foreach (const QByteArray& name, d->oldValues.keys()) {
        ObjectTreeItem* item = d->form->objectTree()->lookup(name);
        if (item) { //we're checking for item!=0 because the name could be of a form widget
            d->form->selectWidget(item->widget(), Form::AddToPreviousSelection | Form::LastSelection);
        }
    }

    d->form->propertySet().changeProperty(d->propertyName, d->value);
    d->form->setUndoing(false);
}

void PropertyCommand::unexecute()
{
    d->form->selectFormWidget();
    d->form->setUndoing(true);

    QHash<QByteArray, QVariant>::ConstIterator endIt = d->oldValues.constEnd();
    for (QHash<QByteArray, QVariant>::ConstIterator it = d->oldValues.constBegin(); it != endIt; ++it) {
        ObjectTreeItem* item = d->form->objectTree()->lookup(it.key());
        if (!item)
            continue; //better this than a crash
        QWidget *widget = item->widget();
        d->form->selectWidget(widget, Form::AddToPreviousSelection | Form::LastSelection | Form::Raise);

        WidgetWithSubpropertiesInterface* subpropIface = dynamic_cast<WidgetWithSubpropertiesInterface*>(widget);
        QWidget *subWidget = (subpropIface && subpropIface->subwidget()) ? subpropIface->subwidget() : widget;
        if (-1 != KexiUtils::indexOfPropertyWithSuperclasses(subWidget, d->propertyName))
            subWidget->setProperty(d->propertyName, it.value());
    }

    d->form->propertySet().changeProperty(d->propertyName, d->oldValues.constBegin().value());
    d->form->setUndoing(false);
}

QString PropertyCommand::name() const
{
    if (d->oldValues.count() >= 2)
        return i18n("Change \"%1\" property for multiple widgets", QString(d->propertyName));
    else
        return i18n("Change \"%1\" property for widget \"%2\"",
                    QString(d->propertyName), QString(d->oldValues.constBegin().key()));
}

QByteArray PropertyCommand::property() const
{
    return d->propertyName;
}

const QHash<QByteArray, QVariant>& PropertyCommand::oldValues() const
{
    return d->oldValues;
}

void PropertyCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" widgets=" << d->oldValues.keys()
        << " value=" << d->value << " oldValues=" << d->oldValues.values();
}

// GeometryPropertyCommand (for multiple widgets)

namespace KFormDesigner
{
class GeometryPropertyCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QStringList names;
    QPoint pos;
    QPoint oldPos;
};
}

GeometryPropertyCommand::GeometryPropertyCommand(Form& form,
        const QStringList &names, const QPoint& oldPos)
        : Command(), d( new Private )
{
    d->form = &form;
    d->names = names;
    d->oldPos = oldPos;
}

GeometryPropertyCommand::~GeometryPropertyCommand()
{
    delete d;
}

void GeometryPropertyCommand::execute()
{
    d->form->setUndoing(true);
    int dx = d->pos.x() - d->oldPos.x();
    int dy = d->pos.y() - d->oldPos.y();

    // We move every widget in our list by (dx, dy)
    foreach (const QString& widgetName, d->names) {
        ObjectTreeItem* item = d->form->objectTree()->lookup(widgetName);
        if (!item)
            continue; //better this than a crash
        QWidget *w = item->widget();
        w->move(w->x() + dx, w->y() + dy);
    }
    d->form->setUndoing(false);
}

void GeometryPropertyCommand::unexecute()
{
    d->form->setUndoing(true);
    int dx = d->pos.x() - d->oldPos.x();
    int dy = d->pos.y() - d->oldPos.y();

    // We move every widget in our list by (-dx, -dy) to undo the move
    foreach (const QString& widgetName, d->names) {
        ObjectTreeItem* item = d->form->objectTree()->lookup(widgetName);
        if (!item)
            continue; //better this than a crash
        QWidget *w = item->widget();
        w->move(w->x() - dx, w->y() - dy);
    }
    d->form->setUndoing(false);
}

void GeometryPropertyCommand::setPos(const QPoint& pos)
{
    d->pos = pos;
// moved    emit d->form->modified();
}

QString GeometryPropertyCommand::name() const
{
    return i18n("Move multiple widgets");
}

void GeometryPropertyCommand::debug()
{
    kDebug() << "pos=" << d->pos << " oldPos=" << d->oldPos
        << " widgets=" << d->names;
}

/////////////////  AlignWidgetsCommand  ////////

namespace KFormDesigner
{
class AlignWidgetsCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    Form::WidgetAlignment alignment;
    QHash<QByteArray, QPoint> pos;
};
}

AlignWidgetsCommand::AlignWidgetsCommand(Form &form, Form::WidgetAlignment alignment, const QWidgetList &list)
        : Command(), d( new Private )
{
    d->form = &form;
    d->alignment = alignment;
    foreach (QWidget *w, list) {
        d->pos.insert(w->objectName().toLatin1().constData(), w->pos());
    }
}

AlignWidgetsCommand::~AlignWidgetsCommand()
{
    delete d;
}

void AlignWidgetsCommand::execute()
{
    // To avoid creation of GeometryPropertyCommand
    d->form->selectFormWidget();

    QWidgetList list;
    foreach (const QByteArray& name, d->pos.keys()) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(name);
        if (item && item->widget())
            list.append(item->widget());
    }

    const int gridX = d->form->gridSize();
    const int gridY = d->form->gridSize();
    QWidget *parentWidget = d->form->selectedWidgets()->first()->parentWidget();

    switch (d->alignment) {
    case Form::AlignToGrid: {
        foreach (QWidget *w, list) {
            const int tmpx = alignValueToGrid(w->x(), gridX);
            const int tmpy = alignValueToGrid(w->y(), gridY);
            if ((tmpx != w->x()) || (tmpy != w->y()))
                w->move(tmpx, tmpy);
        }
        break;
    }
    case Form::AlignToLeft: {
        int tmpx = parentWidget->width();
        foreach (QWidget *w, list) {
            if (w->x() < tmpx)
                tmpx = w->x();
        }
        foreach (QWidget *w, list) {
            w->move(tmpx, w->y());
        }
        break;
    }
    case Form::AlignToRight: {
        int tmpx = 0;
        foreach (QWidget *w, list) {
            if (w->x() + w->width() > tmpx)
                tmpx = w->x() + w->width();
        }
        foreach (QWidget *w, list) {
            w->move(tmpx - w->width(), w->y());
        }
        break;
    }
    case Form::AlignToTop: {
        int tmpy = parentWidget->height();
        foreach (QWidget *w, list) {
            if (w->y() < tmpy)
                tmpy = w->y();
        }
        foreach (QWidget *w, list) {
            w->move(w->x(), tmpy);
        }
        break;
    }
    case Form::AlignToBottom: {
        int tmpy = 0;
        foreach (QWidget *w, list) {
            if (w->y() + w->height() > tmpy)
                tmpy = w->y() + w->height();
        }
        foreach (QWidget *w, list) {
            w->move(w->x(), tmpy - w->height());
        }
        break;
    }
    default:
        return;
    }

    // We restore selection
    foreach (QWidget *w, list) {
        d->form->selectWidget(w, Form::AddToPreviousSelection | Form::LastSelection | Form::Raise);
    }
}

void AlignWidgetsCommand::unexecute()
{
    // To avoid creation of GeometryPropertyCommand
    d->form->selectFormWidget();
    // We move widgets to their original pos
    QHash<QByteArray, QPoint>::ConstIterator endIt = d->pos.constEnd();
    for (QHash<QByteArray, QPoint>::ConstIterator it = d->pos.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (item && item->widget())
            item->widget()->move(d->pos.value(item->widget()->objectName().toLatin1().constData()));
        // we restore selection
        d->form->selectWidget(item->widget(), Form::AddToPreviousSelection | Form::LastSelection | Form::Raise);
    }
}

QString AlignWidgetsCommand::name() const
{
    switch (d->alignment) {
    case Form::AlignToGrid:
        return i18n("Align Widgets to Grid");
    case Form::AlignToLeft:
        return i18n("Align Widgets to Left");
    case Form::AlignToRight:
        return i18n("Align Widgets to Right");
    case Form::AlignToTop:
        return i18n("Align Widgets to Top");
    case Form::AlignToBottom:
        return i18n("Align Widgets to Bottom");
    default:;
    }
    return QString();
}

void AlignWidgetsCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" form=" << d->form->widget()->objectName()
        << " widgets=" << d->pos.keys();
}

///// AdjustSizeCommand ///////////

namespace KFormDesigner
{
class AdjustSizeCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    AdjustSizeCommand::Adjustment type;
    QHash<QByteArray, QPoint> pos;
    QHash<QByteArray, QSize> sizes;
};
}

AdjustSizeCommand::AdjustSizeCommand(Form& form, Adjustment type, const QWidgetList &list)
        : Command(), d( new Private )
{
    d->form = &form;
    d->type = type;
    foreach (QWidget *w, list) {
        if (w->parentWidget() && KexiUtils::objectIsA(w->parentWidget(), "QStackedWidget")) {
            w = w->parentWidget(); // widget is WidgetStack page
            if (w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
                w = w->parentWidget();
        }

        d->sizes.insert(w->objectName().toLatin1().constData(), w->size());
        if (d->type == SizeToGrid) // SizeToGrid also move widgets
            d->pos.insert(w->objectName().toLatin1().constData(), w->pos());
    }
}

AdjustSizeCommand::~AdjustSizeCommand()
{
    delete d;
}

void AdjustSizeCommand::execute()
{
    // To avoid creation of GeometryPropertyCommand
    d->form->selectFormWidget();

    int gridX = d->form->gridSize();
    int gridY = d->form->gridSize();
    int tmpw = 0, tmph = 0;

    QWidgetList list;
    QHash<QByteArray, QSize>::ConstIterator endIt = d->sizes.constEnd();
    for (QHash<QByteArray, QSize>::ConstIterator it = d->sizes.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (item && item->widget())
            list.append(item->widget());
    }

    switch (d->type) {
    case SizeToGrid: {
        int tmpx = 0, tmpy = 0;
        // same as in 'Align to Grid' + for the size
        foreach (QWidget *w, list) {
            tmpx = alignValueToGrid(w->x(), gridX);
            tmpy = alignValueToGrid(w->y(), gridY);
            tmpw = alignValueToGrid(w->width(), gridX);
            tmph = alignValueToGrid(w->height(), gridY);
            if ((tmpx != w->x()) || (tmpy != w->y()))
                w->move(tmpx, tmpy);
            if ((tmpw != w->width()) || (tmph != w->height()))
                w->resize(tmpw, tmph);
        }
        break;
    }

    case SizeToFit: {
        foreach (QWidget *w, list) {
            ObjectTreeItem *item = d->form->objectTree()->lookup(w->objectName());
            if (item && !item->children()->isEmpty()) { // container
                QSize s;
                if (item->container() && item->container()->layout())
                    s = w->sizeHint();
                else
                    s = getSizeFromChildren(item);
                // minimum size for containers
                if (s.width()  <  30)
                    s.setWidth(30);
                if (s.height() < 30)
                    s.setHeight(30);
                // small hack for flow layouts
                int type = item->container() ? item->container()->layoutType() : Form::NoLayout;
                if (type == Form::HFlow)
                    s.setWidth(s.width() + 5);
                else if (type == Form::VFlow)
                    s.setHeight(s.height() + 5);
                w->resize(s);
            } else if (item && item->container()) // empty container
                w->resize(item->container()->form()->gridSize() * 5, item->container()->form()->gridSize() * 5); // basic size
            else {
                QSize sizeHint(w->sizeHint());
                if (sizeHint.isValid())
                    w->resize(sizeHint);
            }
        }
        break;
    }

    case SizeToSmallWidth: {
        foreach (QWidget *w, list) {
            if ((tmpw == 0) || (w->width() < tmpw))
                tmpw = w->width();
        }

        foreach (QWidget *w, list) {
            if (tmpw != w->width())
                w->resize(tmpw, w->height());
        }
        break;
    }

    case SizeToBigWidth: {
        foreach (QWidget *w, list) {
            if (w->width() > tmpw)
                tmpw = w->width();
        }

        foreach (QWidget *w, list) {
            if (tmpw != w->width())
                w->resize(tmpw, w->height());
        }
        break;
    }

    case SizeToSmallHeight: {
        foreach (QWidget *w, list) {
            if ((tmph == 0) || (w->height() < tmph))
                tmph = w->height();
        }

        foreach (QWidget *w, list) {
            if (tmph != w->height())
                w->resize(w->width(), tmph);
        }
        break;
    }

    case SizeToBigHeight: {
        foreach (QWidget *w, list) {
            if (w->height() > tmph)
                tmph = w->height();
        }

        foreach (QWidget *w, list) {
            if (tmph != w->height())
                w->resize(w->width(), tmph);
        }
        break;
    }

    default:
        break;
    }

    // We restore selection
    foreach (QWidget *w, list) {
        d->form->selectWidget(w, Form::AddToPreviousSelection | Form::LastSelection | Form::Raise);
    }
}

QSize AdjustSizeCommand::getSizeFromChildren(ObjectTreeItem *item)
{
    if (!item->container()) { // multi pages containers (eg tabwidget)
        QSize s;
        // get size for each container, and keep the biggest one
        foreach (ObjectTreeItem *titem, *item->children()) {
            s = s.expandedTo(getSizeFromChildren(titem));
        }
        return s;
    }

    int tmpw = 0, tmph = 0;
    foreach (ObjectTreeItem *titem, *item->children()) {
        if (!titem->widget())
            continue;
        tmpw = qMax(tmpw, titem->widget()->geometry().right());
        tmph = qMax(tmph, titem->widget()->geometry().bottom());
    }

    return QSize(tmpw, tmph) + QSize(10, 10);
}

void AdjustSizeCommand::unexecute()
{
    // To avoid creation of GeometryPropertyCommand
    d->form->selectFormWidget();
    // We resize widgets to their original size
    QHash<QByteArray, QSize>::ConstIterator endIt = d->sizes.constEnd();
    for (QHash<QByteArray, QSize>::ConstIterator it = d->sizes.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (item && item->widget()) {
            item->widget()->resize(d->sizes[item->widget()->objectName().toLatin1().constData()]);
            if (d->type == SizeToGrid)
                item->widget()->move(d->pos[item->widget()->objectName().toLatin1().constData()]);
            d->form->selectWidget(item->widget(), 
                Form::AddToPreviousSelection | Form::LastSelection | Form::Raise); // restore selection
        }
    }
}

QString AdjustSizeCommand::name() const
{
    switch (d->type) {
    case SizeToGrid:
        return i18n("Resize Widgets to Grid");
    case SizeToFit:
        return i18n("Resize Widgets to Fit Contents");
    case SizeToSmallWidth:
        return i18n("Resize Widgets to Narrowest");
    case SizeToBigWidth:
        return i18n("Resize Widgets to Widest");
    case SizeToSmallHeight:
        return i18n("Resize Widgets to Shortest");
    case SizeToBigHeight:
        return i18n("Resize Widgets to Tallest");
    default:;
    }
    return QString();
}

void AdjustSizeCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" form="
        << d->form->widget()->objectName() << " widgets=" << d->sizes.keys();
}

// LayoutPropertyCommand

namespace KFormDesigner
{
class LayoutPropertyCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QHash<QByteArray, QRect> geometries;
};
}

LayoutPropertyCommand::LayoutPropertyCommand(Form& form, const QByteArray &wname,
        const QVariant &oldValue, const QVariant &value)
        : PropertyCommand(form, wname, oldValue, value, "layout")
        , d( new Private )
{
    ObjectTreeItem* titem = d->form->objectTree()->lookup(wname);
    if (!titem)
        return; //better this than a crash
    Container *container = titem->container();
    // We save the geometry of each wigdet
    foreach (ObjectTreeItem *titem, *container->objectTree()->children()) {
        d->geometries.insert(titem->name().toLatin1(), titem->widget()->geometry());
    }
}

LayoutPropertyCommand::~LayoutPropertyCommand()
{
    delete d;
}

void LayoutPropertyCommand::execute()
{
    PropertyCommand::execute();
}

void
LayoutPropertyCommand::unexecute()
{
    ObjectTreeItem* titem = d->form->objectTree()->lookup(oldValues().constBegin().key());
    if (!titem)
        return; //better this than a crash
    Container *container = titem->container();
    container->setLayoutType(Form::NoLayout);
    // We put every widget back in its old location
    QHash<QByteArray, QRect>::ConstIterator endIt = d->geometries.constEnd();
    for (QHash<QByteArray, QRect>::ConstIterator it = d->geometries.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *tree = container->form()->objectTree()->lookup(it.key());
        if (tree)
            tree->widget()->setGeometry(it.value());
    }

    PropertyCommand::unexecute();
}

QString LayoutPropertyCommand::name() const
{
    return i18n("Change layout of widget \"%1\"", QString(oldValues().constBegin().key()));
}

void LayoutPropertyCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" oldValues=" << oldValues().keys()
        << " value=" << value();
}

// InsertWidgetCommand

namespace KFormDesigner
{
class InsertWidgetCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QString containerName;
    QPoint pos;
    QByteArray widgetName;
    QByteArray _class;
    QRect insertRect;
};
}

InsertWidgetCommand::InsertWidgetCommand(const Container& container)
        : Command(), d( new Private )
{
    d->form = container.form();
    d->containerName = container.widget()->objectName();
    d->_class = d->form->selectedClass();
    d->pos = container.selectionOrInsertingBegin();
    d->widgetName = d->form->objectTree()->generateUniqueName(
                 d->form->library()->namePrefix(d->_class).toLatin1(),
                 /* !numberSuffixRequired */false);
    d->insertRect = container.selectionOrInsertingRectangle();
}

InsertWidgetCommand::InsertWidgetCommand(const Container& container,
        const QByteArray& className, const QPoint& pos, const QByteArray& namePrefix)
        : Command(), d( new Private )
{
    d->form = container.form();
    d->containerName = container.widget()->objectName();
    d->_class = className;
    d->pos = pos;
    //d->insertRect is null (default)
    if (namePrefix.isEmpty()) {
        d->widgetName = d->form->objectTree()->generateUniqueName(
                     d->form->library()->namePrefix(d->_class).toLatin1());
    } else {
        d->widgetName = d->form->objectTree()->generateUniqueName(
                     namePrefix, false /* !numberSuffixRequired */);
    }
}

InsertWidgetCommand::~InsertWidgetCommand()
{
    delete d;
}

void InsertWidgetCommand::execute()
{
    if (!d->form->objectTree())
        return;
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->containerName);
    if (!titem)
        return; //better this than a crash
    Container *container = titem->container();
    WidgetFactory::CreateWidgetOptions options = WidgetFactory::DesignViewMode | WidgetFactory::AnyOrientation;
    if (d->form->library()->internalProperty(d->_class, "orientationSelectionPopup") == "1") {
        if (d->insertRect.isValid()) {
            if (d->insertRect.width() < d->insertRect.height()) {
                options |= WidgetFactory::VerticalOrientation;
                options ^= WidgetFactory::AnyOrientation;
            } else if (d->insertRect.width() > d->insertRect.height()) {
                options |= WidgetFactory::HorizontalOrientation;
                options ^= WidgetFactory::AnyOrientation;
            }
        }
        if (options & WidgetFactory::AnyOrientation) {
            options ^= WidgetFactory::AnyOrientation;
            options |= d->form->library()->showOrientationSelectionPopup(
                           d->_class, container->widget(),
                           d->form->widget()->mapToGlobal(d->pos));
            if (options & WidgetFactory::AnyOrientation)
                return; //cancelled
        }
    } else
        options |= WidgetFactory::AnyOrientation;

    QWidget *w = d->form->library()->createWidget(d->_class, container->widget(), d->widgetName,
                                                  container, options);

    if (!w) {
        d->form->abortWidgetInserting();
        WidgetInfo *winfo = d->form->library()->widgetInfoForClassName(d->_class);
        KMessageBox::sorry(d->form ? d->form->widget() : 0,
                           i18n("Could not insert widget of type \"%1\". A problem with widget's creation encountered.", 
                                winfo ? winfo->name() : QString()));
        kWarning() << "ERROR: widget creation failed";
        return;
    }
//! @todo allow setting this for data view mode as well
    if (d->form->mode() == Form::DesignMode) {
        //don't generate accelerators for widgets in design mode
        KAcceleratorManager::setNoAccel(w);
    }

//    w->installEventFilter(container);

    // if the insertRect is invalid (ie only one point), we use widget' size hint
    if (((d->insertRect.width() < 21) && (d->insertRect.height() < 21))) {
        QSize s = w->sizeHint();

        if (s.isEmpty())
            s = QSize(20, 20); // Minimum size to avoid creating a (0,0) widget
        int x, y;
        if (d->insertRect.isValid()) {
            x = d->insertRect.x();
            y = d->insertRect.y();
        } else {
            x = d->pos.x();
            y = d->pos.y();
        }
        d->insertRect = QRect(x, y, s.width() + 16/* add some space so more text can be entered*/,
                             s.height());
    }

    // fix widget size is align-to-grid is enabled
    if (d->form->isSnapWidgetsToGridEnabled()) {
        const int grid = d->form->gridSize();
        d->insertRect.setWidth( alignValueToGrid(d->insertRect.width(), grid) );
        d->insertRect.setHeight( alignValueToGrid(d->insertRect.height(), grid) );
    }

    w->move(d->insertRect.x(), d->insertRect.y());
//    w->resize(d->insertRect.width() - 1, d->insertRect.height() - 1); // -1 is not to hide dots
    w->resize(d->insertRect.size());
    w->setStyle(container->widget()->style());
//2.0 not needed    w->setBackgroundOrigin(QWidget::ParentOrigin);
    w->show();

    d->form->abortWidgetInserting();

    // ObjectTreeItem object already exists for widgets which corresponds to a Container
    // it's already created in Container's constructor
    ObjectTreeItem *item = d->form->objectTree()->lookup(d->widgetName);
    if (!item) { //not yet created...
        item = new ObjectTreeItem(d->form->library()->displayName(d->_class), d->widgetName, w, container);
        d->form->objectTree()->addItem(container->objectTree(), item);
    }
    //assign item for its widget if it supports DesignTimeDynamicChildWidgetHandler interface
    //(e.g. KexiDBAutoField)
    if (d->form->mode() == Form::DesignMode && dynamic_cast<DesignTimeDynamicChildWidgetHandler*>(w)) {
        dynamic_cast<DesignTimeDynamicChildWidgetHandler*>(w)->assignItem(item);
    }

    // We add the autoSaveProperties in the modifProp list of the ObjectTreeItem, so that they are saved later
    QList<QByteArray> list(
        d->form->library()->autoSaveProperties(
           w->metaObject()->className())
    );
    foreach (const QByteArray& name, list) {
        item->addModifiedProperty(name, w->property(name));
    }

    container->reloadLayout(); // reload the layout to take the new wigdet into account

    container->selectWidget(w);
    if (d->form->library()->internalProperty(w->metaObject()->className(),
            "dontStartEditingOnInserting").isEmpty()) {
        d->form->library()->startEditing(
            w->metaObject()->className(), w, item->container() ? item->container() : container); // we edit the widget on creation
    }
//! @todo update widget's width for entered text's metrics
    kDebug() << "widget added " << this;
}

void InsertWidgetCommand::unexecute()
{
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->widgetName);
    if (!titem)
        return; //better this than a crash
    QWidget *widget = titem->widget();
    Container *container = d->form->objectTree()->lookup(d->containerName)->container();
    container->deleteWidget(widget);
}

QString InsertWidgetCommand::name() const
{
    if (!d->widgetName.isEmpty())
        return i18n("Insert widget \"%1\"", QString(d->widgetName));
    else
        return i18n("Insert widget");
}

void InsertWidgetCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" generatedName=" << d->widgetName
        << " container=" << d->containerName
        << " form=" << d->form->widget()->objectName() << " class=" << d->_class
        << " rect=" << d->insertRect << " pos=" << d->pos;
}

QByteArray InsertWidgetCommand::widgetName() const {
    return d->widgetName;
}

/// CreateLayoutCommand ///////////////

namespace KFormDesigner
{
class CreateLayoutCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QString containerName;
    QString name;
    QHash<QByteArray, QRect> pos;
    Form::LayoutType layoutType;
};
}

CreateLayoutCommand::CreateLayoutCommand(Form &form, Form::LayoutType layoutType, const QWidgetList &list)
        : Command(), d( new Private )
{
    d->form = &form;
    d->layoutType = layoutType;
    CustomSortableWidgetList *realList = 0;

    switch (d->layoutType) {
    case Form::NoLayout:
    case Form::HBox:
    case Form::Grid:
    case Form::HSplitter:
    case Form::HFlow:
        realList = new HorizontalWidgetList(d->form->toplevelContainer()->widget()); break;
    case Form::VBox:
    case Form::VSplitter:
    case Form::VFlow:
        realList = new VerticalWidgetList(d->form->toplevelContainer()->widget()); break;
    }
    
    foreach (QWidget *w, list) {
        realList->append(w);
    }
    realList->sort(); // we sort them now, before creating the layout

    foreach (QWidget *w, *realList) {
        d->pos.insert(w->objectName().toLatin1().constData(), w->geometry());
    }
    ObjectTreeItem *item = 0;
    if (!realList->isEmpty()) {
        d->form->objectTree()->lookup(realList->first()->objectName());
    }
    if (item && item->parent()->container()) {
        d->containerName = item->parent()->name();
    }
    delete realList;
}

CreateLayoutCommand::CreateLayoutCommand()
        : Command(), d( new Private )
{
    // d will be initialized in subclass
}

CreateLayoutCommand::~CreateLayoutCommand()
{
    delete d;
}

void CreateLayoutCommand::execute()
{
    WidgetLibrary *lib = d->form->library();
    if (!lib)
        return;
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->containerName);
    Container *container = titem ? titem->container() : 0;
    if (!container)
        container = d->form->toplevelContainer(); // use toplevelContainer by default

    QByteArray classname;
    switch (d->layoutType)  {
    case Form::HSplitter:
    case Form::VSplitter:
        classname = "QSplitter";
        break;
    default:
        classname = Container::layoutTypeToString(d->layoutType).toLatin1();
    }

    if (d->name.isEmpty())// the name must be generated only once
        d->name = d->form->objectTree()->generateUniqueName(classname);

    QWidget *w = lib->createWidget(classname, container->widget(), d->name.toLatin1(), container);
//! @todo allow setting this for data view mode as well
    if (w) {
        if (d->form->mode() == Form::DesignMode) {
            //don't generate accelerators for widgets in design mode
            KAcceleratorManager::setNoAccel(w);
        }
    }
    ObjectTreeItem *tree = w ? d->form->objectTree()->lookup(w->objectName()) : 0;
    if (!tree)
        return;

    container->selectWidget(0);
    w->move(d->pos.constBegin().value().topLeft()); // we move the layout at the position of the topleft widget
    // sizeHint of these widgets depends on geometry, so give them appropriate geometry
    if (d->layoutType == Form::HFlow)
        w->resize(QSize(700, 20));
    else if (d->layoutType == Form::VFlow)
        w->resize(QSize(20, 700));
    w->show();

    // We reparent every widget to the Layout and insert them into it
    QHash<QByteArray, QRect>::ConstIterator endIt = d->pos.constEnd();
    for (QHash<QByteArray, QRect>::ConstIterator it = d->pos.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (item && item->widget()) {
            item->widget()->setParent(w);
            item->eventEater()->setContainer(tree->container());
            d->form->objectTree()->reparent(item->name(), d->name);
        }
    }

    if (d->layoutType == Form::HSplitter)
        ((QSplitter*)w)->setOrientation(Qt::Horizontal);
    else if (d->layoutType == Form::VSplitter)
        ((QSplitter*)w)->setOrientation(Qt::Vertical);
    else if (tree->container()) {
        tree->container()->setLayoutType(d->layoutType);
        w->resize(tree->container()->layout()->sizeHint()); // the layout doesn't have its own size
    }

    container->selectWidget(w);
//! @todo 2.0 unused?
    //FormManager::self()->windowChanged(d->form->widget()); // to reload the ObjectTreeView
}

void CreateLayoutCommand::unexecute()
{
    ObjectTreeItem *parent = d->form->objectTree()->lookup(d->containerName);
    if (!parent)
        parent = d->form->objectTree();

    // We reparent every widget to the Container and take them out of the layout
    QHash<QByteArray, QRect>::ConstIterator endIt = d->pos.constEnd();
    for (QHash<QByteArray, QRect>::ConstIterator it = d->pos.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (item && item->widget()) {
            item->widget()->setParent(parent->widget());
            item->widget()->move(0, 0);
            item->eventEater()->setContainer(parent->container());
            if (d->pos.value(it.key()).isValid())
                item->widget()->setGeometry(d->pos.value(it.key()));
            d->form->objectTree()->reparent(item->name(), d->containerName);
        }
    }

    if (!parent->container())
        return;
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->name);
    if (!titem)
        return; //better this than a crash
    QWidget *w = titem->widget();
    parent->container()->deleteWidget(w); // delete the layout widget
//! @todo 2.0 unused?
//    FormManager::self()->windowChanged(d->form->widget()); // to reload ObjectTreeView
}

QString CreateLayoutCommand::name() const
{
    switch (d->layoutType) {
    case Form::HBox:
        return i18n("Group Widgets Horizontally");
    case Form::VBox:
        return i18n("Group Widgets Vertically");
    case Form::Grid:
        return i18n("Group Widgets in a Grid");
    case Form::HSplitter:
        return i18n("Group Widgets Horizontally in a Splitter");
    case Form::VSplitter:
        return i18n("Group Widgets Vertically in a Splitter");
    case Form::HFlow:
        return i18n("Group Widgets By Rows");
    case Form::VFlow:
        return i18n("Group Widgets Vertically By Columns");
    default:
        return i18n("Group widgets");
    }
}

void CreateLayoutCommand::debug()
{
    kDebug() << "name=\"" << name() << "\" generatedName=" << d->name
        << " widgets=" << d->pos.keys() << " container=" << d->containerName
        << " form=" << d->form->widget()->objectName();
}

/// BreakLayoutCommand ///////////////

BreakLayoutCommand::BreakLayoutCommand(const Container &container)
        : CreateLayoutCommand()
{
    d->containerName = container.topLevelWidget()->objectName();
    d->name = container.widget()->objectName();
    d->form = container.form();
    d->layoutType = container.layoutType();

    foreach (ObjectTreeItem *titem, *container.objectTree()->children()) {
        QRect r(
            container.widget()->mapTo(container.widget()->parentWidget(), 
            titem->widget()->pos()), titem->widget()->size()
        );
        d->pos.insert(titem->widget()->objectName().toLatin1().constData(), r);
    }
}

BreakLayoutCommand::~BreakLayoutCommand()
{
}

void BreakLayoutCommand::execute()
{
    CreateLayoutCommand::unexecute();
}

void BreakLayoutCommand::unexecute()
{
    CreateLayoutCommand::execute();
}

QString BreakLayoutCommand::name() const
{
    return i18n("Break Layout: \"%1\"", d->name);
}

void BreakLayoutCommand::debug()
{
    kDebug() << "name=\"" << name()
        << " widgets=" << d->pos.keys() << " container=" << d->containerName
        << " form=" << d->form->widget()->objectName();
}

// PasteWidgetCommand

namespace KFormDesigner
{
class PasteWidgetCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QString data;
    QString containerName;
    QPoint pos;
    QStringList names;
};
}

PasteWidgetCommand::PasteWidgetCommand(const QDomDocument &domDoc, const Container& container, const QPoint& p)
    : Command(), d( new Private )
{
    d->form = container.form();
    d->data = domDoc.toString();
    d->containerName = container.widget()->objectName();
    d->pos = p;

    if (domDoc.firstChildElement("UI").firstChildElement("widget").isNull())
        return;

    QRect boundingRect;
    for (QDomNode n = domDoc.firstChildElement("UI").firstChild(); !n.isNull(); n = n.nextSibling()) { // more than one widget
        const QDomElement el = n.toElement();
        if (el.tagName() != "widget")
            continue;

        QDomElement rect;
        for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling()) {
            if ((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
                rect = n.firstChild().toElement();
        }

        QDomElement x = rect.firstChildElement("x");
        QDomElement y = rect.firstChildElement("y");
        QDomElement w = rect.firstChildElement("width");
        QDomElement h = rect.firstChildElement("height");

        int rx = x.text().toInt();
        int ry = y.text().toInt();
        int rw = w.text().toInt();
        int rh = h.text().toInt();
        QRect r(rx, ry, rw, rh);
        boundingRect = boundingRect.unite(r);
    }

    //2.0 d->pos -= boundingRect.topLeft();
}


PasteWidgetCommand::~PasteWidgetCommand()
{
    delete d;
}

void PasteWidgetCommand::execute()
{
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->containerName);
    if (!titem)
        return; //better this than a crash
    Container *container = titem->container();
    QString errMsg;
    int errLine;
    int errCol;
    QDomDocument domDoc("UI");
    bool parsed = domDoc.setContent(d->data, false, &errMsg, &errLine, &errCol);

    if (!parsed) {
        kDebug() << errMsg;
        kDebug() << "line: " << errLine << " col: " << errCol;
        return;
    }

    kDebug() << domDoc.toString();
    if (!domDoc.firstChildElement("UI").hasChildNodes()) // nothing in the doc
        return;

    QDomElement el = domDoc.firstChildElement("UI").firstChildElement("widget");
    if (el.isNull())
        return;
    QDomNode n;
    for (n = el.nextSibling(); !n.isNull() && n.toElement().tagName() != "widget"; n = n.nextSibling())
        ;
    if (n.isNull()) {
        // only one "widget" child tag, so we can paste it at cursor pos
        QDomElement el = domDoc.firstChildElement("UI").firstChildElement("widget").toElement();
        fixNames(el);
        if (d->pos.isNull())
            fixPos(el, container);
        else
            changePos(el, d->pos);

        d->form->setInteractiveMode(false);
        FormIO::loadWidget(container, el);
        d->form->setInteractiveMode(true);
    }
    else {
        int minX = INT_MAX, minY = INT_MAX;
        if (!d->pos.isNull()) {
            // compute top-left point for the united rectangles
            for (n = domDoc.firstChildElement("UI").firstChild(); !n.isNull(); n = n.nextSibling()) {
                // more than one "widget" child tag
                if (n.toElement().tagName() != "widget") {
                    continue;
                }
                QDomElement el = n.toElement();
                QDomElement rectEl;
                for (QDomNode n2 = el.firstChild(); !n2.isNull(); n2 = n2.nextSibling()) {
                    if ((n2.toElement().tagName() == "property") && (n2.toElement().attribute("name") == "geometry")) {
                        rectEl = n2.firstChild().toElement();
                        break;
                    }
                }
                int x = rectEl.firstChildElement("x").text().toInt();
                if (x < minX)
                    minX = x;
                int y = rectEl.firstChildElement("y").text().toInt();
                if (y < minY)
                    minY = y;
            }
        }
        for (n = domDoc.firstChildElement("UI").firstChild(); !n.isNull(); n = n.nextSibling()) {
            // more than one "widget" child tag
            if (n.toElement().tagName() != "widget") {
                continue;
            }
            QDomElement el = n.toElement();
            fixNames(el);
            if (d->pos.isNull()) {
                fixPos(el, container);
            }
            else {
                //container->widget()->mapTo(
                moveWidgetBy(
                    el, container, 
                    QPoint(-minX, -minY) + d->pos // fix position by subtracting the original 
                                                  // offset and adding the new one
                );
            }

            d->form->setInteractiveMode(false);
            FormIO::loadWidget(container, el);
            d->form->setInteractiveMode(true);
        }
    }

    //FormIO::setCurrentForm(0);
    d->names.clear();
    // We store the names of all the created widgets, to delete them later
    for (n = domDoc.firstChildElement("UI").firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.toElement().tagName() != "widget") {
            continue;
        }
        for (QDomNode m = n.firstChild(); !m.isNull(); m = m.nextSibling()) {
            if ((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name")) {
                d->names.append(m.toElement().text());
                break;
            }
        }
    }

    container->form()->selectFormWidget();
    QStringList::ConstIterator endIt = d->names.constEnd();
    foreach (const QString& widgetName, d->names) { // We select all the pasted widgets
        ObjectTreeItem *item = d->form->objectTree()->lookup(widgetName);
        if (item) {
            container->selectWidget(item->widget(),
                Form::AddToPreviousSelection | Form::LastSelection | Form::Raise);
        }
    }
}

void PasteWidgetCommand::unexecute()
{
    ObjectTreeItem* titem = d->form->objectTree()->lookup(d->containerName);
    if (!titem)
        return; //better this than a crash
    Container *container = titem->container();
    // We just delete all the widgets we have created
    foreach (const QString& widgetName, d->names) {
        ObjectTreeItem* titem = container->form()->objectTree()->lookup(widgetName);
        if (!titem) {
            continue; //better this than a crash
        }
        QWidget *w = titem->widget();
        container->deleteWidget(w);
    }
}

QString PasteWidgetCommand::name() const
{
    return i18n("Paste");
}

void PasteWidgetCommand::changePos(QDomElement &el, const QPoint &newPos)
{
    //QDomElement el = widg.cloneNode(true).toElement();
    QDomElement rect;
    // Find the widget geometry if there is one
    for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if ((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry")) {
            rect = n.firstChild().toElement();
            break;
        }
    }

    QDomElement x = rect.firstChildElement("x");
    x.removeChild(x.firstChild());
    QDomText valueX = el.ownerDocument().createTextNode(QString::number(newPos.x()));
    x.appendChild(valueX);

    QDomElement y = rect.firstChildElement("y");
    y.removeChild(y.firstChild());
    QDomText valueY = el.ownerDocument().createTextNode(QString::number(newPos.y()));
    y.appendChild(valueY);

    //return el;
}

void PasteWidgetCommand::fixPos(QDomElement &el, Container *container)
{
    /* QDomElement rect;
      for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
      {
        if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
          rect = n.firstChild().toElement();
      }

      QDomElement x = rect.namedItem("x").toElement();
      QDomElement y = rect.namedItem("y").toElement();
      QDomElement wi = rect.namedItem("width").toElement();
      QDomElement h = rect.namedItem("height").toElement();

      int rx = x.text().toInt();
      int ry = y.text().toInt();
      int rw = wi.text().toInt();
      int rh = h.text().toInt();
      QRect r(rx, ry, rw, rh);

      QWidget *w = d->form->widget()->childAt(r.x() + 6, r.y() + 6, false);
      if(!w)
        return;

      while((w->geometry() == r) && (w != 0))// there is already a widget there, with the same size
      {
        w = d->form->widget()->childAt(w->x() + 16, w->y() + 16, false);
        r.moveBy(10,10);
      }

      // the pasted wigdet should stay inside container's boundaries
      if(r.x() < 0)
        r.moveLeft(0);
      else if(r.right() > container->widget()->width())
        r.moveLeft(container->widget()->width() - r.width());

      if(r.y() < 0)
        r.moveTop(0);
      else if(r.bottom() > container->widget()->height())
        r.moveTop(container->widget()->height() - r.height());

      if(r != QRect(rx, ry, rw, rh))
        //return el;
      //else
        changePos(el, QPoint(r.x(), r.y()));
    */
    moveWidgetBy(el, container, QPoint(0, 0));
}

void PasteWidgetCommand::moveWidgetBy(QDomElement &el, Container *container, const QPoint &p)
{
    QDomElement rect;
    for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if ((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry")) {
            rect = n.firstChild().toElement();
            break;
        }
    }

    QDomElement x = rect.firstChildElement("x");
    QDomElement y = rect.firstChildElement("y");
    QDomElement wi = rect.firstChildElement("width");
    QDomElement h = rect.firstChildElement("height");

    int rx = x.text().toInt();
    int ry = y.text().toInt();
    int rw = wi.text().toInt();
    int rh = h.text().toInt();
    QRect r(rx + p.x(), ry + p.y(), rw, rh);
    kDebug() << "Moving widget by " << p << " from " << rx << "  " << ry << " to " << r.topLeft();

    QWidget *w = d->form->widget()->childAt(r.x() + 6, r.y() + 6);

    while (w && (w->geometry() == r)) { // there is already a widget there, with the same size
        w = d->form->widget()->childAt(w->x() + 16, w->y() + 16);
        r.translate(10, 10);
    }

    // the pasted wigdet should stay inside container's boundaries
    if (r.x() < 0)
        r.moveLeft(0);
    else if (r.right() > container->widget()->width())
        r.moveLeft(container->widget()->width() - r.width());

    if (r.y() < 0)
        r.moveTop(0);
    else if (r.bottom() > container->widget()->height())
        r.moveTop(container->widget()->height() - r.height());

    if (r != QRect(rx, ry, rw, rh))
        //return el;
        //else
        changePos(el, QPoint(r.x(), r.y()));
}

void
PasteWidgetCommand::fixNames(QDomElement &el)
{
    QString wname;
    for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if ((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name")) {
            wname = n.toElement().text();
            while (d->form->objectTree()->lookup(wname)) { // name already exists
                bool ok;
                int num = wname.right(1).toInt(&ok, 10);
                if (ok)
                    wname = wname.left(wname.length() - 1) + QString::number(num + 1);
                else
                    wname += "2";
            }
            if (wname != n.toElement().text()) { // we change the name, so we recreate the element
                n.removeChild(n.firstChild());
                QDomElement type = el.ownerDocument().createElement("string");
                QDomText valueE = el.ownerDocument().createTextNode(wname);
                type.appendChild(valueE);
                n.toElement().appendChild(type);
            }

        }
        if (n.toElement().tagName() == "widget") { // fix child widgets names
            QDomElement child = n.toElement();
            fixNames(child);
        }
    }
}

void PasteWidgetCommand::debug()
{
    kDebug() << "pos=" << d->pos
        << " widgets=" << d->names << " container=" << d->containerName
        << " form=" << d->form->widget()->objectName()
        << " data=\"" << d->data.left(80) << "...\"";
}

// DeleteWidgetCommand

namespace KFormDesigner
{
class DeleteWidgetCommand::Private
{
public:
    Private()
    {
    }

    Form *form;
    QDomDocument domDoc;
    QHash<QByteArray, QByteArray> containers;
    QHash<QByteArray, QByteArray> parents;
};
}

DeleteWidgetCommand::DeleteWidgetCommand(Form& form, const QWidgetList &list)
        : Command(), d( new Private )
{
    d->form = &form;
    KFormDesigner::widgetsToXML(d->domDoc,
        d->containers, d->parents, *d->form, list);

/* moved
    d->domDoc.appendChild(d->domDoc.createElement("UI"));

    QDomElement parent = d->domDoc.namedItem("UI").toElement();

    QWidgetList topLevelList(list);
    removeChildrenFromList(topLevelList);

    foreach (QWidget *w, topLevelList) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(w->objectName());
        if (!item)
            return;

        // We need to store both parentContainer and parentWidget as they may be different (eg for TabWidget page)
        d->containers.insert(
            item->name().toLatin1(),
            d->form->parentContainer(item->widget())->widget()->objectName().toLatin1().constData()
        );
        d->parents.insert(
            item->name().toLatin1(),
            item->parent()->name().toLatin1()
        );
        FormIO::saveWidget(item, parent, d->domDoc);
        d->form->connectionBuffer()->saveAllConnectionsForWidget(
            item->widget()->objectName(), d->domDoc);
    }

    FormIO::cleanClipboard(parent);*/
}

DeleteWidgetCommand::~DeleteWidgetCommand()
{
    delete d;
}

void DeleteWidgetCommand::execute()
{
    Container *containerToSelect = 0;

    QHash<QByteArray, QByteArray>::ConstIterator endIt = d->containers.constEnd();
    for (QHash<QByteArray, QByteArray>::ConstIterator it = d->containers.constBegin(); it != endIt; ++it) {
        ObjectTreeItem *item = d->form->objectTree()->lookup(it.key());
        if (!item || !item->widget())
            continue;

        Container *cont = d->form->parentContainer(item->widget());
        if (!containerToSelect)
            containerToSelect = cont;
        cont->deleteWidget(item->widget());
    }
    //now we've nothing select: select parent container
    if (containerToSelect) {
        d->form->selectWidget(containerToSelect->widget());
    }
}

void DeleteWidgetCommand::unexecute()
{
    QByteArray wname;
    d->form->setInteractiveMode(false);
    for (QDomNode n = d->domDoc.firstChildElement("UI").firstChild(); !n.isNull(); n = n.nextSibling()) {
#ifdef KFD_SIGSLOTS
        if (n.toElement().tagName() == "connections") // restore the widget connections
            d->form->connectionBuffer()->load(n);
#endif
        if (n.toElement().tagName() != "widget")
            continue;
        // We need first to know the name of the widget
        for (QDomNode m = n.firstChild(); !m.isNull(); n = m.nextSibling()) {
            if ((m.toElement().tagName() == "property") && (m.toElement().attribute("name") == "name")) {
                wname = m.toElement().text().toLatin1();
                break;
            }
        }

        ObjectTreeItem* titem = d->form->objectTree()->lookup(d->containers.value(wname));
        if (!titem)
            return; //better this than a crash
        Container *cont = titem->container();
        ObjectTreeItem *parent = d->form->objectTree()->lookup(d->parents.value(wname));
        QDomElement widg = n.toElement();
        if (parent)
            FormIO::loadWidget(cont, widg, parent->widget());
        else
            FormIO::loadWidget(cont, widg);
    }
    d->form->setInteractiveMode(true);
}

QString DeleteWidgetCommand::name() const
{
    return i18n("Delete widget");
}

void DeleteWidgetCommand::debug()
{
    kDebug() << "containers=" << d->containers.keys()
        << " parents=" << d->parents.keys() << " form=" << d->form->widget()->objectName();
}

// DuplicateWidgetCommand

namespace KFormDesigner
{
class DuplicateWidgetCommand::Private
{
public:
    Private()
     : pasteCommand(0)
    {
    }
    ~Private()
    {
        delete pasteCommand;
    }

    Form *form;
    QDomDocument domDoc;
    QHash<QByteArray, QByteArray> containers;
    QHash<QByteArray, QByteArray> parents;
    PasteWidgetCommand *pasteCommand;
};
}

DuplicateWidgetCommand::DuplicateWidgetCommand(
    const Container& container, 
    const QWidgetList &list, 
    const QPoint& copyToPoint)
        : Command(), d( new Private )
{
    d->form = container.form();
    QDomDocument docToCopy;
    KFormDesigner::widgetsToXML(docToCopy,
        d->containers, d->parents, *d->form, list);

    d->pasteCommand = new PasteWidgetCommand(docToCopy, container, copyToPoint);
}

DuplicateWidgetCommand::~DuplicateWidgetCommand()
{
    delete d;
}

void DuplicateWidgetCommand::execute()
{
    d->pasteCommand->execute();
}

void DuplicateWidgetCommand::unexecute()
{
    d->pasteCommand->unexecute();
}

QString DuplicateWidgetCommand::name() const
{
    return i18n("Duplicate widget");
}

void DuplicateWidgetCommand::debug()
{
    kDebug() << "containers=" << d->containers.keys()
        << " parents=" << d->parents.keys() << " form=" << d->form->widget()->objectName();
}

// CutWidgetCommand

namespace KFormDesigner
{
class CutWidgetCommand::Private
{
public:
    Private()
     : data(0)
    {
    }

    ~Private()
    {
        delete data;
    }

    QMimeData *data;
};
}

CutWidgetCommand::CutWidgetCommand(Form& form, const QWidgetList &list)
        : DeleteWidgetCommand(form, list), d2( new Private )
{
}

CutWidgetCommand::~CutWidgetCommand()
{
    delete d2;
}

void CutWidgetCommand::execute()
{
    DeleteWidgetCommand::execute();
    delete d2->data;
    d2->data = KFormDesigner::deepCopyOfClipboardData(); // save clipboard contents
    // d->domDoc has been filled in DeleteWidgetCommand ctor
    KFormDesigner::copyToClipboard(d->domDoc.toString());
}

void CutWidgetCommand::unexecute()
{
    DeleteWidgetCommand::unexecute();
    QClipboard *cb = QApplication::clipboard();
    cb->setMimeData( d2->data ); // restore prev. clipboard contents
}

QString CutWidgetCommand::name() const
{
    return i18n("Cut");
}

void CutWidgetCommand::debug()
{
    kDebug() << "containers=" << d->containers.keys()
        << " parents=" << d->parents.keys() << " form=" << d->form->widget()->objectName()
        << " data=\"" << d2->data->text().left(80) << "...\"";
}

// CommandGroup

namespace KFormDesigner
{
class SubCommands : public K3MacroCommand
{
public:
    SubCommands(const QString & name)
            : K3MacroCommand(name) {
    }
    QList<K3Command*> commands() const {
        return K3MacroCommand::commands();
    }
};

class CommandGroup::Private
{
public:
    Private(const QString & name)
        : subCommands(name)
    {
    }

    Form *form;
    SubCommands subCommands;

    //! Used to store pointers to subcommands that shouldn't be executed
    //! on CommandGroup::execute()
    QSet<K3Command*> commandsShouldntBeExecuted;
};
}

CommandGroup::CommandGroup(Form &form, const QString & name)
        : Command(), d( new Private(name) )
{
    d->form = &form;
}

CommandGroup::~CommandGroup()
{
    delete d;
}

const QList<K3Command*> CommandGroup::commands() const
{
    return d->subCommands.commands();
}

void CommandGroup::addCommand(K3Command *command, bool allowExecute)
{
    if (!command)
        return;

    d->subCommands.addCommand(command);
    if (!allowExecute)
        d->commandsShouldntBeExecuted.insert(command);
}

void CommandGroup::execute()
{
#ifdef __GNUC__
#warning todo
#endif
//2.0 rm?    FormManager::self()->blockPropertyEditorUpdating(this);
    foreach(K3Command* command, d->subCommands.commands()) {
        if (!d->commandsShouldntBeExecuted.contains( command ))
            command->execute();
    }
//2.0 rm?    FormManager::self()->unblockPropertyEditorUpdating(this, m_propSet);
}

void CommandGroup::unexecute()
{
#ifdef __GNUC__
#warning todo
#endif
//2.0 rm?    FormManager::self()->blockPropertyEditorUpdating(this);
    d->subCommands.unexecute();
//2.0 rm?    FormManager::self()->unblockPropertyEditorUpdating(this, m_propSet);
}

QString CommandGroup::name() const
{
    return d->subCommands.name();
}

void CommandGroup::resetAllowExecuteFlags()
{
    d->commandsShouldntBeExecuted.clear();
}

void CommandGroup::debug()
{
    kDebug() << "name=\"" << name() << "\" #=" << d->subCommands.commands().count();
    uint i = 0;
    foreach(K3Command* command, d->subCommands.commands()) {
        i++;
        kDebug() << "#" << i << ":"
            << (d->commandsShouldntBeExecuted.contains(command) ? "!" : "") << "allowExecute:";
        if (dynamic_cast<Command*>(command)) {
            dynamic_cast<Command*>(command)->debug();
        }
        else if (dynamic_cast<CommandGroup*>(command)) {
            dynamic_cast<CommandGroup*>(command)->debug();
        }
        else {
            kDebug() << "(other KCommand)";
        }
    }
    kDebug() << "End of CommandGroup";
}
