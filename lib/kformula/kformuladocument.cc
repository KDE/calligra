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

#include <qptrlist.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <koDocument.h>

#include "contextstyle.h"
#include "creationstrategy.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "sequenceelement.h"
#include "symboltable.h"
#include "symbolaction.h"

KFORMULA_NAMESPACE_BEGIN


static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.3";

/**
 * The creation strategy that we are going to use.
 */
static OrdinaryCreationStrategy creationStrategy;


struct Document::Document_Impl {

    Document_Impl( KConfig* c )
            : leftBracketChar( LeftRoundBracket ), rightBracketChar( RightRoundBracket ),
              formula(0), firstTime( true ),
              actionsCreated( false ), config( c )
    {
        SequenceElement::setCreationStrategy( &creationStrategy );
        formulae.setAutoDelete( false );
        //kdDebug( DEBUGID ) << "Document::Document_Impl " << formulae.count() << endl;
    }


    ~Document_Impl()
    {
        clear();
        if (ownHistory) {
            delete history;
        }
    }

    void clear() {
        // Destroy remaining formulae. We do it backward because
        // the formulae remove themselves from this document upon
        // destruction.
        int count = formulae.count();
        for ( int i=count-1; i>=0; --i ) {
            delete formulae.at( i );
        }
    }

    // We know our actions, maybe a client is interessted...

    KAction* addNegThinSpaceAction;
    KAction* addThinSpaceAction;
    KAction* addMediumSpaceAction;
    KAction* addThickSpaceAction;
    KAction* addQuadSpaceAction;
    KAction* addBracketAction;
    KAction* addSBracketAction;
    KAction* addCBracketAction;
    KAction* addAbsAction;
    KAction* addFractionAction;
    KAction* addRootAction;
    KAction* addSumAction;
    KAction* addProductAction;
    KAction* addIntegralAction;
    KAction* addMatrixAction;
    KAction* addOneByTwoMatrixAction;
    KAction* addUpperLeftAction;
    KAction* addLowerLeftAction;
    KAction* addUpperRightAction;
    KAction* addLowerRightAction;
    KAction* addGenericUpperAction;
    KAction* addGenericLowerAction;
    KAction* addOverlineAction;
    KAction* addUnderlineAction;
    KAction* addMultilineAction;
    KAction* removeEnclosingAction;
    KAction* makeGreekAction;
    KAction* insertSymbolAction;

    KAction* appendColumnAction;
    KAction* insertColumnAction;
    KAction* removeColumnAction;
    KAction* appendRowAction;
    KAction* insertRowAction;
    KAction* removeRowAction;

    KToggleAction* syntaxHighlightingAction;
    KToggleAction* formatBoldAction;
    KToggleAction* formatItalicAction;

    KSelectAction* leftBracket;
    KSelectAction* rightBracket;
    SymbolAction* symbolNamesAction;

    KSelectAction* fontFamily;

    SymbolType leftBracketChar;
    SymbolType rightBracketChar;
    QString selectedName;

    /**
     * The active formula.
     */
    Container* formula;

    /**
     * Our undo stack. We don't own it. The stack belongs to
     * our parent and might contain not formula related commands
     * as well.
     */
    KoCommandHistory* history;

    /**
     * Tells whether we are responsible to remove our history.
     */
    bool ownHistory;

    /**
     * The documents context style. This is the place where all
     * the user configurable informations are stored.
     */
    ContextStyle contextStyle;

    /**
     * All formulae that belong to this document.
     */
    QPtrList<Container> formulae;

    /**
     * Lazy initialization. Read the symbol table only if
     * we create a container.
     */
    bool firstTime;

    bool actionsCreated;

    /**
     * The applications config object. We need to remember this so that
     * we don't depend on global variables. (We don't know who uses us.)
     */
    KConfig* config;
};


double Document::getXResolution() const { return impl->contextStyle.zoomedResolutionX(); }
double Document::getYResolution() const { return impl->contextStyle.zoomedResolutionY(); }

KoCommandHistory* Document::getHistory() const { return impl->history; }
const SymbolTable& Document::getSymbolTable() const { return impl->contextStyle.symbolTable(); }

