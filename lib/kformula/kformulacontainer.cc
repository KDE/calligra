/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qapp.h>
#include <qdom.h>
#include <qevent.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qpainter.h>

#include <qstring.h>

#include <kdebug.h>
#include <klocale.h>
#include <kprinter.h>

#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacommand.h"
#include "kformulacompatibility.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulamathmlread.h"
#include "kformulamimesource.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "symboltable.h"
#include "spaceelement.h"
#include "textelement.h"


KFORMULA_NAMESPACE_BEGIN
using namespace std;


struct Container::Container_Impl {

    Container_Impl( Document* doc )
            : dirty( true ), cursorMoved( false ), document( doc )
    {
    }

    ~Container_Impl()
    {
        delete internCursor;
        delete rootElement;
    }

    /**
     * If true we need to recalc the formula.
     */
    bool dirty;

    /**
     * Tells whether a request caused the cursor to move.
     */
    bool cursorMoved;

    /**
     * The element tree's root.
     */
    FormulaElement* rootElement;

    /**
     * The active cursor is the one that triggered the last command.
     */
    FormulaCursor* activeCursor;

    /**
     * The cursor that is used if there is no view.
     */
    FormulaCursor* internCursor;

    /**
     * The document we belong to.
     */
    Document* document;
};


FormulaElement* Container::rootElement() const { return impl->rootElement; }
Document* Container::document() const { return impl->document; }

Container::Container(Document* doc)
{
    impl = new Container_Impl(doc);
    impl->rootElement = new FormulaElement(this);
    impl->activeCursor = impl->internCursor = createCursor();
    connect(this, SIGNAL(commandExecuted()),
            document()->getHistory(), SIGNAL(commandExecuted()));
    recalc();
}

Container::~Container()
{
    document()->formulaDies(this);
    delete impl;
}


FormulaCursor* Container::createCursor()
{
    return new FormulaCursor(rootElement());
}


KCommandHistory* Container::getHistory() const
{
    return document()->getHistory();
}


/**
 * Gets called just before the child is removed from
 * the element tree.
 */
void Container::elementRemoval(BasicElement* child)
{
    emit elementWillVanish(child);
}

/**
 * Gets called whenever something changes and we need to
 * recalc.
 */
void Container::changed()
{
    impl->dirty = true;
}

void Container::cursorHasMoved( FormulaCursor* )
{
    impl->cursorMoved = true;
}

FormulaCursor* Container::activeCursor()
{
    return impl->activeCursor;
}

const FormulaCursor* Container::activeCursor() const
{
    return impl->activeCursor;
}


/**
 * Tells the formula that a view got the focus and might want to
 * edit the formula.
 */
void Container::setActiveCursor(FormulaCursor* cursor)
{
    document()->activate(this);
    if (cursor != 0) {
        impl->activeCursor = cursor;
    }
    else {
        //FormulaCursor::CursorData* data = activeCursor->getCursorData();
        //internCursor->setCursorData(data);
        //delete data;
        *(impl->internCursor) = *(impl->activeCursor);
        impl->activeCursor = impl->internCursor;
    }
}


bool Container::hasValidCursor() const
{
    return (impl->activeCursor != 0) && !impl->activeCursor->isReadOnly();
}

bool Container::hasMightyCursor() const
{
    if ( hasValidCursor() ) {
        const SequenceElement* sequence = activeCursor()->normal();
        if ( sequence != 0 ) {
            return sequence->getTokenType() != NAME;
        }
    }
    return false;
}

void Container::testDirty()
{
    if (impl->dirty) {
        recalc();
    }
}

void Container::recalc()
{
    impl->dirty = false;
    ContextStyle& context = document()->getContextStyle();
    rootElement()->calcSizes( context );

    emit formulaChanged( context.layoutUnitToPixelX( rootElement()->getWidth() ),
                         context.layoutUnitToPixelY( rootElement()->getHeight() ) );
    emit formulaChanged( context.layoutUnitPtToPt( context.pixelXToPt( rootElement()->getWidth() ) ),
                         context.layoutUnitPtToPt( context.pixelYToPt( rootElement()->getHeight() ) ) );
    //emit cursorChanged( activeCursor() );
    emit cursorMoved( activeCursor() );
}

bool Container::isEmpty()
{
    return rootElement()->countChildren() == 0;
}


const SymbolTable& Container::getSymbolTable() const
{
    return document()->getSymbolTable();
}


void Container::draw(QPainter& painter, const QRect& r, const QColorGroup& cg)
{
    painter.fillRect( r, cg.base() );
    draw( painter, r );
}


void Container::draw(QPainter& painter, const QRect& r)
{
    ContextStyle& context = document()->getContextStyle( painter.device()->devType() == QInternal::Printer );
    rootElement()->draw( painter, context.pixelToLayoutUnit( r ), context );
}


void Container::checkCursor()
{
    if ( impl->cursorMoved ) {
        impl->cursorMoved = false;
        emit cursorMoved( activeCursor() );
    }
}

void Container::input( QKeyEvent* event )
{
    if ( !hasValidCursor() )
        return;
    execute( activeCursor()->getElement()->input( this, event ) );
    checkCursor();
}


void Container::performRequest( Request* request )
{
    if ( !hasValidCursor() )
        return;
    execute( activeCursor()->getElement()->buildCommand( this, request ) );
    checkCursor();
}


void Container::paste()
{
    if (!hasValidCursor())
        return;
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeSource* source = clipboard->data();
    if (source->provides("application/x-kformula")) {
        QByteArray data = source->encodedData("application/x-kformula");
        QDomDocument formula;
        formula.setContent(data);
        paste( formula, i18n("Paste") );
    }
}

