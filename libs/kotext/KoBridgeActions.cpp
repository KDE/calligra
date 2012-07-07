/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoBridgeActions.h"
#include "KoInlineCite.h"
#include "KoTextEditor.h"
#include <KoOdfBibliographyConfiguration.h>

#include <QDataStream>
#include <QVariantMap>

KoBridgeAction::KoBridgeAction(const QVariantMap &map, Action _name) :
    m_data(map),
    m_out(&m_block, QIODevice::WriteOnly),
    name(_name)
{
}

KoBridgeAction::~KoBridgeAction()
{

}

QByteArray KoBridgeAction::data()
{
    return m_block;
}

InsertCitationBridgeAction::InsertCitationBridgeAction(const QVariantMap &map, KoTextEditor *editor) :
    KoBridgeAction(map, KoBridgeAction::InsertCitation),
    m_editor(editor)
{
    Q_ASSERT(m_editor);

    performAction();
}

void InsertCitationBridgeAction::performAction()
{
    KoInlineCite *cite = m_editor->insertCitation();

    foreach(const QString field, KoOdfBibliographyConfiguration::bibDataFields) {
        cite->setField(field, m_data[field].toString());
    }
}

InsertBibliographyBridgeAction::InsertBibliographyBridgeAction(const QVariantMap &map, KoTextEditor *editor) :
    KoBridgeAction(map, KoBridgeAction::InsertBibliography),
    m_editor(editor)
{
    performAction();
}

void InsertBibliographyBridgeAction::performAction()
{

}

InsertCiteRecordBridgeAction::InsertCiteRecordBridgeAction(const QVariantMap &map) :
    KoBridgeAction(map, KoBridgeAction::InsertCiteRecord)
{
    performAction();
}

void InsertCiteRecordBridgeAction::performAction()
{

}
