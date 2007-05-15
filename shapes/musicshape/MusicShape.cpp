/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.kruiselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "MusicShape.h"
#include <QPainter>
#include <kdebug.h>
#include <KoViewConverter.h>

#include "core/Sheet.h"
#include "core/Part.h"
#include "core/Voice.h"
#include "core/Staff.h"
#include "core/VoiceBar.h"
#include "core/Chord.h"
#include "core/Note.h"
#include "core/Clef.h"
#include "core/Bar.h"
#include "core/KeySignature.h"
#include "core/TimeSignature.h"

#include "MusicStyle.h"
#include "Engraver.h"
#include "Renderer.h"

using namespace MusicCore;

// helper method, used by the constructor to easily create a short piece of music
static Chord* mkNote(Chord::Duration duration, Staff* staff, int pitch)
{
    Chord* c = new Chord(duration);
    c->addNote(staff, pitch);
    return c;
}

MusicShape::MusicShape() : m_style(new MusicStyle), m_engraver(new Engraver()), m_renderer(new MusicRenderer(m_style))
{
    m_sheet = new Sheet();
    Bar* b1 = m_sheet->addBar();
    Bar* b2 = m_sheet->addBar();
    Bar* b3 = m_sheet->addBar();
    
    Part* part = m_sheet->addPart("Violin");
    Staff* staff = part->addStaff();
    Voice* voice = part->addVoice();
    voice->bar(b1)->addElement(new Clef(staff, Clef::Trebble, 2, 0));
    
    part = m_sheet->addPart("Piano");
    staff = part->addStaff();
    Staff* staff2 = part->addStaff();
    voice = part->addVoice();
    Voice* voice2 = part->addVoice();

    voice->bar(b1)->addElement(new Clef(staff, Clef::Trebble, 2, 0));
    voice->bar(b1)->addElement(new KeySignature(staff, -4));
    voice->bar(b1)->addElement(new TimeSignature(staff, 4, 4));
    voice->bar(b1)->addElement(mkNote(Chord::Quarter, staff, 0));
    voice->bar(b1)->addElement(mkNote(Chord::Quarter, staff, 1));
    voice->bar(b1)->addElement(mkNote(Chord::Quarter, staff, 2));
    voice->bar(b1)->addElement(mkNote(Chord::Quarter, staff, 0));
    voice->bar(b2)->addElement(mkNote(Chord::Quarter, staff, 0));
    voice->bar(b2)->addElement(mkNote(Chord::Quarter, staff, 1));
    voice->bar(b2)->addElement(mkNote(Chord::Quarter, staff, 2));
    voice->bar(b2)->addElement(mkNote(Chord::Quarter, staff, 0));
    voice->bar(b3)->addElement(mkNote(Chord::Quarter, staff, 2));
    voice->bar(b3)->addElement(mkNote(Chord::Quarter, staff, 3));
    voice->bar(b3)->addElement(mkNote(Chord::Half, staff, 4));
    voice2->bar(b1)->addElement(new Clef(staff2, Clef::Bass, 3, 0));
    voice2->bar(b1)->addElement(new KeySignature(staff2, 5));
    voice2->bar(b1)->addElement(new TimeSignature(staff2, 4, 4, TimeSignature::Number));
    voice2->bar(b1)->addElement(new Chord(staff2, Chord::Whole));
    voice2->bar(b2)->addElement(new Chord(staff2, Chord::Quarter));
    voice2->bar(b2)->addElement(new Chord(staff2, Chord::Eighth));
    voice2->bar(b2)->addElement(new Chord(staff2, Chord::Sixteenth));
    voice2->bar(b2)->addElement(new Chord(staff2, Chord::Sixteenth));
    voice2->bar(b2)->addElement(new Chord(staff2, Chord::Half));
    voice2->bar(b3)->addElement(mkNote(Chord::Quarter, staff2, 0));
    voice2->bar(b3)->addElement(mkNote(Chord::Quarter, staff2, 1));
    voice2->bar(b3)->addElement(mkNote(Chord::Quarter, staff2, 2));
    voice2->bar(b3)->addElement(mkNote(Chord::Quarter, staff2, 0));
    m_engraver->engraveSheet(m_sheet);
}

MusicShape::~MusicShape()
{
    delete m_sheet;
    delete m_style;
    delete m_renderer;
}

void MusicShape::resize( const QSizeF &newSize )
{
//  kDebug()<<" MusicShape::resize( const QSizeF &newSize ) " << newSize << endl;
    KoShape::resize(newSize);
}

void MusicShape::paint( QPainter& painter, const KoViewConverter& converter )
{
//  kDebug()<<" MusicShape::paint( QPainter& painter, const KoViewConverter& converter )\n";

    applyConversion( painter, converter );

    painter.setClipping(true);
    painter.setClipRect(QRectF(0, 0, size().width(), size().height()));

    m_renderer->renderSheet( painter, m_sheet );
}

void MusicShape::saveOdf( KoShapeSavingContext * context ) const
{
    // TODO
}

bool MusicShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}

Sheet* MusicShape::sheet()
{
    return m_sheet;
}

MusicRenderer* MusicShape::renderer()
{
    return m_renderer;
}

void MusicShape::engrave()
{
    m_engraver->engraveSheet(m_sheet);
}