KAction* Document::getAddNegThinSpaceAction()  { return impl->addNegThinSpaceAction; }
KAction* Document::getAddThinSpaceAction()     { return impl->addThinSpaceAction; }
KAction* Document::getAddMediumSpaceAction()   { return impl->addMediumSpaceAction; }
KAction* Document::getAddThickSpaceAction()    { return impl->addThickSpaceAction; }
KAction* Document::getAddQuadSpaceAction()     { return impl->addQuadSpaceAction; }
KAction* Document::getAddBracketAction()       { return impl->addBracketAction; }
KAction* Document::getAddSBracketAction()      { return impl->addSBracketAction;}
KAction* Document::getAddCBracketAction()      { return impl->addCBracketAction;}
KAction* Document::getAddAbsAction()           { return impl->addAbsAction;}
KAction* Document::getAddFractionAction()      { return impl->addFractionAction; }
KAction* Document::getAddRootAction()          { return impl->addRootAction; }
KAction* Document::getAddSumAction()           { return impl->addSumAction; }
KAction* Document::getAddProductAction()       { return impl->addProductAction; }
KAction* Document::getAddIntegralAction()      { return impl->addIntegralAction; }
KAction* Document::getAddMatrixAction()        { return impl->addMatrixAction; }
KAction* Document::getAddOneByTwoMatrixAction(){ return impl->addOneByTwoMatrixAction; }
KAction* Document::getAddUpperLeftAction()     { return impl->addUpperLeftAction; }
KAction* Document::getAddLowerLeftAction()     { return impl->addLowerLeftAction; }
KAction* Document::getAddUpperRightAction()    { return impl->addUpperRightAction; }
KAction* Document::getAddLowerRightAction()    { return impl->addLowerRightAction; }
KAction* Document::getAddGenericUpperAction()  { return impl->addGenericUpperAction; }
KAction* Document::getAddGenericLowerAction()  { return impl->addGenericLowerAction; }
KAction* Document::getAddOverlineAction()      { return impl->addOverlineAction; }
KAction* Document::getAddUnderlineAction()     { return impl->addUnderlineAction; }
KAction* Document::getAddMultilineAction()     { return impl->addMultilineAction; }
KAction* Document::getRemoveEnclosingAction()  { return impl->removeEnclosingAction; }
KAction* Document::getMakeGreekAction()        { return impl->makeGreekAction; }
KAction* Document::getInsertSymbolAction()     { return impl->insertSymbolAction; }

KAction* Document::getAppendColumnAction()     { return impl->appendColumnAction; }
KAction* Document::getInsertColumnAction()     { return impl->insertColumnAction; }
KAction* Document::getRemoveColumnAction()     { return impl->removeColumnAction; }
KAction* Document::getAppendRowAction()        { return impl->appendRowAction; }
KAction* Document::getInsertRowAction()        { return impl->insertRowAction; }
KAction* Document::getRemoveRowAction()        { return impl->removeRowAction; }

KSelectAction* Document::getLeftBracketAction()  { return impl->leftBracket; }
KSelectAction* Document::getRightBracketAction() { return impl->rightBracket; }
KSelectAction* Document::getSymbolNamesAction()  { return impl->symbolNamesAction; }
KToggleAction* Document::getSyntaxHighlightingAction() { return impl->syntaxHighlightingAction; }
KToggleAction* Document::getFormatBoldAction()   { return impl->formatBoldAction; }
KToggleAction* Document::getFormatItalicAction() { return impl->formatItalicAction; }

KSelectAction* Document::getFontFamilyAction() { return impl->fontFamily; }

Container* Document::formula() const { return impl->formula; }

SymbolType Document::leftBracketChar() const  { return impl->leftBracketChar; }
SymbolType Document::rightBracketChar() const { return impl->rightBracketChar; }


Document::Document( KConfig* config,
                    KActionCollection* collection,
                    KoCommandHistory* his )
{
    impl = new Document_Impl( config );

    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "kformula/pics/");
    createActions(collection);
    //kdDebug( DEBUGID ) << "Document::Document " << collection << endl;
    impl->contextStyle.readConfig( impl->config );
    impl->syntaxHighlightingAction->setChecked( impl->contextStyle.syntaxHighlighting() );

    if (his == 0) {
        impl->history = new KoCommandHistory(collection);
        impl->ownHistory = true;
    }
    else {
        impl->history = his;
        impl->ownHistory = false;
    }
}


Document::Document( KConfig* config, KoCommandHistory* his )
{
    impl = new Document_Impl( config );
    impl->contextStyle.readConfig( impl->config );
    if (his == 0) {
        impl->history = new KoCommandHistory;
        impl->ownHistory = true;
    }
    else {
        impl->history = his;
        impl->ownHistory = false;
    }
}


Document::~Document()
{
    delete impl;
}


Container* Document::createFormula( int pos )
{
    Container* formula = new Container( this, pos );
    formula->initialize();
    return formula;
}


QPtrListIterator<Container> Document::formulas()
{
    return QPtrListIterator<Container>( impl->formulae );
}


