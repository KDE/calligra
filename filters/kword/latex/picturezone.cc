/* MEMO: to see the unicode table
 * xset +fp /usr/X11R6/lib/X11/fonts/ucs/
 * xfd -fn '-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso10646-1'
 */
/*
** A program to convert the XML rendered by KWord into LATEX.
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

#include <kdebug.h>		/* for kdDebug() stream */

#include "picturezone.h"
#include "para.h"


/*******************************************/
/* Constructor                             */
/*******************************************/
PictureZone::PictureZone(Para *para)
{
	setPara(para);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
PictureZone::~PictureZone()
{
	kdDebug() << "Destruction of a picture zone" << endl;
}

/*******************************************/
/* analyse                                 */
/*******************************************/
/* Analyse a text format, get the text used*/
/* by this format.                         */
/*******************************************/
void PictureZone::analyse(const Markup * balise_initiale)
{
	kdDebug() << "FORMAT" << endl;
	/* Get header information (size, position)
	 * Get infos. to delete the text
	 */
	
	if(balise_initiale != 0)
		analysePictureFormat(balise_initiale);
	
	/* Convert the picture */

	kdDebug() << "END FORMAT" << endl;
}

/*******************************************/
/* generate                                */
/*******************************************/
/* Generate the text formated (if needed). */
/*******************************************/
void PictureZone::generate(QTextStream &out)
{

	generate_format_begin(out);

	/* Display the picture */
	//convert(getFilename());
	out << "\\includegraphics{" << getFilenameEPS()<< "}" << endl;

	generate_format_end(out);
}

/*******************************************/
/* generate_format_begin                   */
/*******************************************/
/* Write the begining format markup.       */
/*******************************************/
void PictureZone::generate_format_begin(QTextStream & out)
{
	kdDebug() << "GENERATE PICTURE FORMAT" << endl;
}

/*******************************************/
/* generate_format_end                     */
/*******************************************/
/* Write the format end markup.            */
/*******************************************/
void PictureZone::generate_format_end(QTextStream & out)
{

}
