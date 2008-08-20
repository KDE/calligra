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

#include "kexiformdataiteminterface.h"
#include "kexiformscrollview.h"
#include <kexidb/queryschema.h>
#include <kexiutils/utils.h>

KexiFormDataItemInterface::KexiFormDataItemInterface()
        : KexiDataItemInterface()
        , m_columnInfo(0)
        , m_displayParametersForEnteredValue(0)
        , m_displayParametersForDefaultValue(0)
        , m_displayDefaultValue(false)
{
}

KexiFormDataItemInterface::~KexiFormDataItemInterface()
{
    delete m_displayParametersForEnteredValue;
    delete m_displayParametersForDefaultValue;
}

void KexiFormDataItemInterface::undoChanges()
{
// m_disable_signalValueChanged = true;
    setValueInternal(QString(), false);
// m_disable_signalValueChanged = false;
}

KexiDB::Field* KexiFormDataItemInterface::field() const
{
    return m_columnInfo ? m_columnInfo->field : 0;
}

void KexiFormDataItemInterface::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
    m_displayDefaultValue = displayDefaultValue;
    if (!m_displayParametersForDefaultValue) {
        m_displayParametersForEnteredValue = new KexiDisplayUtils::DisplayParameters(widget);
        m_displayParametersForDefaultValue = new KexiDisplayUtils::DisplayParameters();
        KexiDisplayUtils::initDisplayForDefaultValue(*m_displayParametersForDefaultValue, widget);
    }
}

void KexiFormDataItemInterface::cancelEditor()
{
    QWidget *parentWidget = dynamic_cast<QWidget*>(this)->parentWidget();
    KexiFormScrollView* view = KexiUtils::findParent<KexiFormScrollView*>(parentWidget);
    if (view)
        view->cancelEditor();
}
