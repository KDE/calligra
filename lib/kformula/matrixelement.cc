/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qmemarray.h>
#include <qpainter.h>
#include <qptrlist.h>

#include <kdebug.h>
#include <klocale.h>

#include "MatrixDialog.h"
#include "formulaelement.h"
#include "formulacursor.h"
#include "kformulacontainer.h"
#include "kformulacommand.h"
#include "matrixelement.h"
#include "sequenceelement.h"

KFORMULA_NAMESPACE_BEGIN


class MatrixSequenceElement : public SequenceElement {
    typedef SequenceElement inherited;
public:

    MatrixSequenceElement( BasicElement* parent = 0 ) : SequenceElement( parent ) {}

    /**
     * This is called by the container to get a command depending on
     * the current cursor position (this is how the element gets choosen)
     * and the request.
     *
     * @returns the command that performs the requested action with
     * the containers active cursor.
     */
    virtual KCommand* buildCommand( Container*, Request* );
};


class KFCRemoveRow : public Command {
public:
    KFCRemoveRow( const QString& name, Container* document, MatrixElement* m, uint r, uint c );
    ~KFCRemoveRow();

    virtual void execute();
    virtual void unexecute();

protected:
    MatrixElement* matrix;
    uint rowPos;
    uint colPos;

    QPtrList<MatrixSequenceElement>* row;
};


class KFCInsertRow : public KFCRemoveRow {
public:
    KFCInsertRow( const QString& name, Container* document, MatrixElement* m, uint r, uint c );

    virtual void execute()   { KFCRemoveRow::unexecute(); }
    virtual void unexecute() { KFCRemoveRow::execute(); }
};


class KFCRemoveColumn : public Command {
public:
    KFCRemoveColumn( const QString& name, Container* document, MatrixElement* m, uint r, uint c );
    ~KFCRemoveColumn();

    virtual void execute();
    virtual void unexecute();

protected:
    MatrixElement* matrix;
    uint rowPos;
    uint colPos;

    QPtrList<MatrixSequenceElement>* column;
};


class KFCInsertColumn : public KFCRemoveColumn {
public:
    KFCInsertColumn( const QString& name, Container* document, MatrixElement* m, uint r, uint c );

    virtual void execute()   { KFCRemoveColumn::unexecute(); }
    virtual void unexecute() { KFCRemoveColumn::execute(); }
};


