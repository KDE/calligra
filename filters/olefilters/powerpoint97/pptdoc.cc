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
#include <qlist.h>

PptDoc::PptDoc(
    const myFile &mainStream,
    const myFile &currentUser) :
        Powerpoint()
{
    m_mainStream = mainStream;
    m_currentUser = currentUser;
}

PptDoc::~PptDoc()
{
}

void PptDoc::gotSlide(
    Powerpoint::Slide &slide)
{
    Slide ourSlide;
    unsigned i;

    for (i = 0; i < slide.text.count(); i++)
    {
        SlideText *text = slide.text.at(i);
        QStringList data(QStringList::split(QChar('\r'), text->data, true));
        unsigned j;

    kdDebug() << "textxxxxxxxxxxxxxxxxxxxxxxx" << text->data << endl;
    kdDebug() << "textxxxxxxxxxxxxxxxxxxxxxxx" << data.join("\n") << endl;
        switch (text->type)
        {
        case 0:
        case 6:
            ourSlide.title = data.join("\n");
            break;
        case 2:
            ourSlide.notes = data.join("\n");
            break;
        default:
            for (j = 0; j < data.count(); j++)
            {
                ourSlide.body.append(data[j]);
            }
            break;
        }
    }
    gotSlide(ourSlide);
}

bool PptDoc::parse()
{
    return Powerpoint::parse(m_mainStream, m_currentUser);
}
