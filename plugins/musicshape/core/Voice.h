/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSIC_CORE_VOICE_H
#define MUSIC_CORE_VOICE_H

#include <QObject>
#include <QString>

namespace MusicCore {

class Part;
class VoiceBar;
class Bar;

/**
 * A voice contains the actual musical elements in a piece of music.
 */
class Voice : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a new voice for the given part. This does not actually add the voice to the part, for that call the
     * addVoice method of the part.
     *
     * @param part the part this voice belongs to
     */
    explicit Voice(Part* part);

    /**
     * Destructor.
     */
    ~Voice() override;

    /**
     * Returns the part this voice is part of.
     */
    Part* part();

    /**
     * Sets the part this voice belongs to. Do not call this method when the voice already is added to a part.
     *
     * @param part the new part this voice belongs to
     */
    void setPart(Part* part);

    /**
     * Returns the bar in this voice that contains the elements in the given bar in this piece of music.
     *
     * @param bar the bar for which to return the VoiceBar instance.
     */
    VoiceBar* bar(Bar* bar);

    VoiceBar* bar(int barIdx);
private:
    class Private;
    Private * const d;
};

} // namespace MusicCore

#endif // MUSIC_CORE_VOICE_H
