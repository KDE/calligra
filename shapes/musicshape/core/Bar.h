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
#ifndef MUSIC_CORE_BAR_H
#define MUSIC_CORE_BAR_H

#include <QtCore/QObject>
#include <QtCore/QPointF>

namespace MusicCore {

class Sheet;
class Voice;
class VoiceBar;
class Staff;
class StaffElement;

/**
 * A bar (in the US also known as a measure) is a part of a piece of music. A piece of music is a two-dimensional
 * thing, with multiple staffs that are played concurrently and multiple bars that are played sequentially.
 */
class Bar : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new bar in the given sheet of music. The bar is not actually added to the sheet, to do that call
     * the addBar method of the sheet.
     *
     * @param sheet the sheet to create a bar for
     */
    explicit Bar(Sheet* sheet);

    /**
     * Destructor.
     */
    ~Bar();

    /**
     * Returns the sheet this bar is part of.
     */
    Sheet* sheet();

    /**
     * Changes the sheet this bar is part of. This method should not be called after the bar has been added to a sheet.
     */
    void setSheet(Sheet* sheet);

    /**
     * Returns a VoiceBar instance for a specific voice.
     *
     * @param voice the voice for which to return the VoiceBar
     */
    VoiceBar* voice(Voice* voice);

    /**
     * Returns the top-left corner of the bounding box of the bar.
     */
    QPointF position() const;

    /**
     * Returns the size of the bar.
     */
    double size() const;
    
    /**
     * The prefix of a bar contains any staff elements at time 0 such as clefs and key signatures.
     * When this bar is the first bar of a staff system the prefix will be drawn at the end of the previous
     * system instead of the start of this system.
     *
     * The prefix is not included in the size of the bar, so the actual size of a bar as it will be drawn is
     * size() + prefix(), and the size a bar wants to have is desiredSize() + prefix(). The actual contents of the
     * bar start at position() + (size(), 0); all coordinates (including staff elements with start time 0) are relative
     * to position(). This means that elements in the prefix have negative x coordinates.
     */
    double prefix() const;
    void setPrefix(double prefix);
    QPointF prefixPosition() const;
    void setPrefixPosition(const QPointF& pos);
    
    double desiredSize() const;
    double scale() const;

    int staffElementCount(Staff* staff) const;
    StaffElement* staffElement(Staff* staff, int index);
    int indexOfStaffElement(StaffElement* element);
    void addStaffElement(StaffElement* element, int indexHint = -1);
    void removeStaffElement(StaffElement* element, bool deleteElement = true);
public slots:
    /**
     * Sets the top-left corner of the bounding box of this bar.
     */
    void setPosition(const QPointF& position, bool setPrefix=true);
    
    /**
     * Sets the size of the bar.
     *
     * @param size the new size of the bar
     */
    void setSize(double size);
    
    void setDesiredSize(double size);
signals:
    void positionChanged(const QPointF& position);
    void sizeChanged(double size);
    void desiredSizeChanged(double size);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_PART_H
