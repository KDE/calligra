/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_ENGRAVER_H
#define MUSIC_ENGRAVER_H

#include <QSizeF>

namespace MusicCore
{
class Bar;
class VoiceBar;
class Sheet;
class Part;
}

/**
 * This class is responsible for layint out notes and other musical elements inside a bar, and for laying out bars
 * within staff systems.
 */
class Engraver
{
public:
    Engraver();
    void engraveSheet(MusicCore::Sheet *sheet, int firstSystem, QSizeF size, bool engraveBars = true, int *lastSystem = nullptr);
    void engraveBar(MusicCore::Bar *bar, qreal sizeFactor = 1.0);
    qreal engraveBars(MusicCore::Sheet *sheet, int firstBar, int lastBar, qreal sizeFactor);
    void rebeamBar(MusicCore::Part *part, MusicCore::VoiceBar *bar);
};

#endif // MUSIC_ENGRAVER_H
