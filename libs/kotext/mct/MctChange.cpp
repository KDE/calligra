/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#include "MctChange.h"
#include "MctChangeTypes.h"
#include "MctPosition.h"
#include "MctStaticData.h"


MctChange::MctChange(MctPosition *position, MctChangeTypes changeType, MctNode *changeEntity,  QDomElement changePtr, MctPosition *movedpos)
    : m_position(position)
    , m_changeType(changeType)
    , m_changeEntity(changeEntity)
    , m_change(changePtr)
    , m_movedPosition(movedpos)
{    
}

MctChange::MctChange(MctPosition *position, MctChangeTypes changeType, MctNode *changeEntity, MctPosition *movedpos)
    : m_position(position)
    , m_changeType(changeType)
    , m_changeEntity(changeEntity)
    , m_movedPosition(movedpos)
{
}

MctChange::~MctChange()
{
    if(m_position) {
        delete m_position;
    }
    if(m_movedPosition) {
        delete m_movedPosition;
    }
    if(m_changeEntity) {
        delete m_changeEntity;
    }
}

/**
 * @brief This stores XML node corresponding to the change node in change attribute.
 * @param change The XML node representing the change event.
 */
void MctChange::addElementTreeNode(const QDomElement &change)
{
    this->m_change = change;
    this->m_position = NULL;
    this->m_movedPosition = NULL;
}

/**
 * @brief This gets the Id number of the change node.
 * @return Returns with the Id number.
 */
//int MctChange::nodeId()
//{
//    return m_change.attribute("id").toInt();
//}

/**
 * @brief This gets the position of the change node.
 * @return Returns with position of the change.
 */
MctPosition * MctChange::position() const
{
    if (this->m_position) {
        return new MctPosition(*this->m_position); // duplicate
    } else if (this->m_change.isNull()) {
        return NULL;
    } else {
        return MctStaticData::instance()->getPosFromElementreeNode(this->m_change);
    }
}

/**
 * @brief This sets the position of the change node.
 * @param position The position to set
 */
void MctChange::setPosition(MctPosition *position)
{
    if (position == NULL) {
        return;
    } else if (this->m_change.isNull()) {
        this->m_position = new MctPosition(*position); // duplicate
    } else {
         MctStaticData::instance()->addPos2change(&m_change, position);
    }
}

/**
 * @brief This gets the moved position of the change node.
 * @return Returns with moved position of the change.
 */
MctPosition * MctChange::movedPosition() const
{
    if (this->m_changeType != MctChangeTypes::MovedString)
        return NULL;
    else if (this->m_movedPosition != NULL)
        return new MctPosition(*this->m_movedPosition);
    else
        return MctStaticData::instance()->getPosFromElementreeNode(this->m_change, true);

}

/**
 * @brief This sets the moved position of the change node.
 * @param position The position to set
 */
void MctChange::setMovedPosition(MctPosition *position)
{
    if (position == NULL)
        return;
    else if (this->m_change.isNull())
        this->m_movedPosition = new MctPosition(*position);
    else
        MctStaticData::instance()->addPos2change( &m_change, position, true );
}

/**
 * @brief This sets the change type of the change node.
 * @param changeType
 */
void MctChange::setChangeType(MctChangeTypes changeType)
{
    this->m_changeType = changeType;
}

/**
 * @brief This gets the change type of the change node.
 * @return Returns with the change type.
 */
MctChangeTypes MctChange::changeType() const
{
    return this->m_changeType;
}

/**
 * @brief This gets the change entity of the change node.
 * @return Returns with the change entity.
 */
MctNode * MctChange::changeEntity()
{
    return this->m_changeEntity;
}

/**
 * @brief This gets the xml change node.
 * @return Returns with the change node.
 */
QDomElement MctChange::changeNode()
{
    return m_change;
}

/**
 * @brief This sets the xml change node.
 * @param change The xml node to set
 */
void MctChange::setChangeNode(const QDomElement &element)
{
    this->m_change = element;
}
