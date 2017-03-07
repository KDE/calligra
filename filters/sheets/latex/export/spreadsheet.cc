/*
** A program to convert the XML rendered by Words into LATEX.
**
** Copyright (C) 2000, 2001, 2002, 2003 Robert JACOLIN
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

#include "spreadsheet.h"

#include <stdlib.h>  /* for atoi function    */

#include "LatexDebug.h"
#include "fileheader.h"
//#include "paper.h"

#include <QTextStream>

/*******************************************/
/* Constructor                             */
/*******************************************/
Spreadsheet::Spreadsheet()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Spreadsheet::~Spreadsheet()
{
    debugLatex << "Corps Destructor";
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Spreadsheet::analyze(const QDomNode node)
{
    debugLatex << getChildName(node, 0);
    analyze_attr(node);
    //_paper.analyze(getChild(node, "paper"));
    _map.analyze(getChild(node, "map"));
    //_locale.analyze(getChild(node, "locale"));
    //_areaname.analyze(getChild(node, "areaname"));

    debugLatex << "SPREADSHEET END";
}

/*******************************************/
/* AnalyzePixmaps                          */
/*******************************************/
void Spreadsheet::analyze_attr(const QDomNode /*node*/)
{

}

/*******************************************/
/* Generate                                */
/*******************************************/
void Spreadsheet::generate(QTextStream &out, bool hasPreamble)
{
    debugLatex << "DOC. GENERATION.";

    if (!Config::instance()->isEmbeded())
        generatePreamble(out);
    debugLatex << "preamble :" << hasPreamble;

    /* Body */
    if (hasPreamble) {
        out << "\\begin{document}" << endl;
        indent();
    }

    _map.generate(out);

    if (hasPreamble)
        out << "\\end{document}" << endl;
    unindent();
    if (getIndentation() != 0)
        errorLatex << "Error : indent != 0 at the end ! " << endl;
}

/*******************************************/
/* GeneratePreamble                        */
/*******************************************/
void Spreadsheet::generatePreamble(QTextStream &out)
{
    FileHeader::instance()->generate(out);
    /* For each header */
    //if(getFileHeader()->hasHeader())
    //{
    // debugLatex <<"header :" << _headers.count();

    /* default : no rule */
    // out << "\\renewcommand{\\headrulewidth}{0pt}" << endl;
    // for(header = _headers.first(); header != 0; header = _headers.next())
    // {
    //  generateTypeHeader(out, header);
    // }
    //}

    /* For each footer */
    /*if(getFileHeader()->hasFooter())
    {
     debugLatex <<"footer :" << _footers.count();
    */
    /* default : no rule */
    /* out << "\\renewcommand{\\footrulewidth}{0pt}" << endl;
     for(footer = _footers.first(); footer != 0; footer = _footers.next())
     {
      generateTypeFooter(out, footer);
     }
    }*/
    /* Specify what header/footer style to use */
    /*if(getFileHeader()->hasHeader() || getFileHeader()->hasFooter())
     out << "\\pagestyle{fancy}" << endl;
    else
    {
     out << "\\pagestyle{empty}" << endl;
    }*/
}

/*******************************************/
/* GenerateTypeHeader                      */
/*******************************************/
void Spreadsheet::generateTypeHeader(QTextStream &/*out*/)
{
    /*debugLatex <<"generate header";
    if((_fileHeader->getHeadType() == TH_ALL ||
     _fileHeader->getHeadType() == TH_FIRST) && header->getInfo() == SI_EVEN)
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
    }*/
}

/*******************************************/
/* GenerateTypeFooter                      */
/*******************************************/
void Spreadsheet::generateTypeFooter(QTextStream &/*out*/)
{
    /*if(_fileHeader->getFootType() == TH_ALL && footer->getInfo() == SI_EVEN)
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
       out << "}";
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
    }*/
}

