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

#include <kdebug.h>
#include <stdlib.h>

#include "document.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
/*Document::Document(QString fileIn, QString fileOut): XmlParser(fileIn), _file(fileOut)
{
	kdDebug() << fileOut.latin1() << endl;
	_filename = fileOut;
	setLatexType(LT_PSTRICKS);
}*/

/*******************************************/
/* Constructor                             */
/*******************************************/
/*Document::Document(QByteArray in, QString fileOut): XmlParser(in), _file(fileOut)
{
	kdDebug() << fileOut.latin1() << endl;
	_filename = fileOut;
	setLatexType(LT_PSTRICKS);
}*/

/*******************************************/
/* Constructor                             */
/*******************************************/
Document::Document(const KoStore& in, QString fileOut, QString config):
						XmlParser(in), _file(fileOut)
{
	//_in = new KoStore(in);
	kdDebug() << fileOut.latin1() << endl;
	_filename = fileOut;
	setFileHeader(_fileHeader);
	//setRoot(this);
	_isEmbeded = false;
	analyse_config(config);
}

void Document::analyse_config(QString config)
{
	kdDebug() << config << endl;
	if(config.contains("EMBEDED") > 0)
		_isEmbeded = true;
	if(config.contains("PSTRICKS") > 0)
		setLatexType(LT_PSTRICKS);
	else if(config.contains("KWORD") > 0)
		useKwordStyle();
	if(config.contains("UNICODE") > 0)
		useUnicodeEnc();
	else if(config.contains("LATIN1") > 0)
		useLatin1Enc();
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
void Document::analyse()
{
	QDomNode balise;

	kdDebug() << "BEGIN THE ANALYSE OF A DOCUMENT" << endl;
	balise = init();
	//balise = getChild(balise, "kontour");
	//analysePreambule(balise);
	analyseDocument(balise);
	kdDebug() << "END OF ANALYSE OF A DOCUMENT" << endl;
}

void Document::analysePreambule(const QDomNode balise)
{
	/* Get parameters */
	//balise.getAttr(balise, "mime");
	_version= getAttr(balise, "version").toInt();
	//balise.getAttr(balise, "editor");
}

/*******************************************/
/* AnalyseDocument                         */
/*******************************************/
void Document::analyseDocument(const QDomNode balise)
{
	_header.analyse(getChild(balise, "head"));
	
	for(int index = 1; index < getNbChild(balise); index++)
	{
		Page* page = 0;
		kdDebug() << "balise : " << getChildName(balise, index) << endl;
		if(getChildName(balise, index).compare("page") == 0)
		{
			page = new Page();
			page->analyse(getChild(balise, index));
			_pages.append(page);
		}
	}
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Document::generate() //QTextStream &out)
{
	if(_file.open(IO_WriteOnly))
	{
		kdDebug() << "GENERATION" << endl;
		_out.setDevice(&_file);

		/* Generation */
		if(!isEmbeded())
		{
			_header.generate(_out);

			if(_pages.first()->getFormat() == TF_CUSTOM)
			{
				_out << "\\setlength{\\paperwidth}{"  << _pages.first()->getWidth()  << "mm}" << endl;
				_out << "\\setlength{\\paperheight}{" << _pages.first()->getHeight() << "mm}" << endl;
			}

			_out << "\\setlength{\\textwidth}{"  << (_pages.first()->getWidth() - _pages.first()->getLeftMargin() - _pages.first()->getRightMargin())  << "mm}" << endl;
			_out << "\\setlength{\\textheight}{" << (_pages.first()->getHeight()) << "mm}" << endl;

			/* Margin */
			_out << "\\setlength{\\topmargin}{" << _pages.first()->getTopMargin() << "mm}" << endl;
			_out << "\\addtolength{\\leftmargin}{" << _pages.first()->getLeftMargin() << "mm}" << endl;
			_out << endl;
			_out << "\\begin{document}" << endl;
		}
		for(Page* page = _pages.first(); page != 0; page = _pages.next())
		{
			if(page->getOrientation())
				_out << "\\begin{sidewaysfigure}" << endl;
			if(getLatexType() == LT_PSTRICKS)
			{
				_out << "\\begin{pspicture}(";
				if(_header.isGridShow())
					generateGrid(_out);
				if(!isEmbeded())
				{
					_out << page->getWidth() << "mm,";
					_out << page->getHeight() << "mm)" << endl;
				}
				else
				{
					_out << getMaxX() << "pt,";
					_out << getMaxY() << "pt)" << endl;
				}
				page->generatePSTRICKS(_out);
				_out << "\\end{pspicture}" << endl;
			}
			if(page->getOrientation())
				_out << "\\end{sidewaysfigure}" << endl;
		}
		if(!isEmbeded())
			_out << "\\end{document}" << endl;
		_out << getDocument();
	}
	else
		kdDebug() << "Can't use the file ..." << endl;
	_file.close();
}

/*******************************************/
/* GenerateGrid                            */
/*******************************************/
void Document::generateGrid(QTextStream &out)
{
	out << "\\psgrid[xunit=" << _header.getDx() << "pt,yunit=" << _header.getDy() << "pt,";
	out << "gridcolor=" << _header.getGridColorName() << ",";
	out << "gridwidth=1pt,subgriddiv=1]" << endl;

}

