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

#include <stdlib.h>		/* for atoi function    */

#include <kdebug.h>

#include "fileheader.h"
#include "document.h"
#include "texte.h"
#include "formula.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Document::Document()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Document::~Document()
{
	kdDebug() << "Corps Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Document::analyse(const QDomNode balise)
{
	//QDomNode balise = getChild(balise_initial, "FRAMESET");
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Element *elt = 0;
		kdDebug() << "NEW ELEMENT" << endl;
		
		kdDebug() << getChildName(balise, index) << endl;
		switch(getTypeFrameset(getChild(balise, index)))
		{
			case ST_NONE: 
				kdDebug() << "NONE" << endl;
				break;
			case ST_TEXT: 
				kdDebug() << "TEXT" << endl;
				elt = new Texte;
				elt->analyse(getChild(balise, index));
				break;
			case ST_PICTURE:
				kdDebug() << "PICTURE" << endl;
				// elt = new Image;
				// elt->analyse(getChild(balise, index));
				break;
			case ST_PART:
				kdDebug() << "PART" << endl;
				//elt = new Part;
				//elt->analyse(getChild(balise, index));
				break;
			case ST_FORMULA:
				/* Just save the frameset in a QString input
				 * call the formula latex export filter
				 * save in output
				 * generate : write the output
				 */
				kdDebug() << "FORMULA" << endl;
				elt = new Formula;
				elt->analyse(getChild(balise, index));
				break;
			default:
				kdDebug() << "error " << elt->getType() << " " << ST_TEXT << endl;
		}
			
		/* 3. Add the Element in one of the lists */
		if(elt != 0)
		{
			kdDebug() << "INFO : " << elt->getSection() << endl;
			switch(elt->getSection())
			{
				case SS_FOOTERS: kdDebug() << " FOOTER" <<endl;
					       _footers.append(elt);
					       break;
				case SS_HEADERS: kdDebug() << " HEADER" << endl;
						_headers.append(elt);
					break;
				case SS_BODY:
					if(!elt->isTable())
					{
						switch(elt->getType())
						{
							case ST_TEXT:
							case ST_PICTURE:
									_corps.append(elt);
									kdDebug() << " BODY" << endl;
								break;
							case ST_PART:
									kdDebug() << " PART" <<endl;
									//_parts.append(elt);
								break;
							case ST_FORMULA:
									kdDebug() << " FORMULA" <<endl;
									_formulas.append(elt);
								break;
						}
					}
					break;
				case SS_TABLE:
					kdDebug() << " TABLE" <<endl;
					/* Don't add simplely the cell */
					/* heriter ListTable de ListElement et surcharger
					 * la methode add. Une cellule est un element.
					 */
					_tables.add(elt);
					if(_fileHeader!= 0)
						_fileHeader->useTable();
					break;
				case SS_FOOTNOTES: /* Just for the new kwd file version */
						_footnotes.append(elt);
				break;
				default: kdDebug() << "UNKNOWN" << endl;
					break;
			}
		}
		kdDebug() << "END OF ANALYSE OF A FRAMESET" << endl;
	}
}

/*******************************************/
/* getTypeFrameset                         */
/*******************************************/
SType Document::getTypeFrameset(const QDomNode balise)
{
	SType type = ST_NONE;

	type = (SType) getAttr(balise, "frameType").toInt();
	kdDebug() << "(end type analyse)" << endl;
	return type;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Document::generate(QTextStream &out, bool hasPreambule)
{
	kdDebug() << "DOC. GENERATION." << endl;
	
	if(hasPreambule)
		generatePreambule(out);
	kdDebug() << "preambule : " << hasPreambule << endl;

	/* Body */
	kdDebug() << endl << "body : " << _corps.count() << endl;

	if(hasPreambule)
		out << "\\begin{document}" << endl;
	if(_corps.getFirst() != 0)
		_corps.getFirst()->generate(out);

	/* Just for test */
	/*if(_tables.getFirst() != 0)
		_tables.getFirst()->generate(out);
	if(_formulas.getFirst() != 0)
		_formulas.getFirst()->generate(out);*/
	if(hasPreambule)
		out << "\\end{document}" << endl;

}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void Document::generatePreambule(QTextStream &out)
{
	Element* header;
	Element* footer;

	/* For each header */
	if(getFileHeader()->hasHeader())
	{
		kdDebug() << "header : " << _headers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\headrulewidth}{0pt}" << endl;
		for(header = _headers.first(); header != 0; header = _headers.next())
		{
			generateTypeHeader(out, header);
		}
	}
	
	/* For each footer */
	if(getFileHeader()->hasFooter())
	{
		kdDebug() << "footer : " << _footers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\footrulewidth}{0pt}" << endl;
		for(footer = _footers.first(); footer != 0; footer = _footers.next())
		{
			generateTypeFooter(out, footer);
		}
	}
	/* Specify what header/footer style to use */
	if(getFileHeader()->hasHeader() || getFileHeader()->hasFooter())
		out << "\\pagestyle{fancy}" << endl;
	else
	{
		out << "\\pagestyle{empty}" << endl;
	}
}

