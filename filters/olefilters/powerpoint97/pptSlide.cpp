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

static const int s_area = 30512;

PptSlide::PptSlide()
{
	m_slideLayout = 0;		//type of slide
	m_numberOfPholders = 0;	//number of placeholder on the slide
	m_psrReference = 0;		//logical reference
}

//PptSlide::~PptSlide()
//{
//}

QStringList PptSlide::getPlaceholderText(void)
{
	return m_currentPholder->paragraphs;
}

//styleRun PptSlide::getPlaceholderStyleRun(void)
//{
//	return m_currentPholder->styleRun;
//}

Q_UINT16 PptSlide::getPlaceholderType(void)
{
	return m_currentPholder->type;
}

Q_UINT16 PptSlide::gotoPlaceholder(Q_UINT16 pholderNumber)
{
	m_currentPholder = m_placeholderList.at(pholderNumber);
}
	
Q_INT32 PptSlide::getPsrReference(void)
{
	return m_psrReference;
}

void PptSlide::setPsrReference(Q_INT32 psr)
{
	m_psrReference = psr;
}

Q_UINT16 PptSlide::getNumberOfPholders()
{
	return m_numberOfPholders;
}
	
void PptSlide::addText(QString text, Q_UINT16 type)
{
	unsigned j;
	m_currentPholder = new placeholder;
	m_placeholderList.append(m_currentPholder);
	m_numberOfPholders++;
	m_currentPholder->type = type;
	kdError(s_area) << "adding to slide now!!!!!!!!!!!! m_numberOfPholders: " << m_numberOfPholders << endl;
	
	switch (type)
	{
	case TITLE_TEXT:
	case CENTER_TITLE_TEXT:
		m_currentPholder->paragraphs.append(text);
		//m_titleText.append("\n");
		break;
	case NOTES_TEXT:
		m_currentPholder->paragraphs.append(text);
		m_currentPholder->paragraphs.append("\n");
		//m_notesText.append("\n");
		break;
	case BODY_TEXT:
	case OTHER_TEXT:
	case CENTER_BODY_TEXT:
	case HALF_BODY_TEXT:
	case QUARTER_BODY_TEXT:
	        QStringList data(QStringList::split(QChar('\r'), text, true));
		for (j = 0; j < data.count(); j++)
		{
			m_currentPholder->paragraphs.append(data[j]);
			//m_body.paragraphs.append(data[j]);
		}
		//m_body.type = type;
		break;
	}
}//addtext

