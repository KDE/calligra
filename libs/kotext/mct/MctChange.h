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
/**
 * Practial node in the abstract change-tracking graph
 *
 * The MctChange represents an actual change in the document history.
 * The class connect the position, the change type, the id and the related Dom element.
 */
class MctChange : public MctNode
{
public:
    /// constructor
    MctChange(MctPosition *m_position, MctChangeTypes m_changeType, MctNode *m_changeEntity, QDomElement m_change, MctPosition *movedpos = NULL);
    MctChange(MctPosition *m_position, MctChangeTypes m_changeType, MctNode *m_changeEntity, MctPosition *movedpos = NULL);
    ~MctChange();

    /**
     * setter of the corresponding XML node
     *
     * @param change XML node representing the change event.
     * @note position and movedPosition will be set to nullptr
     */
    void addElementTreeNode(const QDomElement &m_change);

    /// getter
    MctChangeTypes changeType() const;
    /// setter
    void setChangeType(MctChangeTypes m_changeType);

    /**
     * getter for change position
     *
     * A copy of the stored position pointer is returned, if member was set.
     * The function will return nullptr if the related DOM object is undefined,
     * otherwise the XML will be read and retunr with a new position object.
     */
    MctPosition * position() const;
    /// setter for change position
    void setPosition(MctPosition *m_position);

    /// setter
    void setMovedPosition(MctPosition *m_position);
    /// getter
    MctPosition * movedPosition() const;
    /// getter
    MctNode * changeEntity();
    /// setter
    void setChangeNode(const QDomElement &element);
    /// getter
    QDomElement changeNode();

private:    

    MctPosition *m_position;        ///< position of the change
    MctChangeTypes m_changeType;    ///< type of the change
    MctNode *m_changeEntity;        ///< change entity, change id
    QDomElement m_change;           ///< change node in the XML
    MctPosition *m_movedPosition;   ///< position of the change

};

#endif // MCTCHANGE_H
