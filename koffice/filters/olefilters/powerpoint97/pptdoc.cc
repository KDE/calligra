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

    This file implements a simplified abstraction of Microsoft Powerpoint documents.
*/

#include <kdebug.h>
#include <pptdoc.h>
#include <qptrlist.h>

const int PptDoc::s_area = 30512;

PptDoc::PptDoc(
    const myFile &mainStream,
    const myFile &currentUser,
    const myFile &pictures) :
        Powerpoint()
{
    m_mainStream = mainStream;
    m_currentUser = currentUser;
    m_pictures = pictures;
}

PptDoc::~PptDoc()
{
}
/*
void PptDoc::gotSlide(
    PptSlide &slide)
{
    Slide ourSlide;
    unsigned i;

    for (i = 0; i < slide.text.count(); i++)
    {
        SlideText *text = slide.text.at(i);
        QStringList data(QStringList::split(QChar('\r'), text->data, true));
        unsigned j;

        switch (text->type)
        {
        case TITLE_TEXT:
        case CENTER_TITLE_TEXT:
        	ourSlide.title = data.join("\n");
            	break;
        case NOTES_TEXT:
            	ourSlide.notes = data.join("\n");
            	break;
	case BODY_TEXT:
	case OTHER_TEXT:
	case CENTER_BODY_TEXT:
	case HALF_BODY_TEXT:
	case QUARTER_BODY_TEXT:
		for (j = 0; j < data.count(); j++)
		{
			ourSlide.body.append(data[j]);
            	}
            	break;
        }
    }

    gotSlide(slide);
}
*/

bool PptDoc::parse()
{
    return Powerpoint::parse(m_mainStream, m_currentUser, m_pictures);
}
