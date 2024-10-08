/*
** A program to convert the XML rendered by Words into LATEX.
**
** SPDX-FileCopyrightText: 2000 Robert JACOLIN
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

#include "table.h"

#include <QBitArray>
#include <QTextStream>

#include "LatexDebug.h"
#include "cell.h"
#include "column.h"
#include "row.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Table::Table()
{
    _maxCol = 0;
    _maxRow = 0;
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Table::~Table()
= default;

void Table::setMaxColumn(int col)
{
    if (_maxCol < col) _maxCol = col;
}

void Table::setMaxRow(int row)
{
    if (_maxRow < row) _maxRow = row;
}

void Table::analyze(const QDomNode node)
{
    debugLatex << "New table";
    if (getAttr(node, "columnnumber") == "1")
        setColumnNumber();
    if (getAttr(node, "borders") == "1")
        setBorders();
    if (getAttr(node, "hide") == "1")
        setHide();
    if (getAttr(node, "hidezero") == "1")
        setHideZero();
    if (getAttr(node, "firstletterupper") == "1")
        setFirstletterupper();
    if (getAttr(node, "grid") == "1")
        setGrid();
    if (getAttr(node, "printgrid") == "1")
        setPrintGrid();
    if (getAttr(node, "printCommentIndicator") == "1")
        setPrintCommentIndicator();
    if (getAttr(node, "printFormulaIndicator") == "1")
        setPrintFormulaIndicator();
    if (getAttr(node, "showFormula") == "1")
        setShowFormula();
    if (getAttr(node, "showFormulaIndicator") == "1")
        setShowFormulaIndicator();
    if (getAttr(node, "lcmode") == "1")
        setLCMode();
    setName(getAttr(node, "name"));

    analyzePaper(getChild(node, "paper"));

    int max = getNbChild(node);
    for (int index = 0; index < max; index++) {
        QString name = getChildName(node, index);
        if (name == "cell") {
            debugLatex << "----- cell -----";
            Cell* cell = new Cell();
            cell->analyze(getChild(node, index));
            _cells.append(cell);
            setMaxColumn(cell->getCol());
            setMaxRow(cell->getRow());
        } else if (name == "column") {
            debugLatex << "----- column -----";
            Column* column = new Column();
            column->analyze(getChild(node, index));
            _columns.append(column);
        } else if (name == "row") {
            debugLatex << "----- row -----";
            Row* row = new Row();
            row->analyze(getChild(node, index));
            _rows.append(row);
        } else
            debugLatex << "name :" << name;
    }
}

void Table::analyzePaper(const QDomNode node)
{
    setFormat(getAttr(node, "format"));
    setOrientation(getAttr(node, "orientation"));

    /* borders */
    QDomNode border = getChild(node, "borders");
    Q_UNUSED(border);
    setBorderRight(getAttr(node, "right").toLong());
    setBorderLeft(getAttr(node, "left").toLong());
    setBorderBottom(getAttr(node, "bottom").toLong());
    setBorderTop(getAttr(node, "top").toLong());
}

Cell* Table::searchCell(int col, int row)
{
    debugLatex << "search in list of" << _cells.count() << " cells";
    foreach(Cell* cell, _cells) {
        debugLatex << "cell:" << cell->getRow() << "-" << cell->getCol();
        if (cell->getCol() == col && cell->getRow() == row)
            return cell;
    }
    return nullptr;
}

Column* Table::searchColumn(int col)
{
    foreach(Column* column, _columns) {
        if (column->getCol() == col)
            return column;
    }
    return nullptr;
}

Row* Table::searchRow(int rowNumber)
{

    foreach(Row* row, _rows) {
        if (row->getRow() == rowNumber)
            return row;
    }
    return nullptr;
}

