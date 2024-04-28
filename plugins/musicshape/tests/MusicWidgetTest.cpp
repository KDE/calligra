/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "MusicWidget.h"
#include "../core/Bar.h"
#include "../core/Chord.h"
#include "../core/Clef.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/VoiceBar.h"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QTest>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Load the font that is used by the music widget
    QString fontFile = QFINDTESTDATA("../fonts/Emmentaler-14.ttf");
    if (QFontDatabase::addApplicationFont(fontFile) == -1) {
        qWarning() << "Could not load emmentaler font";
    }

    MusicWidget w;
    // Setup a simple sheet, with one part existing of one staff and one voice, and a single bar
    MusicCore::Sheet *sheet = new MusicCore::Sheet(&w);
    MusicCore::Bar *bar = sheet->addBar();
    MusicCore::Part *part = sheet->addPart("Part 1");
    MusicCore::Staff *staff = part->addStaff();
    MusicCore::Voice *voice = part->addVoice();

    // Add a clef to the staff
    MusicCore::Clef *clef = new MusicCore::Clef(staff, 0, MusicCore::Clef::Trebble, 2, 0);
    bar->addStaffElement(clef);

    // And add some notes (middle C up-to C one octave higher)
    for (int i = 0; i < 8; i++) {
        MusicCore::Chord *chord = new MusicCore::Chord(MusicCore::HalfNote);
        MusicCore::Note *note = chord->addNote(staff, i); // central C
        Q_ASSERT(note);
        Q_UNUSED(note);
        bar->voice(voice)->addElement(chord);
    }
    w.setSheet(sheet);
    w.setScale(1.5);
    w.show();
    return app.exec();
}