int Document::formulaPos( Container* formula )
{
    return impl->formulae.find( formula );
}


Container* Document::formulaAt( uint pos )
{
    return impl->formulae.at( pos );
}


int Document::formulaCount()
{
    return impl->formulae.count();
}


bool Document::loadXML( QDomDocument doc )
{
    //impl->clear();
    QDomElement root = doc.documentElement();

    // backward compatiblity
    if ( root.tagName() == "FORMULA" ) {
        Container* formula = newFormula( 0 );
        return formula->load( root );
    }

    QDomNode node = root.firstChild();
    if ( node.isElement() ) {
        QDomElement element = node.toElement();
        if ( element.tagName() == "FORMULASETTINGS" ) {
            if ( !loadDocumentPart( element ) ) {
                return false;
            }
        }
        node = node.nextSibling();
    }
    uint number = 0;
    while ( !node.isNull() ) {
        if ( node.isElement() ) {
            QDomElement element = node.toElement();
            Container* formula = newFormula( number );
            if ( !formula->load( element ) ) {
                return false;
            }
            number += 1;
        }
        node = node.nextSibling();
    }
    return impl->formulae.count() > 0;
}

bool Document::loadDocumentPart( QDomElement /*node*/ )
{
    return true;
}

QDomDocument Document::saveXML()
{
    QDomDocument doc = createDomDocument();
    QDomElement root = doc.documentElement();
    root.appendChild( saveDocumentPart( doc ) );
    uint count = impl->formulae.count();
    for ( uint i=0; i<count; ++i ) {
        impl->formulae.at( i )->save( root );
    }
    return doc;
}


QDomElement Document::saveDocumentPart( QDomDocument doc )
{
    QDomElement settings = doc.createElement( "FORMULASETTINGS" );
    return settings;
}


QDomDocument Document::createDomDocument()
{
    return KoDocument::createDomDocument( "kformula", "KFORMULA", CURRENT_DTD_VERSION );
}

void Document::registerFormula( Container* f, int pos )
{
    lazyInit();
    if ( ( pos > -1 ) && ( static_cast<uint>( pos ) < impl->formulae.count() ) ) {
        impl->formulae.insert( pos, f );
        //emit sigInsertFormula( f, pos );
    }
    else {
        impl->formulae.append( f );
        //emit sigInsertFormula( f, impl->formulae.count()-1 );
    }
}

void Document::activate(Container* f)
{
    impl->formula = f;
}

void Document::formulaDies( Container* formula )
{
    if ( formula == impl->formula ) {
        impl->formula = 0;
    }
    impl->formulae.remove( formula );
}


Container* Document::newFormula( uint number )
{
    if ( number < impl->formulae.count() ) {
        return impl->formulae.at( number );
    }
    return createFormula();
}


void Document::lazyInit()
{
    if ( impl->firstTime ) {
        kdDebug( DEBUGID ) << "Document::lazyInit" << endl;
        impl->firstTime = false;
        impl->contextStyle.init();

        initSymbolNamesAction();
    }
}

void Document::initSymbolNamesAction()
{
    if ( impl->actionsCreated ) {
        const SymbolTable& st = impl->contextStyle.symbolTable();

        QStringList names = st.allNames();
        //QStringList i18nNames;
        QValueList<QFont> fonts;
        QMemArray<uchar> chars( names.count() );

        int i = 0;
        for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it, ++i ) {
            QChar ch = st.unicode( *it );
            //i18nNames.push_back( i18n( ( *it ).latin1() ) );

            fonts.append( st.font( ch ) );
            chars[ i ] = st.character( ch );
            //kdDebug( DEBUGID ) << "Document::initSymbolNamesAction: " << *it << " " << st.font( ch ).family() << " " << QString( ch ) << endl;
        }
        impl->symbolNamesAction->setSymbols( names, fonts, chars );
        impl->selectedName = names[0];
    }
}

ContextStyle& Document::getContextStyle( bool edit )
{
    impl->contextStyle.setEdit( edit );
    return impl->contextStyle;
}

void Document::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    impl->contextStyle.setZoomAndResolution( zoom, dpiX, dpiY );
}

void Document::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
    if ( updateViews ) {
        recalc();
    }
}

void Document::setZoomAndResolution( int zoom, double zoomX, double zoomY, bool updateViews, bool forPrint )
{
    if ( getContextStyle( !forPrint ).setZoomAndResolution( zoom, zoomX, zoomY, updateViews, forPrint ) && updateViews ) {
        recalc();
    }
}


