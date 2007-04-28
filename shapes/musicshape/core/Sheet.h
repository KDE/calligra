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
#ifndef MUSIC_CORE_SHEET_H
#define MUSIC_CORE_SHEET_H

#include <QtCore/QString>

namespace MusicCore {

class Part;
class PartGroup;
class Staff;
class Bar;

/**
 * A sheet is the largest unit in a piece of music. A sheet consists of zero or more
 * parts. A sheet can also contain zero or more part groups, which can be used to group similar parts
 * together (for example all string parts).
 *
 * A sheet also contains bars. Musical elements are always inserted in a specifc bar/part combination (where the
 * part is further divided in a staff and a voice).
 */
class Sheet
{
public:
    /**
     * Constructor, this will create a sheet containing no parts.
     */
    Sheet();
    ~Sheet();

    /**
     * Returns the number of parts this piece of music contains.
     */
    int partCount() const;

    /**
     * Returns the part at the given index, or NULL if index < 0 or >= partCount().
     *
     * @param index the index of the part to return
     */
    Part* part(int index);

    /**
     * Adds a new part to this sheet. The part will have the given name, and will be added after all currently
     * existing parts.
     *
     * @param name the name of the part to create
     */
    Part* addPart(QString name);

    /**
     * Inserts a new part into this sheet. The part will be inserted before the part with index before, and be names
     * name.
     *
     * @param before index of the part before which to insert a new part
     * @param name the name of the part to create
     */
    Part* insertPart(int before, QString name);

    void removePart(int index);
    void removePart(Part* part);

    /**
     * Returns the number of groups in this sheet.
     */
    int partGroupCount() const;

    /**
     * Returns the group at the given index. Part groups are ordered by the order in which they were added.
     *
     * @param index the index of the part group to return
     */
    PartGroup* partGroup(int index);

    /**
     * Adds a new part group to this sheet. The part group starts at the part indexed firstPart, and ends at the part
     * indexed lastPart.
     *
     * @param firstPart the index of the first part that is part of the new part group
     * @param lastPart the index of the last part that is part of the new part group
     */
    PartGroup* addPartGroup(int firstPart, int lastPart);

    void removePartGroup(PartGroup* group);

    /**
     * Returns the number of bars/measures in this piece of music.
     */
    int barCount() const;

    /**
     * Returns the bar with the given index.
     *
     * @param index the index of the bar to return.
     */
    Bar* bar(int index);

    /**
     * Adds count new bars at the end of this piece of music.
     *
     * @param count the number of bars to add to this sheet.
     */
    void addBars(int count);
    Bar* addBar();

    /**
     * Inserts a new bar before the bar with index before.
     *
     * @param before the index of the bar before which to insert the new bars.
     */
    Bar* insertBar(int before);

    void removeBar(int index);
    void removeBars(int index, int count);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_SHEET_H
