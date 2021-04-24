/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_NOTE_H
#define MUSIC_CORE_NOTE_H

#include <QObject>

namespace MusicCore {

class Staff;
class Chord;

/**
 * This class represents one note in a chord. You should not add the same note instance to more than one chord, nor
 * should you add a note to a chord in a different part than the part in which staff this note belongs to is in.
 */
class Note : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new note instance, displayed on the given staff.
     *
     * @param chord the chord
     * @param staff the staff the note should be on
     * @param pitch the pitch of the new note
     * @param accidentals the accidentals of the new note
     */
    Note(Chord* chord, Staff* staff, int pitch, int accidentals = 0);

    /**
     * Destructor.
     */
    ~Note() override;

    /**
     * Returns the staff for this note.
     */
    Staff* staff();

    void setStaff(Staff* staff);

    Chord* chord();
    
    /**
     * Returns the pitch for this note.
     */
    int pitch() const;

    /**
     * Returns the accidentals for this note.
     */
    int accidentals() const;

    void setAccidentals(int accidentals);

    bool drawAccidentals() const;
    void setDrawAccidentals(bool drawAccidentals);

    bool isStartTie() const;
    void setStartTie(bool startTie);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_NOTE_H