KCommand* MatrixSequenceElement::buildCommand( Container* container, Request* request )
{
    switch ( *request ) {
    case req_appendColumn:
    case req_appendRow:
    case req_insertColumn:
    case req_removeColumn:
    case req_insertRow:
    case req_removeRow: {
        MatrixElement* matrix = static_cast<MatrixElement*>( getParent() );
        FormulaCursor* cursor = container->activeCursor();
        for ( uint row = 0; row < matrix->getRows(); row++ ) {
            for ( uint col = 0; col < matrix->getColumns(); col++ ) {
                if ( matrix->getElement( row, col ) == cursor->getElement() ) {
                    switch ( *request ) {
                    case req_appendColumn:
                        return new KFCInsertColumn( i18n( "Append Column" ), container, matrix, row, matrix->getColumns() );
                    case req_appendRow:
                        return new KFCInsertRow( i18n( "Append Row" ), container, matrix, matrix->getRows(), col );
                    case req_insertColumn:
                        return new KFCInsertColumn( i18n( "Insert Column" ), container, matrix, row, col );
                    case req_removeColumn:
                        if ( matrix->getColumns() > 1 ) {
                            return new KFCRemoveColumn( i18n( "Remove Column" ), container, matrix, row, col );
                        }
                        break;
                    case req_insertRow:
                        return new KFCInsertRow( i18n( "Insert Row" ), container, matrix, row, col );
                    case req_removeRow:
                        if ( matrix->getRows() > 1 ) {
                            return new KFCRemoveRow( i18n( "Remove Row" ), container, matrix, row, col );
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        kdWarning( DEBUGID ) << "MatrixSequenceElement::buildCommand: Sequence not found." << endl;
        break;
    }
    default:
        break;
    }
    return inherited::buildCommand( container, request );
}


KFCRemoveRow::KFCRemoveRow( const QString& name, Container* document, MatrixElement* m, uint r, uint c )
    : Command( name, document ), matrix( m ), rowPos( r ), colPos( c ), row( 0 )
{
}

KFCRemoveRow::~KFCRemoveRow()
{
    delete row;
}

void KFCRemoveRow::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    row = matrix->content.at( rowPos );
    FormulaElement* formula = matrix->formula();
    for ( uint i = matrix->getColumns(); i > 0; i-- ) {
        formula->elementRemoval( row->at( i-1 ) );
    }
    matrix->content.take( rowPos );
    formula->changed();
    if ( rowPos < matrix->getRows() ) {
        matrix->getElement( rowPos, colPos )->goInside( cursor );
    }
    else {
        matrix->getElement( rowPos-1, colPos )->goInside( cursor );
    }
    testDirty();
}

void KFCRemoveRow::unexecute()
{
    matrix->content.insert( rowPos, row );
    row = 0;
    FormulaCursor* cursor = getExecuteCursor();
    matrix->getElement( rowPos, colPos )->goInside( cursor );
    matrix->formula()->changed();
    testDirty();
}


KFCInsertRow::KFCInsertRow( const QString& name, Container* document, MatrixElement* m, uint r, uint c )
    : KFCRemoveRow( name, document, m, r, c )
{
    row = new QPtrList< MatrixSequenceElement >;
    row->setAutoDelete( true );
    for ( uint i = 0; i < matrix->getColumns(); i++ ) {
        row->append( new MatrixSequenceElement( matrix ) );
    }
}


KFCRemoveColumn::KFCRemoveColumn( const QString& name, Container* document, MatrixElement* m, uint r, uint c )
    : Command( name, document ), matrix( m ), rowPos( r ), colPos( c )
{
    column = new QPtrList< MatrixSequenceElement >;
    column->setAutoDelete( true );
}

KFCRemoveColumn::~KFCRemoveColumn()
{
    delete column;
}

void KFCRemoveColumn::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    FormulaElement* formula = matrix->formula();
    for ( uint i = 0; i < matrix->getRows(); i++ ) {
        column->append( matrix->getElement( i, colPos ) );
        formula->elementRemoval( column->at( i ) );
        matrix->content.at( i )->take( colPos );
    }
    formula->changed();
    if ( colPos < matrix->getColumns() ) {
        matrix->getElement( rowPos, colPos )->goInside( cursor );
    }
    else {
        matrix->getElement( rowPos, colPos-1 )->goInside( cursor );
    }
    testDirty();
}

void KFCRemoveColumn::unexecute()
{
    for ( uint i = 0; i < matrix->getRows(); i++ ) {
        matrix->content.at( i )->insert( colPos, column->take( 0 ) );
    }
    FormulaCursor* cursor = getExecuteCursor();
    matrix->getElement( rowPos, colPos )->goInside( cursor );
    matrix->formula()->changed();
    testDirty();
}


KFCInsertColumn::KFCInsertColumn( const QString& name, Container* document, MatrixElement* m, uint r, uint c )
    : KFCRemoveColumn( name, document, m, r, c )
{
    for ( uint i = 0; i < matrix->getRows(); i++ ) {
        column->append( new MatrixSequenceElement( matrix ) );
    }
}


MatrixElement::MatrixElement(uint rows, uint columns, BasicElement* parent)
    : BasicElement(parent)
{
    for (uint r = 0; r < rows; r++) {
        QPtrList< MatrixSequenceElement >* list = new QPtrList< MatrixSequenceElement >;
        list->setAutoDelete(true);
        for (uint c = 0; c < columns; c++) {
            list->append(new MatrixSequenceElement(this));
        }
        content.append(list);
    }
    content.setAutoDelete(true);
}

MatrixElement::~MatrixElement()
{
}


BasicElement* MatrixElement::goToPos( FormulaCursor* cursor, bool& handled,
                                      const LuPixelPoint& point, const LuPixelPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPixelPoint myPos(parentOrigin.x() + getX(),
                           parentOrigin.y() + getY());

        uint rows = getRows();
        uint columns = getColumns();

        for (uint r = 0; r < rows; r++) {
            for (uint c = 0; c < columns; c++) {
                BasicElement* element = getElement(r, c);
                e = element->goToPos(cursor, handled, point, myPos);
                if (e != 0) {
                    return e;
                }
            }
        }

        // We are in one of those gaps.
        luPixel dx = point.x() - myPos.x();
        luPixel dy = point.y() - myPos.y();

        uint row = rows;
        for (uint r = 0; r < rows; r++) {
            BasicElement* element = getElement(r, 0);
            if (element->getY() > dy) {
                row = r;
                break;
            }
        }
        if (row == 0) {
            BasicElement* element = getParent();
            element->moveLeft(cursor, this);
            handled = true;
            return element;
        }
        row--;

        uint column = columns;
        for (uint c = 0; c < columns; c++) {
            BasicElement* element = getElement(row, c);
            if (element->getX() > dx) {
                column = c;
                break;
            }
        }
        if (column == 0) {
            BasicElement* element = getParent();
            element->moveLeft(cursor, this);
            handled = true;
            return element;
        }
        column--;

        // Rescan the rows with the actual colums required.
        row = rows;
        for (uint r = 0; r < rows; r++) {
            BasicElement* element = getElement(r, column);
            if (element->getY() > dy) {
                row = r;
                break;
            }
        }
        if (row == 0) {
            BasicElement* element = getParent();
            element->moveLeft(cursor, this);
            handled = true;
            return element;
        }
        row--;

        BasicElement* element = getElement(row, column);
        element->moveLeft(cursor, this);
        handled = true;
        return element;
    }
    return 0;
}


// drawing
//
// Drawing depends on a context which knows the required properties like
// fonts, spaces and such.
// It is essential to calculate elements size with the same context
// before you draw.

/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void MatrixElement::calcSizes(const ContextStyle& style, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle)
{
    QMemArray<luPixel> toMidlines(getRows());
    QMemArray<luPixel> fromMidlines(getRows());
    QMemArray<luPixel> widths(getColumns());

    toMidlines.fill(0);
    fromMidlines.fill(0);
    widths.fill(0);

    uint rows = getRows();
    uint columns = getColumns();

    ContextStyle::TextStyle i_tstyle = style.convertTextStyleFraction(tstyle);
    ContextStyle::IndexStyle i_istyle = style.convertIndexStyleUpper(istyle);

    for (uint r = 0; r < rows; r++) {
        QPtrList< MatrixSequenceElement >* list = content.at(r);
        for (uint c = 0; c < columns; c++) {
            SequenceElement* element = list->at(c);
            element->calcSizes( style, i_tstyle, i_istyle );
            toMidlines[r] = QMAX(toMidlines[r], element->axis( style, i_tstyle ));
            fromMidlines[r] = QMAX(fromMidlines[r],
                                   element->getHeight()-element->axis( style, i_tstyle ));
            widths[c] = QMAX(widths[c], element->getWidth());
        }
    }

    luPixel distX = style.ptToPixelX( style.getThinSpace( tstyle ) );
    luPixel distY = style.ptToPixelY( style.getThinSpace( tstyle ) );

    luPixel yPos = 0;
    for (uint r = 0; r < rows; r++) {
        QPtrList< MatrixSequenceElement >* list = content.at(r);
        luPixel xPos = 0;
        yPos += toMidlines[r];
        for (uint c = 0; c < columns; c++) {
            SequenceElement* element = list->at(c);
            switch (style.getMatrixAlignment()) {
            case ContextStyle::left:
                element->setX(xPos);
                break;
            case ContextStyle::center:
                element->setX(xPos + (widths[c] - element->getWidth())/2);
                break;
            case ContextStyle::right:
                element->setX(xPos + widths[c] - element->getWidth());
                break;
            }
            element->setY(yPos - element->axis( style, i_tstyle ));
            xPos += widths[c] + distX;
        }
        yPos += fromMidlines[r] + distY;
    }

    luPixel width = distX * (columns - 1);
    luPixel height = distY * (rows - 1);

    for (uint r = 0; r < rows; r++) height += toMidlines[r] + fromMidlines[r];
    for (uint c = 0; c < columns; c++) width += widths[c];

    setWidth(width);
    setHeight(height);
    if ((rows == 2) && (columns == 1)) {
        setBaseline( getMainChild()->getHeight() + distY / 2 + style.axisHeight( tstyle ) );
    }
    else {
        setBaseline( height/2 + style.axisHeight( tstyle ) );
    }
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void MatrixElement::draw( QPainter& painter, const LuPixelRect& rect,
                          const ContextStyle& style,
                          ContextStyle::TextStyle tstyle,
                          ContextStyle::IndexStyle istyle,
                          const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( rect ) )
        return;

    uint rows = getRows();
    uint columns = getColumns();

    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < columns; c++) {
            getElement(r, c)->draw(painter, rect, style,
				   style.convertTextStyleFraction(tstyle),
				   style.convertIndexStyleUpper(istyle),
				   myPos);
        }
    }

    // Debug
    //painter.setPen(Qt::red);
    //painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
}