void Document::setEnabled( bool enabled )
{
    //kdDebug( DEBUGID ) << "Document::setEnabled " << enabled << endl;
    getAddNegThinSpaceAction()->setEnabled( enabled );
    getMakeGreekAction()->setEnabled( enabled );
    getAddGenericUpperAction()->setEnabled( enabled );
    getAddGenericLowerAction()->setEnabled( enabled );
    getAddOverlineAction()->setEnabled( enabled );
    getAddUnderlineAction()->setEnabled( enabled );
    getRemoveEnclosingAction()->setEnabled( enabled );
    getInsertSymbolAction()->setEnabled( enabled );
    getAddThinSpaceAction()->setEnabled( enabled );
    getAddMediumSpaceAction()->setEnabled( enabled );
    getAddThickSpaceAction()->setEnabled( enabled );
    getAddQuadSpaceAction()->setEnabled( enabled );
    getAddBracketAction()->setEnabled( enabled );
    getAddSBracketAction()->setEnabled( enabled );
    getAddCBracketAction()->setEnabled( enabled );
    getAddAbsAction()->setEnabled(enabled);
    getAddFractionAction()->setEnabled( enabled );
    getAddRootAction()->setEnabled( enabled );
    getAddSumAction()->setEnabled( enabled );
    getAddProductAction()->setEnabled( enabled );
    getAddIntegralAction()->setEnabled( enabled );
    getAddMatrixAction()->setEnabled( enabled );
    getAddOneByTwoMatrixAction()->setEnabled( enabled );
    getAddUpperLeftAction()->setEnabled( enabled );
    getAddLowerLeftAction()->setEnabled( enabled );
    getAddUpperRightAction()->setEnabled( enabled );
    getAddLowerRightAction()->setEnabled( enabled );
    getAppendColumnAction()->setEnabled( enabled );
    getInsertColumnAction()->setEnabled( enabled );
    getRemoveColumnAction()->setEnabled( enabled );
    getAppendRowAction()->setEnabled( enabled );
    getInsertRowAction()->setEnabled( enabled );
    getRemoveRowAction()->setEnabled( enabled );

    if ( enabled ) {
        getAddGenericUpperAction()->setShortcut( KShortcut( CTRL + Key_U ) );
        getAddGenericLowerAction()->setShortcut( KShortcut( CTRL + Key_L ) );
        getRemoveEnclosingAction()->setShortcut( KShortcut( CTRL + Key_R ) );
        getMakeGreekAction()->setShortcut( KShortcut( CTRL + Key_G ) );
        getInsertSymbolAction()->setShortcut( KShortcut( CTRL + Key_I ) );
    }
    else {
        getAddGenericUpperAction()->setShortcut( KShortcut() );
        getAddGenericLowerAction()->setShortcut( KShortcut() );
        getRemoveEnclosingAction()->setShortcut( KShortcut() );
        getMakeGreekAction()->setShortcut( KShortcut() );
        getInsertSymbolAction()->setShortcut( KShortcut() );
    }
}


