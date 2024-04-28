/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Voice.h"
#include "Bar.h"
#include "Part.h"
#include "Sheet.h"
#include "VoiceBar.h"
#include <QList>

namespace MusicCore
{

class Voice::Private
{
public:
};

Voice::Voice(Part *part)
    : QObject(part)
    , d(new Private)
{
}

Voice::~Voice()
{
    delete d;
}

Part *Voice::part()
{
    return qobject_cast<Part *>(parent());
}

VoiceBar *Voice::bar(Bar *bar)
{
    return bar->voice(this);
}

VoiceBar *Voice::bar(int barIdx)
{
    return bar(part()->sheet()->bar(barIdx));
}

} // namespace MusicCore
