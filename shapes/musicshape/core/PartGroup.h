/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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
#ifndef MUSIC_CORE_PARTGROUP_H
#define MUSIC_CORE_PARTGROUP_H

#include <QtCore/QString>

namespace MusicCore {

class Sheet;

/**
 * A PartGroup can be used to group a consequetive number of parts together. This can for example be usefull
 * to group all the string instruments. A group is defined by the indices of the first and last parts in the group.
 */
class PartGroup
{
public:
    /// Used to identify the symbol shown for the group
    enum GroupSymbol {
        None,
        Brace,
        Line,
        Bracket,
        DefaultSymbol = None
    };


    /**
     * Returns the sheet this group is part of.
     */
    Sheet* sheet();

    /**
     * Returns the index of the first part in this group.
     */
    int firstPart() const;

    /**
     * Sets the index of the first part in this group. The index should be >= 0 and < sheet().partCount().
     *
     * @param index the index at which this group should start.
     */
    void setFirstPart(int index);

    /**
     * Returns the index of the last part in this group.
     */
    int lastPart() const;

    /**
     * Sets the index of the last part in this group. The index should be >= 0 and < sheet().partCount().
     *
     * @param index the index at which this group should end.
     */
    void setLastPart(int index);

    /**
     * Returns the name of this group.
     */
    QString name() const;

    /**
     * Changes the name of this group.
     *
     * @param name the new name of this group.
     */
    void setName(QString name);

    /**
     * Returns the grouping symbol of this group.
     */
    GroupSymbol symbol() const;

    /**
     * Changes the grouping symbol of this group.
     *
     * @param symbol the new grouping symbol.
     */
    void setSymbol(GroupSymbol symbol);

    /**
     * Returns true if the group should have common bar-lines.
     *
     * XXX MusicXML has Mensurstrich as third option... what is that, and do I want that? :)
     */
    bool commonBarLines() const;

    /**
     * Set wether the group should have common bar-lines.
     *
     * @param commonBarLines true if the group should have common bar-lines
     */
    void setCommonBarLines(bool commonBarLines);
private:
    PartGroup(Sheet* sheet, int firstPart, int lastPart);
    ~PartGroup();
    friend class Sheet;
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_PART_H
