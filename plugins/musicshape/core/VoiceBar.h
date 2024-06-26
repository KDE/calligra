/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_VOICEBAR_H
#define MUSIC_CORE_VOICEBAR_H

#include <QObject>
#include <QRectF>

namespace MusicCore
{

class Voice;
class Bar;
class VoiceElement;

/**
 * A VoiceBar contains the music elements in a specific voice in a specific bar. A VoiceBar is in many ways
 * simply a wrapper around a QList containing the actual music elements.
 */
class VoiceBar : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new empty voice bar.
     */
    explicit VoiceBar(Bar *bar);

    /**
     * Destructor.
     */
    ~VoiceBar() override;

    Bar *bar();

    /**
     * Returns the number of elements in the bar.
     */
    int elementCount() const;

    /**
     * Returns the element at the given index in this bar.
     *
     * @param index the index of the element to return
     */
    VoiceElement *element(int index);

    int indexOfElement(VoiceElement *element);

    /**
     * Adds an element to this bar. You should not add an element to more than one bar, because when the bar is deleted
     * all elements in the bar are also deleted.
     *
     * @param element the element to add to this bar
     */
    void addElement(VoiceElement *element);

    /**
     * Inserts an element into this bar. You should not add an element to more than one bar, because when the bar is deleted
     * all elements in the bar are also deleted.
     *
     * @param element the element to insert into the bar
     * @param before the index of the element before which to insert the element
     */
    void insertElement(VoiceElement *element, int before);

    /**
     * Inserts an element into the bar. You should not add an element to more than one bar, because when the bar is deleted
     * all elements in the bar are also deleted.
     *
     * @param element the element to insert into the bar
     * @param before the element before which to insert the element
     */
    void insertElement(VoiceElement *element, VoiceElement *before);

    /**
     * Removes an element from this bar. If deleteElement is true, the element is not only removed but also deleted.
     *
     * @param index the index of the element to remove
     * @param deleteElement should the element not only be removed but also deleted
     */
    void removeElement(int index, bool deleteElement = true);

    /**
     * Removes an element from this bar. If deleteElement is true, the element is not only removed but also deleted.
     *
     * @param element the element to remove
     * @param deleteElement should the element not only be removed but also deleted
     */
    void removeElement(VoiceElement *element, bool deleteElement = true);

    void updateAccidentals();

private:
    class Private;
    Private *const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_VOICEBAR_H
