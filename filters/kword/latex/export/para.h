/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000,2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_PARA_H__
#define __KWORD_PARA_H__

#include <QString>
#include <q3ptrstack.h>		/* historic list */
#include <q3ptrlist.h>		/* for list of format */
//Added by qt3to4:
#include <QTextStream>

/*#include "listeformat.h"*/	/* children class contents the zone (italic, footnote,
				   variable. */
#include "layout.h"		/* set of information about the paragraph style. */
#include "element.h"		/* to use the father class. */
#include "format.h"		/* child class */

enum EP_INFO
{
	EP_NONE,
	EP_FOOTNOTE
};

/*enum EP_HARDBRK
{
	EP_FLOW,
	EP_NEXT
};*/

class TextFrame;

/***********************************************************************/
/* Class: Para                                                         */
/***********************************************************************/

/**
 * This class hold a real paragraph. It tells about the text in this
 * paragraph, its format, etc. The complete text is a list of Para instances.
 * A footnote is a list of paragraph instances (now but not in the "futur").
 */
class Para: public Layout
{
	/* MARKUP DATA */
	QString        _text;
	QString*       _name;
	EP_INFO        _info;
	//EP_HARDBRK     _hardbrk;
	Q3PtrList<Format>* _lines;

	/* TO MANAGE THE LIST */

	/* USEFULL DATA */
	TextFrame*             	_element;		/* Father frame */
	unsigned int          	_currentPos;	/* Begining of the text to use the good format */
	static Q3PtrStack<EType> _historicList;	/* opened lists but not closed */
	int                   	_nbLines;		/* Nb of lines in a cell (table) */
	static int				_tabulation;	/* Size of the para tabulation (for lists). */

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Para.
		 *
		 * @param textFrame The text frame this paragraph is belonging to.
		 */
		Para(TextFrame *textFrame = 0);

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of little zones.
		 */
		virtual ~Para();

		/**
		 * Accessors
		 */

		/**
		 * @return true if the paragraph has a colored word.
		 */
		//bool     isColored    () const;
		/**
		 * @return true if the paragraph has a underlined word.
		 */
		//bool     isUlined     () const;
		//Para*    getNext      () const { return _next;      }
		//Para*    getPrevious  () const { return _previous;  }
		/**
		 *  @return the paragraph's name.
		 */
		QString* getName      () const { return _name;      }
		/**
		 * @return the paragraph's type (contents or footnote).
		 */
		EP_INFO  getInfo      () const { return _info;      }
		/**
		 * @return the frame the paragraph belonging to.
		 */
		TextFrame*   getFrame     () const { return _element;   }
		/**
		 * @return the frame type (Header, footer, body, footnote or table, ...).
		 */
		SSect    getFrameType () const;
		/**
		 * @return the next format type (picture, text, variable, footnote).
		 */
		EFormat  getTypeFormat(const QDomNode) const;
		/**
		 * @return count the number of characters in the paragraph.
		 */
		int getNbCharPara() const;

		bool notEmpty() const { return (_lines == 0) ? false : (_lines->count() != 0); }
		/**
		 * Modifiers
		 */
		//void setNext    (Para *p)  { _next      = p;    }
		//void setPrevious(Para *p)  { _previous  = p;    }

		/**
		 * Helpful functions
		 */

		/**
		 * Get information from a markup tree.
		 */
		void analyse         (const QDomNode);

		/**
		 * Write the paragraph in a file.
		 */
		void generate        (QTextStream&);

		/**
		 * If the paragraph has a different environment, change it
		 */
		void generateBeginEnv(QTextStream&);

		/**
		 * If the next paragraph has a different environment, close it
		 */
		void generateEndEnv(QTextStream&);

		/**
		 * If the paragraph is a title, generate the command.
		 */
		void generateTitle    (QTextStream&);

		/**
		 * Write the markup to begin a list
		 */
		void openList         (QTextStream&);

		/**
		 * Write the markup to close a list
		 */
		void closeList        (QTextStream&, Para*);

	private:
		void analyseParam     (const QDomNode);
		void analyseName      (const QDomNode);
		void analyseInfo      (const QDomNode);
		//void analyseBrk       (const QDomNode);
		void analyseLayoutPara(const QDomNode);
		void analyseFormat    (const QDomNode);
		void analyseFormats   (const QDomNode);

		/**
		 * Write the paragraph style, format.
		 */
		void generateDebut    (QTextStream&);
		void generateFin      (QTextStream&);

		/**
		 * Write the markup to close a list
		 */
		void closeList        (EType, QTextStream&);

};

#endif /* __KWORD_PARA_H__ */
