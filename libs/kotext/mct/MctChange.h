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

#ifndef MCTCHANGE_H
#define MCTCHANGE_H

#include "MctNode.h"
#include "MctChangeTypes.h"

#include "KoXmlReader.h"

class MctPosition;

class MctChange : public MctNode
{
public:
    MctChange(MctPosition *position, MctChangeTypes changeType, MctNode *changeEntity,  QDomElement change, MctPosition *movedpos = NULL);
    MctChange(MctPosition *position, MctChangeTypes changeType, MctNode *changeEntity, MctPosition *movedpos = NULL);
    ~MctChange();

    void addElementTreeNode(QDomElement change);
    int getNodeId();

    MctChangeTypes getChangeType() const;
    void setChangeType(MctChangeTypes changeType);

    MctPosition * getPosition() const;
    void setPosition(MctPosition *position);

    MctPosition * getMovedPosition() const;
    void setMovedPosition(MctPosition *position);

    MctNode * getChangeEntity();

    QDomElement getChangeNode();
    void setChangeNode(QDomElement);

private:    

    MctPosition *position;         // Position of the change
    MctChangeTypes changeType;     // ChangeType
    MctNode *changeEntity;         // Change entity
    QDomElement change;            // The change node in the ElementTree
    MctPosition *movedPosition;    // Position of the change

};

#endif // MCTCHANGE_H
