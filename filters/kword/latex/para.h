/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_PARA_H__
#define __KWORD_PARA_H__

#include <qstring.h>

#include "listeformat.h"	/* children class contents the zone (italic, footnote,
				   variable. */
#include "layout.h"		/* set of informations about the paragraph style. */
#include "element.h"		/* to use the father class. */
//#include "heap.h"		/* to manage a heap. */

enum EP_INFO
{
	EP_NONE,
	EP_FOOTNOTE
};

enum EP_HARDBRK
{
	EP_FLOW,
	EP_NEXT
};

class Texte;

/***********************************************************************/
/* Class: Para                                                         */
/***********************************************************************/

/**
 * This class hold a real paragraph. It tells about the text in this
 * paragraph, its format, etc. The complete text is a list of Para instances.
 * A footnote is a list of paragprah instances.
 */
class Para: public Layout
{
	/* MARKUP DATA */
	QString        _texte;
	QString*       _name;
	EP_INFO        _info;
	EP_HARDBRK     _hardbrk;
	ListeFormat*   _lines;

	/* TO MANAGE THE LIST */
	Para*          _next;
	Para*          _previous;

	/* USEFULL DATA */
	Texte*         _element;	/* Father frame */
	unsigned int   _currentPos;	/* Begining of the text for use the good format */
	//Heap           _heapEnum;	/* opened enum but not closed */

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instances of Para.
		 *
		 * @param Texte the text this paragraph is belonging to.
		 */
		Para(Texte *texte = 0);

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
		Para*    getNext      () const { return _next;      }
		Para*    getPrevious  () const { return _previous;  }
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
		Texte*   getTexte     () const { return _element;   }
		/**
		 * @return the frame type (Header, footer, body or footnote).
		 */
		SSect    getFrameType () const;
		/**
		 * @return the next format type (picture, text, variable, footnote).
		 */
		EFormat  getTypeFormat(const Markup*) const;

		/**
		 * Modifiors
		 */
		void setNext    (Para *p)  { _next      = p;    }
		void setPrevious(Para *p)  { _previous  = p;    }

		/**
		 * Catch informations from a markup list.
		 */
		void analyse         (const Markup*);
		/**
		 * Write the paragraph in a file.
		 */
		void generate        (QTextStream&);

	private:
		void analyseParam     (const Markup*);
		void analyseName      (const Markup*);
		void analyseInfo      (const Markup*);
		void analyseBrk       (const Markup*);
		void analyseLayoutPara(const Markup*);
		void analyseFormat    (const Markup*);
		void analyseFormats   (const Markup*);

		/**
		 * If the paragraph is a title, generate the command.
		 */
		void generateTitle(QTextStream&);

		/**
		 * Write the paragraph style, format.
		 */
		void generateDebut   (QTextStream&);
		void generateFin     (QTextStream&);
};

#endif /* __KWORD_PARA_H__ */