// navigation
//
// The elements are responsible to handle cursor movement themselves.
// To do this they need to know the direction the cursor moves and
// the element it comes from.
//
// The cursor might be in normal or in selection mode.

/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void MatrixElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(getRows()-1, getColumns()-1)->moveLeft(cursor, this);
        }
        else {
            bool linear = cursor->getLinearMovement();
            uint row = 0;
            uint column = 0;
            if (searchElement(from, row, column)) {
                if (column > 0) {
                    getElement(row, column-1)->moveLeft(cursor, this);
                }
                else if (linear && (row > 0)) {
                    getElement(row-1, getColumns()-1)->moveLeft(cursor, this);
                }
                else {
                    getParent()->moveLeft(cursor, this);
                }
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void MatrixElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            bool linear = cursor->getLinearMovement();
            uint row = 0;
            uint column = 0;
            if (searchElement(from, row, column)) {
                if (column < getColumns()-1) {
                    getElement(row, column+1)->moveRight(cursor, this);
                }
                else if (linear && (row < getRows()-1)) {
                    getElement(row+1, 0)->moveRight(cursor, this);
                }
                else {
                    getParent()->moveRight(cursor, this);
                }
            }
            else {
                getParent()->moveRight(cursor, this);
            }
        }
    }
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void MatrixElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            uint row = 0;
            uint column = 0;
            if (searchElement(from, row, column)) {
                if (row > 0) {
                    getElement(row-1, column)->moveRight(cursor, this);
                }
                else {
                    getParent()->moveUp(cursor, this);
                }
            }
            else {
                getParent()->moveUp(cursor, this);
            }
        }
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void MatrixElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            uint row = 0;
            uint column = 0;
            if (searchElement(from, row, column)) {
                if (row < getRows()-1) {
                    getElement(row+1, column)->moveRight(cursor, this);
                }
                else {
                    getParent()->moveDown(cursor, this);
                }
            }
            else {
                getParent()->moveDown(cursor, this);
            }
        }
    }
}

