/*
** A program to convert the XML rendered by KSpread into LATEX.
**
** SPDX-FileCopyrightText: 2002, 2003 Robert JACOLIN
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

#include "cell.h"

#include "LatexDebug.h"
#include "table.h"
#include "column.h"

#include <QTextStream>

/*******************************************/
/* Constructor                             */
/*******************************************/
Cell::Cell(): Format()
{
    setCol(0);
    setRow(0);
    setText("");
    setTextDataType("none");
    setResultDataType("none");
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Cell::~Cell()
= default;

void Cell::analyze(const QDomNode node)
{
    _row = getAttr(node, "row").toLong();
    _col = getAttr(node, "column").toLong();
    debugLatex << getRow() << "-" << getCol();
    Format::analyze(getChild(node, "format"));
    analyzeText(node);
}

void Cell::analyzeText(const QDomNode node)
{
    setTextDataType(getAttr(getChild(node, "text"), "dataType"));
    setText(getAttr(getChild(node, "text"), "outStr"));
    debugLatex << "text(" << getTextDataType() << "):" << getText();
}

/*******************************************/
/* generate                                */
/*******************************************/
void Cell::generate(QTextStream& out, Table* table)
{
    /*if(getMulticol() > 0)
     out << "\\multicol{" << getMulticol() << "}{";
    else*/
    if (getMultirow() > 0)
        out << "\\multirow{" << getMultirow() << "}{";
    debugLatex << "Generate cell...";

    out << "\\multicolumn{1}{";
    Format::generate(out, table->searchColumn(_col));
    out << "}{" << Qt::endl;

    if (getTextDataType() == "Str" || getTextDataType() == "Num") {
        generateTextFormat(out, getText());
        //out << getText();
    }

    out << "}" << Qt::endl;

    /*if(getColSpan() > 0)
     out << "}" << Qt::endl;
    else*/ if (getMultirow() > 0)
        out << "}" << Qt::endl;

    /*Element* elt = 0;
    debugLatex <<"GENERATION OF A TABLE" << count();
    out << Qt::endl << "\\begin{tabular}";
    generateCellHeader(out);
    out << Qt::endl;
    indent();

    int row= 0;
    while(row <= getMaxRow())
    {
    generateTopLineBorder(out, row);
    for(int col= 0; col <= getMaxCol(); col++)
    {
     writeIndent(out);
    */
    /* Search the cell in the list */
    /* elt = searchCell(row, col);

     out << "\\multicolumn{1}{";
     if(elt->hasLeftBorder())
      out << "|";
     out << "m{" << getCellSize(col) << "pt}";

     if(elt->hasRightBorder())
      out << "|";
     out << "}{" << Qt::endl;

     generateCell(out, row, col);
     out << "}" << Qt::endl;
     if(col < getMaxCol())
      out << "&" << Qt::endl;
    }
    out << "\\\\" << Qt::endl;
    writeIndent(out);
    row = row + 1;
    }
    generateBottomLineBorder(out, row - 1);
    out << "\\end{tabular}" << Qt::endl << Qt::endl;
    unindent();*/
    debugLatex << "END OF GENERATION OF A CELL";
}

