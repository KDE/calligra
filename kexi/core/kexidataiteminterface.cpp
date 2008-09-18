/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

KexiDataItemInterface::KexiDataItemInterface()
        : m_listener(0)
        , m_listenerIsQObject(false)
        , m_parentDataItemInterface(0)
        , m_hasFocusableWidget(true)
        , m_disable_signalValueChanged(false)
        , m_acceptEditorAfterDeleteContents(false)
{
}

KexiDataItemInterface::~KexiDataItemInterface()
{
}

void KexiDataItemInterface::setValue(const QVariant& value, const QVariant& add,
                                     bool removeOld, const QVariant* visibleValue)
{
    m_disable_signalValueChanged = true; //to prevent emmiting valueChanged()
//needed? clear();
    if (dynamic_cast<QObject*>(this)) {
        kDebug() << "KexiDataItemInterface::setValue(): " <<
        dynamic_cast<QObject*>(this)->metaObject()->className() << " "
        << dynamic_cast<QWidget*>(this)->objectName()
        << " value=" << value << " add=" << add;
    }
    m_origValue = value;
    setValueInternal(add, removeOld);
    if (visibleValue)
        setVisibleValueInternal(*visibleValue);
    m_disable_signalValueChanged = false;
}

void KexiDataItemInterface::setVisibleValueInternal(const QVariant& value)
{
    Q_UNUSED(value);
}

void KexiDataItemInterface::signalValueChanged()
{
    if (m_disable_signalValueChanged || isReadOnly())
        return;
    if (m_parentDataItemInterface) {
        m_parentDataItemInterface->signalValueChanged();
        return;
    }
    if (m_listener) {
        beforeSignalValueChanged();
        m_listener->valueChanged(this);
    }
}

bool KexiDataItemInterface::valueChanged()
{
// bool ok;
// kDebug() << m_origValue.toString() << " ? " << value(ok).toString();
// return (m_origValue != value(ok)) && ok;
    kDebug() << "KexiDataItemInterface::valueChanged(): " << m_origValue.toString() << " ? " << value().toString();
    return m_origValue != value();
}

/*
void KexiDataItemInterface::setValue(const QVariant& value)
{
  m_disable_signalValueChanged = true; //to prevent emmiting valueChanged()
  setValueInternal( value );
  m_disable_signalValueChanged = false;
}*/

KexiDataItemChangesListener* KexiDataItemInterface::listener()
{
    if (!m_listener || !m_listenerIsQObject)
        return m_listener;
    if (!m_listenerObject)
        m_listener = 0; //destroyed, update pointer
    return m_listener;
}

void KexiDataItemInterface::installListener(KexiDataItemChangesListener* listener)
{
    m_listener = listener;
    m_listenerIsQObject = dynamic_cast<QObject*>(listener);
    if (m_listenerIsQObject)
        m_listenerObject = dynamic_cast<QObject*>(listener);
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
    m_parentDataItemInterface = parentDataItemInterface;
}

bool KexiDataItemInterface::cursorAtNewRow()
{
    return listener() ? listener()->cursorAtNewRow() : false;
}