void Document::createActions(KActionCollection* collection)
{
    impl->addNegThinSpaceAction = new KAction( i18n( "Add Negative Thin Space" ),
                                    0,
                                    this, SLOT( addNegThinSpace() ),
                                    collection, "formula_addnegthinspace") ;
    impl->addThinSpaceAction = new KAction( i18n( "Add Thin Space" ),
                                    0,
                                    this, SLOT( addThinSpace() ),
                                    collection, "formula_addthinspace") ;
    impl->addMediumSpaceAction = new KAction( i18n( "Add Medium Space" ),
                                    0,
                                    this, SLOT( addMediumSpace() ),
                                    collection, "formula_addmediumspace" );
    impl->addThickSpaceAction = new KAction( i18n( "Add Thick Space" ),
                                    0,
                                    this, SLOT( addThickSpace() ),
                                    collection, "formula_addthickspace" );
    impl->addQuadSpaceAction = new KAction( i18n( "Add Quad Space" ),
                                    0,
                                    this, SLOT( addQuadSpace() ),
                                    collection, "formula_addquadspace" );

    impl->addIntegralAction = new KAction(i18n("Add Integral"),
                                    "int",
                                    0,
                                    this, SLOT(addIntegral()),
                                    collection, "formula_addintegral");
    impl->addSumAction      = new KAction(i18n("Add Sum"),
                                    "sum",
                                    0,
                                    this, SLOT(addSum()),
                                    collection, "formula_addsum");
    impl->addProductAction  = new KAction(i18n("Add Product"),
                                    "prod",
                                    0,
                                    this, SLOT(addProduct()),
                                    collection, "formula_addproduct");
    impl->addRootAction     = new KAction(i18n("Add Root"),
                                    "sqrt",
                                    0,
                                    this, SLOT(addRoot()),
                                    collection, "formula_addroot");
    impl->addFractionAction = new KAction(i18n("Add Fraction"),
                                    "frac",
                                    0,
                                    this, SLOT(addFraction()),
                                    collection, "formula_addfrac");
    impl->addBracketAction  = new KAction(i18n("Add Bracket"),
                                    "paren",
                                    0,
                                    this, SLOT(addDefaultBracket()),
                                    collection,"formula_addbra");
    impl->addSBracketAction = new KAction(i18n("Add Square Bracket"),
                                    "brackets",
                                    0,
                                    this, SLOT(addSquareBracket()),
                                    collection,"formula_addsqrbra");
    impl->addCBracketAction = new KAction(i18n("Add Curly Bracket"),
                                    "math_brace",
                                    0,
                                    this, SLOT(addCurlyBracket()),
                                    collection,"formula_addcurbra");
    impl->addAbsAction      = new KAction(i18n("Add Abs"),
                                    "abs",
                                    0,
                                    this, SLOT(addLineBracket()),
                                    collection,"formula_addabsbra");

    impl->addMatrixAction   = new KAction(i18n("Add Matrix..."),
                                    "matrix",
                                    0,
                                    this, SLOT(addMatrix()),
                                    collection, "formula_addmatrix");

    impl->addOneByTwoMatrixAction   = new KAction(i18n("Add 1x2 Matrix"),
                                    "onetwomatrix",
                                    0,
                                    this, SLOT(addOneByTwoMatrix()),
                                    collection, "formula_add_one_by_two_matrix");


    impl->addUpperLeftAction  = new KAction(i18n("Add Upper Left Index"),
                                      "lsup",
                                      0,
                                      this, SLOT(addUpperLeftIndex()),
                                      collection, "formula_addupperleft");
    impl->addLowerLeftAction  = new KAction(i18n("Add Lower Left Index"),
                                      "lsub",
                                      0,
                                      this, SLOT(addLowerLeftIndex()),
                                      collection, "formula_addlowerleft");
    impl->addUpperRightAction = new KAction(i18n("Add Upper Right Index"),
                                      "rsup",
                                      0,
                                      this, SLOT(addUpperRightIndex()),
                                      collection, "formula_addupperright");
    impl->addLowerRightAction = new KAction(i18n("Add Lower Right Index"),
                                      "rsub",
                                      0,
                                      this, SLOT(addLowerRightIndex()),
                                      collection, "formula_addlowerright");

    impl->addGenericUpperAction = new KAction(i18n("Add Upper Index"),
                                              /*CTRL + Key_U*/0,
                                      this, SLOT(addGenericUpperIndex()),
                                      collection, "formula_addupperindex");
    impl->addGenericLowerAction = new KAction(i18n("Add Lower Index"),
                                      0,
                                      this, SLOT(addGenericLowerIndex()),
                                      collection, "formula_addlowerindex");

    impl->addOverlineAction = new KAction(i18n("Add Overline"),
                                          "over",
                                          0,
                                          this, SLOT(addOverline()),
                                          collection, "formula_addoverline");
    impl->addUnderlineAction = new KAction(i18n("Add Underline"),
                                           "under",
                                           0,
                                           this, SLOT(addUnderline()),
                                           collection, "formula_addunderline");

    impl->addMultilineAction = new KAction(i18n("Add Multiline"),
                                           "multiline",
                                           0,
                                           this, SLOT(addMultiline()),
                                           collection, "formula_addmultiline");

    impl->removeEnclosingAction = new KAction(i18n("Remove Enclosing Element"),
                                        0,
                                        this, SLOT(removeEnclosing()),
                                        collection, "formula_removeenclosing");

    impl->makeGreekAction = new KAction(i18n("Convert to Greek"),
                                  0,
                                  this, SLOT(makeGreek()),
                                  collection, "formula_makegreek");

    impl->appendColumnAction = new KAction( i18n( "Append Column" ),
                                            "inscol",
                                            0,
                                            this, SLOT( appendColumn() ),
                                            collection, "formula_appendcolumn" );
    impl->insertColumnAction = new KAction( i18n( "Insert Column" ),
                                            "inscol",
                                            0,
                                            this, SLOT( insertColumn() ),
                                            collection, "formula_insertcolumn" );
    impl->removeColumnAction = new KAction( i18n( "Remove Column" ),
                                            "remcol",
                                            0,
                                            this, SLOT( removeColumn() ),
                                            collection, "formula_removecolumn" );
    impl->appendRowAction = new KAction( i18n( "Append Row" ),
                                         "insrow",
                                         0,
                                         this, SLOT( appendRow() ),
                                         collection, "formula_appendrow" );
    impl->insertRowAction = new KAction( i18n( "Insert Row" ),
                                         "insrow",
                                         0,
                                         this, SLOT( insertRow() ),
                                         collection, "formula_insertrow" );
    impl->removeRowAction = new KAction( i18n( "Remove Row" ),
                                         "remrow",
                                         0,
                                         this, SLOT( removeRow() ),
                                         collection, "formula_removerow" );

    impl->syntaxHighlightingAction = new KToggleAction(i18n("Syntax Highlighting"),
                                                 0,
                                                 this, SLOT(toggleSyntaxHighlighting()),
                                                 collection, "formula_syntaxhighlighting");

    impl->formatBoldAction = new KToggleAction( i18n( "&Bold" ), "text_bold",
                                                0, //CTRL + Key_B,
                                                this, SLOT( textBold() ),
                                                collection, "formula_format_bold" );
    impl->formatItalicAction = new KToggleAction( i18n( "&Italic" ), "text_italic",
                                                  0, //CTRL + Key_I,
                                                  this, SLOT( textItalic() ),
                                                  collection, "formula_format_italic" );
    impl->formatBoldAction->setEnabled( false );
    impl->formatItalicAction->setEnabled( false );

    QStringList delimiter;
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("{"));
    delimiter.append(QString("<"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("|"));
    delimiter.append(QString(" "));
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString("}"));
    delimiter.append(QString(">"));
    impl->leftBracket = new KSelectAction(i18n("Left Delimiter"),
                                    0, this, SLOT(delimiterLeft()),
                                    collection, "formula_typeleft");
    impl->leftBracket->setItems(delimiter);
    //leftBracket->setCurrentItem(0);

    delimiter.clear();
    delimiter.append(QString(")"));
    delimiter.append(QString("]"));
    delimiter.append(QString("}"));
    delimiter.append(QString(">"));
    delimiter.append(QString("/"));
    delimiter.append(QString("\\"));
    delimiter.append(QString("|"));
    delimiter.append(QString(" "));
    delimiter.append(QString("("));
    delimiter.append(QString("["));
    delimiter.append(QString("{"));
    delimiter.append(QString("<"));
    impl->rightBracket = new KSelectAction(i18n("Right Delimiter"),
                                     0, this, SLOT(delimiterRight()),
                                     collection, "formula_typeright");
    impl->rightBracket->setItems(delimiter);
    //rightBracket->setCurrentItem(0);

    impl->insertSymbolAction = new KAction(i18n("Insert Symbol"),
                                           "key_enter",
                                           /*CTRL + Key_I*/0,
                                           this, SLOT(insertSymbol()),
                                           collection, "formula_insertsymbol");
    impl->symbolNamesAction = new SymbolAction(i18n("Symbol Names"),
                                               0, this, SLOT(symbolNames()),
                                               collection, "formula_symbolnames");

    QStringList ff;
    ff.append( i18n( "Normal" ) );
    ff.append( i18n( "Script" ) );
    ff.append( i18n( "Fraktur" ) );
    ff.append( i18n( "Double Struck" ) );
    impl->fontFamily = new KSelectAction(i18n("Font Family"),
                                         0, this, SLOT(fontFamily()),
                                         collection, "formula_fontfamily");
    impl->fontFamily->setItems( ff );
    impl->fontFamily->setEnabled( false );

    impl->actionsCreated = true;
}


