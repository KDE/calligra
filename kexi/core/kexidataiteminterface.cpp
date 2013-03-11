/* This file is part of the KDE project
   Copyright (C) 2005-2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidataiteminterface.h"

#include <kdebug.h>

KexiDataItemChangesListener::KexiDataItemChangesListener()
{
}

KexiDataItemChangesListener::~KexiDataItemChangesListener()
{
}

//-----------------------------------------------
class KexiDataItemInterface::Private
{
public:
    Private();
    ~Private();

    QPointer<QObject> listenerObject;
    KexiDataItemChangesListener* listener;
    bool listenerIsQObject;
    QVariant origValue;

    /*! @see parentDataItemInterface() */
    KexiDataItemInterface* parentDataItemInterface;
    bool hasFocusableWidget;
    bool disable_signalValueChanged;
    bool acceptEditorAfterDeleteContents;
    bool lengthExceededEmittedAtPreviousChange;
};

KexiDataItemInterface::Private::Private()
    : listener(0), listenerIsQObject(false) ,parentDataItemInterface(0)
    ,hasFocusableWidget(true), disable_signalValueChanged(false), acceptEditorAfterDeleteContents(false)
    ,lengthExceededEmittedAtPreviousChange(false)
{

}

KexiDataItemInterface::Private::~Private()
{

}

KexiDataItemInterface::KexiDataItemInterface()
    : d(new Private())
{
}

KexiDataItemInterface::~KexiDataItemInterface()
{
    delete d;
}

KexiDataItemInterface* KexiDataItemInterface::parentDataItemInterface() const
{
    return d->parentDataItemInterface;
}

bool KexiDataItemInterface::acceptEditorAfterDeleteContents() const
{
    return d->acceptEditorAfterDeleteContents;
}

bool KexiDataItemInterface::hasFocusableWidget() const
{
    return d->hasFocusableWidget;
}

void KexiDataItemInterface::setValue(const QVariant& value, const QVariant& add,
                                     bool removeOld, const QVariant* visibleValue)
{
    d->disable_signalValueChanged = true; //to prevent emmiting valueChanged()
//needed? clear();
    if (dynamic_cast<QObject*>(this)) {
        kDebug() <<
            dynamic_cast<QObject*>(this)->metaObject()->className()
            << dynamic_cast<QWidget*>(this)->objectName()
            << "value=" << value << "add=" << add;
    }
    d->origValue = value;
    setValueInternal(add, removeOld);
    if (visibleValue)
        setVisibleValueInternal(*visibleValue);
    d->disable_signalValueChanged = false;
}

void KexiDataItemInterface::setVisibleValueInternal(const QVariant& value)
{
    Q_UNUSED(value);
}

void KexiDataItemInterface::signalValueChanged()
{
    if (d->disable_signalValueChanged || isReadOnly())
        return;
    if (d->parentDataItemInterface) {
        d->parentDataItemInterface->signalValueChanged();
        return;
    }
    if (d->listener) {
        beforeSignalValueChanged();
        d->listener->valueChanged(this);
    }
}

void KexiDataItemInterface::signalLengthExceeded(bool lengthExceeded)
{
    if (d->listener) {
        d->listener->lengthExceeded(this, lengthExceeded);
    }
}

void KexiDataItemInterface::signalUpdateLengthExceededMessage()
{
    if (d->listener) {
        d->listener->updateLengthExceededMessage(this);
    }
}

void KexiDataItemInterface::emitLengthExceededIfNeeded(bool lengthExceeded)
{
    if (lengthExceeded && !d->lengthExceededEmittedAtPreviousChange) {
        d->lengthExceededEmittedAtPreviousChange = true;
        signalLengthExceeded(true);
    }
    else if (!lengthExceeded && d->lengthExceededEmittedAtPreviousChange) {
        d->lengthExceededEmittedAtPreviousChange = false;
        signalLengthExceeded(false);
    }
    else if (lengthExceeded) {
        signalUpdateLengthExceededMessage();
    }
}

bool KexiDataItemInterface::valueChanged()
{
    kDebug() << d->origValue.toString() << " ? " << value().toString();
    return d->origValue != value();
}

KexiDataItemChangesListener* KexiDataItemInterface::listener()
{
    if (!d->listener || !d->listenerIsQObject)
        return d->listener;
    if (!d->listenerObject)
        d->listener = 0; //destroyed, update pointer
    return d->listener;
}

void KexiDataItemInterface::installListener(KexiDataItemChangesListener* listener)
{
    d->listener = listener;
    d->listenerIsQObject = dynamic_cast<QObject*>(listener);
    if (d->listenerIsQObject)
        d->listenerObject = dynamic_cast<QObject*>(listener);
}

void KexiDataItemInterface::showFocus(const QRect& r, bool readOnly)
{
    Q_UNUSED(r);
    Q_UNUSED(readOnly);
}

void KexiDataItemInterface::hideFocus()
{
}

void KexiDataItemInterface::clickedOnContents()
{
}

bool KexiDataItemInterface::valueIsValid()
{
    return true;
}

void KexiDataItemInterface::setParentDataItemInterface(KexiDataItemInterface* parentDataItemInterface)
{
    d->parentDataItemInterface = parentDataItemInterface;
}

bool KexiDataItemInterface::cursorAtNewRow()
{
    return listener() ? listener()->cursorAtNewRow() : false;
}

bool KexiDataItemInterface::isComboBox() const
{
    return false;
}

QWidget* KexiDataItemInterface::internalEditor() const
{
    return 0;
}

bool KexiDataItemInterface::fixup()
{
    return true;
}

void KexiDataItemInterface::setFocusableWidget(bool set)
{
    d->hasFocusableWidget = set;
}

void KexiDataItemInterface::setFocus()
{
    if (widget())
        widget()->setFocus();
}

void KexiDataItemInterface::showWidget()
{
    if (widget())
        widget()->show();
}

void KexiDataItemInterface::hideWidget()
{
    if (widget())
        widget()->hide();
}

QVariant KexiDataItemInterface::visibleValue()
{
    return QVariant();
}

bool KexiDataItemInterface::isReadOnly() const
{

    return false;

}

void KexiDataItemInterface::handleAction(const QString &actionName)
{
    Q_UNUSED(actionName);
}

QVariant KexiDataItemInterface::originalValue() const
{
    return d->origValue;
}

void KexiDataItemInterface::setHasFocusableWidget(bool set) const
{
    d->hasFocusableWidget = set;
}

void KexiDataItemInterface::setAcceptEditorAfterDeleteContents(bool set) const
{
    d->acceptEditorAfterDeleteContents = set;
}

bool KexiDataItemInterface::lengthExceededEmittedAtPreviousChange() const
{
    return d->lengthExceededEmittedAtPreviousChange;
}

void KexiDataItemInterface::setLengthExceededEmittedAtPreviousChange(bool set)
{
    d->lengthExceededEmittedAtPreviousChange = set;
}