/**
 * Sets the cursor inside this element to its start position.
 * For most elements that is the main child.
 */
void MatrixElement::goInside(FormulaCursor* cursor)
{
    getElement(0, 0)->goInside(cursor);
}


// If there is a main child we must provide the insert/remove semantics.
SequenceElement* MatrixElement::getMainChild()
{
    return content.at(0)->at(0);
}

void MatrixElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    uint rows = getRows();
    uint columns = getColumns();
    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < columns; c++) {
            if (child == getElement(r, c)) {
                cursor->setTo(this, r*columns+c);
            }
        }
    }
}

bool MatrixElement::searchElement(BasicElement* element, uint& row, uint& column)
{
    uint rows = getRows();
    uint columns = getColumns();
    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < columns; c++) {
            if (element == getElement(r, c)) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}


/**
 * Appends our attributes to the dom element.
 */
void MatrixElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    uint rows = getRows();
    uint cols = getColumns();

    element.setAttribute("ROWS", rows);
    element.setAttribute("COLUMNS", cols);

    QDomDocument doc = element.ownerDocument();

    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < cols; c++) {
    	    QDomElement tmp = getElement(r,c)->getElementDom(doc);
            element.appendChild(tmp);
	}
        element.appendChild(doc.createComment("end of row"));
    }
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool MatrixElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    uint rows = 0;
    QString rowStr = element.attribute("ROWS");
    if(!rowStr.isNull()) {
        rows = rowStr.toInt();
    }
    if (rows == 0) {
        kdWarning( DEBUGID ) << "Rows <= 0 in MatrixElement." << endl;
        return false;
    }

    QString columnStr = element.attribute("COLUMNS");
    uint cols = 0;
    if(!columnStr.isNull()) {
        cols = columnStr.toInt();
    }
    if (cols == 0) {
        kdWarning( DEBUGID ) << "Columns <= 0 in MatrixElement." << endl;
        return false;
    }

    content.clear();
    for (uint r = 0; r < rows; r++) {
        QPtrList< MatrixSequenceElement >* list = new QPtrList< MatrixSequenceElement >;
        list->setAutoDelete(true);
        content.append(list);
        for (uint c = 0; c < cols; c++) {
            MatrixSequenceElement* element = new MatrixSequenceElement(this);
            list->append(element);
	}
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool MatrixElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    uint rows = getRows();
    uint cols = getColumns();

    uint r = 0;
    uint c = 0;
    while ( !node.isNull() && r < rows ) {
        if ( node.isElement() ) {
            SequenceElement* element = getElement( r, c );
            QDomElement e = node.toElement();
            if ( !element->buildFromDom( e ) ) {
                return false;
            }
            c++;
            if ( c == cols ) {
                c = 0;
                r++;
            }
        }
        node = node.nextSibling();
    }
    return true;
}

QString MatrixElement::toLatex()
{
    //All the border handling must be implemented here too

    QString matrix;
    uint cols=getColumns();
    uint rows=getRows();

    matrix="\\begin{array}{ ";
    for(uint i=0;i<cols;i++)
	matrix+="c ";

    matrix+="}";

    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < cols; c++) {
            matrix+=getElement(r, c)->toLatex();
	    if( c < cols-1)    matrix+=" & ";
        }
    	if(r < rows-1 ) matrix+=" \\\\ ";
    }

    matrix+="\\end{array}";

    return matrix;
}

QString MatrixElement::formulaString()
{
    QString matrix = "[";
    uint cols=getColumns();
    uint rows=getRows();
    for (uint r = 0; r < rows; r++) {
        matrix += "[";
        for (uint c = 0; c < cols; c++) {
            matrix+=getElement(r, c)->formulaString();
	    if ( c < cols-1 ) matrix+=", ";
        }
        matrix += "]";
    	if ( r < rows-1 ) matrix += ", ";
    }
    matrix += "]";
    return matrix;
}

KFORMULA_NAMESPACE_END
