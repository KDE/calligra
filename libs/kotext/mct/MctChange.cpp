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
    : position(position)
    , changeType(changeType)
    , changeEntity(changeEntity)
    , change(changePtr)
    , movedPosition(movedpos)
{    
}

MctChange::MctChange(MctPosition *position, MctChangeTypes changeType, MctNode *changeEntity, MctPosition *movedpos)
    : position(position)
    , changeType(changeType)
    , changeEntity(changeEntity)
    , movedPosition(movedpos)
{
}

MctChange::~MctChange()
{
    if(position) {
        delete position;
    }
    if(movedPosition) {
        delete movedPosition;
    }
    if(changeEntity) {
        delete changeEntity;
    }
}

/**
 * @brief This stores XML node corresponding to the change node in change attribute.
 * @param change The XML node representing the change event.
 */
void MctChange::addElementTreeNode(QDomElement change)
{
    this->change = change;
    this->position = NULL;
    this->movedPosition = NULL;
}

/**
 * @brief This gets the Id number of the change node.
 * @return Returns with the Id number.
 */
int MctChange::getNodeId()
{
    return change.attribute("id").toInt();
}

/**
 * @brief This gets the position of the change node.
 * @return Returns with position of the change.
 */
MctPosition * MctChange::getPosition() const
{
    if (this->position) {
        return new MctPosition(*this->position); // duplicate
    } else if (this->change.isNull()) {
        return NULL;
    } else {
        return MctStaticData::instance()->getPosFromElementreeNode(this->change);
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
    } else if (this->change.isNull()) {
        this->position = new MctPosition(*position); // duplicate
    } else {
         MctStaticData::instance()->addPos2change(&change, position);
    }
}

/**
 * @brief This gets the moved position of the change node.
 * @return Returns with moved position of the change.
 */
MctPosition * MctChange::getMovedPosition() const
{
    if (this->changeType != MctChangeTypes::MovedString)
        return NULL;
    else if (this->movedPosition != NULL)
        return new MctPosition(*this->movedPosition);
    else
        return MctStaticData::instance()->getPosFromElementreeNode(this->change, true);

}

/**
 * @brief This sets the moved position of the change node.
 * @param position The position to set
 */
void MctChange::setMovedPosition(MctPosition *position)
{
    if (position == NULL)
        return;
    else if (this->change.isNull())
        this->movedPosition = new MctPosition(*position);
    else
        MctStaticData::instance()->addPos2change( &change, position, true );
}

/**
 * @brief This sets the change type of the change node.
 * @param changeType
 */
void MctChange::setChangeType(MctChangeTypes changeType)
{
    this->changeType = changeType;
}

/**
 * @brief This gets the change type of the change node.
 * @return Returns with the change type.
 */
MctChangeTypes MctChange::getChangeType() const
{
    return this->changeType;
}

/**
 * @brief This gets the change entity of the change node.
 * @return Returns with the change entity.
 */
MctNode * MctChange::getChangeEntity()
{
    return this->changeEntity;
}

/**
 * @brief This gets the xml change node.
 * @return Returns with the change node.
 */
QDomElement MctChange::getChangeNode()
{
    return change;
}

/**
 * @brief This sets the xml change node.
 * @param change The xml node to set
 */
void MctChange::setChangeNode(QDomElement change)
{
    this->change = change;
}