void Document::paste()
{
    if (hasFormula()) {
        formula()->paste();
    }
}

void Document::copy()
{
    if (hasFormula()) {
        formula()->copy();
    }
}

void Document::cut()
{
    if (hasFormula()) {
        formula()->cut();
    }
}

void Document::addNegThinSpace()
{
    if (hasFormula()) {
        SpaceRequest r( NEGTHIN );
        formula()->performRequest( &r );
    }
}
void Document::addThinSpace()
{
    if (hasFormula()) {
        SpaceRequest r( THIN );
        formula()->performRequest( &r );
    }
}
void Document::addMediumSpace()
{
    if (hasFormula()) {
        SpaceRequest r( MEDIUM );
        formula()->performRequest( &r );
    }
}
void Document::addThickSpace()
{
    if (hasFormula()) {
        SpaceRequest r( THICK );
        formula()->performRequest( &r );
    }
}
void Document::addQuadSpace()
{
    if (hasFormula()) {
        SpaceRequest r( QUAD );
        formula()->performRequest( &r );
    }
}

void Document::addDefaultBracket()
{
    if (hasFormula()) {
        BracketRequest r( impl->leftBracketChar, impl->rightBracketChar );
        formula()->performRequest( &r );
    }
}

void Document::addBracket( SymbolType left, SymbolType right )
{
    if (hasFormula()) {
        BracketRequest r( left, right );
        formula()->performRequest( &r );
    }
}

