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
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>

//--text types
#define TITLE_TEXT 			0	//title
#define BODY_TEXT 			1	//body
#define NOTES_TEXT 		2	//notes
#define NOTUSED_TEXT 		3	//not used
#define OTHER_TEXT 		4	//other(test in shape)
#define CENTER_BODY_TEXT 	5	//center body(subtitle in title slide)
#define CENTER_TITLE_TEXT 	6	//center title(title in title slide)
#define HALF_BODY_TEXT 	7	//half body(body in two-column slide)
#define QUARTER_BODY_TEXT 	8	//quarter body(body in four-body slide)
//--

//--char style types
enum
{
	BOLD_STYLE 		= 1,
	ITALIC_STYLE 		= 2,
	UNDERLINE_STYLE 	= 3
};
//--

class PptSlide
{
public:
	typedef struct
	{
		Q_UINT16 		style;				//the style of the text run
		Q_UINT16 		length;			//length of the style run
	}styleRunType;
	
	typedef QPtrList<styleRunType>  styleRun;
	
				PptSlide();
 	QStringList 	getPlaceholderText(void);					//gets the list of paragraphs from the placeholder
	//styleRun 		getPlaceholderStyleRun(void);				//gets the list of placeholder style runs
	Q_UINT16 		getPlaceholderType(void);					//gets the type of the placeholder
	Q_UINT16 		gotoPlaceholder(Q_UINT16 pholderNumber);		//goto the n'th placeholder
	Q_UINT16 		getNumberOfPholders();					//gets the number of placeholders on slide
	
	Q_INT32	 	getPsrReference(void);
	void 			setPsrReference(Q_INT32 psr);
	
	void 			addText(QString text, Q_UINT16 type);				//adds text of the given type
	void 			addToStyleRun(Q_UINT16 style, Q_UINT16 length);	//adds to the current style run

private:
	typedef struct
	{
		QStringList 		paragraphs;		//text of the placeholder
		Q_UINT16 			type;				//what is the text type
		styleRun			style;				//char style info
	}placeholder;

	Q_INT16 			m_slideLayout;		//type of slide
	Q_INT16 			m_numberOfPholders;	//number of placeholder on the slide
	Q_INT32			m_psrReference;		//logical reference
     	QPtrList<placeholder>	m_placeholderList;	//list of all the placeholders on the slide
     	placeholder*		m_currentPholder;	//pointer to current placeholder

};

//#endif
