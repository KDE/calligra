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

//#ifndef PPTSLIDE_H
//#define PPTSLIDE_H

#include <kdebug.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

#define TITLE_TEXT 			0	//title
#define BODY_TEXT 			1	//body
#define NOTES_TEXT 		2	//notes
#define NOTUSED_TEXT 		3	//not used
#define OTHER_TEXT 		4	//other(test in shape)
#define CENTER_BODY_TEXT 	5	//center body(subtitle in title slide)
#define CENTER_TITLE_TEXT 	6	//center title(title in title slide)
#define HALF_BODY_TEXT 	7	//half body(body in two-column slide)
#define QUARTER_BODY_TEXT 	8	//quarter body(body in four-body slide)


class PptSlide
{
public:
 	inline QString* 		GetTitleText(void){return &m_titleText;}
	inline QStringList 	GetBodyText(void){return m_body.bodyText;}
	inline Q_UINT16 	GetBodyType(void){return m_body.type;}
	inline QString*		GetNotesText(void){return &m_notesText;}
	inline Q_INT32 		GetPsrReference(void){return m_psrReference;}
	inline Q_INT32 		SetPsrReference(Q_INT32 psr){m_psrReference = psr;}
	         void 		AddText(QString text, Q_UINT16 type);		//adds text of the given type

private:

typedef struct
{
        QStringList 	bodyText;		//text of the body of the slide
	Q_UINT16 		type;				//what is the text type
}bodyPlaceholder;

	Q_INT16 			m_slideLayout;		//type of slide
	Q_INT32			m_psrReference;		//logical reference
        QString 			m_titleText;			//text of the title of slide
        QString 			m_notesText;		//text of the notes of the slide
	bodyPlaceholder		m_body;			//body of the slide
};

//#endif
