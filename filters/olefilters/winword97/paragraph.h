/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file implements an abstraction for paragraph properties in Microsoft
    Word documents. In other words, it is an abstraction for the PAP structure.
*/

#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include <msword.h>

class Paragraph
{
public:

    // Create a paragraph with default properties.

    Paragraph(MsWord &document);
    ~Paragraph();

    // Modify the paragraph with style information from various sources...
    //
    // An array of SPRMs (grpprl) with an optional TAP.
    // An existing base style.
    // List format.
    // Property exceptions.
    // Paragraph height.
    // Predefined style from stylesheet.

    void apply(const MsWord::U8 *grpprl, unsigned count, MsWord::TAP *tap = NULL);
    void apply(MsWord::U16 style);
    void apply(MsWord::LFO &style);
    void apply(MsWord::PAPXFKP &style);
    void apply(MsWord::PHE &layout);
    void apply(MsWord::STD &style);

private:
    friend class MsWord;

    MsWord &m_document;
    MsWord::PAP m_pap;
};
#endif // PARAGRAPH_H
