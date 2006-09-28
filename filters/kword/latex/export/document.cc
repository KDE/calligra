/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2001, 2002 Robert JACOLIN
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

#include <stdlib.h>		/* for atoi function    */

#include <kdebug.h>
#include <ktempfile.h>
#include <KoStore.h>

#include <QDir>
//Added by qt3to4:
#include <QTextStream>

#include "fileheader.h"
#include "document.h"
#include "textFrame.h"
#include "formula.h"
#include "pixmapFrame.h"

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
	kDebug(30522) << "Document destructor" << endl;
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Document::analyze(const QDomNode balise)
{
	//QDomNode balise = getChild(balise_initial, "FRAMESET");
	kDebug(30522) << getChildName(balise, 0) << endl;
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Element *elt = 0;
		kDebug(30522) << "--------------------------------------------------" << endl;

		kDebug(30522) << getChildName(balise, index) << endl;
		switch(getTypeFrameset(getChild(balise, index)))
		{
			case ST_NONE:
				kDebug(30522) << "NONE" << endl;
				break;
			case ST_TEXT:
				kDebug(30522) << "TEXT" << endl;
				elt = new TextFrame;
				elt->analyze(getChild(balise, index));
				break;
			case ST_PICTURE:
				kDebug(30522) << "PICTURE" << endl;
				elt = new PixmapFrame();
				elt->analyze(getChild(balise, index));
				break;
			case ST_PART:
				kDebug(30522) << "PART" << endl;
				//elt = new Part;
				//elt->analyze(getChild(balise, index));
				break;
			case ST_FORMULA:
				/* Just save the frameset in a QString input
				 * call the formula latex export filter
				 * save in output
				 * generate: write the output
				 */
				kDebug(30522) << "FORMULA" << endl;
				elt = new Formula;
				elt->analyze(getChild(balise, index));
				break;
			case ST_HLINE:
				kDebug(30522) << "HLINE" << endl;
				break;
			default:
				kDebug(30522) << "error " << elt->getType() << " " << ST_TEXT << endl;
		}

		/* 3. Add the Element in one of the lists */
		if(elt != 0)
		{
			kDebug(30522) << "INFO : " << elt->getSection() << endl;
			switch(elt->getSection())
			{
				case SS_FOOTERS: kDebug(30522) << " FOOTER" <<endl;
					       _footers.append(elt);
					       break;
				case SS_HEADERS: kDebug(30522) << " HEADER" << endl;
						_headers.append(elt);
					break;
				case SS_BODY:
					if(!elt->isTable())
					{
						switch(elt->getType())
						{
							case ST_TEXT:
									_corps.append(elt);
									kDebug(30522) << " BODY" << endl;
								break;
							case ST_PART:
									kDebug(30522) << " PART" <<endl;
									//_parts.append(elt);
								break;
							case ST_FORMULA:
									kDebug(30522) << " FORMULA" <<endl;
									_formulas.append(elt);
								break;
							case ST_PICTURE:
									kDebug(30522) << " PIXMAP" <<endl;
									_pixmaps.append(elt);
								break;
							default:
									kError(30522) << "Element frame type no supported or type unexpected." << endl;
						}
					}
					break;
				case SS_TABLE:
					kDebug(30522) << " TABLE" <<endl;
					/* Don't add simplely the cell */
					/* heriter ListTable de ListElement et surcharger
					 * la methode add. Une cellule est un element.
					 */
					_tables.add(elt);
					FileHeader::instance()->useTable();
					break;
				case SS_FOOTNOTES: /* Just for the new kwd file version */
						_footnotes.append(elt);
				break;
				default: kDebug(30522) << "UNKNOWN" << endl;
					break;
			}
		}
		kDebug(30522) << "END OF ANALYSIS OF A FRAMESET" << endl;
	}
}

/*******************************************/
/* AnalyzePixmaps                          */
/*******************************************/
void Document::analyzePixmaps(const QDomNode balise)
{
	//QDomNode balise = getChild(balise_initial, "FRAMESET");
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Key *key = 0;
		kDebug(30522) << "NEW PIXMAP" << endl;

		key = new Key(Key::PIXMAP);
		key->analyze(getChild(balise, "KEY"));
		_keys.append(key);
	}
}