/*******************************************/
/* GenerateTypeHeader                      */
/*******************************************/
void Document::generateTypeHeader(QTextStream &out, Element *header)
{
	kdDebug() << "generate header" << endl;
	if((_fileHeader->getHeadType() == TH_ALL ||
		_fileHeader->getHeadType() == TH_FIRST) && header->getInfo() == SI_EVEN)
	{
		out << "\\fancyhead[L]{}" << endl;
		out << "\\fancyhead[C]{";
		header->generate(out);
		out << "}" << endl;
		out << "\\fancyhead[R]{}" << endl;
	}
	/*if(_fileHeader->getHeadType() == TH_EVODD)
	{*/
		switch(header->getInfo())
		{
			case SI_NONE:
			case SI_FIRST:
				break;
			case SI_ODD:
				out << "\\fancyhead[RO]{}" << endl;
				out << "\\fancyhead[CO]{";
				header->generate(out);
				out << "}" << endl;
				out << "\\fancyhead[LO]{}" << endl;
				break;
			case SI_EVEN:
				out << "\\fancyhead[RE]{}" << endl;
				out << "\\fancyhead[CE]{";
				header->generate(out);
				out << "}" << endl;
				out << "\\fancyhead[LE]{}" << endl;
				break;
		}
	//}
	//if(_fileHeader->getHeadType() == TH_FIRST && header->getInfo() == SI_FIRST)
	if(header->getInfo() == SI_FIRST)
	{
		out << "\\fancyhead{";
		header->generate(out);
		out << "}" << endl;
		out << "\\thispagestyle{fancy}" << endl;
	}
}

/*******************************************/
/* GenerateTypeFooter                      */
/*******************************************/
void Document::generateTypeFooter(QTextStream &out, Element *footer)
{
	if(_fileHeader->getFootType() == TH_ALL && footer->getInfo() == SI_EVEN)
	{
		out << "\\fancyfoot[L]{}" << endl;
		out << "\\fancyfoot[C]{";
		footer->generate(out);
		out << "}" << endl;
		out << "\\fancyfoot[R]{}" << endl;
	}
	else if(_fileHeader->getFootType() == TH_EVODD)
	{
		switch(footer->getInfo())
		{
			case SI_NONE:
			case SI_FIRST:
				break;
			case SI_ODD:
				out << "\\fancyfoot[CO]{";
				footer->generate(out);
				out << "}";;
				break;
			case SI_EVEN:
				out << "\\fancyfoot[CE]{";
				footer->generate(out);
				out << "}";
				break;
		}
	}
	else if(_fileHeader->getFootType() == TH_FIRST && footer->getInfo() == SI_FIRST)
	{
		out << "\\fanycfoot{";
		footer->generate(out);
		out << "}" << endl;
		out << "\\thispagestyle{fancy}" << endl;
	}
}

Element* Document::searchAnchor(QString anchor)
{
	Element *elt = _tables.first();
	while(elt != 0)
	{
		kdDebug() << elt->getGrpMgr() << endl;
		if(elt->getGrpMgr() == anchor)
			return elt;
		elt = _tables.next();
	}
	kdDebug() << "Pas de table, recherche dans les formules" << endl;
	elt = _formulas.first();
	while(elt != 0)
	{
		if(elt->getName() == anchor)
			return elt;
		elt = _formulas.next();
	}
	return NULL;

}

Element* Document::searchFootnote(QString footnote)
{
	Element* elt = _footnotes.first();
	while(elt != 0)
	{
		if(elt->getName() == footnote)
			return elt;
		elt = _footnotes.next();
	}
	return NULL;

}