void Document::addParenthesis()
{
    if (hasFormula()) {
        BracketRequest r( LeftRoundBracket, RightRoundBracket );
        formula()->performRequest( &r );
    }
}

void Document::addSquareBracket()
{
    if (hasFormula()) {
        BracketRequest r( LeftSquareBracket, RightSquareBracket );
        formula()->performRequest( &r );
    }
}

void Document::addCurlyBracket()
{
    if (hasFormula()) {
        BracketRequest r( LeftCurlyBracket, RightCurlyBracket );
        formula()->performRequest( &r );
    }
}

void Document::addLineBracket()
{
    if (hasFormula()) {
        BracketRequest r( LeftLineBracket, RightLineBracket );
        formula()->performRequest( &r );
    }
}

void Document::addFraction()
{
    if (hasFormula()) {
        Request r( req_addFraction );
        formula()->performRequest( &r );
    }
}

void Document::addRoot()
{
    if (hasFormula()) {
        Request r( req_addRoot );
        formula()->performRequest( &r );
    }
}

void Document::addIntegral()
{
    if (hasFormula()) {
        SymbolRequest r( Integral );
        formula()->performRequest( &r );
    }
}

void Document::addProduct()
{
    if (hasFormula()) {
        SymbolRequest r( Product );
        formula()->performRequest( &r );
    }
}

void Document::addSum()
{
    if (hasFormula()) {
        SymbolRequest r( Sum );
        formula()->performRequest( &r );
    }
}

void Document::addMatrix( uint rows, uint columns )
{
    if (hasFormula()) {
        MatrixRequest r( rows, columns );
        formula()->performRequest( &r );
    }
}

void Document::addOneByTwoMatrix()
{
    if (hasFormula()) {
        Request r( req_addOneByTwoMatrix );
        formula()->performRequest( &r );
    }
}

void Document::addNameSequence()
{
    if (hasFormula()) {
        Request r( req_addNameSequence );
        formula()->performRequest( &r );
    }
}

void Document::addLowerLeftIndex()
{
    if (hasFormula()) {
        IndexRequest r( lowerLeftPos );
        formula()->performRequest( &r );
    }
}

void Document::addUpperLeftIndex()
{
    if (hasFormula()) {
        IndexRequest r( upperLeftPos );
        formula()->performRequest( &r );
    }
}

void Document::addLowerRightIndex()
{
    if (hasFormula()) {
        IndexRequest r( lowerRightPos );
        formula()->performRequest( &r );
    }
}

void Document::addUpperRightIndex()
{
    if (hasFormula()) {
        IndexRequest r( upperRightPos );
        formula()->performRequest( &r );
    }
}

void Document::addGenericLowerIndex()
{
    if (hasFormula()) {
        IndexRequest r( lowerMiddlePos );
        formula()->performRequest( &r );
    }
}

void Document::addGenericUpperIndex()
{
    if (hasFormula()) {
        IndexRequest r( upperMiddlePos );
        formula()->performRequest( &r );
    }
}

void Document::addOverline()
{
    if (hasFormula()) {
        Request r( req_addOverline );
        formula()->performRequest( &r );
    }
}

void Document::addUnderline()
{
    if (hasFormula()) {
        Request r( req_addUnderline );
        formula()->performRequest( &r );
    }
}

void Document::addMultiline()
{
    if (hasFormula()) {
        Request r( req_addMultiline );
        formula()->performRequest( &r );
    }
}

void Document::removeEnclosing()
{
    if (hasFormula()) {
        DirectedRemove r( req_removeEnclosing, beforeCursor );
        formula()->performRequest( &r );
    }
}

void Document::makeGreek()
{
    if (hasFormula()) {
        Request r( req_makeGreek );
        formula()->performRequest( &r );
    }
}

void Document::insertSymbol()
{
    if ( hasFormula() && impl->contextStyle.symbolTable().contains( impl->selectedName ) ) {
        QChar ch = impl->contextStyle.symbolTable().unicode( impl->selectedName );
        if ( ch != QChar::null ) {
            TextCharRequest r( ch, true );
            formula()->performRequest( &r );
        }
        else {
            TextRequest r( impl->selectedName );
            formula()->performRequest( &r );
        }
    }
}

