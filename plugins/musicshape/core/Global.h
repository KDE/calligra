/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_GLOBAL_H
#define MUSIC_CORE_GLOBAL_H

#include <QString>

namespace MusicCore {
    /**
     * This enum contains constants representing the lengths of various notes. The length of a 128th note is
     * 1*2*3*4*5*7 ticks to allow n-tuples with n from 3..10. The length of the other notes are multiples of
     * this number. All durations and time-stamps are calculated in these units.
     */
    enum NoteLength {
        Note128Length = 1*2*3*4*5*7,
        Note64Length = Note128Length * 2,
        Note32Length = Note64Length * 2,
        Note16Length = Note32Length * 2,
        Note8Length = Note16Length * 2,
        QuarterLength = Note8Length * 2,
        HalfLength = QuarterLength * 2,
        WholeLength = HalfLength * 2,
        DoubleWholeLength = WholeLength * 2
    };
    
    /**
     * This enum represents the various supported durations for chords/rests.
     */
    enum Duration {
        HundredTwentyEighthNote,
        SixtyFourthNote,
        ThirtySecondNote,
        SixteenthNote,
        EighthNote,
        QuarterNote,
        HalfNote,
        WholeNote,
        BreveNote
    };
    
    enum StemDirection {
        StemUp,
        StemDown
    };
    
    enum BeamType {
        BeamStart,
        BeamContinue,
        BeamEnd,
        BeamFlag,
        BeamForwardHook,
        BeamBackwardHook
    };
    
    
    /**
     * Convert a duration to a number of ticks.
     *
     * @param duration the duration to convert to ticks
     */
    int durationToTicks(Duration duration);
    
    /**
     * Concert a duration to a string representation as it is expected when written to a MusicXML file.
     *
     * @param duration the duration to convert to a string
     */
    QString durationToString(Duration duration);
    
    
} // namespace MusicCore

#endif // MUSIC_CORE_GLOBAL_H
