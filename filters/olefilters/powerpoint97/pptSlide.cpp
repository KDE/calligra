/*
    Copyright (C) 2002, M.Marcucio <michaelmarcucio@hotmail.com>.
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

*/

#include <pptSlide.h>

//PptSlide::PptSlide()
//{
//}

//PptSlide::~PptSlide()
//{
//}

void PptSlide::AddText(QString text, Q_UINT16 type)
{
	unsigned j;

	switch (type)
	{
	case TITLE_TEXT:
	case CENTER_TITLE_TEXT:
		m_titleText = text;
		//m_titleText.append("\n");
		break;
	case NOTES_TEXT:
		m_notesText = text;
		m_notesText.append("\n");
		break;
	case BODY_TEXT:
	case OTHER_TEXT:
	case CENTER_BODY_TEXT:
	case HALF_BODY_TEXT:
	case QUARTER_BODY_TEXT:
	        QStringList data(QStringList::split(QChar('\r'), text, true));
		for (j = 0; j < data.count(); j++)
		{
			m_body.bodyText.append(data[j]);
		}
		m_body.type = type;
		break;
	}
}//addtext