void Container::paste( QDomDocument document, QString desc )
{
    FormulaCursor* cursor = activeCursor();
    QPtrList<BasicElement> list;
    list.setAutoDelete( true );
    if ( cursor->buildElementsFromDom( document, list ) ) {
        uint count = list.count();
        // You must not execute an add command that adds nothing.
        if (count > 0) {
            KFCReplace* command = new KFCReplace( desc, this );
            for (uint i = 0; i < count; i++) {
                command->addElement(list.take(0));
            }
            execute(command);
        }
    }
}

void Container::copy()
{
    // read-only cursors are fine for copying.
    FormulaCursor* cursor = activeCursor();
    if (cursor != 0) {
        QDomDocument formula = cursor->copy();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setData(new MimeSource(formula));
    }
}

void Container::cut()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = activeCursor();
    if (cursor->isSelection()) {
        copy();
        //remove();
    }
}


void Container::execute(Command* command)
{
    if ( command != 0 ) {
        command->execute();
        if (!command->isSenseless()) {
            getHistory()->addCommand(command, false);
            emit commandExecuted();
        }
        else {
            delete command;
        }
    }
}


QRect Container::boundingRect()
{
    const ContextStyle& context = document()->getContextStyle();
    return QRect( context.layoutUnitToPixelX( rootElement()->getX() ),
                  context.layoutUnitToPixelY( rootElement()->getY() ),
                  context.layoutUnitToPixelX( rootElement()->getWidth() ),
                  context.layoutUnitToPixelY( rootElement()->getHeight() ) );
}

int Container::baseline() const
{
    const ContextStyle& context = document()->getContextStyle();
    return context.layoutUnitToPixelY( rootElement()->getBaseline() );
}

void Container::moveTo( int x, int y )
{
    const ContextStyle& context = document()->getContextStyle();
    rootElement()->setX( context.pixelToLayoutUnitX( x ) );
    rootElement()->setY( context.pixelToLayoutUnitY( y ) );
}


// This doesn't work.
void Container::setFontSize( int /*pointSize*/, bool /*forPrint*/ )
{
    // taken from KWTextFrameSet::zoom
//     double factor = document()->getContextStyle( forPrint ).getYResolution() *
//                     ( forPrint ? 1.0 : 72.0 / QPaintDevice::x11AppDpiY() );

//     double newSize = factor*pointSize;
//     if ( rootElement()->getBaseSize() != newSize ) {
//         rootElement()->setBaseSize( newSize );
//         kdDebug( DEBUGID ) << "Container::save " << newSize << endl;
//         recalc();
//     }
}


QDomDocument Container::domData()
{
    QDomDocument doc("KFORMULA");
    save(doc);
    return doc;
}

void Container::save(QString file)
{
    QFile f(file);
    if(!f.open(IO_Truncate | IO_ReadWrite)) {
        kdDebug( DEBUGID ) << "Error opening file " << file.latin1() << endl;
        return;
    }
    QCString data=domData().toCString();
    //cerr << (const char *)data << endl;

    QTextStream stream(&f);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    domData().save(stream,4);
    f.close();
}

/**
 * Saves the data into the document.
 */
void Container::save(QDomNode doc)
{
    QDomDocument ownerDoc = doc.ownerDocument();
    doc.appendChild(rootElement()->getElementDom(ownerDoc));
}

void Container::load(QString file)
{
    QFile f(file);
    if (!f.open(IO_ReadOnly)) {
        kdDebug( DEBUGID ) << "Error opening file " << file.latin1() << endl;
        return;
    }
    QTextStream stream(&f);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    QString content = stream.read();
    //kdDebug( DEBUGID ) << content << endl;
    QDomDocument doc;
    if (!doc.setContent(content)) {
        f.close();
        return;
    }
    if (load(doc)) {
        getHistory()->clear();
    }
    f.close();
}

void Container::loadMathMl(QString file)
{
    QFile f(file);
    if (!f.open(IO_ReadOnly)) {
        kdDebug( DEBUGID ) << "Error opening file " << file.latin1() << endl;
        return;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        f.close();
        return;
    }
    MathMl2KFormula filter(&doc);
    //cerr << "Filtering" << endl;

    filter.startConversion();
    if(filter.isDone())
        if (load(filter.getKFormulaDom())) {
    	    getHistory()->clear();
	}
    f.close();
}

/**
 * Loads a formula from the document.
 */
bool Container::load(QDomNode doc)
{
    //cerr << "Loading" << endl;
    QDomElement fe = doc.firstChild().toElement();
    if (!fe.isNull()) {
        FormulaElement* root = new FormulaElement(this);
        if (root->buildFromDom(fe)) {
            delete impl->rootElement;
            impl->rootElement = root;
            emit formulaLoaded(rootElement());

            recalc();
            return true;
        }
        else {
            delete root;
            kdDebug( DEBUGID ) << "Error constructing element tree." << endl;
        }
    }
    return false;
}


void Container::print(KPrinter& printer)
{
    //printer.setFullPage(true);
    QPainter painter;
    if (painter.begin(&printer)) {
        rootElement()->draw(painter, boundingRect(), document()->getContextStyle(true));
    }
}

QString Container::texString()
{
    return rootElement()->toLatex();
}


bool Container::importOldText(QString text)
{
    Compatibility converter;
    QDomDocument doc = converter.buildDOM(text);
    QCString data = doc.toCString();
    cerr << (const char *)data << endl;
    return load(doc);
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulacontainer.moc"