/*******************************************/
/* getTypeFrameset                         */
/*******************************************/
SType Document::getTypeFrameset(const QDomNode balise)
{
	SType type = ST_NONE;
	type = (SType) getAttr(balise, "frameType").toInt();
	return type;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Document::generate(QTextStream &out, bool hasPreambule)
{
	kDebug(30522) << "DOC. GENERATION." << endl;

	if(hasPreambule)
		generatePreambule(out);
	kDebug(30522) << "preambule : " << hasPreambule << endl;

	/* Body */
	kDebug(30522) << endl << "body : " << _corps.count() << endl;

	if(hasPreambule)
	{
		out << "\\begin{document}" << endl;
		Config::instance()->indent();
	}
	QString dir = "";
	if( !Config::instance()->getPicturesDir().isEmpty() &&
			Config::instance()->getPicturesDir() != NULL &&
			FileHeader::instance()->hasGraphics())
	{
		out << endl << "\\graphicspath{{" << Config::instance()->getPicturesDir() << "}}" << endl;
	}

	if(_corps.getFirst() != 0)
		_corps.getFirst()->generate(out);

	/* Just for test */
	/*if(_tables.getFirst() != 0)
		_tables.getFirst()->generate(out);
	if(_formulas.getFirst() != 0)
		_formulas.getFirst()->generate(out);*/
	if(hasPreambule)
		out << "\\end{document}" << endl;
	Config::instance()->desindent();
	if(Config::instance()->getIndentation() != 0)
			kError(30522) << "Error : indent != 0 at the end ! " << endl;
}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void Document::generatePreambule(QTextStream &out)
{
	Element* header;
	Element* footer;

	/* For each header */
	if(FileHeader::instance()->hasHeader())
	{
		kDebug(30522) << "header : " << _headers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\headrulewidth}{0pt}" << endl;
		for(header = _headers.first(); header != 0; header = _headers.next())
		{
			generateTypeHeader(out, header);
		}
	}

	/* For each footer */
	if(FileHeader::instance()->hasFooter())
	{
		kDebug(30522) << "footer : " << _footers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\footrulewidth}{0pt}" << endl;
		for(footer = _footers.first(); footer != 0; footer = _footers.next())
		{
			generateTypeFooter(out, footer);
		}
	}
	/* Specify what header/footer style to use */
	if(FileHeader::instance()->hasHeader() || FileHeader::instance()->hasFooter())
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
	kDebug(30522) << "generate header" << endl;
	if((FileHeader::instance()->getHeadType() == FileHeader::TH_ALL ||
		FileHeader::instance()->getHeadType() == FileHeader::TH_FIRST) && header->getInfo() == SI_EVEN)
	{
		out << "\\fancyhead[L]{}" << endl;
		out << "\\fancyhead[C]{";
		header->generate(out);
		out << "}" << endl;
		out << "\\fancyhead[R]{}" << endl;
	}

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
	if(FileHeader::instance()->getFootType() == FileHeader::TH_ALL &&
			footer->getInfo() == SI_EVEN)
	{
		out << "\\fancyfoot[L]{}" << endl;
		out << "\\fancyfoot[C]{";
		footer->generate(out);
		out << "}" << endl;
		out << "\\fancyfoot[R]{}" << endl;
	}
	else if(FileHeader::instance()->getFootType() == FileHeader::TH_EVODD)
	{
		switch(footer->getInfo())
		{
			case SI_NONE:
			case SI_FIRST:
				break;
			case SI_ODD:
				out << "\\fancyfoot[CO]{";
				footer->generate(out);
				out << "}";
				break;
			case SI_EVEN:
				out << "\\fancyfoot[CE]{";
				footer->generate(out);
				out << "}";
				break;
		}
	}
	else if(FileHeader::instance()->getFootType() == FileHeader::TH_FIRST &&
			footer->getInfo() == SI_FIRST)
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
		kDebug(30522) << elt->getGrpMgr() << endl;
		if(elt->getGrpMgr() == anchor)
			return elt;
		elt = _tables.next();
	}
	kDebug(30522) << "Not in table, search in formula list." << endl;
	elt = _formulas.first();
	while(elt != 0)
	{
		if(elt->getName() == anchor)
			return elt;
		elt = _formulas.next();
	}
	kDebug(30522) << "Not in table and formula list, search in pictures." << endl;
	elt = _pixmaps.first();
	while(elt != 0)
	{
		if(elt->getName() == anchor)
			return elt;
		elt = _pixmaps.next();
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

Key* Document::searchKey(QString keyName)
{
	Key* key = _keys.first();
	while(key != 0)
	{
		kDebug(30522) << "key " << key->getFilename() << endl;
		if(key->getFilename() == keyName)
			return key;
		key = _keys.next();
	}
	return NULL;

}

QString Document::extractData(QString key)
{
	QString data = searchKey(key)->getName();
	kDebug(30522) << "Opening " << data << endl;
	if(!getStorage()->isOpen())
	{
		if(!getStorage()->open(data))
		{
			kError(30522) << "Unable to open " << data << endl;
			return QString("");
		}
	}

	/* Temp file with the default name in the default temp dir */
	KTempFile temp;
	//temp.setAutoDelete(true);
	QFile* tempFile = temp.file();

	const Q_LONG buflen = 4096;
	char buffer[ buflen ];
	Q_LONG readBytes = getStorage()->read( buffer, buflen );

	while ( readBytes > 0 )
	{
		tempFile->write( buffer, readBytes );
		readBytes = getStorage()->read( buffer, buflen );
	}
	temp.close();
	if(!getStorage()->close())
	{
		kError(30522) << "Unable to close " << data << endl;
		return QString("");
	}
	kDebug(30522) << "temp filename : " << temp.name() << endl;
	return temp.name();
}