void Document::insertSymbol( QString name )
{
    if ( hasFormula() && impl->contextStyle.symbolTable().contains( name ) ) {
        QChar ch = impl->contextStyle.symbolTable().unicode( name );
        if ( ch != QChar::null ) {
            TextCharRequest r( ch, true );
            formula()->performRequest( &r );
            return;
        }
    }
    TextRequest r( name );
    formula()->performRequest( &r );
}

void Document::appendColumn()
{
    if ( hasFormula() ) {
        Request r( req_appendColumn );
        formula()->performRequest( &r );
    }
}

void Document::insertColumn()
{
    if ( hasFormula() ) {
        Request r( req_insertColumn );
        formula()->performRequest( &r );
    }
}

void Document::removeColumn()
{
    if ( hasFormula() ) {
        Request r( req_removeColumn );
        formula()->performRequest( &r );
    }
}

void Document::appendRow()
{
    if ( hasFormula() ) {
        Request r( req_appendRow );
        formula()->performRequest( &r );
    }
}

void Document::insertRow()
{
    if ( hasFormula() ) {
        Request r( req_insertRow );
        formula()->performRequest( &r );
    }
}

void Document::removeRow()
{
    if ( hasFormula() ) {
        Request r( req_removeRow );
        formula()->performRequest( &r );
    }
}

void Document::toggleSyntaxHighlighting()
{
    //kdDebug( DEBUGID ) << "Document::toggleSyntaxHighlighting" << endl;
    impl->contextStyle.setSyntaxHighlighting( impl->syntaxHighlightingAction->isChecked() );
    // Only to notify all views. We don't expect to get new values.
    // Here is a really bad bug.
    recalc();
}

void Document::textBold()
{
    if ( hasFormula() ) {
        CharStyleRequest r( req_formatBold,
                            getFormatBoldAction()->isChecked(),
                            getFormatItalicAction()->isChecked() );
        formula()->performRequest( &r );
    }
}

void Document::textItalic()
{
    if ( hasFormula() ) {
        CharStyleRequest r( req_formatItalic,
                            getFormatBoldAction()->isChecked(),
                            getFormatItalicAction()->isChecked() );
        formula()->performRequest( &r );
    }
}

void Document::delimiterLeft()
{
    QString left = impl->leftBracket->currentText();
    switch ( left.at(0).latin1() ) {
    case '[':
    case ']':
    case '{':
    case '}':
    case '<':
    case '>':
    case '(':
    case ')':
    case '/':
    case '\\':
        impl->leftBracketChar = static_cast<SymbolType>( left.at(0).latin1() );
        break;
    case '|':
        impl->leftBracketChar = LeftLineBracket;
        break;
    case ' ':
        impl->leftBracketChar = EmptyBracket;
        break;
    }
}

void Document::delimiterRight()
{
    QString right = impl->rightBracket->currentText();
    switch ( right.at(0).latin1() ) {
    case '[':
    case ']':
    case '{':
    case '}':
    case '<':
    case '>':
    case '(':
    case ')':
    case '/':
    case '\\':
        impl->rightBracketChar = static_cast<SymbolType>( right.at(0).latin1() );
        break;
    case '|':
        impl->rightBracketChar = RightLineBracket;
        break;
    case ' ':
        impl->rightBracketChar = EmptyBracket;
        break;
    }
}

void Document::symbolNames()
{
    impl->selectedName = impl->symbolNamesAction->currentText();
}


void Document::fontFamily()
{
    if ( hasFormula() ) {
        int i = impl->fontFamily->currentItem();
        CharFamily cf = anyFamily;
        switch( i ) {
        case 0: cf = normalFamily; break;
        case 1: cf = scriptFamily; break;
        case 2: cf = frakturFamily; break;
        case 3: cf = doubleStruckFamily; break;
        }
        CharFamilyRequest r( cf );
        formula()->performRequest( &r );
    }
}


void Document::undo()
{
    getHistory()->undo();
}

void Document::redo()
{
    getHistory()->redo();
}

bool Document::hasFormula()
{
    return ( formula() != 0 ) && ( formula()->activeCursor() != 0 );
}

void Document::recalc()
{
    //kdDebug( DEBUGID ) << "Document::recalc " << impl->formulae.count() << endl;
    for ( Container* f = impl->formulae.first(); f != 0; f=impl->formulae.next() ) {
        f->recalc();
    }
}

void Document::updateConfig()
{
    lazyInit();
    impl->syntaxHighlightingAction->setChecked( impl->contextStyle.syntaxHighlighting() );
    initSymbolNamesAction();
    recalc();
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformuladocument.moc"
