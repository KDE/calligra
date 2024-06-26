/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TimeSignature.h"
#include "Global.h"

#include <QString>

namespace MusicCore
{

class TimeSignature::Private
{
public:
    int beats;
    int beat;
    TimeSignatureType type;
    Private()
        : beats(0)
        , beat(0)
        , type(Classical)
    {
    }
};

TimeSignature::TimeSignature(Staff *staff, int startTime, int beats, int beat, TimeSignatureType type)
    : StaffElement(staff, startTime)
    , d(new Private)
{
    setBeats(beats);
    setBeat(beat);
    d->type = type;
}

TimeSignature::~TimeSignature()
{
    delete d;
}

int TimeSignature::priority() const
{
    return 50;
}

int TimeSignature::beats() const
{
    return d->beats;
}

void TimeSignature::setBeats(int beats)
{
    if (d->beats == beats)
        return;
    d->beats = beats;
    int beatsLen = QString::number(d->beats).length();
    int beatLen = QString::number(d->beat).length();
    setWidth(8 * qMax(beatsLen, beatLen));
    Q_EMIT beatsChanged(beats);
}

int TimeSignature::beat() const
{
    return d->beat;
}

void TimeSignature::setBeat(int beat)
{
    if (d->beat == beat)
        return;
    Q_ASSERT((beat & (beat - 1)) == 0);
    d->beat = beat;
    int beatsLen = QString::number(d->beats).length();
    int beatLen = QString::number(d->beat).length();
    setWidth(8 * qMax(beatsLen, beatLen));
    Q_EMIT beatChanged(beat);
}

TimeSignature::TimeSignatureType TimeSignature::type() const
{
    return d->type;
}

void TimeSignature::setType(TimeSignatureType type)
{
    if (d->type == type)
        return;
    d->type = type;
    Q_EMIT typeChanged(type);
}

QList<int> TimeSignature::beatLengths() const
{
    int beatLength;
    QList<int> res;
    switch (d->beat) {
    case 1:
        beatLength = WholeLength;
        break;
    case 2:
        beatLength = HalfLength;
        break;
    case 4:
        beatLength = QuarterLength;
        break;
    case 8:
        beatLength = Note8Length;
        break;
    case 16:
        beatLength = Note16Length;
        break;
    case 32:
        beatLength = Note32Length;
        break;
    case 64:
        beatLength = Note64Length;
        break;
    case 128:
        beatLength = Note128Length;
        break;
    default:
        beatLength = QuarterLength;
    }
    if (d->beats % 3 == 0) {
        for (int i = 0; i < d->beats / 3; i++) {
            res.append(beatLength * 3);
        }
    } else {
        int totalLength = beatLength * d->beats;
        int groupLength = beatLength;
        if (d->beat >= 8)
            groupLength = 4 * beatLength;
        while (totalLength > 0) {
            if (totalLength >= groupLength) {
                res.append(groupLength);
            } else {
                res.append(totalLength);
            }
            totalLength -= groupLength;
        }
    }

    return res;
}

} // namespace MusicCore
