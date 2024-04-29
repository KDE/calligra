/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSICCURSOR_H
#define MUSICCURSOR_H

#include <QObject>

namespace MusicCore
{
class Sheet;
class Staff;
class VoiceBar;
}

/**
 * Class that provides information for the current (keyboard) cursor position in a music shape.
 */
class MusicCursor : public QObject
{
    Q_OBJECT
public:
    explicit MusicCursor(MusicCore::Sheet *sheet, QObject *parent = nullptr);

    void moveRight();
    void moveLeft();
    void moveUp();
    void moveDown();
    void setVoice(int voice);

    MusicCore::Staff *staff() const
    {
        return m_staff;
    }
    int voice() const
    {
        return m_voice;
    }
    int bar() const
    {
        return m_bar;
    }
    int element() const
    {
        return m_element;
    }
    int line() const
    {
        return m_line;
    }
    MusicCore::VoiceBar *voiceBar() const;

private:
    MusicCore::Sheet *m_sheet;
    MusicCore::Staff *m_staff;
    int m_voice;
    int m_bar;
    int m_element;
    int m_line;
};

#endif // MUSICCURSOR_H
