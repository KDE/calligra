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

#ifndef MCTNODE_H
#define MCTNODE_H

#include <QString>
/**
 * A basic node for the abstract graph
 */
class MctNode
{
public:
    /// constructor
    MctNode(unsigned long id);
    MctNode();
    virtual ~MctNode();

    /// setter
    virtual void setId(unsigned long id);
    /// getter
    virtual unsigned long id() const;
    /// setter
    void setName(const QString &name);
    /// getter
    QString name();

protected:
    unsigned long m_id; ///< indetifier number
    QString m_name;     ///< optional name or comment
};

#endif // MCTNODE_H