/*******************************************/
/* generate                                */
/*******************************************/
void Table::generate(QTextStream& out)
{
    debugLatex << "GENERATION OF A TABLE" << getMaxRow() << " -" << getMaxColumn()
    << Qt::endl;
    out << Qt::endl << "%% " << getName() << Qt::endl;
    if (getOrientation() == "Portrait") {
        out << "\\begin{sidewaystable}" << Qt::endl << Qt::endl;
        indent();
        writeIndent(out);
    }

    out << "\\begin{tabular}";
    generateTableHeader(out);
    out << Qt::endl;
    indent();
    int rowNumber = 1;
    while (rowNumber <= getMaxRow()) {
        generateTopLineBorder(out, rowNumber);
        Row* row = searchRow(rowNumber);
        if (row != nullptr)
            row->generate(out);

        for (int col = 1; col <= getMaxColumn(); col++) {
            writeIndent(out);
            generateCell(out, rowNumber, col);

            if (col < getMaxColumn())
                out << " & " << Qt::endl;
        }
        out << "\\\\" << Qt::endl;
        rowNumber++;
    }
    generateBottomLineBorder(out, rowNumber - 1);
    unindent();
    writeIndent(out);
    out << "\\end{tabular}" << Qt::endl << Qt::endl;
    unindent();

    if (getOrientation() == "Portrait") {
        out << "\\end{sidewaystable}" << Qt::endl;
        unindent();
    }
    /*Element* elt = 0;
    debugLatex <<"GENERATION OF A TABLE" << count();
    out << Qt::endl << "\\begin{tabular}";
    generateTableHeader(out);
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
    debugLatex << "END OF GENERATION OF A TABLE";
}

/*******************************************/
/* generateTopLineBorder                   */
/*******************************************/
void Table::generateTopLineBorder(QTextStream& out, int row)
{

    Cell* cell = nullptr;
    QBitArray border(getMaxColumn());
    bool fullLine = true;
    for (int index = 1; index <= getMaxColumn(); index++) {
        /* Search the cell in the list */
        debugLatex << "search" << row << "," << index;
        cell = searchCell(index, row);

        if (cell == nullptr) {
            cell = new Cell(row, index);
            _cells.append(cell);
        }

        /* If the element has a border display it here */
        border[ index - 1 ] = cell->hasTopBorder();
        if (! cell->hasTopBorder())
            fullLine = false;
    }

    if (fullLine) {
        /* All column have a top border */
        writeIndent(out);
        out << "\\hline" << Qt::endl;
    } else {
        int index = 0;
        while (index < getMaxColumn()) {
            if (border[index]) {
                int begin = index;
                int end;
                index++;
                while (index < getMaxColumn() && border[index]) {
                    index++;
                }
                end = index - 1;
                out << "\\cline{" << begin << "-" << end << "} " << Qt::endl;
            }
            index++;
        }
    }

    /*Row * row;
    row = searchRow(row);
    if(row != nullptr)
     row->generate(out);*/
}

/*******************************************/
/* generateBottomLineBorder                */
/*******************************************/
void Table::generateBottomLineBorder(QTextStream& out, int row)
{
    Cell* cell = nullptr;
    QBitArray border(getMaxColumn());
    bool fullLine = true;

    for (int index = 1; index <= getMaxColumn(); index++) {
        /* Search the cell in the list */
        cell = searchCell(index, row);

        if (cell == nullptr) {
            cell = new Cell(row, index);
            _cells.append(cell);
        }

        /* If the element has a border display it here */
        border[ index - 1 ] = cell->hasBottomBorder();
        if (! cell->hasBottomBorder())
            fullLine = false;
    }

    if (fullLine) {
        /* All column have a bottom border */
        writeIndent(out);
        out << "\\hline" << Qt::endl;
    } else {
        int index = 0;
        while (index < getMaxColumn()) {
            if (border[index]) {
                int begin = index;
                int end;
                ++index;
                while (index < getMaxColumn() && border[index]) {
                    ++index;
                }
                end = index - 1;
                out << "\\cline{" << begin << "-" << end << "} " << Qt::endl;
            }
            ++index;
        }
    }
}

/*******************************************/
/* generateCell                            */
/*******************************************/
void Table::generateCell(QTextStream& out, int row, int col)
{
    debugLatex << "GENERATE CELL :" << row << "," << col;

    /* Search the cell in the list */
    Cell *cell = searchCell(col, row);
    if (cell != nullptr) {
        debugLatex << "generate cell with text:" << cell->getText();
        cell->generate(out, this);
    }

    debugLatex << "END OF A CELL";
}

/*******************************************/
/* generateTableHeader                     */
/*******************************************/
void Table::generateTableHeader(QTextStream& out)
{
    Column* column = nullptr;

    out << "{";

    for (int col = 1; col <= getMaxColumn(); col++) {
        column = searchColumn(col);
        if (column != nullptr)
            column->generate(out);
        else {
            out << "m{20pt}";
        }
    }
    out << "}";

}

