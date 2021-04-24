/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Global.h"

namespace MusicCore {

int durationToTicks(Duration duration)
{
    switch (duration) {
        case HundredTwentyEighthNote: return Note128Length;
        case SixtyFourthNote:         return Note64Length;
        case ThirtySecondNote:        return Note32Length;
        case SixteenthNote:           return Note16Length;
        case EighthNote:              return Note8Length;
        case QuarterNote:             return QuarterLength;
        case HalfNote:                return HalfLength;
        case WholeNote:               return WholeLength;
        case BreveNote:               return DoubleWholeLength;
    }
    return 0;
}

QString durationToString(Duration duration)
{
    switch (duration) {
        case HundredTwentyEighthNote:   return "128th";
        case SixtyFourthNote:           return "64th";
        case ThirtySecondNote:          return "32nd";
        case SixteenthNote:             return "16th";
        case EighthNote:                return "eighth";
        case QuarterNote:               return "quarter";
        case HalfNote:                  return "half";
        case WholeNote:                 return "whole";
        case BreveNote:                 return "breve";
    }
    return "[unknown note length]";
}

} // namespace MusicCore
