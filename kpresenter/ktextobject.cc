/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "ktextobject.h"
#include "ktextobject.moc"

#include <qkeycode.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qfontmetrics.h>
#include <qevent.h>
#include <qcursor.h>
#include <qpicture.h>
#include <qscrollbar.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpixmap.h>
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qaccel.h>

#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/******************************************************************/
/* class TxtCursor - Text Cursor				  */
/******************************************************************/

/*======================= constructor ============================*/
TxtCursor::TxtCursor( KTextObject *to )
{
    // init the object
    absPos = 0;
    paragraph = 0;
    inParagraph = 0;
    line = 0;
    inLine = 0;
    objMaxPos = 0;
    txtObj = to;
    _xpos = 0;
}

/*======================= defualt constructor =====================*/
TxtCursor::TxtCursor()
{
    // init the object
    absPos = 0;
    paragraph = 0;
    inParagraph = 0;
    line = 0;
    inLine = 0;
    objMaxPos = 0;
    txtObj = 0;
    _xpos = 0;
}

/*====================== character ===============================*/
QChar TxtCursor::character()
{
    int obj, i, pos0 = 0;

    calcPos(); //maybe remove later

    i = positionInLine();
    obj = linePtr->getInObj( i, &pos0 );

    if ( obj == -1 )
	obj = linePtr->getBeforeObj( i, &pos0 );

    if ( obj == -1 ) //end of line or paragraph
	return 0;

    return linePtr->itemAt( obj )->text().at( i - pos0 );
}

/*==================== cursor char forward =======================*/
void TxtCursor::charForward()
{
    if ( txtObj )
	setMaxPosition( txtObj->textLength() );
    if ( absPos < objMaxPos-1 )
    {
	absPos++;
	calcPos();
    }
}

/*=================== cursor char backward =======================*/
void TxtCursor::charBackward()
{
    if ( absPos > 0 )
    {
	absPos--;
	calcPos();
    }
}

/*====================== cursor line up ==========================*/
void TxtCursor::lineUp()
{
    if ( absPos > 0 && txtObj )
    {
	// if the cursor is in the first line -> do nothing
	if ( line == 0 && paragraph == 0 ) return;

	// else: if the upper line is in the upper paragraph and the line is at least as long as the current one
	// -> jump exactly one line up
	else if ( line == 0 && paragraph != 0 &&
		  txtObj->paragraphAt( paragraph-1 )->lineAt( txtObj->paragraphAt( paragraph-1 )->
							      lines()-1 )->lineLength() >= inLine )
	    absPos -= txtObj->paragraphAt( paragraph-1 )->lineAt( txtObj->paragraphAt( paragraph-1 )->
								  lines()-1 )->lineLength();

	// else: if the upper line is in the same paragraph and the line is at least as long as the current one
	// -> jump exactly one line up
	else if ( line != 0 &&
		  txtObj->paragraphAt( paragraph )->lineAt( line-1 )->lineLength() >= inLine )
	    absPos -= txtObj->paragraphAt( paragraph )->lineAt( line-1 )->lineLength();

	// else: if the upper line is shorter as the current one, jump to the end of the upper line
	// ( here it's irrelevant, if the upper line is in the same paragraph, or not )
	else
	    absPos -= inLine+1;

	// calculate the new position
	calcPos();
    }
}

/*==================== cursor line down ==========================*/
void TxtCursor::lineDown()
{
    if ( txtObj )
	setMaxPosition( txtObj->textLength() );
    if ( absPos < objMaxPos-1 && txtObj )
    {

	// if the cursor is in the last line -> do nothing
	if ( paragraph == txtObj->paragraphs()-1 &&
	     line == txtObj->paragraphAt( paragraph )->lines()-1 )
	    return;

	// else: if the below line is in the below paragraph and the line is at least as long as the current one
	// -> jump exactly one line down
	else if ( paragraph != txtObj->paragraphs()-1 &&
		  line == txtObj->paragraphAt( paragraph )->lines()-1 &&
		  txtObj->paragraphAt( paragraph+1 )->lineAt( 0 )->lineLength() >= inLine )
	    absPos += txtObj->paragraphAt( paragraph )->lineAt( line )->lineLength();

	// else: if the below line is in the same paragraph and the line is at least as long as the current one
	// -> jump exactly one line down
	else if ( line != txtObj->paragraphAt( paragraph )->lines()-1 &&
		  txtObj->paragraphAt( paragraph )->lineAt( line+1 )->lineLength() >= inLine )
	    absPos += txtObj->paragraphAt( paragraph )->lineAt( line )->lineLength();

	// else: if the below line is shorter than the current one, and the below line is one paragraph below
	// -> jump to the end of the below line
	else if ( paragraph != txtObj->paragraphs()-1 &&
		  line == txtObj->paragraphAt( paragraph )->lines()-1 )
	    absPos += txtObj->paragraphAt( paragraph )->lineAt( line )->lineLength() - inLine +
		      txtObj->paragraphAt( paragraph+1 )->lineAt( 0 )->lineLength() - 1;

	// else: if the below line is shorter than the current one, and the below line is in the same paragraph
	// -> jump to the end of the below line
	else if ( line != txtObj->paragraphAt( paragraph )->lines()-1 )
	    absPos += txtObj->paragraphAt( paragraph )->lineAt( line )->lineLength() - inLine +
		      txtObj->paragraphAt( paragraph )->lineAt( line+1 )->lineLength() - 1;

	// calculate the new position
	if ( absPos > objMaxPos-1 ) absPos = objMaxPos-1;
	calcPos();
    }
}

/*==================== cursor word forward =======================*/
void TxtCursor::wordForward()
{
    if ( txtObj )
	setMaxPosition( txtObj->textLength() );
    char c;

    c = character();
    if ( c == 0 )
    {
	charForward();
	c = character();
    }

    while ( isspace( c ) && c != 0 && positionAbs() < objMaxPos - 1 )
    {
	charForward();
	c = character();
    }

    while ( !isspace( c ) && c != 0  && positionAbs() < objMaxPos - 1 )
    {
	charForward();
	c = character();
    }
}

/*=================== cursor word backward =======================*/
void TxtCursor::wordBackward()
{
    char c;

    c = character();

    do
    {
	charBackward();
	c = character();
    }
    while ( isspace( c ) && c != 0 && positionAbs() > 0 );


    do
    {
	charBackward();
	c = character();
    }
    while ( !isspace( c ) && c != 0 && positionAbs() > 0 );

    if ( isspace( c ) || c == 0 )
	charForward();
}

/*==================== cursor line forward =======================*/
void TxtCursor::lineForward()
{
}

/*=================== cursor line backward =======================*/
void TxtCursor::lineBackward()
{
}

/*================== cursor paragraph forward ====================*/
void TxtCursor::paragraphForward()
{
}

/*================= cursor paragraph backward ====================*/
void TxtCursor::paragraphBackward()
{
}

/*======================== cursor pos1 ===========================*/
void TxtCursor::pos1()
{
}

/*========================== cursor end ==========================*/
void TxtCursor::end()
{
}

/*=================== set absolute position ======================*/
void TxtCursor::setPositionAbs( int pos )
{
    absPos = pos;
    calcPos();
}

/*================== set position in a paragraph =================*/
void TxtCursor::setPositionParagraph( int paragraph, int pos )
{
    absPos = 0;
    int i;

    for ( i = 0; i < paragraph; i++ )
	absPos += txtObj->paragraphAt( i )->paragraphLength();

    absPos += pos;

    calcPos();
}

/*================== set position in a paragraph =================*/
void TxtCursor::setPositionParagraph( int paragraph, int line, int pos )
{
    absPos = 0;
    int i;

    for ( i = 0; i < paragraph; i++ )
	absPos += txtObj->paragraphAt( i )->paragraphLength();

    for ( i = 0; i < line; i++ )
	absPos += txtObj->paragraphAt( paragraph )->lineAt( i )->lineLength();

    absPos += pos;

    calcPos();
}

/*================== set position in a line ======================*/
void TxtCursor::setPositionLine( int line, int pos )
{
    absPos = 0;
    int i;

    for ( i = 0; i < line; i++ )
	absPos += txtObj->lineAt( i )->lineLength();

    absPos += pos;

    calcPos();
}

/*============== calculate the position of the cursor ============*/
void TxtCursor::calcPos()
{
    //debug( "calc pos begin" );

    // if the textobject is valid
    if ( txtObj ) {
	int l1 = 0, l2 = 0, i, j;

	for ( i = 0; i < txtObj->paragraphs(); i++ ) {
	    paragraphPtr = txtObj->paragraphAt( i );

	    // if the cursor is in the current paragraph
	    if ( absPos >= l1 && absPos < l1 + paragraphPtr->paragraphLength() ) {
		// calculate first values
		paragraph = i;
		inParagraph = absPos - l1;
		for ( j = 0; j < paragraphPtr->lines(); j++ ) {
		    linePtr = paragraphPtr->lineAt( j );

		    // if the cursor is in the current line
		    if ( inParagraph >= l2 && inParagraph < l2 + linePtr->lineLength() ) {

			// calculate other values
			line = j;
			inLine = inParagraph - l2;
			break;
		    }
		    l2 += linePtr->lineLength();
		}
		break;
	    }
	    l1 += paragraphPtr->paragraphLength();
	}
    }
    //debug( "calc pos end" );
}

/*============ get minimum of two cursors ========================*/
TxtCursor* TxtCursor::minCursor(TxtCursor *c)
{
    if ( c->absPos < absPos ) return c;
    else return this;
}

/*============ get maximum of two cursors ========================*/
TxtCursor* TxtCursor::maxCursor(TxtCursor *c)
{
    if ( c->absPos > absPos ) return c;
    else return this;
}

/*============================ copy ==============================*/
TxtCursor& TxtCursor::copy( TxtCursor& c )
{
    absPos = c.positionAbs();

    paragraph = c.positionParagraph();
    inParagraph = c.positionInParagraph();

    line = c.positionLine();
    inLine = c.positionInLine();

    objMaxPos = c.maxPosition();

    _xpos = c.xpos();
    _ypos = c.ypos();
    __height = c.height();

    return *this;
}

/******************************************************************/
/* class TxtObj - Text Object					  */
/******************************************************************/

/*======================== constructor ===========================*/
TxtObj::TxtObj()
{
    // init the object
    objType = TEXT;
    objColor = Qt::black;
    objFont = QFont( "utopia", 20 );
    objFont.setBold( true );
    objVertAlign = NORMAL;
    objText = "";
    _origSize = 12;
}

/*================= overloaded constructor =======================*/
TxtObj::TxtObj( const QString &text, QFont f, QColor c, VertAlign va, ObjType ot )
{
    // init the object
    objType = ot;
    objColor = c;
    objFont = f;
    objVertAlign = va;
    objText = "";
    objText.insert( 0, text );
    _origSize = 10;
}

/*===================== width of the object ======================*/
int TxtObj::width()
{
    QFontMetrics fm( objFont );

    return fm.width( objText.data() );
}

/*===================== height of the object =====================*/
int TxtObj::height()
{
    QFontMetrics fm( objFont );

    return fm.height();
}

/*===================== ascent of the object =====================*/
int TxtObj::ascent()
{
    QFontMetrics fm( objFont );

    return fm.ascent();
}

/*==================== descent of the object =====================*/
int TxtObj::descent()
{
    QFontMetrics fm( objFont );

    return fm.descent();
}

/*======================= get psoition in object =================*/
int TxtObj::getPos( int pos )
{
    QFontMetrics fm( objFont );
    int w = 0;

    for ( unsigned int i = 0; i < objText.length(); i++ )
    {
	if ( pos >= w && pos <= w + fm.width( objText[ i ] ) )
	    return i;

	w += fm.width( objText[ i ] );
    }

    return -1;
}

/******************************************************************/
/* class TxtLine - Text Line					  */
/******************************************************************/

/*==================== constructor ===============================*/
TxtLine::TxtLine( bool init )
{
    // init the object
    objList.setAutoDelete( true );

    QFont _f = QFont( "utopia", 20 );
    _f.setBold( true );

    if ( init )
	append( " ", _f, Qt::black, TxtObj::NORMAL, TxtObj::SEPARATOR );

}

/*===================== insert a text ============================*/
void TxtLine::insert( int /*pos*/, const QString &/*text*/, QFont /*f*/, QColor /*c*/,
		      TxtObj::VertAlign /*va*/ )
{
}

/*===================== insert a char ============================*/
void TxtLine::insert( int /*pos*/, char /*text*/, QFont /*f*/, QColor /*c*/,
		      TxtObj::VertAlign /*va*/ )
{
}

/*===================== delete character =========================*/
void TxtLine::deleteChar( int pos )
{
    int obj = getInObj( pos );

    if ( obj != -1 )
    {
	int relPos, i, w = 0;
//	 TxtObj *obj1 = new TxtObj();
//	 TxtObj *obj2 = new TxtObj();

	// get the cursor position relative to the object
	for ( i = 0; i < obj; i++ )
	    w += itemAt( i )->textLength();
	relPos = pos - w;

	itemAt( obj )->deleteChar( relPos );
	if ( itemAt( obj )->textLength() == 0 ) objList.remove( obj );
    }
}

/*===================== delete first character ===================*/
void TxtLine::deleteFirstChar( int obj )
{
    if ( itemAt( obj ) )
    {
	itemAt( obj )->deleteChar( 0 );
	if ( itemAt( obj )->textLength() == 0 ) objList.remove( obj );
    }
}

/*===================== backspace char ===========================*/
void TxtLine::backspaceChar( int pos )
{
    int obj = getInObj( pos );

    if ( obj != -1 )
    {
	int relPos, i, w = 0;

	// get the cursor position relative to the object
	for ( i = 0; i < obj; i++ )
	    w += itemAt( i )->textLength();
	relPos = pos - w;

	itemAt( obj )->deleteChar( relPos-1 );
	if ( itemAt( obj )->textLength() == 0 ) objList.remove( obj );
    }
}

/*=================== backspace last char ========================*/
void TxtLine::backspaceLastChar( int obj )
{
    if ( itemAt( obj ) )
    {
	itemAt( obj )->deleteChar( itemAt( obj )->textLength()-1 );
	if ( itemAt( obj )->textLength() == 0 ) objList.remove( obj );
    }
}

/*=================== get length of the line =====================*/
int TxtLine::lineLength()
{
    int l = 0;

    for ( objPtr = objList.first(); objPtr != 0; objPtr = objList.next() )
	l += objPtr->textLength();

    return l;
}

/*==================== width of the line =========================*/
int TxtLine::width()
{
    int w = 0;

    for ( objPtr = objList.first(); objPtr != 0; objPtr = objList.next() )
	w += objPtr->width();

    return w;
}

/*==================== height of the line ========================*/
int TxtLine::height( TxtParagraph *_parag )
{
    int h = 0;

    for ( objPtr = objList.first(); objPtr != 0; objPtr = objList.next() )
	h = max(h,objPtr->height());

    h += _parag->getLineSpacing();
    if ( _parag->find( this ) == 0 ) h += _parag->getDistBefore();
    if ( _parag->find( this ) == static_cast<int>( _parag->lines() - 1 ) ) h += _parag->getDistAfter();

    return h;
}

/*==================== ascent of the line ========================*/
int TxtLine::ascent( TxtParagraph *_parag )
{
    int a = 0;

    for ( objPtr = objList.first(); objPtr != 0; objPtr = objList.next() )
	a = max(a,objPtr->ascent());

    if ( _parag->find( this ) == 0 ) a += _parag->getDistBefore();

    return a;
}

/*==================== descent of the line =======================*/
int TxtLine::descent( TxtParagraph *_parag )
{
    int d = 0;

    for ( objPtr = objList.first(); objPtr != 0; objPtr = objList.next() )
	d = max(d,objPtr->ascent());

    if ( _parag->find( this ) == static_cast<int>( _parag->lines() - 1 ) ) d += _parag->getDistAfter();

    return d;
}

/*======================== operator = ============================*/
TxtLine& TxtLine::operator=( TxtLine *l )
{
    int i;
    clear();

    for ( i = 0; i < l->items(); i++ )
	append( new TxtObj( *l->itemAt( i ) ) );

    return *this;
}

/*======================== operator += ===========================*/
TxtLine& TxtLine::operator+=( TxtLine *l )
{
    int i;

    for ( i = 0; i < l->items(); i++ )
	append( new TxtObj( *l->itemAt( i ) ) );

    return *this;
}

/*======================= split an object ========================*/
void TxtLine::splitObj( int pos )
{
    int obj = getInObj( pos );

    if ( obj != -1 )
    {
	int relPos, i, w = 0;
	TxtObj *obj1 = new TxtObj();
	TxtObj *obj2 = new TxtObj();

	// get the cursor position relative to the object
	for ( i = 0; i < obj; i++ )
	    w += itemAt( i )->textLength();
	relPos = pos - w;

	// insert the text in front of the cursor into the first object
	obj1->append( itemAt( obj )->text().left( relPos ).data() );
	obj1->setFont( itemAt( obj )->font() );
	obj1->setColor( itemAt( obj )->color() );
	obj1->setType( itemAt( obj )->type() );
	obj1->setVertAlign( itemAt( obj )->vertAlign() );

	// insert the text after the object into the second object
	obj2->append( itemAt( obj )->text().right( itemAt( obj )->textLength()-relPos ).data() );
	obj2->setFont( itemAt( obj )->font() );
	obj2->setColor( itemAt( obj )->color() );
	obj2->setType( itemAt( obj )->type() );
	obj2->setVertAlign( itemAt( obj )->vertAlign() );

	// remove original object
	objList.remove( obj );

	// insert the two new objects
	objList.insert( obj, obj2 );
	objList.insert( obj, obj1 );
    }
}

/*=============== get position in an object ======================*/
int TxtLine::getInObj( int pos, int *startpos	)
{
    int i, obj = 0, objNum = 0;

    // avoid segfaults :- )
    if ( pos > lineLength() || pos == 0 ) return -1;

    for ( i = 0; i <= pos; i++ )
    {

	// if we are at the end of an object
	if ( i == obj+objList.at( objNum )->textLength() )
	{
	    obj = i;

	    if ( startpos != 0 )
		*startpos = obj;

	    objNum++;
	    if ( pos == i ) return -1;
	}

	// if we are in an object and we are at the given position -> return object number
	else if ( pos == i ) return static_cast<int>( objNum );
    }

    // avoid compiler warnings :- )
    return -1;
}

/*============== get position before an object ===================*/
int TxtLine::getBeforeObj( int pos, int *startpos  )
{
    int i, obj = 0, objNum = 0;

    // avoid segfaults :- )
    if ( pos > lineLength() ) return -1;

    // if pos at the beginning
    if ( pos == 0 ) return 0;

    for ( i = 0; i <= pos; i++ )
    {

	// if we are at the end of an object == beginning of next object
	if ( i == obj+objList.at( objNum )->textLength() )
	{
	    obj += objList.at( objNum )->textLength();

	    if ( startpos != 0 )
		*startpos = obj;

	    objNum++;
	    if ( pos == i && i < lineLength()-1 ) return objNum;
	}
    }

    return -1;
}

/*=============== get position after an object ===================*/
int TxtLine::getAfterObj( int pos, int *startpos  )
{
    int i, obj = 0, objNum = 0;

    // avoid segfaults :- )
    if ( pos > lineLength() ) return -1;

    for ( i = 0; i <= pos; i++ )
    {

	// if we are at the end of an object
	if ( i == obj+objList.at( objNum )->textLength() )
	{
	    if ( startpos != 0 )
		*startpos = obj;

	    obj += objList.at( objNum )->textLength();
	    if ( pos == i && i > 0 ) return objNum;
	    objNum++;
	}
    }

    return -1;
}

/*============== get text of a part of the line ==================*/
QString TxtLine::getPartOfText( int _from, int _to )
{
    QString str;
    int currPos = 0;
    int _f, _t;

    for ( int i = 0; i < static_cast<int>( items() ); i++ )
    {
	_f = _from - currPos;
	_f = _f > 0 ? _f : 0;
	_f = _f < static_cast<int>( itemAt( i )->textLength() ) ? _f : static_cast<int>( itemAt( i )->textLength() );
	_t = _to - currPos;
	_t = _t > 0 ? _t : 0;
	_t = _t < static_cast<int>( itemAt( i )->textLength() ) ? _t : static_cast<int>( itemAt( i )->textLength() );
	str.append( itemAt( i )->text().mid( _f, _t - _f ) );

	currPos += itemAt( i )->textLength();
    }

    str.append( "\n" );
    return str;
}

/*===================== get number of words ======================*/
int TxtLine::words()
{
    int _words = 0, _ind, ind;

    QString str = getText();
    str = str.simplifyWhiteSpace();
    if ( !str.isEmpty() )
    {
	ind = 0;
	while ( true )
	{
	    _ind = str.find( " ", ind );
	    if ( static_cast<int>( _ind ) == -1 ) break;
	    _words++;
	    ind = _ind + 1;
	}
	_words++;
    }

    return _words;
}

/*======================= get word ===============================*/
QString TxtLine::wordAt( int pos, int &ind )
{
    if ( pos < words() )
    {
	QString str = getText();
	int _word = 0, _ind;
	ind = 0;

	str = str.simplifyWhiteSpace();

	if ( !str.isEmpty() )
	{
	    str.append( " " );
	    ind = 0;
	    while ( true )
	    {
		_ind = str.find( " ", ind );

		if ( _ind == -1 ) break;

		if ( _word == static_cast<int>( pos ) ) return str.mid( ind, _ind - ind );

		_word++;
		ind = _ind + 1;
	    }
	}
    }

    return QString( "" );
}

/******************************************************************/
/* class TxtParagraph - Text Paragraph				  */
/******************************************************************/

/*===================== constructor ==============================*/
TxtParagraph::TxtParagraph( bool init )
{
    // init the objects
    lineList.setAutoDelete( true );
    objHorzAlign = LEFT;
    _depth = 0;
    _leftIndent = 0;
    lineSpacing = 0;
    distBefore = 0;
    distAfter = 0;

    if ( init )
    {
	TxtLine *l = new TxtLine( init );
	lineList.append( l );
    }
}

/*======================== insert a text =========================*/
void TxtParagraph::insert( TxtCursor, const QString &, QFont,
			   QColor, TxtObj::VertAlign )
{
}

/*======================== insert a char =========================*/
void TxtParagraph::insert( TxtCursor , char , QFont ,
			   QColor , TxtObj::VertAlign  )
{
}

/*======================= append a text ==========================*/
void TxtParagraph::append( const QString & , QFont , QColor ,
			   TxtObj::VertAlign  )
{
}

/*====================== append a char ===========================*/
void TxtParagraph::append( char , QFont , QColor , TxtObj::VertAlign  )
{
}

/*====================== insert a line ===========================*/
void TxtParagraph::insert( int i, TxtLine *l )
{
    lineList.insert( i, l );
}

/*====================== append a line ===========================*/
void TxtParagraph::append( TxtLine *l )
{
    lineList.append( l );
}

/*======================= append an object =======================*/
void TxtParagraph::insert( int /*i*/, TxtObj* )
{
}

/*====================== append an object ========================*/
void TxtParagraph::append( TxtObj* to )
{
    if ( lineList.isEmpty() )
    {
	lin = new TxtLine();
	lineList.append( lin );
    }

    lineList.at( lineList.count()-1 )->append( to->text().data(), to->font(),
					       to->color(), to->vertAlign(), to->type() );
}

/*============= return textlength of the paragraph ===============*/
int TxtParagraph::paragraphLength()
{
    int l = 0;

    for ( linePtr = lineList.first(); linePtr != 0; linePtr = lineList.next() )
	l += linePtr->lineLength();

    return l;
}

/*==================== width of the paragraph ====================*/
int TxtParagraph::width()
{
    int w = 0;

    for ( linePtr = lineList.first(); linePtr != 0; linePtr = lineList.next() )
	w = QMAX( w, linePtr->width() );

    return w;
}

/*==================== height of the paragraph ===================*/
int TxtParagraph::height()
{
    int h = 0;

    for ( linePtr = lineList.first(); linePtr != 0; linePtr = lineList.next() )
	h += linePtr->height( this );

    h += distBefore + distAfter;

    return h;
}

/*======================= break lines ============================*/
QRect TxtParagraph::breakLines( int wid, bool regExpMode , bool composerMode )
{
    //debug( "break lines begin" );
    //debug( " concat lines begin" );
    // concat all lines to one
    linePtr = toOneLine();
    //debug( " concat lines end" );

    if ( regExpMode && regExpList && !composerMode )
	;

    line = new TxtLine();
    int i, j, w = 0;

  // if a line exists
    if ( linePtr ) {
	if ( linePtr->itemAt( linePtr->items() - 1 )->type() == TxtObj::SEPARATOR && linePtr->items() > 1 ) {
	    QFont font = linePtr->itemAt( linePtr->items() - 2 )->font();
	    QColor color = linePtr->itemAt( linePtr->items() - 2 )->color();
	    if ( linePtr->itemAt( linePtr->items() - 1 )->font() != font )
		linePtr->itemAt( linePtr->items() - 1 )->setFont( font );
	    if ( linePtr->itemAt( linePtr->items() - 1 )->color() != color )
		linePtr->itemAt( linePtr->items() - 1 )->setColor( color );
	}

	for ( i = 0; i < linePtr->items(); i++ ) {
	    obj = linePtr->itemAt( i );

	    // if the object fits into the line or is a separator
	    // ( separators have always to be appended, never insert a separator as first char in a line ->
	    //looks ugly! )
	    if ( w + widthToNextSep( i ) <= wid ) { // || obj->type() == TxtObj::SEPARATOR )
		line->append( obj );
		w += obj->width();
	    } else { // if the object doesn't fit
		if ( line->items() > 0 ) lineList.append( line );
		line = new TxtLine();
		w = 0;

		// insert all objects until the next separator
		for ( j = i; j < linePtr->items() && linePtr->itemAt( j )->type() != TxtObj::SEPARATOR; j++ ) {
		    obj = linePtr->itemAt( j );
		    line->append( obj );
		    w += obj->width();
		}
		i = j-1;
	    }
	}

	// don't forget the last line!
	if ( line->items() > 0 )
	    lineList.append( line );

    }
    //debug( "break lines end" );

    return QRect();
}

/*===================== break lines ==============================*/
void TxtParagraph::break_Lines( int wid, bool regExpMode, bool composerMode )
{
    linePtr = toOneLine();

    if ( regExpMode && regExpList && !composerMode )
	;

    line = new TxtLine();
    int i, j, w = 0;

    // if a line exists
    if ( linePtr ) {
	for ( i = 0; i < linePtr->items(); i++ ) {
	    obj = linePtr->itemAt( i );

	    // if the object fits into the line or is a separator
	    // ( separators have always to be appended, never insert a separator as first char in a line ->
	    //looks ugly! )
	    if ( w + charsToNextSep( i ) <= wid ) { // || obj->type() == TxtObj::SEPARATOR )
		line->append( obj );
		w += obj->textLength();
	    } else { // if the object doesn't fit
		if ( line->items() > 0 ) lineList.append( line );
		line = new TxtLine();
		w = 0;

		// insert all objects until the next separator
		for ( j = i; j < linePtr->items() && linePtr->itemAt( j )->type() != TxtObj::SEPARATOR; j++ ) {
		    obj = linePtr->itemAt( j );
		    line->append( obj );
		    w += obj->textLength();
		}
		i = j-1;
	    }
	}

	// don't forget the last line!
	if ( line->items() > 0 ) lineList.append( line );
    }
}

/*==================== concat all lines to one line ==============*/
TxtLine* TxtParagraph::toOneLine()
{
    int i;
    linePtr = new TxtLine();

    if ( !lineList.isEmpty() ) {
	// concat all lines to one line
	if ( lines() == 1 )
	    *linePtr = lineAt( 0 );
	else
	    for ( i = 0; i < lines(); i++ )
		*linePtr += lineAt( i );

	// clear the list and return the pointer to the resulting line
	lineList.clear();
	return linePtr;
    }

    return 0;
}

/*==================== get number of TxtObjs =====================*/
int TxtParagraph::items()
{
    int _items = 0;

    if ( !lineList.isEmpty() )
    {
	for ( int i = 0; i < static_cast<int>( lineList.count() ); i++ )
	    _items += lineList.at( i )->items();
    }

    return _items;
}

/*=================== get number of words ========================*/
int TxtParagraph::words()
{
    int _words = 0;

    if ( !lineList.isEmpty() )
    {
	for ( int i = 0; i < static_cast<int>( lineList.count() ); i++ )
	    _words += lineList.at( i )->words();
    }

    return _words;
}

/*====================== get TxtObj ==============================*/
TxtObj* TxtParagraph::itemAt( int pos )
{
    int _item = 0;

    if ( !lineList.isEmpty() )
    {
	for ( int i = 0; i < static_cast<int>( lines() ); i++ )
	{
	    if ( pos < _item + lineAt( i )->items() )
	    {
		pos -= _item;
		return lineAt( i )->itemAt( pos );
	    }
	    else _item += lineAt( i )->items();
	}
    }

    return 0;
}

/*======================= get word ===============================*/
QString TxtParagraph::wordAt( int pos, int &ind )
{
    int _word = 0;
    ind = 0;

    if ( pos < words() && !lineList.isEmpty() )
    {
	for ( int i = 0; i < static_cast<int>( lines() ); i++ )
	{
	    if ( pos < _word + lineAt( i )->words() )
	    {
		pos -= _word;
		QString r = lineAt( i )->wordAt( pos, ind );
		return r;
	    }
	    else _word += lineAt( i )->words();
	}
    }

    return QString( "" );
}

/*====================== delete Region ===========================*/
void TxtParagraph::deleteRegion( int _start, int _stop )
{
    if ( _stop == -1 ) _stop = paragraphLength() - 1;

    linePtr = toOneLine();

    enum CurPos {C_IN, C_BEFORE, C_AFTER};
    int start_pos = 0, start_cpos = C_IN, i;
    int stop_pos = 0, stop_cpos = C_IN, objnum, start, stop;

    objnum = linePtr->getInObj( _start );
    if ( objnum == -1 )
    {
	objnum = linePtr->getBeforeObj( _start );
	if ( objnum == -1 )
	{
	    objnum = linePtr->getAfterObj( _start );
	    if ( objnum == -1 )
		// something wrong here - let's exit!
		return;
	    else start_cpos = C_AFTER;
	}
	else start_cpos = C_BEFORE;

    }
    else start_cpos = C_IN;

    start_pos = objnum;

    objnum = linePtr->getInObj( _stop );
    if ( objnum == -1 )
    {
	objnum = linePtr->getBeforeObj( _stop );
	if ( objnum == -1 )
	{
	    objnum = linePtr->getAfterObj( _stop );
	    if ( objnum == -1 )
		// something wrong here - let's exit!
		return;
	    else stop_cpos = C_AFTER;
	}
	else stop_cpos = C_BEFORE;

    }
    else stop_cpos = C_IN;

    stop_pos = objnum;

    if ( stop_cpos == C_AFTER ) stop_pos++;

    if ( start_cpos == C_IN )
    {
	linePtr->splitObj( _start );
	start_pos++;
	start_cpos = C_BEFORE;
	stop_pos++;
    }

    if ( stop_cpos == C_IN )
    {
	linePtr->splitObj( _stop );
	//stop_cpos = C_BEFORE;
    }

    if ( start_cpos == C_AFTER ) start = start_pos + 1;
    else start = start_pos;

    if ( stop_cpos == C_AFTER ) stop = stop_pos + 1;
    else if ( stop_cpos == C_BEFORE ) stop = stop_pos - 1;
    else stop = stop_pos;

    for ( i = stop; i >= start; i-- )
    {
	if ( i < static_cast<int>( linePtr->items() ) )
	    linePtr->deleteItem( static_cast<int>( i ) );
    }

    if ( linePtr->items() > 0 ) {
	if ( linePtr->itemAt( linePtr->items() - 1 )->type() != TxtObj::SEPARATOR )
	    linePtr->append( " ", linePtr->itemAt( linePtr->items() - 1 )->font(),
			     linePtr->itemAt( linePtr->items() - 1 )->color(), TxtObj::NORMAL, TxtObj::SEPARATOR );
    }

    append( linePtr );
}

/*============= with from an obj to next separator ===============*/
int TxtParagraph::widthToNextSep( int pos )
{
    int i, w = 0;

    for ( i = pos; i < linePtr->items() && linePtr->itemAt( i )->type() != TxtObj::SEPARATOR; i++ )
	w += linePtr->itemAt( i )->width();

    return w;
}

/*============= chars from an obj to next separator ==============*/
int TxtParagraph::charsToNextSep( int pos )
{
    int i, w = 0;

    for ( i = pos; i < linePtr->items() && linePtr->itemAt( i )->type() != TxtObj::SEPARATOR; i++ )
	w += linePtr->itemAt( i )->textLength();

    return w;
}

/*================================================================*/
bool TxtParagraph::isEmpty()
{
    bool empty = true;
    QString str;

    if ( !lineList.isEmpty() )
    {
	for ( int i = 0; i < static_cast<int>( lines() ); i++ )
	{
	    str = lineAt( i )->getText();
	    str = str.simplifyWhiteSpace();
	    if ( str.length() > 0 )
	    {
		empty = false;
		break;
	    }
	}
    }

    return empty;
}

/*================================================================*/
void TxtParagraph::setDepth( int d )
{
    _depth = d;
    QFont font;

    if ( !lineList.isEmpty() && lineList.at( 0 )->items() > 0 )
	font = lineList.at( 0 )->itemAt( 0 )->font();
    else
	font = QFont( "times", 12 );

    QFontMetrics fm( font );

    _leftIndent = fm.width( 'W' ) * _depth;
}

/*================================================================*/
void TxtParagraph::setOrigSpacings( int ls, int db, int da )
{
    oLineSpacing = ls;
    oDistBefore = db;
    oDistAfter = da;
}

/*================================================================*/
void TxtParagraph::getOrigSpacings( int &ls, int &db, int &da )
{
    ls = oLineSpacing;
    db = oDistBefore;
    da = oDistAfter;
}

/******************************************************************/
/* class KTextObject - KTextObject				  */
/******************************************************************/

/*====================== constructor =============================*/
KTextObject::KTextObject( QWidget *parent, const char *name, ObjType ot,
			  int c, int r, int __width )
    : QTableView( parent, name )
{
    currPageNum = 1;
    gap = 0;
    // init the objects
    _parent = parent;
    doRepaints = true;

    composerMode = false;
    linebreak_width = __width;
    _width = 0;

    setFocusPolicy( QWidget::StrongFocus );
    setBackgroundColor( white );

    objEnumListType.type = NUMBER;
    objEnumListType.before = "";
    objEnumListType.after = ".";
    objEnumListType.start = 1;
    objEnumListType.font = QFont( "times", 12 );
    objEnumListType.color = Qt::black;

    objUnsortListType.font = new QList<QFont>;
    objUnsortListType.color = new QList<QColor>;
    objUnsortListType.ofont = new QList<QFont>;
    objUnsortListType.chr = new QList<QChar>;

    for ( int i = 0; i < 16; i++ )
    {
	objUnsortListType.font->append( new QFont( "times", 20 ) );
	objUnsortListType.color->append( new QColor( Qt::black ) );
	objUnsortListType.chr->append( new QChar( '-' ) );
	objUnsortListType.font->append( new QFont() );
    }
    objUnsortListType.font->setAutoDelete( true );
    objUnsortListType.color->setAutoDelete( true );
    objUnsortListType.ofont->setAutoDelete( true );

    paragraphList.setAutoDelete( true );

    cellWidths.setAutoDelete( true );
    cellHeights.setAutoDelete( true );
    objRow = r;
    objCol = c;

    sCursor = true;
    txtCursor = new TxtCursor( ( KTextObject* )this );
    cursorChanged = true;
    setCursor( ibeamCursor );
    searchIndexFrom.setPositionAbs( 0 );
    searchIndexTo.setPositionAbs( 0 );

    startCursor.setKTextObject( ( KTextObject* )this );
    stopCursor.setKTextObject( ( KTextObject* )this );

    setFocusPolicy( StrongFocus );

    drawLine = -1;
    drawParagraph = -1;
    drawBelow = false;

    currFont = QFont( "utopia", 20 );
    currFont.setBold( true );
    currColor = Qt::black;

    setMouseTracking( true );
    mousePressed = false;
    drawPic = false;
    ystart = 0;
    drawSelection = false;

    selectionColor = QColor( 0, 255, 255 );

    TxtParagraph *para;
    para = new TxtParagraph( true );
    paragraphList.append( para );

    CellWidthHeight *wh = new CellWidthHeight;
    wh->wh = width();
    cellWidths.append( wh );
    setNumCols( 1 );

    wh = new CellWidthHeight;
    wh->wh = 0;
    cellHeights.append( wh );
    setNumRows( 1 );

    setShowCursor( true );

    setObjType( ot );

    createRBMenu();

    // this is required for correct init of headers/footers
    _modified = true;

    regexpMode = false;
    regExpList.setAutoDelete( true );

    autoReplace.setAutoDelete( true );

    allInOneColor = false;
    autoFocus = false;

    setKeyCompression( true );

    QAccel *a = new QAccel( this );
    a->connectItem( a->insertItem( CTRL + Key_B ),
		    this, SLOT( formatBold() ) );
    a->connectItem( a->insertItem( CTRL + Key_I ),
		    this, SLOT( formatItalic() ) );
    a->connectItem( a->insertItem( CTRL + Key_U ),
		    this, SLOT( formatUnderline() ) );
    a->connectItem( a->insertItem( CTRL + Key_X ),
		    this, SLOT( clipCut() ) );
    a->connectItem( a->insertItem( CTRL + Key_V ),
		    this, SLOT( clipPaste() ) );
    a->connectItem( a->insertItem( CTRL + Key_C ),
		    this, SLOT( clipCopy() ) );
}

/*======================== destructor ============================*/
KTextObject::~KTextObject()
{
    clear( false );
    cellWidths.clear();
    cellHeights.clear();
    delete txtCursor;
    regExpList.clear();
    autoReplace.clear();

    objUnsortListType.font->clear();
    objUnsortListType.color->clear();
    objUnsortListType.chr->clear();
    delete objUnsortListType.font;
    delete objUnsortListType.color;
    delete objUnsortListType.ofont;
}

/*===================== set objecttype ===========================*/
void KTextObject::setObjType( ObjType ot )
{
    obType = ot;
    recalc();
    repaint( true );
}

/*================= get length of the text =======================*/
int KTextObject::textLength()
{
    int l = 0;

    for ( paragraphPtr = paragraphList.first(); paragraphPtr != 0; paragraphPtr = paragraphList.next() )
	l += paragraphPtr->paragraphLength();

    return l;
}

/*==================== get horizontal alignemnt ==================*/
TxtParagraph::HorzAlign KTextObject::horzAlign( int p )
{
    if ( p == -1 ) p = txtCursor->positionParagraph();

    return paragraphAt( p )->horzAlign();
}

/*=================== get QPicture of the obj ====================*/
QPicture* KTextObject::getPic( int _x, int _y, int _w, int _h, bool presMode, int from, int to, bool _clip )
{
    QPainter p;

    drawPic = true;
    p.begin( &pic );
    if ( _clip ) {
	p.setClipping( true );
	p.setClipRect( _x, _y, _w, _h );
    }
    ystart = 0;

    from = min(from,to);
    to = max(from,to);

    if ( paragraphList.count() == 1 && paragraphAt( 0 )->lines() == 1 &&
	 paragraphAt( 0 )->lineAt( 0 )->items() == 1 &&
	 !presMode ) {
	QFont _font( "helvetica", 12 );
	_font.setBold( true );
	p.setFont( _font );
	p.setPen( Qt::red );
	p.drawText( 0, 10, _w, _h, AlignLeft, i18n( "Doubleclick to edit" ) );
    } else {
	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ ) {
	    if ( from == -1 && to == -1 || from <= i && to >= i )
		paintCell( &p, i, 0 );
	    ystart += cellHeights.at( i )->wh;
	}
    }

    p.end();
    ystart = 0;
    drawPic = false;
    return &pic;
}

/*=================== get QPicture of the obj ====================*/
void KTextObject::draw( QPainter &p, int _x, int _y, int _w, int _h, bool presMode,
			int from, int to, bool _clip, bool _drawempty )
{
    p.save();

    drawPic = true;

    if ( _clip ) {
	p.setClipping( true );
	p.setClipRect( _x, _y, _w, _h );
    }
    ystart = 0;

    from = min(from,to);
    to = max(from,to);

     if ( paragraphList.count() == 1 && paragraphAt( 0 )->lines() == 1 &&
	  paragraphAt( 0 )->lineAt( 0 )->items() == 1 &&
	 !presMode && _drawempty ) {
	QFont _font( "helvetica", 12 );
	_font.setBold( true );
	p.setFont( _font );
	p.setPen( Qt::red );
	p.drawText( 0, 10, _w, _h, AlignLeft, i18n( "Doubleclick to edit" ) );
    } else {
	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	{
	    if ( from == -1 && to == -1 || from <= i && to >= i )
		paintCell( &p, i, 0 );
	    ystart += cellHeights.at( i )->wh;
	}
    }

    ystart = 0;
    drawPic = false;

    p.restore();
}

/*====================== zoom text ===============================*/
void KTextObject::zoom( float _fakt )
{
    _modified = true;

    TxtObj *txtObj;
    TxtLine *txtLine;
    TxtParagraph *txtParagraph;
    int i, j, k;
    QFont font;

    for ( i = 0; i < paragraphs(); i++ ) {
	txtParagraph = paragraphAt( i );
	txtParagraph->setOrigSpacings( txtParagraph->getLineSpacing(),
				       txtParagraph->getDistBefore(),
				       txtParagraph->getDistAfter() );
	txtParagraph->setLineSpacing( static_cast<int>( static_cast<float>( txtParagraph->getLineSpacing() )
							* _fakt ) );
	txtParagraph->setDistBefore( static_cast<int>( static_cast<float>( txtParagraph->getDistBefore() ) * _fakt ) );
	txtParagraph->setDistAfter( static_cast<int>( static_cast<float>( txtParagraph->getDistAfter() ) * _fakt ) );
	
	for ( j = 0; j < txtParagraph->lines(); j++ ) {
	    txtLine = txtParagraph->lineAt( j );

	    for ( k = 0; k < txtLine->items(); k++ ) {
		txtObj = txtLine->itemAt( k );
		font = txtObj->font();
		txtObj->setOrigSize( txtObj->font().pointSize() );
		font.setPointSize( static_cast<int>( static_cast<float>( font.pointSize() ) * _fakt ) );
		txtObj->setFont( font );
	    }
	}
	txtParagraph->setDepth( txtParagraph->getDepth() );
    }

    font = objEnumListType.font;
    objEnumListType.ofont = font;
    font.setPointSize( static_cast<int>( static_cast<float>( font.pointSize() ) * _fakt ) );
    objEnumListType.font = font;

    objUnsortListType.ofont->clear();
    for ( i = 0; i < 16; i++ ) {
	font = *objUnsortListType.font->at( i );
	objUnsortListType.ofont->append( new QFont( font ) );
    }

    objUnsortListType.font->clear();
    for ( i = 0; i < 16; i++ ) {
	font = *objUnsortListType.ofont->at( i );
	font.setPointSize( static_cast<int>( static_cast<float>( font.pointSize() ) * _fakt ) );
	objUnsortListType.font->append( new QFont( font ) );
    }

    recalc();
    repaint( false );
}

/*==================== zoom to original size =====================*/
void KTextObject::zoomOrig()
{
    _modified = true;

    TxtObj *txtObj;
    TxtLine *txtLine;
    TxtParagraph *txtParagraph;
    int i, j, k;
    QFont font;

    for ( i = 0; i < paragraphs(); i++ ) {
	txtParagraph = paragraphAt( i );
	int ls, db, da;
	txtParagraph->getOrigSpacings( ls, db, da );
	txtParagraph->setLineSpacing( ls );
	txtParagraph->setDistBefore( db );
	txtParagraph->setDistAfter( da );
	
	for ( j = 0; j < txtParagraph->lines(); j++ ) {
	    txtLine = txtParagraph->lineAt( j );

	    for ( k = 0; k < txtLine->items(); k++ ) {
		txtObj = txtLine->itemAt( k );
		font = txtObj->font();
		font.setPointSize( txtObj->origSize() );
		txtObj->setFont( font );
	    }
	}
	txtParagraph->setDepth( txtParagraph->getDepth() );
    }

    objEnumListType.font = objEnumListType.ofont;

    objUnsortListType.font->clear();
    for ( i = 0; i < 16; i++ )
	objUnsortListType.font->append( new QFont( *objUnsortListType.ofont->at( i ) ) ); ;

	recalc();
	repaint( false );
}

/*========================= add paragraph ========================*/
TxtParagraph* KTextObject::addParagraph()
{
    _modified = true;

    TxtParagraph *para;
    para = new TxtParagraph( false );
    paragraphList.append( para );

    wh = new CellWidthHeight;
    wh->wh = 0;
    cellHeights.append( wh );
    setAutoUpdate( false );
    setNumRows( numRows()+1 );
    setAutoUpdate( true );

    return paragraphList.at( paragraphList.count()-1 );
}

/*======================== clear =================================*/
void KTextObject::clear( bool init )
{
    _modified = true;

    if ( init ) {
	paragraphList.clear();
	cellHeights.clear();

	TxtParagraph *para;
	para = new TxtParagraph( true );
	paragraphList.append( para );

	wh = new CellWidthHeight;
	wh->wh = 0;
	cellHeights.append( wh );
	setNumRows( 1 );

	delete txtCursor;
	txtCursor = new TxtCursor( ( KTextObject* )this );

	recalc();
	repaint( false );
	updateTableSize();
	updateScrollBars();
    } else {
	paragraphList.clear();
	cellHeights.clear();
	setNumRows( 0 );
	setNumCols( 1 );
    }
}

/*==================== return ascii text =========================*/
QString KTextObject::toASCII( bool linebreak, bool makelist )
{
    QString str;

    TxtObj *txtObj;
    TxtLine *txtLine;
    TxtParagraph *txtParagraph;
    int i, j, k;
    QString chr;
    QString space = "		       ";

    for ( i = 0; i < paragraphs(); i++ ) {
	txtParagraph = paragraphAt( i );

	// TODO: Alignment

	if ( makelist ) {
	    switch ( objType() ) {
	    case PLAIN: break;
	    case UNSORT_LIST: str += "- "; break;
	    case ENUM_LIST: {
		if ( objEnumListType.type == NUMBER )
		    chr.sprintf( "%s%d%s ", objEnumListType.before.data(), i+objEnumListType.start,
				 objEnumListType.after.data() );
		else
		    chr.sprintf( "%s%c%s ", objEnumListType.before.data(), i+objEnumListType.start,
				 objEnumListType.after.data() );
		str += chr;
	    } break;
	    default: break;
	    }
	}

	for ( j = 0; j < txtParagraph->lines(); j++ ) {
	    txtLine = txtParagraph->lineAt( j );
	    if ( j > 0 ) {
		if ( makelist ) {
		    switch ( objType() ) {
		    case PLAIN: break;
		    case UNSORT_LIST: str += "	"; break;
		    case ENUM_LIST: str += space.left( chr.length() ); break;
		    default: break;
		    }
		}
	    }

	    for ( k = 0; k < txtLine->items(); k++ ) {
		txtObj = txtLine->itemAt( k );

		str += txtObj->text();
	    }

	    if ( linebreak ) str += "\n";
	}
	if ( !linebreak ) str += "\n";
    }

    return str;
}

/*======================= save ASCII text ========================*/
void KTextObject::saveASCII( QString filename, bool linebreak )
{
    FILE *file;

    file = fopen( filename.data(), "w" );
    fprintf( file, toASCII( linebreak ).data() );
    fclose( file );
}

/*======================== add text ==============================*/
void KTextObject::addText( QString text, QFont font, QColor color,
			   bool newParagraph, TxtParagraph::HorzAlign align,
			   bool _recalc, bool htmlMode )
{
    _modified = true;

    TxtObj *txtObj;
    TxtParagraph *txtParagraph;
    int br = -1, sp = -1;

    if ( htmlMode )
	text = text.replace( QRegExp( "\x26nbsp\x3b" ), " " );

    QTextStream t( text, IO_ReadOnly );
    QString s = "", tmp;

    while ( !t.eof() ) {
	tmp = t.readLine();
	if ( !isValid( tmp ) ) tmp = " ";
	tmp += "\n";
	s.append( tmp );
    }

    text = s;

    if ( newParagraph || paragraphs() < 1 ) {
	txtParagraph = addParagraph();
	txtParagraph->setHorzAlign( align );
    } else {
	txtParagraph = paragraphAt( paragraphs()-1 );
	if ( txtParagraph->lineAt( txtParagraph->lines()-1 )->
	     itemAt( txtParagraph->lineAt( txtParagraph->lines()-1 )->items()-1 )->type() == TxtObj::SEPARATOR )
	    txtParagraph->lineAt( txtParagraph->lines()-1 )->
		deleteItem( txtParagraph->lineAt( txtParagraph->lines()-1 )->items()-1 );
    }

    txtObj = new TxtObj();
    txtObj->setFont( font );
    txtObj->setColor( color );
    txtObj->setType( TxtObj::TEXT );

    while ( true ) {
	sp = text.find( QRegExp( "[ \x20\t ]" ) );
	br = text.find( "\n" );

	if ( br == -1 && sp == -1 )
	    break;
	else {
	    if ( br == -1 || ( sp != -1 && sp < br ) ) {
		if ( isValid( text.left( sp ) ) ) {
		    if ( !txtParagraph ) {
			txtParagraph = addParagraph();
			txtParagraph->setHorzAlign( align );
		    }
		    txtObj->append( text.left( sp ) );
		    txtParagraph->append( txtObj );
		}
		if ( text.mid( sp, 1 ) == " " )
		    txtObj = new TxtObj( " ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );
		else
		    txtObj = new TxtObj( "    ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );

		if ( !txtParagraph ) {
		    txtParagraph = addParagraph();
		    txtParagraph->setHorzAlign( align );
		}
		txtParagraph->append( txtObj );

		txtObj = new TxtObj();
		txtObj->setFont( font );
		txtObj->setColor( color );
		txtObj->setType( TxtObj::TEXT );

		text.remove( 0, sp+1 );
	    } else if ( sp == -1 || ( br != -1 && br < sp ) ) {
		if ( isValid( text.left( br ) ) ) {
		    txtObj->append( text.left( br ) );
		    if ( !txtParagraph ) {
			txtParagraph = addParagraph();
			txtParagraph->setHorzAlign( align );
		    }
		    txtParagraph->append( txtObj );
		}

		txtObj = new TxtObj();
		txtObj->setFont( font );
		txtObj->setColor( color );
		txtObj->setType( TxtObj::TEXT );

		txtParagraph = 0;

		text.remove( 0, br+1 );
	    }
	}
    }

    if ( text.length() > 0 && isValid( text ) ) {
	if ( !txtParagraph ) {
	    txtParagraph = addParagraph();
	    txtParagraph->setHorzAlign( align );
	}
	txtObj->append( text.left( sp ) );
	txtParagraph->append( txtObj );
    }

    if ( !txtParagraph ) {
	txtParagraph = addParagraph();
	txtParagraph->setHorzAlign( align );
    }

    txtObj = new TxtObj( " ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );
    txtParagraph->append( txtObj );

    if ( _recalc ) {
	recalc();
	repaint( true );
	updateTableSize();
    }

    txtCursor->setMaxPosition( textLength() );
}

/*===================== open ascii file ==========================*/
void KTextObject::openASCII( QString filename )
{
    setCursor( waitCursor );
    doRepaints = false;

    QFile f( filename );
    QTextStream t( &f );
    QString s = "", tmp;

    if ( f.open( IO_ReadOnly ) ) {
	while ( !t.eof() ) {
	    tmp = t.readLine();
	    if ( !isValid( tmp ) ) tmp = " ";
	    tmp += "\n";
	    s.append( tmp );
	}
	f.close();
    }

    addText( s, KGlobal::generalFont(), colorGroup().text(), false, TxtParagraph::LEFT, false );

    doRepaints = true;

    recalc();
    repaint( false );
    updateTableSize();
    setCursor( ibeamCursor );
}

/*==================== set linebreak =============================*/
void KTextObject::setLineBreak( int _width )
{
    _modified = true;

    linebreak_width = _width;
    recalc();
    repaint( false );
}

/*===================== get part of text =========================*/
QString KTextObject::getPartOfText( TxtCursor *_from, TxtCursor *_to )
{
    QString str;
    int para1 = _from->positionParagraph();
    int line1 = _from->positionLine();
    int pos1 = _from->positionInLine();
    int para2 = _to->positionParagraph();
    int line2 = _to->positionLine();
    int pos2 = _to->positionInLine();
    int i, j, fromLine, toLine, fromPos, toPos;

    for ( i = para1; i <= para2; i++ ) {
	paragraphPtr = paragraphAt( i );

	fromLine = 0;
	toLine = paragraphPtr->lines() - 1;
	if ( para1 == i ) fromLine = line1;
	if ( para2 == i ) toLine = line2;

	for ( j = fromLine; j <= toLine; j++ ) {
	    linePtr = paragraphPtr->lineAt( j );

	    fromPos = 0;
	    toPos = linePtr->lineLength();
	    if ( para1 == i && line1 == j ) fromPos = pos1;
	    if ( para2 == i && line2 == j ) toPos = pos2;

	    str.append( linePtr->getPartOfText( fromPos, toPos ) );
	}
    }

    if ( str.right( 1 ) == "\n" ) str.truncate( str.length() - 1 );
    return str;
}

/*====================== copy region =============================*/
void KTextObject::copyRegion( bool hideSelection )
{
    if ( drawSelection && stopCursor.positionAbs() != startCursor.positionAbs() ) {
	QString buffer = getPartOfText( &startCursor, &stopCursor );

	QClipboard *cb = QApplication::clipboard();
	cb->setText( buffer.data() );

	if ( hideSelection ) {
	    drawSelection = false;
	    redrawSelection( startCursor, stopCursor );
	    startCursor.setPositionAbs( 0 );
	    stopCursor.setPositionAbs( 0 );
	}
    }
}

/*====================== cut region =============================*/
void KTextObject::cutRegion()
{
    if ( drawSelection && stopCursor.positionAbs() != startCursor.positionAbs() ) {
	QString buffer = getPartOfText( &startCursor, &stopCursor );

	QClipboard *cb = QApplication::clipboard();
	cb->setText( buffer.data() );

	deleteRegion( &startCursor, &stopCursor );

	drawSelection = false;
	redrawSelection( startCursor, stopCursor );
	startCursor.setPositionAbs( 0 );
	stopCursor.setPositionAbs( 0 );

	_modified = true;
    }
}

/*=============================================================*/
void KTextObject::insertPageNum()
{
    drawSelection = false;
    redrawSelection( startCursor, stopCursor );
    startCursor.setPositionAbs( 0 );
    stopCursor.setPositionAbs( 0 );

    insertText( "\\pn", txtCursor, currFont, currColor );
}

/*======================= paste from clipboard ===================*/
void KTextObject::paste()
{
    drawSelection = false;
    redrawSelection( startCursor, stopCursor );
    startCursor.setPositionAbs( 0 );
    stopCursor.setPositionAbs( 0 );

    QClipboard *cb = QApplication::clipboard();
    if ( cb->text() )
	insertText( cb->text(), txtCursor, currFont, currColor );

    _modified = true;
}

/*================== get number of TxtObjs =======================*/
int KTextObject::items()
{
    int _items = 0;

    for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ ) {
	for ( int j = 0; j < static_cast<int>( paragraphAt( i )->lines() ); j++ )
	    _items += paragraphAt( i )->lineAt( j )->items();
    }

    return _items;
}

/*=================== get number of TxtObjs  in a line ===========*/
int KTextObject::itemsInLine( int line )
{
    if ( line < static_cast<int>( lines() ) )
	return lineAt( line )->items();

    return 0;
}

/*========== get number of TxtObjs in a line in a para ===========*/
int KTextObject::itemsInLine( int line, int para )
{
    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) )
	return paragraphAt( para )->lineAt( line )->items();
    return 0;
}

/*=================== get number of TxtObjs in a para ============*/
int KTextObject::itemsInParagraph( int para )
{
    if ( para < static_cast<int>( paragraphs() ) )
	return paragraphAt( para )->items();
    return 0;
}

/*==================== get number of words =======================*/
int KTextObject::words()
{
    int _words = 0;
    QString str;
    int ind, _ind;

    for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
    {
	for ( int j = 0; j < static_cast<int>( paragraphAt( i )->lines() ); j++ )
	{
	    str = paragraphAt( i )->lineAt( j )->getText();
	    str = str.simplifyWhiteSpace();
	    if ( !str.isEmpty() )
	    {
		ind = 0;
		while ( true )
		{
		    _ind = str.find( " ", ind );
		    if ( _ind == -1 ) break;
		    _words++;
		    ind = _ind + 1;
		}
		_words++;
	    }
	}
    }

    return _words;
}

/*=================== get number of words in a line ==============*/
int KTextObject::wordsInLine( int line )
{
    if ( line < static_cast<int>( lines() ) )
	return lineAt( line )->words();

    return 0;
}

/*========== get number of words in a line in a para =============*/
int KTextObject::wordsInLine( int line, int para )
{
    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) )
	return paragraphAt( para )->lineAt( line )->words();

    return 0;
}

/*=================== get number of words in a para ==============*/
int KTextObject::wordsInParagraph( int para )
{
    if ( para < static_cast<int>( paragraphs() ) )
	return paragraphAt( para )->words();

    return 0;
}

/*================== get number of lines =========================*/
int KTextObject::lines()
{
    int _lines = 0;

    for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	_lines += paragraphAt( i )->lines();

    return _lines;
}

/*=================== get number of lines in a para ==============*/
int KTextObject::linesInParagraph( int para )
{
    if ( para < static_cast<int>( paragraphs() ) )
	return paragraphAt( para )->lines();

    return 0;
}

/*==================== get TxtObj ================================*/
TxtObj* KTextObject::itemAt( int pos )
{
    if ( pos < static_cast<int>( items() ) )
    {
	int _item = 0;

	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	{
	    if ( pos < static_cast<int>( _item ) + static_cast<int>( paragraphAt( i )->items() ) )
	    {
		pos -= _item;
		return paragraphAt( i )->itemAt( pos );
	    }
	    else _item += paragraphAt( i )->items();
	}
    }

    return 0;
}

/*==================== get TxtObj ================================*/
TxtObj* KTextObject::itemAtLine( int pos, int line )
{
    if ( line < static_cast<int>( lines() ) && pos < static_cast<int>( lineAt( line )->items() ) )
	return lineAt( line )->itemAt( pos );

    return 0;
}

/*==================== get TxtObj ================================*/
TxtObj* KTextObject::itemAtPara( int pos, int para )
{
    if ( para < static_cast<int>( paragraphs() ) && pos < static_cast<int>( paragraphAt( para )->items() ) )
	return paragraphAt( para )->itemAt( pos );

    return 0;
}

/*==================== get TxtObj ================================*/
TxtObj* KTextObject::itemAt( int pos, int line, int para )
{
    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) &&
	 pos < static_cast<int>( paragraphAt( para )->lineAt( line )->items() ) )
	return paragraphAt( para )->lineAt( line )->itemAt( pos );

    return 0;
}

/*================== get text of a word ==========================*/
QString KTextObject::wordAt( int pos, int &ind )
{
    int _word = 0;
    ind = 0;

    if ( pos < static_cast<int>( words() ) )
    {
	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	{
	    if ( pos < static_cast<int>( _word ) + static_cast<int>( paragraphAt( i )->words() ) )
	    {
		pos -= _word;
		QString r = paragraphAt( i )->wordAt( pos, ind );
		return r;
	    }
	    else _word += paragraphAt( i )->words();
	}
    }

    return QString( "" );
}

/*================== get text of a word ==========================*/
QString KTextObject::wordAtLine( int pos, int line, int &ind )
{
    ind = 0;

    if ( line < static_cast<int>( lines() ) && pos < static_cast<int>( lineAt( line )->words() ) )
    {
	QString r = lineAt( line )->wordAt( pos, ind );
	return r;
    }

    return QString( "" );
}

/*================== get text of a word ==========================*/
QString KTextObject::wordAtPara( int pos, int para, int &ind )
{
    ind = 0;

    if ( para < static_cast<int>( paragraphs() ) && pos < static_cast<int>( paragraphAt( para )->words() ) )
    {
	QString r = paragraphAt( para )->wordAt( pos, ind );
	return r;
    }

    return QString( "" );
}

/*================== get text of a word ==========================*/
QString KTextObject::wordAt( int pos, int line, int para, int &ind )
{
    ind = 0;

    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) &&
	 pos < static_cast<int>( paragraphAt( para )->lineAt( line )->words() ) )
    {
	QString r = paragraphAt( para )->lineAt( line )->wordAt( pos, ind );
	return r;
    }

    return QString( "" );
}

/*===================== get line =================================*/
TxtLine* KTextObject::lineAt( int line )
{
    if ( line < static_cast<int>( lines() ) )
    {
	int _line = 0;

	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	{
	    if ( line < _line + static_cast<int>( paragraphAt( i )->lines() ) )
	    {
		line -= _line;
		return paragraphAt( i )->lineAt( line );
	    }
	    else _line += paragraphAt( i )->lines();
	}
    }

    else return 0;

    return 0;
}

/*===================== get line =================================*/
TxtLine* KTextObject::lineAt( int line, int para )
{
    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) )
	return paragraphAt( para )->lineAt( line );

    return 0;
}

/*====================== get the region ==========================*/
QList<TxtObj>* KTextObject::regionAt( TxtCursor * /*_startCursor*/, TxtCursor * /*_stopCursor*/ )
{
//   int start_line = 0, start_para = 0, start_pos = 0, i;
//   int stop_line = 0, stop_para = 0, stop_pos = 0;

//   for ( i = 0; i < _startCursor->positionParagraph(); i++ )
//     start_line += paragraphAt( i )->lines();

//   start_line += _startCursor->positionLine();

//   //*******************************************
//   //* HIER WEITERMACHEN ***********************
//   //*******************************************

    return 0;
}

/*======================= delete item ============================*/
void KTextObject::deleteItem( int pos )
{
    _modified = true;

    int para, line;
    getPara( pos, line, para );

    if ( para >= 0 && para < static_cast<int>( paragraphs() ) && line >= 0 &&
	 line < static_cast<int>( paragraphAt( para )->lines() ) )
	deleteItem( pos, line, para );
}

/*======================= delete item ============================*/
void KTextObject::deleteItemInLine( int pos, int line )
{
    _modified = true;

    int para;
    getPara( line, para );

    if ( para >= 0 && para < static_cast<int>( paragraphs() ) && line >= 0 &&
	 line < static_cast<int>( paragraphAt( para )->lines() ) )
	deleteItem( pos, line, para );
}

/*======================= delete item ============================*/
void KTextObject::deleteItemInPara( int pos, int para )
{
    _modified = true;

    int line;
    getLine( pos, para, line );

    if ( para >= 0 && para < static_cast<int>( paragraphs() ) && line >= 0 &&
	 line < static_cast<int>( paragraphAt( para )->lines() ) )
	deleteItem( pos, line, para );
}

/*======================= delete item ============================*/
void KTextObject::deleteItem( int pos, int line, int para )
{
    _modified = true;

    bool breakPara = true;

    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) &&
	 pos < static_cast<int>( paragraphAt( para )->lineAt( line )->items() ) )
    {
	txtCursor->setMaxPosition(txtCursor->maxPosition() -
				  paragraphAt(para)->lineAt(line)->itemAt(pos)->textLength());
	txtCursor->setPositionAbs( txtCursor->positionAbs() -
				   paragraphAt( para )->lineAt( line )->itemAt( pos )->textLength() );
	paragraphAt( para )->lineAt( line )->deleteItem( pos );
	if ( paragraphAt( para )->lineAt( line )->items() == 0 )
	{
	    paragraphAt( para )->deleteLine( line );
	    if ( paragraphAt( para )->lines() == 0 )
	    {
		paragraphList.remove( para );
		cellHeights.remove( para );
		setAutoUpdate( false );
		setNumRows( numRows() - 1 );
		setAutoUpdate( true );
		breakPara = false;
	    }
	}

	if ( breakPara ) changedParagraphs.append( ( int* )( para ) );
	recalc( false );
	changedParagraphs.clear();

	// should be more efficient!!
	repaint( false );
    }
}

/*====================== delete word =============================*/
void KTextObject::deleteWord( int pos )
{
    _modified = true;

    int x1, x2;
    int _pos = pos;
    getAbsPosOfWord( pos, x1, x2 );

    TxtCursor cur1, cur2;

    cur1.setKTextObject( ( KTextObject* )this );
    cur2.setKTextObject( ( KTextObject* )this );

    cur1.setPositionAbs( x1 );
    cur2.setPositionAbs( x2 );

    deleteRegion( &cur1, &cur2 );
    pos = _pos;
}

/*====================== delete word =============================*/
void KTextObject::deleteWordInLine( int pos, int line )
{
    _modified = true;

    int x1, x2;
    int _pos = pos;
    getAbsPosOfWordInLine( pos, line, x1, x2 );

    TxtCursor cur1, cur2;

    cur1.setKTextObject( ( KTextObject* )this );
    cur2.setKTextObject( ( KTextObject* )this );

    cur1.setPositionAbs( x1 );
    cur2.setPositionAbs( x2 );

    deleteRegion( &cur1, &cur2 );
    pos = _pos;
}

/*====================== delete word =============================*/
void KTextObject::deleteWordInPara( int pos, int para )
{
    _modified = true;

    int x1, x2;
    int _pos = pos;
    getAbsPosOfWordInPara( pos, para, x1, x2 );

    TxtCursor cur1, cur2;

    cur1.setKTextObject( ( KTextObject* )this );
    cur2.setKTextObject( ( KTextObject* )this );

    cur1.setPositionAbs( x1 );
    cur2.setPositionAbs( x2 );

    deleteRegion( &cur1, &cur2 );
    pos = _pos;
}

/*====================== delete word =============================*/
void KTextObject::deleteWord( int pos, int line, int para )
{
    _modified = true;

    int x1, x2;
    int _pos = pos;
    getAbsPosOfWord( pos, line, para, x1, x2 );

    TxtCursor cur1, cur2;

    cur1.setKTextObject( ( KTextObject* )this );
    cur2.setKTextObject( ( KTextObject* )this );

    cur1.setPositionAbs( x1 );
    cur2.setPositionAbs( x2 );

    deleteRegion( &cur1, &cur2 );
    pos = _pos;
}

/*====================== delete line =============================*/
void KTextObject::deleteLine( int line )
{
    _modified = true;

    int para;
    getPara( line, para );

    if ( para >= 0 && para < static_cast<int>( paragraphs() ) && line >= 0 &&
	 line < static_cast<int>( paragraphAt( para )->lines() ) )
	deleteLine( line, para );
}

/*====================== delete line =============================*/
void KTextObject::deleteLine( int line, int para )
{
    _modified = true;

    bool breakPara = true;

    if ( para < static_cast<int>( paragraphs() ) && line < static_cast<int>( paragraphAt( para )->lines() ) )
    {
	txtCursor->setMaxPosition(txtCursor->maxPosition() - paragraphAt(para)->lineAt(line)->lineLength());
	txtCursor->setPositionAbs( txtCursor->positionAbs() - paragraphAt( para )->lineAt( line )->lineLength() );
	paragraphAt( para )->deleteLine( line );
	if ( paragraphAt( para )->lines() == 0 )
	{
	    paragraphList.remove( para );
	    cellHeights.remove( para );
	    setAutoUpdate( false );
	    setNumRows( numRows() - 1 );
	    setAutoUpdate( true );
	    breakPara = false;
	}
	if ( breakPara )
	    changedParagraphs.append( ( int* )( para ) );
	recalc( false );
	changedParagraphs.clear();
	repaint( false );
    }
}

/*====================== delete paragraph ========================*/
void KTextObject::deleteParagraph( int para, bool _update )
{
    _modified = true;

    if ( para < static_cast<int>( paragraphs() ) )
    {
	txtCursor->setMaxPosition(txtCursor->maxPosition() - paragraphAt(para)->paragraphLength());
	txtCursor->setPositionAbs( txtCursor->positionAbs() - paragraphAt( para )->paragraphLength() );
	paragraphList.remove( para );
	cellHeights.remove( para );
	setAutoUpdate( false );
	setNumRows( numRows() - 1 );
	setAutoUpdate( true );
	if ( _update )
	{
	    recalc( false );
	    repaint( false );
	}
    }
}

/*====================== delete region ===========================*/
void KTextObject::deleteRegion( TxtCursor *_startCursor, TxtCursor *_stopCursor )
{
    _modified = true;

    // if we have only to delete some objs in one paragraph
    if ( _startCursor->positionParagraph() == _stopCursor->positionParagraph() ) {
	if ( _startCursor->positionLine() == 0 && _startCursor->positionInLine() == 0 &&
	     _stopCursor->positionLine() == paragraphAt( _startCursor->positionParagraph() )->lines() - 1 &&
	     _stopCursor->positionInLine() ==
	     paragraphAt( _startCursor->positionParagraph() )->lineAt( _stopCursor->positionLine() )->lineLength() -
	     1 ) {
	    deleteParagraph( _startCursor->positionParagraph(), false );
	    if ( paragraphs() == 0 ) clear();
	    return;
	}

	int pos = 0;
	for ( int i = 0; i < static_cast<int>( _startCursor->positionParagraph() ); i++ )
	    pos += paragraphAt( i )->paragraphLength();

	paragraphAt( _startCursor->positionParagraph() )->
	    deleteRegion( _startCursor->positionAbs() - pos, _stopCursor->positionAbs() - pos );

	recalc();
    } else { // if we have to delete objs in more paragraphs
	bool join = false;
	int start = _startCursor->positionParagraph();
	int stop = _stopCursor->positionParagraph();

	if ( _stopCursor->positionLine() == paragraphAt( _stopCursor->positionParagraph() )->lines() - 1 &&
	     _stopCursor->positionInLine() ==
	     paragraphAt( _stopCursor->positionParagraph() )->lineAt( _stopCursor->positionLine() )->
	     lineLength() - 1 ) {
	    deleteParagraph( _stopCursor->positionParagraph(), false );
	    join = false;
	} else {
	    int pos = 0;
	    for ( int i = 0; i < static_cast<int>( _stopCursor->positionParagraph() ); i++ )
		pos += paragraphAt( i )->paragraphLength();

	    paragraphAt( _stopCursor->positionParagraph() )->
		deleteRegion( 0, _stopCursor->positionAbs() - pos );

	    join = true;
	}

	recalc();

	if ( stop - start > 1 ) {
	    for ( int i = stop - 1; i > static_cast<int>( start ); i-- )
		deleteParagraph( i, false );
	    recalc();
	}

	if ( _startCursor->positionLine() == 0 && _startCursor->positionInLine() == 0 ) {
	    deleteParagraph( _startCursor->positionParagraph(), false );
	    join = false;
	} else {
	    int pos = 0;
	    for ( int i = 0; i < static_cast<int>( _startCursor->positionParagraph() ); i++ )
		pos += paragraphAt( i )->paragraphLength();

	    paragraphAt( _startCursor->positionParagraph() )->
		deleteRegion( _startCursor->positionAbs() - pos, -1 );
	}

	recalc();

	if ( join ) joinParagraphs( start, start + 1 );

	if ( paragraphs() == 0 ) clear();
    }

    txtCursor->setMaxPosition( textLength() );
    txtCursor->setPositionAbs( _startCursor->positionAbs() );
    int pos = QMAX( _startCursor->positionAbs(), _stopCursor->positionAbs() );
    if ( pos > textLength() - 1 )
	pos = textLength() - 1;
    txtCursor->setPositionAbs( pos );

    recalc();

    repaint( false );
}

/*===================== insert text ==============================*/
void KTextObject::insertText( QString text, int pos, QFont font, QColor color )
{
    _modified = true;

    int x1, x2;
    getAbsPosOfWord( pos, x1, x2 );

    TxtCursor cur;

    cur.setKTextObject( ( KTextObject* )this );

    cur.setPositionAbs( x1 );

    insertText( text, &cur, font, color );
}

/*===================== insert text ==============================*/
void KTextObject::insertTextInLine( QString text, int pos, int line, QFont font, QColor color )
{
    _modified = true;

    int x1, x2;
    getAbsPosOfWordInLine( pos, line, x1, x2 );

    TxtCursor cur;

    cur.setKTextObject( ( KTextObject* )this );

    cur.setPositionAbs( x1 );

    insertText( text, &cur, font, color );
}

/*===================== insert text ==============================*/
void KTextObject::insertTextInPara( QString text, int pos, int para, QFont font, QColor color )
{
    _modified = true;

    int x1, x2;
    getAbsPosOfWordInPara( pos, para, x1, x2 );

    TxtCursor cur;

    cur.setKTextObject( ( KTextObject* )this );

    cur.setPositionAbs( x1 );

    insertText( text, &cur, font, color );
}

/*===================== insert text ==============================*/
void KTextObject::insertText( QString text, int pos, int line, int para, QFont font, QColor color )
{
    _modified = true;

    int x1, x2;
    getAbsPosOfWord( pos, line, para, x1, x2 );

    TxtCursor cur;

    cur.setKTextObject( ( KTextObject* )this );

    cur.setPositionAbs( x1 );

    insertText( text, &cur, font, color );
}

/*===================== insert text ==============================*/
void KTextObject::insertText( QString text, TxtCursor *_cursor, QFont font, QColor color )
{
    _modified = true;

    if ( !text.isEmpty() )
    {
	if ( text.find( '\n' ) == -1 )
	{
	    bool _space = false;
	    if ( text.right( 1 ) == " " ) _space = true;

	    text = text.simplifyWhiteSpace();

	    if ( text.find( " " ) == -1 )
	    {
		QList<TxtObj> il;
		il.setAutoDelete( true );

		TxtObj *o = new TxtObj( text, font, color, TxtObj::NORMAL, TxtObj::TEXT );
		il.append( o );

		if ( _space )
		{
		    TxtObj *o = new TxtObj( " ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );
		    il.append( o );
		}

		insertItems( &il, _cursor );

		il.clear();
		return;
	    }
	}

	int ind = 0, _ind = 0;
	QString str;
	QList<TxtObj> il;
	il.setAutoDelete( true );
	bool spaceAppended = false;

	if ( text.right( 1 ) != " " )
	{
	    spaceAppended = true;
	    text.append( " " );
	}

	while ( true )
	{
	    _ind = text.find( QRegExp( "[ \x20\t\n ]" ), ind );

	    if ( _ind == -1 ) break;

	    str = text.mid( ind, _ind - ind );
	    str = str.stripWhiteSpace();

	    if ( !str.isEmpty() )
	    {
		TxtObj *o = new TxtObj( str, font, color, TxtObj::NORMAL, TxtObj::TEXT );
		il.append( o );
	    }

	    switch ( QChar( text[ _ind ] ) )
	    {
	    case ' ':
	    {
		if ( !( _ind == static_cast<int>( text.length() ) - 1 && spaceAppended ) )
		{
		    TxtObj *o = new TxtObj( " ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );
		    il.append( o );
		}

		insertItems( &il, _cursor, false );

		if ( !( _ind == static_cast<int>( text.length() ) - 1 && spaceAppended ) )
		    _cursor->setPositionAbs( _cursor->positionAbs() + str.length() + 1 );
		else
		    _cursor->setPositionAbs( _cursor->positionAbs() + str.length() );
	    } break;
	    case '\n':
	    {
		if ( !str.isEmpty() )
		    insertItems( &il, _cursor, false );
		_cursor->setPositionAbs( _cursor->positionAbs() + str.length() );

		TxtCursor *c = txtCursor;
		txtCursor = _cursor;
		splitParagraph();
		c->setMaxPosition(txtCursor->maxPosition());
		txtCursor = c;
	    } break;
	    case '\t':
	    {
		TxtObj *o = new TxtObj( "    ", font, color, TxtObj::NORMAL, TxtObj::SEPARATOR );
		il.append( o );
		insertItems( &il, _cursor, false );
		_cursor->setPositionAbs( _cursor->positionAbs() + str.length() + 4 );
	    } break;
	    }

	    il.clear();
	    ind = _ind + 1;

	}

	repaint( false );

    }
}

/*======================== insert items ==========================*/
void KTextObject::insertItems( QList<TxtObj> *items, int pos )
{
    _modified = true;

    int line, i, absPos = 0;
    getLine( pos, line );

    for ( i = 0; i < line; i++ )
	absPos += lineAt( i )->lineLength();

    for ( i = 0; i < pos; i++ )
	absPos += lineAt( line )->itemAt( i )->textLength();

    TxtCursor _cursor;
    _cursor.setKTextObject( ( KTextObject* )this );
    _cursor.setPositionAbs( absPos );
    insertItems( items, &_cursor );
}

/*======================== insert items ==========================*/
void KTextObject::insertItemsInLine( QList<TxtObj> *items, int pos, int line )
{
    _modified = true;

    int i, absPos = 0;

    for ( i = 0; i < line; i++ )
	absPos += lineAt( i )->lineLength();

    for ( i = 0; i < pos; i++ )
	absPos += lineAt( line )->itemAt( i )->textLength();

    TxtCursor _cursor;
    _cursor.setKTextObject( ( KTextObject* )this );
    _cursor.setPositionAbs( absPos );
    insertItems( items, &_cursor );
}

/*======================== insert items ==========================*/
void KTextObject::insertItemsInPara( QList<TxtObj> *items, int pos, int para )
{
    _modified = true;

    int i, absPos = 0;

    for ( i = 0; i < para; i++ )
	absPos += paragraphAt( i )->paragraphLength();

    for ( i = 0; i < pos; i++ )
	absPos += paragraphAt( para )->itemAt( i )->textLength();

    TxtCursor _cursor;
    _cursor.setKTextObject( ( KTextObject* )this );
    _cursor.setPositionAbs( absPos );
    insertItems( items, &_cursor );
}

/*======================== insert items ==========================*/
void KTextObject::insertItems( QList<TxtObj> *items, int pos, int line, int para )
{
    _modified = true;

    int i, absPos = 0;

    for ( i = 0; i < para; i++ )
	absPos += paragraphAt( i )->paragraphLength();

    for ( i = 0; i < line; i++ )
	absPos += paragraphAt( para )->lineAt( i )->lineLength();

    for ( i = 0; i < pos; i++ )
	absPos += paragraphAt( para )->lineAt( line )->itemAt( i )->textLength();

    TxtCursor _cursor;
    _cursor.setKTextObject( ( KTextObject* )this );
    _cursor.setPositionAbs( absPos );
    insertItems( items, &_cursor );
}

/*======================== insert items ==========================*/
void KTextObject::insertItems( QList<TxtObj> *items, TxtCursor *_cursor, bool redraw )
{
    _modified = true;

    TxtObj *item;

    int i, objPos, w = 0;
    int para = _cursor->positionParagraph();
    //int _h = cellHeight( para );

    lin = paragraphAt( para )->toOneLine();
    paragraphAt( para )->append( lin );
    _cursor->calcPos();
    int inLine = _cursor->positionInLine();
    lin = paragraphAt( para )->lineAt( 0 );

    changedParagraphs.append( ( int* )para );

    if ( !items->isEmpty() )
    {
	for ( item = items->last(); item != 0; item = items->prev() )
	{
	    switch ( item->type() )
	    {
	    case TxtObj::SEPARATOR:
	    {
		if ( lin->getInObj( inLine ) != -1 ) lin->splitObj( inLine );
		if ( lin->getAfterObj( inLine ) != -1 )
		    objPos = lin->getAfterObj( inLine )+1;
		else
		    objPos = lin->getBeforeObj( inLine );
		if ( static_cast<int>( objPos ) != -1 )
		{
		    obj = new TxtObj( item->text(), item->font(), item->color(), item->vertAlign(), item->type() );
		    lin->insert( objPos, obj );
		}
	    } break;
	    default:
	    {
		if ( lin->getInObj( inLine ) != -1 )
		{
		    if ( lin->itemAt( lin->getInObj( inLine ) )->color() == item->color() &&
			 lin->itemAt( lin->getInObj( inLine ) )->font() == item->font() )
		    {
			objPos = lin->getInObj( inLine );
			for ( i = 0; i < objPos; i++ )
			    w += lin->itemAt( i )->textLength();
			lin->itemAt( objPos )->insert( inLine-w, item->text() );
			break;
		    }
		    else
			lin->splitObj( inLine );
		}
		if ( lin->getAfterObj( inLine ) != -1 &&
		     lin->itemAt( lin->getAfterObj( inLine ) )->type() != TxtObj::SEPARATOR &&
		     ( lin->itemAt( lin->getAfterObj( inLine ) )->color() == item->color() &&
		       lin->itemAt( lin->getAfterObj( inLine ) )->font() == item->font() ) )
		{
		    objPos = lin->getAfterObj( inLine );
		    lin->itemAt( objPos )->append( item->text() );
		}
		else if ( lin->getBeforeObj( inLine ) != -1 &&
			  lin->itemAt( lin->getBeforeObj( inLine ) )->type() != TxtObj::SEPARATOR &&
			  ( lin->itemAt( lin->getBeforeObj( inLine ) )->color() == item->color() &&
			    lin->itemAt( lin->getBeforeObj( inLine ) )->font() == item->font() ) )
		{
		    objPos = lin->getBeforeObj( inLine );
		    lin->itemAt( objPos )->insert( 0, item->text() );
		}
		else
		{
		    if ( lin->getAfterObj( inLine ) != -1 )
			objPos = lin->getAfterObj( inLine )+1;
		    else
			objPos = lin->getBeforeObj( inLine );
		    if ( static_cast<int>( objPos ) != -1 )
		    {
			obj = new TxtObj( item->text(), item->font(), item->color(), TxtObj::NORMAL, TxtObj::TEXT );
			lin->insert( objPos, obj );
		    }
		}
	    } break;
	    }
	}
	// for test
	recalc( false );
	if ( redraw ) repaint( false );
    }
    txtCursor->setMaxPosition( textLength() );
}

/*==================== replace region ============================*/
void KTextObject::replaceItems( QList<TxtObj> *items, int pos, int len )
{
    _modified = true;

    for ( int i = 0; i < len; i++ )
	deleteItem( pos );

    insertItems( items, pos );
}

/*==================== replace items =============================*/
void KTextObject::replaceItemsInLine( QList<TxtObj> *items, int pos, int line, int len )
{
    _modified = true;

    for ( int i = 0; i < len; i++ )
	deleteItemInLine( pos, line );

    insertItemsInLine( items, pos, line );
}

/*==================== replace items =============================*/
void KTextObject::replaceItemsInPara( QList<TxtObj> *items, int pos, int para, int len )
{
    _modified = true;

    for ( int i = 0; i < len; i++ )
	deleteItemInPara( pos, para );

    insertItemsInPara( items, pos, para );
}

/*==================== replace items =============================*/
void KTextObject::replaceItems( QList<TxtObj> *items, int pos, int line, int para, int len )
{
    _modified = true;

    for ( int i = 0; i < len; i++ )
	deleteItem( pos, line, para );

    insertItems( items, pos, line, para );
}

/*==================== replace word ==============================*/
void KTextObject::replaceWord( QString text, int pos, QFont font, QColor color )
{
    _modified = true;

    if ( text.right( 1 ) != " " ) text.append( " " );
    insertText( text, pos, font, color );
    deleteWord( pos + 1 );
}

/*==================== replace word ==============================*/
void KTextObject::replaceWordInLine( QString text, int pos, int line, QFont font, QColor color )
{
    _modified = true;

    if ( text.right( 1 ) != " " ) text.append( " " );
    insertTextInLine( text, pos, line, font, color );
    deleteWordInLine( pos + 1, line );
}

/*==================== replace word ==============================*/
void KTextObject::replaceWordInPara( QString text, int pos, int para, QFont font, QColor color )
{
    _modified = true;

    if ( text.right( 1 ) != " " ) text.append( " " );
    insertTextInPara( text, pos, para, font, color );
    deleteWordInPara( pos + 1, para );
}

/*==================== replace word ==============================*/
void KTextObject::replaceWord( QString text, int pos, int line, int para, QFont font, QColor color )
{
    _modified = true;

    if ( text.right( 1 ) != " " ) text.append( " " );
    insertText( text, pos, line, para, font, color );
    deleteWord( pos + 1, line, para );
}

/*==================== replace region ============================*/
void KTextObject::replaceRegion( QList<TxtObj> *items, TxtCursor *_startCursor, TxtCursor *_stopCursor )
{
    _modified = true;

    deleteRegion( _startCursor, _stopCursor );
    insertItems( items, _startCursor );
}

/*==================== replace region ============================*/
void KTextObject::replaceRegion( QString text, TxtCursor *_startCursor, TxtCursor *_stopCursor, QFont font, QColor color )
{
    _modified = true;

    deleteRegion( _startCursor, _stopCursor );
    insertText( text, _startCursor, font, color );
}

/*================ change attributes of the region ===============*/
void KTextObject::changeRegionAttribs( TxtCursor *_startCursor, TxtCursor *_stopCursor, QFont font, QColor color )
{
    _modified = true;

    enum CurPos {C_IN, C_BEFORE, C_AFTER};
    int start_pos = 0, start_cpos = C_IN, i;
    int stop_pos = 0, stop_cpos = C_IN, objnum;
    int start, stop;

    for ( i = 0; i < static_cast<int>( _startCursor->positionParagraph() ); i++ )
	start_pos += paragraphAt( i )->items();

    for ( i = 0; i < static_cast<int>( _startCursor->positionLine() ); i++ )
	start_pos += paragraphAt( _startCursor->positionParagraph() )->lineAt( i )->items();

    objnum = paragraphAt( _startCursor->positionParagraph() )->lineAt( _startCursor->positionLine() )->
	     getInObj( _startCursor->positionInLine() );
    if ( objnum == -1 )
    {
	objnum = paragraphAt( _startCursor->positionParagraph() )->
		 lineAt( _startCursor->positionLine() )->getBeforeObj( _startCursor->positionInLine() );
	if ( objnum == -1 )
	{
	    objnum = paragraphAt( _startCursor->positionParagraph() )->
		     lineAt( _startCursor->positionLine() )->getAfterObj( _startCursor->positionInLine() );
	    if ( objnum == -1 )
		// something wrong here - let's exit!
		return;
	    else start_cpos = C_AFTER;
	}
	else start_cpos = C_BEFORE;

    }
    else start_cpos = C_IN;

    start_pos += objnum;

    for ( i = 0; i < static_cast<int>( _stopCursor->positionParagraph() ); i++ )
	stop_pos += paragraphAt( i )->items();

    for ( i = 0; i < static_cast<int>( _stopCursor->positionLine() ); i++ )
	stop_pos += paragraphAt( _stopCursor->positionParagraph() )->lineAt( i )->items();

    objnum = paragraphAt( _stopCursor->positionParagraph() )->lineAt( _stopCursor->positionLine() )->
	     getInObj( _stopCursor->positionInLine() );
    if ( objnum == -1 )
    {
	objnum = paragraphAt( _stopCursor->positionParagraph() )->
		 lineAt( _stopCursor->positionLine() )->getBeforeObj( _stopCursor->positionInLine() );
	if ( objnum == -1 )
	{
	    objnum = paragraphAt( _stopCursor->positionParagraph() )->
		     lineAt( _stopCursor->positionLine() )->getAfterObj( _stopCursor->positionInLine() );
	    if ( objnum == -1 )
		// something wrong here - let's exit!!
		return;
	    else stop_cpos = C_AFTER;
	}
	else stop_cpos = C_BEFORE;

    }
    else stop_cpos = C_IN;

    stop_pos += objnum;
    if ( stop_cpos == C_AFTER ) stop_pos++;

    if ( start_cpos == C_IN )
    {
	paragraphAt( _startCursor->positionParagraph() )->lineAt( _startCursor->positionLine() )->
	    splitObj( _startCursor->positionInLine() );
	start_pos++;
	start_cpos = C_BEFORE;
	stop_pos++;
    }

    if ( stop_cpos == C_IN )
    {
	paragraphAt( _stopCursor->positionParagraph() )->lineAt( _stopCursor->positionLine() )->
	    splitObj( _stopCursor->positionInLine() );
	//stop_cpos = C_BEFORE;
    }

    if ( start_cpos == C_AFTER ) start = start_pos + 1;
    else start = start_pos;

    if ( stop_cpos == C_AFTER ) stop = stop_pos + 1;
    else if ( stop_cpos == C_BEFORE ) stop = stop_pos - 1;
    else stop = stop_pos;

    for ( i = start; i <= stop; i++ )
    {
	if ( i < static_cast<int>( items() ) )
	{
	    itemAt( i )->setFont( font );
	    itemAt( i )->setColor( color );
	}
    }

    redrawSelection( *_startCursor, *_stopCursor );

    recalc( TRUE );

    int pos = txtCursor->positionAbs();
    txtCursor->setPositionAbs( pos );
    pos = _startCursor->positionAbs();
    _startCursor->setPositionAbs( pos );
    pos = _stopCursor->positionAbs();
    _stopCursor->setPositionAbs( pos );

    repaint( FALSE );
}

/*=============== changen alignment of the region ================*/
void KTextObject::changeRegionAlign( TxtCursor *_startCursor, TxtCursor *_stopCursor, TxtParagraph::HorzAlign _align )
{
    _modified = true;

    int start_para = _startCursor->positionParagraph();
    int stop_para = _stopCursor->positionParagraph(), i;

    for ( i = start_para; i <= stop_para; i++ )
	setHorzAlign( _align, i );
}

/*================= get position of a word =======================*/
void KTextObject::getAbsPosOfWord( int pos, int &x1, int &x2 )
{
    x1 = x2 = 0;

    int line = 0, _pos = pos, i, ind;

    line = lines() - 1;
    for ( i = 0; i < static_cast<int>( lines() ); i++ )
    {
	_pos -= lineAt( i )->words() - 1;
	if ( _pos <= 0 )
	{
	    line = i;
	    break;
	}
    }


    QString word = wordAt( pos, ind );
    QString _line = lineAt( line )->getText();

    for ( i = 0; i < line; i++ )
    {
	x1 += lineAt( i )->lineLength();
	pos -= lineAt( i )->words();
    }

    x1 += ind;

    x2 = x1 + word.length();
}

/*================= get position of a word =======================*/
void KTextObject::getAbsPosOfWordInLine( int pos, int line, int &x1, int &x2 )
{
    x1 = x2 = 0;

    int i, ind;

    QString word = wordAtLine( pos, line, ind );
    QString _line = lineAt( line )->getText();

    for ( i = 0; i < line; i++ )
    {
	x1 += lineAt( i )->lineLength();
	pos -= lineAt( i )->words();
    }

    x1 += ind; //_line.find( word );

    x2 = x1 + word.length();
}

/*================= get position of a word =======================*/
void KTextObject::getAbsPosOfWordInPara( int pos, int para, int &x1, int &x2 )
{
    x1 = x2 = 0;

    int i, line = 0, _pos = pos, ind;

    for ( i = 0; i < para; i++ )
	x1 += paragraphAt( i )->paragraphLength();

    line = paragraphAt( para )->lines() - 1;
    for ( i = 0; i < static_cast<int>( paragraphAt( para )->lines() ); i++ )
    {
	_pos -= paragraphAt( para )->lineAt( i )->words() - 1;
	if ( _pos <= 0 )
	{
	    line = i;
	    break;
	}
    }

    for ( i = 0; i < line; i++ )
    {
	x1 += paragraphAt( para )->lineAt( i )->lineLength();
	pos -= paragraphAt( para )->lineAt( i )->words();
    }

    QString word = wordAt( pos, line, para, ind );
    QString _line = paragraphAt( para )->lineAt( line )->getText();

    x1 += ind; //_line.find( word );

    x2 = x1 + word.length();
}

/*================= get position of a word =======================*/
void KTextObject::getAbsPosOfWord( int pos, int line, int para, int &x1, int &x2 )
{
    x1 = x2 = 0;

    int i, __line = 0, ___line = line, ind;

    for ( i = 0; i < para; i++ )
    {
	x1 += paragraphAt( i )->paragraphLength();
	__line += paragraphAt( i )->lines() - 1;
    }

    line += __line;

    for ( i = 0; i < line; i++ )
	x1 += lineAt( i )->lineLength();

    QString word = wordAt( pos, ___line, para, ind );
    QString _line = lineAt( line )->getText();

    x1 += ind; //_line.find( word );

    x2 = x1 + word.length();
}

/*========================= get line ============================*/
void KTextObject::getLine( int &pos, int &line )
{
    line = -1;

    if ( pos < static_cast<int>( items() ) )
    {
	int _item = 0;

	for ( int i = 0; i < static_cast<int>( lines() ); i++ )
	{
	    if ( pos < static_cast<int>( _item ) + static_cast<int>( lineAt( i )->items() ) )
	    {
		pos -= _item;
		line = i;
		return;
	    }
	    else _item += lineAt( i )->items();
	}
    }
}

/*========================= get line in paragraph ===============*/
void KTextObject::getLine( int &pos, int para, int &line )
{
    line = -1;

    if ( para < static_cast<int>( paragraphs() ) )
    {
	int _item = 0;

	for ( int i = 0; i < static_cast<int>( paragraphAt( para )->lines() ); i++ )
	{
	    if ( pos < static_cast<int>( _item ) + static_cast<int>( paragraphAt( para )->lineAt( i )->items() ) )
	    {
		pos -= _item;
		line = i;
		return;
	    }
	    else _item += paragraphAt( para )->lineAt( i )->items();
	}
    }
}

/*========================= get paragraph =======================*/
void KTextObject::getPara( int &line, int &para )
{
    para = -1;

    if ( line < static_cast<int>( lines() ) )
    {
	int _line = 0;

	for ( int i = 0; i < static_cast<int>( paragraphs() ); i++ )
	{
	    if ( line < static_cast<int>( _line ) + static_cast<int>( paragraphAt( i )->lines() ) )
	    {
		para = i;
		line -= _line;
		return;
	    }
	    else _line += paragraphAt( i )->lines();
	}
    }
}

/*========================= get paragraph =======================*/
void KTextObject::getPara( int &pos, int &line, int &para )
{
    getLine( pos, line );
    getPara( line, para );
}

/*========================= operator = ==========================*/
KTextObject& KTextObject::operator=( KTextObject &txtObj )
{
    clear( false );

    setObjType( txtObj.objType() );
    setLineBreak( txtObj.getLineBreakWidth() );
    setEnumListType( txtObj.enumListType() );
    setUnsortListType( txtObj.unsortListType() );
    setShowCursor( txtObj.showCursor() );
    toggleModified( txtObj.isModified() );

    resize( size() );

    for ( int i = 0; i < static_cast<int>( txtObj.paragraphs() ); i++ )
    {
	para1 = txtObj.paragraphAt( i );
	para2 = addParagraph();
	para2->setHorzAlign( para1->horzAlign() );

	for ( int j = 0; j < static_cast<int>( para1->lines() ); j++ )
	{
	    lin = para1->lineAt( j );
	    linePtr = new TxtLine();

	    for ( int k = 0; k < static_cast<int>( lin->items() ); k++ )
	    {
		obj = lin->itemAt( k );
		objPtr = new TxtObj( obj->text(), obj->font(), obj->color(), obj->vertAlign(), obj->type() );
		objPtr->setOrigSize( obj->origSize() );

		linePtr->append( objPtr );
	    }

	    para2->append( linePtr );
	}
    }

    txtCursor->setPositionAbs( txtObj.getTxtCursor()->positionAbs() );
    recalc();

    return *this;
}

/*====================== search first ============================*/
bool KTextObject::searchFirst( QString text, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    searchIndexFrom.setPositionAbs( txtCursor->positionAbs() );
    searchIndexTo.setPositionAbs( txtCursor->positionAbs() );

    return searchNext( text, from, to, caseSensitive );
}

/*====================== search next =============================*/
bool KTextObject::searchNext( QString text, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    QString contents = toASCII( false, false );
    contents.replace( QRegExp( "\n" ), "" );

    if ( !contents.isEmpty() )
    {
	int index = contents.find( text, searchIndexTo.positionAbs(), caseSensitive );
	if ( index == -1 ) return false;

	cursorChanged = true;

	searchIndexFrom.setPositionAbs( index );
	searchIndexTo.setPositionAbs( index + text.length() );

	from->setPositionAbs( index );
	to->setPositionAbs( index + text.length() );

	if ( drawSelection )
	{
	    drawSelection = false;
	    redrawSelection( startCursor, stopCursor );
	}

	startCursor.setPositionAbs( txtCursor->positionAbs() - 1 );
	stopCursor.setPositionAbs( txtCursor->positionAbs() + 1 );
	txtCursor->setPositionAbs( index );
	redrawSelection( startCursor, stopCursor );

	startCursor.setPositionAbs( index );
	stopCursor.setPositionAbs( index + text.length() );

	drawSelection = true;
	redrawSelection( startCursor, stopCursor );

	if ( ( tableFlags() & Tbl_vScrollBar || tableFlags() & Tbl_hScrollBar ) &&
	     !rowIsVisible( txtCursor->positionParagraph() ) )
	    setTopCell( txtCursor->positionParagraph() );

	return true;
    }

    return false;
}

/*====================== search first reverse ====================*/
bool KTextObject::searchFirstRev( QString text, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    searchIndexFrom.setPositionAbs( txtCursor->positionAbs() );
    searchIndexTo.setPositionAbs( txtCursor->positionAbs() );

    return searchNextRev( text, from, to, caseSensitive );
}

/*====================== search next reverse =====================*/
bool KTextObject::searchNextRev( QString text, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    QString contents = toASCII( false, false );
    contents.replace( QRegExp( "\n" ), "" );

    if ( !contents.isEmpty() )
    {
	int index = contents.findRev( text, searchIndexFrom.positionAbs() - 1, caseSensitive );
	if ( index == -1 ) return false;

	cursorChanged = true;

	searchIndexFrom.setPositionAbs( index );
	searchIndexTo.setPositionAbs( index + text.length() );

	from->setPositionAbs( index );
	to->setPositionAbs( index + text.length() );

	if ( drawSelection )
	{
	    drawSelection = false;
	    redrawSelection( startCursor, stopCursor );
	}

	startCursor.setPositionAbs( txtCursor->positionAbs() - 1 );
	stopCursor.setPositionAbs( txtCursor->positionAbs() + 1 );
	txtCursor->setPositionAbs( index );
	redrawSelection( startCursor, stopCursor );

	startCursor.setPositionAbs( index );
	stopCursor.setPositionAbs( index + text.length() );

	drawSelection = true;
	redrawSelection( startCursor, stopCursor );

	if ( ( tableFlags() & Tbl_vScrollBar || tableFlags() & Tbl_hScrollBar ) &&
	     !rowIsVisible( txtCursor->positionParagraph() ) )
	    setTopCell( txtCursor->positionParagraph() );

	return true;
    }

    return false;
}

/*====================== replace first ===========================*/
bool KTextObject::replaceFirst( QString search, QString replace, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    _modified = true;

    searchIndexFrom.setPositionAbs( txtCursor->positionAbs() );
    searchIndexTo.setPositionAbs( txtCursor->positionAbs() );

    return replaceNext( search, replace, from, to, caseSensitive );
}

/*====================== replace next ============================*/
bool KTextObject::replaceNext( QString search, QString replace, TxtCursor *from, TxtCursor *to, bool caseSensitive )
{
    _modified = true;

    bool found = searchNext( search, from, to, caseSensitive );

    if ( found )
    {
	cutRegion();
	insertText( replace, txtCursor, currFont, currColor );
	to->setPositionAbs( from->positionAbs() + replace.length() );

	if ( drawSelection )
	{
	    drawSelection = false;
	    redrawSelection( startCursor, stopCursor );
	}

	txtCursor->setPositionAbs( from->positionAbs() );

	startCursor.setPositionAbs( from->positionAbs() );
	stopCursor.setPositionAbs( to->positionAbs() );

	drawSelection = true;
	redrawSelection( startCursor, stopCursor );
    }

    return found;
}

/*====================== replace first reverse ===================*/
bool KTextObject::replaceFirstRev( QString search, QString replace, TxtCursor *from, TxtCursor *to,
				   bool caseSensitive )
{
    _modified = true;

    searchIndexFrom.setPositionAbs( txtCursor->positionAbs() );
    searchIndexTo.setPositionAbs( txtCursor->positionAbs() );

    return replaceNextRev( search, replace, from, to, caseSensitive );
}

/*====================== replace next reverse ====================*/
bool KTextObject::replaceNextRev( QString search, QString replace, TxtCursor *from, TxtCursor *to,
				  bool caseSensitive )
{
    _modified = true;

    bool found = searchNextRev( search, from, to, caseSensitive );

    if ( found )
    {
	cutRegion();
	insertText( replace, txtCursor, currFont, currColor );
	to->setPositionAbs( from->positionAbs() + replace.length() );

	if ( drawSelection )
	{
	    drawSelection = false;
	    redrawSelection( startCursor, stopCursor );
	}

	txtCursor->setPositionAbs( from->positionAbs() );

	startCursor.setPositionAbs( from->positionAbs() );
	stopCursor.setPositionAbs( to->positionAbs() );

	drawSelection = true;
	redrawSelection( startCursor, stopCursor );
    }

    return found;
}

/*================================================================*/
void KTextObject::setFontToAll( QFont _font )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
    {
	for ( int j = 0; j < paragraphList.at( i )->lines(); j++ )
	{
	    for ( int k = 0; k < paragraphList.at( i )->lineAt( j )->items(); k++ )
	    {
		paragraphList.at( i )->lineAt( j )->itemAt( k )->setFont( _font );
	    }
	}
    }
    recalc();
}

/*================================================================*/
void KTextObject::setColorToAll( QColor _color )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
    {
	for ( int j = 0; j < paragraphList.at( i )->lines(); j++ )
	{
	    for ( int k = 0; k < paragraphList.at( i )->lineAt( j )->items(); k++ )
	    {
		paragraphList.at( i )->lineAt( j )->itemAt( k )->setColor( _color );
	    }
	}
    }
    recalc();
}

/*================================================================*/
void KTextObject::setHorzAlignToAll( TxtParagraph::HorzAlign _align )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
    {
	paragraphList.at( i )->setHorzAlign( _align );
    }
    recalc();
}

/*====================== paint cell ==============================*/
void KTextObject::paintCell( class QPainter* painter, int row, int )
{
    if ( !drawPic && !rowIsVisible( row ) ) return;

    //debug( "Redraw Pargraph: %d", row );

    paragraphPtr2 = 0;
    paragraphPtr = 0;
    int x = xstart + getLeftIndent( row ), y = ystart;
    int w = cellWidth( 0 ); //, h = cellHeight( row );
    int i = 0, j = 0, chars = 0, len = 0;
    bool drawCursor = false, cursorDrawn = false;
    QPoint c1, c2;
    int scrBar = 0, wid = 0;
    char chr[ 11 ];
    int ry;
    QPixmap pix;
    QPainter *p = 0;
    QFontMetrics fm( font() );

    // get pointer to the paragraph, which should be drwan
    paragraphPtr = paragraphList.at( row );

    // if the paragraph exists, draw it
    if ( paragraphPtr ) {

	if ( drawPic ) p = painter;
	else p = new QPainter();


	// object type
	switch ( obType ) {
	case PLAIN: case TABLE: break;
	case ENUM_LIST: {
	    if ( !drawPic ) {
		pix.resize( xstart + getLeftIndent( row ), paragraphPtr->height() );
		pix.fill( backgroundColor() );
		p->begin( &pix );
	    }

	    if ( objEnumListType.type == NUMBER )
		sprintf( chr, "%s%d%s", objEnumListType.before.data(), getParagNum( paragraphPtr ) +
			 objEnumListType.start,
			 objEnumListType.after.data() );
	    else
		sprintf( chr, "%s%c%s", objEnumListType.before.data(), getParagNum( paragraphPtr ) +
			 objEnumListType.start,
			 objEnumListType.after.data() );
	    p->setFont( objEnumListType.font );
	    fm = QFontMetrics( p->font() );
	    if ( !allInOneColor )
		p->setPen( objEnumListType.color );
	    else
		p->setPen( allColor );
	    if ( !paragraphPtr->isEmpty() )
		p->drawText( 0, ( !drawPic ? 0 : y ) + paragraphPtr->lineAt( 0 )->ascent( paragraphPtr ) -
			     fm.ascent(),
			     xstart + getLeftIndent( row ), fm.height() + paragraphPtr->getLineSpacing(),
			     AlignLeft, chr );

	    if ( !drawPic ) {
		p->end();
		painter->drawPixmap( 0, y, pix );
	    }
	} break;
	case UNSORT_LIST: {
	    if ( !drawPic ) {
		pix.resize( xstart + getLeftIndent( row ), paragraphPtr->height() + paragraphPtr->getLineSpacing() );
		pix.fill( backgroundColor() );
		p->begin( &pix );
	    }

	    p->setFont( *objUnsortListType.font->at( paragraphPtr->getDepth() ) );
	    fm = QFontMetrics( p->font() );
	    if ( !allInOneColor )
		p->setPen( *objUnsortListType.color->at( paragraphPtr->getDepth() ) );
	    else
		p->setPen( allColor );
	    QChar _chr = *objUnsortListType.chr->at( paragraphPtr->getDepth() );
	    if ( !paragraphPtr->isEmpty() )
		p->drawText( 0 + getLeftIndent( row ), ( !drawPic ? 0 : y ) + paragraphPtr->lineAt( 0 )->
			     ascent( paragraphPtr ) - fm.ascent(),
			     xstart, fm.height() + paragraphPtr->getLineSpacing(), AlignLeft, _chr );

	    if ( !drawPic ) {
		p->end();
		painter->drawPixmap( 0, y, pix );
	    }
	} break;
	}

	// draw lines
	for ( i = 0; i < paragraphPtr->lines(); i++ ) {
	    linePtr = paragraphPtr->lineAt( i );
	    x = !drawPic ? 0 : xstart + getLeftIndent( row );

	    // check, if the line should be drawn
	    if ( drawLine == -1 || drawParagraph == -1 ||
		 !drawBelow && drawLine == static_cast<int>( i ) && drawParagraph == row ||
		 drawBelow && ( row > drawParagraph ||
				row == drawParagraph && static_cast<int>( i ) >= drawLine ) ) {
		if ( !drawPic ) {
		    if ( w > 0 && linePtr->height( paragraphPtr ) > 0 ) {
			pix.resize( w - getLeftIndent( row ), linePtr->height( paragraphPtr ) );
			pix.fill( backgroundColor() );
			p->begin( &pix );
		    }
		}

		// alignment
		switch ( paragraphPtr->horzAlign() ) {
		case TxtParagraph::LEFT: w -= scrBar; break;
		case TxtParagraph::CENTER: x += ( w - linePtr->width() ) / 2 - scrBar / 2; break;
		case TxtParagraph::RIGHT: x += w - linePtr->width() - scrBar; break;
		default: break;
		}

		// draw all objects of the line
		wid = 0;
		for ( j = 0; j < linePtr->items(); wid += linePtr->itemAt( j )->textLength(), j++ ) {
		    objPtr = linePtr->itemAt( j );
		    len = objPtr->textLength();
		    p->setFont( objPtr->font() );
		    fm = QFontMetrics( p->font() );

		    // check, if cursor should be drawn - if yes calculate it
		    if ( drawCursor = !drawPic && !cursorDrawn && showCursor() &&
			 static_cast<int>( txtCursor->positionParagraph() ) == row &&
			 txtCursor->positionLine() == i && txtCursor->positionInLine() >= chars &&
			 txtCursor->positionInLine() <= chars+len ) {
			c1.setX( x + fm.width( objPtr->text().left( txtCursor->positionInLine() - chars ) ) );
			c1.setY( !drawPic ? 0 : y );
			c2.setX( c1.x() );
			c2.setY( ( !drawPic ? 0 : y ) + linePtr->height( paragraphPtr ) );
		    }

		    // draw Text
		    if ( !allInOneColor )
			p->setPen( objPtr->color() );
		    else
			p->setPen( allColor );

		    //debug( "%d", x );
		    if ( drawPic && objPtr->text().contains( "\\pn" ) ) {
			QString s = objPtr->text();
			s.replace( QRegExp( "\\pn" ), QString::number( currPageNum ) );
			s.remove( s.find( "\\" ), 1 );
			p->drawText( x, y + linePtr->ascent( paragraphPtr ), s );
		    } else
			p->drawText( x, (!drawPic ? 0 : y) + linePtr->ascent( paragraphPtr ), objPtr->text() );

		    // draw Cursor
		    if ( drawCursor ) {
			if ( cursorChanged ) {
			    emit fontChanged( ( QFont* )&p->font() );
			    emit colorChanged( ( QColor* )&p->pen().color() );
			    emit horzAlignChanged( paragraphPtr->horzAlign() );
			    cursorChanged = false;
			    currFont = p->font();
			    currColor = p->pen().color();
			}
			txtCursor->setXPos( c1.x() );
			rowYPos( row, &ry );
			txtCursor->setYPos( y + ry );
			txtCursor->setHeight( c2.y() - c1.y() );
			p->setPen( QPen( Qt::black, 1, SolidLine ) );
			if ( p->font().italic() )
			    c1.setX( c1.x() + static_cast<int>( static_cast<float>( linePtr->height( paragraphPtr ) )
								/ 3.732 ) );
			p->drawLine( c1, c2 );
			cursorDrawn = true;
		    }

		    // draw selection
		    if ( selectionInObj( row, i, j ) ) {
			RasterOp ro = p->rasterOp();
			p->setRasterOp( NotROP );
			int sx, sw;
			bool select_full = selectFull( row, i, j, sx, sw );
			if ( select_full )
			    p->fillRect( x, ( !drawPic ? 0 : y ), objPtr->width(), linePtr->height( paragraphPtr ),
					 Qt::black );
			else
			    p->fillRect( x + sx, ( !drawPic ? 0 : y ), sw, linePtr->height( paragraphPtr ),
					 Qt::black );
			p->setRasterOp( ro );
		    }
		    if ( drawPic && objPtr->text() == "\\pn" ) {
			QString s = objPtr->text();
			s.replace( QRegExp( "\\pn" ), QString::number( currPageNum ) );
			s.remove( s.find( "\\" ), 1 );
			x += QFontMetrics( objPtr->font() ).width( s );
		    } else
			x += objPtr->width();
		    chars += len;
		}

		if ( !drawPic && p->isActive() ) {
		    p->end();
		    painter->drawPixmap( xstart + getLeftIndent( row ), y, pix );
		}
	    }

	    // calculate coordinates for the next line
	    y += linePtr->height( paragraphPtr );
	    x = xstart + getLeftIndent( row );
	    chars = 0;
	}

	if ( !drawPic ) {
	    delete p;
	    //delete pix;
	}
    }
}

/*===================== paint event ==============================*/
void KTextObject::paintEvent( QPaintEvent *e )
{
    QPainter p;

    //debug( "REPAINT EVENT - doRepaints = %d", doRepaints );

    // call superclass methode
    if ( doRepaints )
	QTableView::paintEvent( e );

    // erase unused space
    if ( totalHeight() < height() && !drawPic ) {
	p.begin( this );
	p.setPen( NoPen );
	p.setBrush( backgroundColor() );
	p.drawRect( 0, totalHeight(), width(), height()-totalHeight() );
	p.end();
    }
}

/*====================== focus in event ==========================*/
void KTextObject::focusInEvent( QFocusEvent* )
{
//   setShowCursor( true );
//   if ( txtCursor )
//     updateCell( txtCursor->positionParagraph(), 0, false );
//   debug( "focus in" );
}

/*===================== focus out event ==========================*/
void KTextObject::focusOutEvent( QFocusEvent* )
{
//   setShowCursor( true );
//   if ( txtCursor )
//     updateCell( txtCursor->positionParagraph(), 0, false );
//   debug( "focus out" );
}

/*======================= leave event ============================*/
void KTextObject::leaveEvent( QEvent* )
{
    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
	copyRegion();
}

/*======================= enter event ============================*/
void KTextObject::enterEvent( QEvent* )
{
    //if ( autoFocus ) setFocus();
}

/*====================== return cell width =======================*/
int KTextObject::cellWidth( int i )
{
    if ( i < numCols() )
	return cellWidths.at( i )->wh - gap;
    else return 0;
}

/*====================== return cell height ======================*/
int KTextObject::cellHeight( int i )
{
    if ( i < numRows() && i < static_cast<int>( cellHeights.count() ) )
	return cellHeights.at( i )->wh;
    else return 0;
}

/*====================== return total width ======================*/
int KTextObject::totalWidth()
{
    int w = 0;
    int i;

    for ( i = 0, cwhPtr = cellWidths.first(); cwhPtr != 0; cwhPtr = cellWidths.next(), i++ )
	w += cellWidth( i );

    return w;
}

/*====================== return total height =====================*/
int KTextObject::totalHeight()
{
    int h = 0;
    int i;

    for ( i = 0, cwhPtr = cellHeights.first(); cwhPtr != 0; cwhPtr = cellHeights.next(), i++ )
	h += cellHeight( i );

    return h;
}

/*====================== resize event ============================*/
void KTextObject::resizeEvent( QResizeEvent* e )
{
    setAutoUpdate( false );
    resize( e->size() );
    recalc();
    QTableView::resizeEvent( e );
    setAutoUpdate( true );
}

/*====================== key press event =========================*/
void KTextObject::keyPressEvent( QKeyEvent* e )
{
    //debug( "keyPressEvent begin" );
    if ( showCursor() ) {
	int i = 0;
	bool drawAbove = false;
	changedParagraphs.clear();
	bool drawFullPara = false;
	bool doDelete = true;

	if ( drawSelection ) {
	    if ( e->key() == Key_Return || e->key() == Key_Enter || e->key() == Key_Delete
		 || e->key() == Key_Backspace || e->ascii() && e->ascii() > 31 ) {
		int posAbs = QMIN( startCursor.positionAbs(), stopCursor.positionAbs() );
		int parag = QMIN( startCursor.positionParagraph(), stopCursor.positionParagraph() );
		int line = 0;
		if ( startCursor.positionParagraph() < stopCursor.positionParagraph() )
		    line = startCursor.positionLine();
		else
		    line = stopCursor.positionLine();
		cutRegion();
		_modified = true;
		txtCursor->setPositionAbs( posAbs );
		txtCursor->calcPos();
		if ( txtCursor->positionLine() != line ||
		    txtCursor->positionParagraph() != parag ) {
		    int i = 0;
		    if ( txtCursor->positionLine() > line ) {
			while ( txtCursor->positionLine() > line || txtCursor->positionParagraph() > parag ) {
			    i++;
			    txtCursor->charBackward();
			    if ( i > 10 )
				break;
			}
		    } else {
			while ( txtCursor->positionLine() < line || txtCursor->positionParagraph() > parag ) {
			    i++;
			    txtCursor->charForward();
			    if ( i > 10 )
				break;
			}
		    }
		    insertChar( QChar( ' ' ) );
		    cursorChanged = TRUE;
		    drawFullPara = TRUE;
		    drawAbove = TRUE;
		}
		txtCursor->calcPos();
		doDelete = false;
	    }
	    if ( e->key() != Key_Shift && e->key() != Key_Control && e->key() != Key_Alt &&
		 !( e->key() == Key_Right && e->state() & ShiftButton ) &&
		 !( e->key() == Key_Left && e->state() & ShiftButton ) &&
		 !( e->key() == Key_Up && e->state() & ShiftButton ) &&
		 !( e->key() == Key_Down && e->state() & ShiftButton ) ) {
		if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
		    copyRegion();
		drawSelection = false;
		redrawSelection( startCursor, stopCursor );
		startCursor.setPositionAbs( 0 );
		stopCursor.setPositionAbs( 0 );
	    }
	}

	// always update the maximal cursor position when a key is pressed
	//txtCursor->setMaxPosition( textLength() );

	TxtCursor *oldCursor = new TxtCursor( ( KTextObject* )this );
	//oldCursor->setPositionAbs( txtCursor->positionAbs() );
	oldCursor->copy( *txtCursor );

	changedParagraphs.clear();

	// react on the pressed key
	switch ( e->key() ) {
	case Key_Right: {
	    if ( e->state() & ControlButton )
		txtCursor->wordForward();
	    else
		txtCursor->charForward();

	    if ( e->state() & ShiftButton )
		selectText( oldCursor, C_RIGHT );

	    cursorChanged = true;
	} break;
	case Key_Left: {
	    if ( e->state() & ControlButton )
		txtCursor->wordBackward();
	    else
		txtCursor->charBackward();

	    if ( e->state() & ShiftButton )
		selectText( oldCursor, C_LEFT );

	    cursorChanged = true;
	} break;
	case Key_Up: {
	    txtCursor->lineUp();

	    if ( e->state() & ShiftButton )
		selectText( oldCursor, C_UP );

	    cursorChanged = true;
	} break;
	case Key_Down: {
	    txtCursor->lineDown();

	    if ( e->state() & ShiftButton )
		selectText( oldCursor, C_DOWN );

	    cursorChanged = true;
	} break;
	case Key_Return: case Key_Enter: {
	    _modified = true;
	    splitParagraph();
	    drawBelow = true;
	    drawAbove = true;
	    cursorChanged = true;
	} break;
	case Key_Backspace: {
	    if ( doDelete ) {
		_modified = true;
		if ( kbackspace() )
		    drawBelow = true;
		else {
		    drawBelow = false;
		    drawFullPara = true;
		}
	    }
	    cursorChanged = true;
	} break;
	case Key_Delete: {
	    if ( doDelete ) {
		_modified = true;
		if ( kdelete() )
		    drawBelow = true;
		else {
		    drawBelow = false;
		    drawFullPara = true;
		}
	    }
	    cursorChanged = true;
	} break;
	default: {
	    if ( !e->text().isEmpty() ) {
		_modified = true;
		for ( unsigned int i = 0; i < e->text().length(); i++ ) {
		    if ( insertChar( e->text()[ i ] ) )
			drawBelow = true;
		    else {
			drawBelow = false;
			drawFullPara = true;
		    }
		    cursorChanged = true;
		}
	    }
	};
	}

	// if only one line has changed, redraw it
	if ( oldCursor->positionParagraph() == txtCursor->positionParagraph() &&
	     oldCursor->positionLine() == txtCursor->positionLine() ) {
	    drawParagraph = txtCursor->positionParagraph();
	    drawLine = txtCursor->positionLine();
	    if ( drawFullPara ) drawLine = -1;
	    updateCell( txtCursor->positionParagraph(), 0, false );
	    drawLine = -1;
	    drawParagraph = -1;
	} else { // else redraw both lines
	    drawParagraph = txtCursor->positionParagraph();
	    drawLine = txtCursor->positionLine();
	    if ( drawFullPara ) drawLine = -1;
	    updateCell( txtCursor->positionParagraph(), 0, false );
	    drawParagraph = oldCursor->positionParagraph();
	    drawLine = oldCursor->positionLine();
	    if ( drawFullPara ) drawLine = -1;
	    updateCell( oldCursor->positionParagraph(), 0, false );
	    drawLine = -1;
	    drawParagraph = -1;
	}

	// if a line and everything below should be drawn
	if ( drawBelow ) {
	    //oldCursor->calcPos();
	    TxtCursor *minCursor = txtCursor->minCursor(oldCursor);
	    drawParagraph = minCursor->positionParagraph();
	    drawLine = minCursor->positionLine();
	    for ( i = drawParagraph; i < paragraphs(); i++ )
		updateCell( i, 0, false );
	    i = drawParagraph;
	    drawLine = -1;
	    drawParagraph = -1;
	    drawBelow = false;
	}

	if ( drawAbove ) {
	    drawLine = -1;
	    drawParagraph = -1;
	    updateCell( i, 0, false );
	}

	if ( tableFlags() & Tbl_vScrollBar || tableFlags() & Tbl_hScrollBar )
	    makeCursorVisible();

	changedParagraphs.clear();
	delete oldCursor;
    }
    //debug( "keyPressEvent end" );
}

/*====================== mouse press event ========================*/
void KTextObject::mousePressEvent( QMouseEvent *e )
{
    //setFocus();

    emit giveMeFocus();

    if ( e->button() != RightButton ) {
	drawSelection = false;

	redrawSelection( startCursor, stopCursor );
	startCursor.setPositionAbs( 0 );
	stopCursor.setPositionAbs( 0 );
    }

    switch ( e->button() ) {
    case LeftButton: {
	bool dummy;
	mousePressed = true;
	startCursor = getCursorPos( e->x(), e->y(), dummy, true, true );
	stopCursor = getCursorPos( e->x(), e->y(), dummy, true, true );
    } break;
    case MidButton: {
	bool dummy;
	getCursorPos( e->x(), e->y(), dummy, true, true );
	mousePressed = false;
	paste();
    } break;
    case RightButton: {
	mousePressed = false;

	if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() ) {
	    rbMenu->setItemEnabled( CB_CUT, true );
	    rbMenu->setItemEnabled( CB_COPY, true );
	} else {
	    rbMenu->setItemEnabled( CB_CUT, false );
	    rbMenu->setItemEnabled( CB_COPY, false );
	}

	QClipboard *cb = QApplication::clipboard();
	if ( cb->text() )
	    rbMenu->setItemEnabled( CB_PASTE, true );
	else
	    rbMenu->setItemEnabled( CB_PASTE, false );

	QPoint pnt;
	pnt = QCursor::pos();
	rbMenu->popup( pnt );
    } break;
    default: break;
    }
}

/*====================== mouse release event ======================*/
void KTextObject::mouseReleaseEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
	mousePressed = false;
	bool dummy;

	TxtCursor cursor = getCursorPos( e->x(), e->y(), dummy, true, false );
	if ( cursor.positionAbs() > startCursor.positionAbs() )
	    stopCursor = cursor;
	else
	    startCursor = cursor;

	if ( stopCursor.positionAbs() != startCursor.positionAbs() ) {
	    drawSelection = true;
	    copyRegion();
	}

	redrawSelection( startCursor, stopCursor );
    }

    mousePressed = false;
}

/*====================== mouse move event =========================*/
void KTextObject::mouseMoveEvent( QMouseEvent *e )
{
    if ( !hasFocus() )
	emit giveMeFocus();

    if ( mousePressed )
    {
	TxtCursor _startCursor = startCursor;
	TxtCursor _stopCursor = stopCursor;
	TxtCursor cursor;

	bool dummy;
	cursor = getCursorPos( e->x(), e->y(), dummy, true, false );

	if ( cursor.positionAbs() > startCursor.positionAbs() )
	    stopCursor = cursor;
	else
	    startCursor = cursor;

	TxtCursor c1 = *startCursor.minCursor(&stopCursor);
	TxtCursor c2 = *startCursor.maxCursor(&stopCursor);

	startCursor = c1;
	stopCursor = c2;

	if ( stopCursor.positionAbs() != startCursor.positionAbs() )
	    drawSelection = true;

	redrawSelection( startCursor, stopCursor );

	if ( _startCursor.positionAbs() < startCursor.positionAbs() )
	    redrawSelection( _startCursor, startCursor );

	if ( _stopCursor.positionAbs() > stopCursor.positionAbs() )
	    redrawSelection( stopCursor, _stopCursor );
    }
}

/*================================================================*/
void KTextObject::mouseDoubleClickEvent( QMouseEvent * )
{
    TxtCursor *c1 = new TxtCursor( this );
    *c1 = *txtCursor;
    TxtCursor *c2 = new TxtCursor( this );
    *c2 = *txtCursor;

    c1->wordBackward();
    c2->wordForward();
    startCursor = *c1;
    stopCursor = *c2;

    if ( stopCursor.positionAbs() != startCursor.positionAbs() )
	drawSelection = true;

    recalc();
    repaint( FALSE );
    redrawSelection( startCursor, stopCursor );

    delete c1;
    delete c2;
}

/*=================== recalcualte everything =====================*/
void KTextObject::recalc( bool breakAllLines )
{
    switch ( obType ) {
    case PLAIN: xstart = 0; break;
    case ENUM_LIST: {
	QFontMetrics fm( objEnumListType.font );
	char chr[ 12 ];
	sprintf( chr, "%s99 %s", objEnumListType.before.data(), objEnumListType.after.data() );
	xstart = fm.width( chr );
    } break;
    case UNSORT_LIST: {
	QFontMetrics fm( *objUnsortListType.font->at( 0 ) );
	xstart = fm.width( *objUnsortListType.chr->at( 0 ) ) + fm.width( *objUnsortListType.chr->at( 0 ) ) / 3;
    } break;
    default: break;
    }

    cellWidths.at( 0 )->wh = width() - xstart;
    if ( tableFlags() & Tbl_vScrollBar ) cellWidths.at( 0 )->wh -= verticalScrollBar()->width();

    bool breakLines = true;
    if ( !breakAllLines )
	breakLines = changedParagraphs.count() > 0 ? true : false;
    else breakLines = breakAllLines;

    //debug( "break lines begin" );
    for ( paragraphPtr = paragraphList.first(); paragraphPtr != 0; paragraphPtr = paragraphList.next() ) {
	// break the lines, and resize the cell
	if ( breakAllLines || breakLines && changedParagraphs.containsRef( ( int* )( paragraphList.at() ) ) ) {
	    //debug( "break line: %d", paragraphList.at() );
	    if ( regexpMode )
		paragraphPtr->setRegExpList( &regExpList );

	    if ( linebreak_width < 1 ) {
		paragraphPtr->breakLines( cellWidth( 0 ) - getLeftIndent( paragraphList.at() ),
					  regexpMode, composerMode );
		cellWidths.at( 0 )->wh = width() - xstart;
		if ( tableFlags() & Tbl_vScrollBar ) cellWidths.at( 0 )->wh -= verticalScrollBar()->width();
	    } else {
		paragraphPtr->break_Lines( linebreak_width - getLeftIndent( paragraphList.at() ),
					   regexpMode, composerMode );
		_width = max(_width,static_cast<int>(paragraphPtr->width()));
		cellWidths.at( 0 )->wh = _width;
	    }

	}

	cellHeights.at( paragraphList.at() )->wh = paragraphPtr->height();
    }
    //debug( "break lines end" );

    // calculate the new cursorposition
    txtCursor->calcPos();
    changedParagraphs.clear();

    _modified = TRUE;
}

/*====================== split paragraph =========================*/
void KTextObject::splitParagraph()
{
    _modified = true;

    para1 = new TxtParagraph();
    para2 = new TxtParagraph();
    para3 = new TxtParagraph();

    para1->setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() );
    para2->setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() );
    para3->setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() );
    para1->setLeftIndent( paragraphList.at( txtCursor->positionParagraph() )->getLeftIndent() );
    para2->setLeftIndent( paragraphList.at( txtCursor->positionParagraph() )->getLeftIndent() );
    para3->setLeftIndent( paragraphList.at( txtCursor->positionParagraph() )->getLeftIndent() );
    para1->setLineSpacing( paragraphList.at( txtCursor->positionParagraph() )->getLineSpacing() );
    para1->setDistBefore( paragraphList.at( txtCursor->positionParagraph() )->getDistBefore() );
    para1->setDistAfter( paragraphList.at( txtCursor->positionParagraph() )->getDistAfter() );
    para2->setLineSpacing( paragraphList.at( txtCursor->positionParagraph() )->getLineSpacing() );
    para2->setDistBefore( paragraphList.at( txtCursor->positionParagraph() )->getDistBefore() );
    para2->setDistAfter( paragraphList.at( txtCursor->positionParagraph() )->getDistAfter() );
    para3->setLineSpacing( paragraphList.at( txtCursor->positionParagraph() )->getLineSpacing() );
    para3->setDistBefore( paragraphList.at( txtCursor->positionParagraph() )->getDistBefore() );
    para3->setDistAfter( paragraphList.at( txtCursor->positionParagraph() )->getDistAfter() );

    int i;
    int para = txtCursor->positionParagraph();
    int line = txtCursor->positionLine();
    int p1 = para+1, p2 = para+2;
    obj = 0;
    TxtParagraph::HorzAlign ha;
    bool do_key_delete = false;

    changedParagraphs.append( ( int* )( para ) );

    // if the cursor is at the beginning of a line
    if ( txtCursor->positionInLine() == 0 ) {
	// if the cursor is in the first line -> just append an empty paragraph below
	if ( line == 0 ) {
	    lin = new TxtLine();
	    lin->append( " ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR );
	    para1->append( lin );
	    paragraphList.insert( para, para1 );

	    delete para2;
	    delete para3;
	} else { // if the cursor is in another line
	    // 1. paragraph: text before the line, in which the cursor is
	    for ( i = 0; i < static_cast<int>( line ); i++ )
		para1->append( paragraphAt( para )->lineAt( i ) );

	    // 2. paragraph: empty ( new ) pragraph
	    lin = new TxtLine();
	    lin->append( "  ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR );
	    para2->append( lin );

	    // 3. paragraph: text after the line in which the cursor is
	    for ( i = line; i < static_cast<int>( paragraphAt( para )->lines() ); i++ )
		para3->append( paragraphAt( para )->lineAt( i ) );

	    // remove original paragraph
	    paragraphList.take( para );

	    // insert new paragraphs
	    if ( para3->lines() > 0 && para3->lineAt( 0 )->items() > 0 )
		paragraphList.insert( para, para3 );
	    if ( para2->lines() > 0 && para2->lineAt( 0 )->items() > 0 )
		paragraphList.insert( para, para2 );
	    if ( para1->lines() > 0 && para1->lineAt( 0 )->items() > 0 )
		paragraphList.insert( para, para1 );

	    changedParagraphs.append( ( int* )( p1 ) );
	    changedParagraphs.append( ( int* )( p2 ) );

	    // insert the new row into the table
	    wh = new CellWidthHeight;
	    wh->wh = 0;
	    cellHeights.append( wh );
	    setAutoUpdate( false );
	    setNumRows( numRows()+1 );
	    setAutoUpdate( true );
	}

	// move cursor forward
	txtCursor->setMaxPosition( textLength() );
	txtCursor->charForward();
    } else if ( txtCursor->positionInLine() == paragraphAt( para )->lineAt( txtCursor->positionLine() )->lineLength()-1 ) {
	// if the cursor is at the end of a line
	
	// if the cursor is in the last line, just insert an empty paragraph below
	if ( line == paragraphAt( para )->lines()-1 ) {
	    lin = new TxtLine();
	    lin->append( "  ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR );
	    para1->setHorzAlign( paragraphAt( para )->horzAlign() );
	    para1->append( lin );
	    paragraphList.insert( para+1, para1 );
	    changedParagraphs.append( ( int* )( p1 ) );
	    delete para2;
	    delete para3;
	    do_key_delete = true;
	} else { // if the cursor is in another line
	    // 1. paragraph: text before the line, in which the cursor is, and text of the line in which the cursor is
	    for ( i = 0; i <= static_cast<int>( line ); i++ )
		para1->append( paragraphAt( para )->lineAt( i ) );

	    // 2. paragraph: empty ( new ) pragraph
	    lin = new TxtLine();
	    lin->append( "  ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR );
	    para2->append( lin );

	    // 3. paragraph: text after the line in which the cursor is
	    for ( i = line+1; i < static_cast<int>( paragraphAt( para )->lines() ); i++ )
		para3->append( paragraphAt( para )->lineAt( i ) );

	    // remove original paragraph
	    ha = paragraphAt( para )->horzAlign();
	    paragraphList.take( para );

	    // insert new paragraphs
	    para1->setHorzAlign( ha );
	    para2->setHorzAlign( ha );
	    para3->setHorzAlign( ha );
	    paragraphList.insert( para, para3 );
	    paragraphList.insert( para, para2 );
	    paragraphList.insert( para, para1 );
	    changedParagraphs.append( ( int* )( p1 ) );
	    changedParagraphs.append( ( int* )( p2 ) );

	    // insert the new row into the table
	    wh = new CellWidthHeight;
	    wh->wh = 0;
	    cellHeights.append( wh );
	    setAutoUpdate( false );
	    setNumRows( numRows()+1 );
	    setAutoUpdate( true );
	}

	// move cursor forward
	txtCursor->setMaxPosition( textLength() );
	txtCursor->charForward();
    } else { // if the cursor is anywhere else in the line
	// create two empty paragraphs
	lin = new TxtLine();
	para1->append( lin );
	lin = new TxtLine();
	para2->append( lin );

	// concat the paragraph to one line and recalc the cursorposition
	lin = paragraphAt( para )->toOneLine();
	paragraphAt( para )->append( lin );
	txtCursor->calcPos();
	int inLine = txtCursor->positionInLine();

	// if the cursor is in an object, split it
	if ( lin->getInObj( inLine ) != -1 ) lin->splitObj( inLine );

	// remeber the object ( type ) of the object, which is in front of the cursor
	if ( lin->getAfterObj( inLine ) != -1 )
	    obj = lin->itemAt( lin->getAfterObj( inLine ) );

	// insert the obejcts, which are in front of the cursor, into the first paragraph
	for ( i = 0; i <= lin->getAfterObj( inLine ); i++ )
	    para1->append( lin->itemAt( i ) );

	para1->append( new TxtObj( " ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR ) );
	
	// insert the objects, which are after the cursor, into the second paragraph
	for ( i = lin->getBeforeObj( inLine ); i < static_cast<int>( lin->items() ); i++ )
	    para2->append( lin->itemAt( i ) );

	// remove the original paragraph
	ha = paragraphAt( para )->horzAlign();
	paragraphList.take( para );

	// insert the two new paragraphs
	para1->setHorzAlign( ha );
	para2->setHorzAlign( ha );
	if ( para2->lines() > 0 && para2->lineAt( 0 )->items() > 0 )
	    paragraphList.insert( para, para2 );
	if ( para1->lines() > 0 && para1->lineAt( 0 )->items() > 0 )
	    paragraphList.insert( para, para1 );
	changedParagraphs.append( ( int* )( p1 ) );
	delete para3;

	// if the remembered object ( type ) is not a separator -> move cursor one position forward
	if ( obj && obj->type() != TxtObj::SEPARATOR )
	    txtCursor->charForward();
    }

    // insert the new row into the table
    wh = new CellWidthHeight;
    wh->wh = 0;
    cellHeights.append( wh );
    setAutoUpdate( false );
    setNumRows( numRows()+1 );
    setAutoUpdate( true );

    // recalculate everything
    recalc( false );

    if ( do_key_delete ) kdelete();
}

/*======================= join paragraph =========================*/
void KTextObject::joinParagraphs( int p1, int p2 )
{
    if ( p1 < 0 || p2 < 0 || p1 > paragraphs() - 1 || p2 > paragraphs() -1 ) {
	qWarning( "KTextObject::joinParagraphs(): p1 or p2 out of range" );
	return;
    }

    if ( txtCursor->positionParagraph() > paragraphs() - 1 ) {
	qWarning( "KTextObject::joinParagraphs(): txtCursor->positionParagraph() out of range" );
	return;
    }

    _modified = true;

    int para1n = min(p1,p2);
    int para2n = max(p1,p2);
    lin = new TxtLine();
    para1 = new TxtParagraph();
    para1->append( lin );
    para1->setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() );
    para1->setLeftIndent( paragraphList.at( txtCursor->positionParagraph() )->getLeftIndent() );
    para1->setLineSpacing( paragraphList.at( txtCursor->positionParagraph() )->getLineSpacing() );
    para1->setDistBefore( paragraphList.at( txtCursor->positionParagraph() )->getDistBefore() );
    para1->setDistAfter( paragraphList.at( txtCursor->positionParagraph() )->getDistAfter() );

    lin = paragraphAt( para1n )->toOneLine();
    linePtr = paragraphAt( para2n )->toOneLine();
    lin->deleteItem( lin->items()-1 );
    lin->operator+=( linePtr );

    para1->setHorzAlign( paragraphAt( para1n )->horzAlign() );

    paragraphList.remove( para2n );
    paragraphList.remove( para1n );

    para1->append( lin );
    paragraphList.insert( para1n, para1 );

    cellHeights.remove( para2n );
    setAutoUpdate( false );
    setNumRows( numRows()-1 );
    setAutoUpdate( true );
}

/*======================= key backspace ==========================*/
bool KTextObject::kbackspace()
{
    _modified = true;

    //int i;
    int para = txtCursor->positionParagraph();
    //int line = txtCursor->positionLine();

    changedParagraphs.append( ( int* )( para ) );
    int tmp = para-1;
    changedParagraphs.append( ( int* )( tmp ) );
    tmp = para+1;
    changedParagraphs.append( ( int* )( tmp ) );

    int _h = cellHeight( para );

    // if the cursor is at the first position of a paragraph -> this and the upper paragraph have to be joined
    if ( txtCursor->positionLine() == 0 && txtCursor->positionInLine() == 0 && para > 0 )
    {
	joinParagraphs( para, para-1 );
	_h = -1;
    }
    // if the cursor is elsewhere
    else
    {
	// concat the paragraph to one line and recalc the cursorposition
	lin = paragraphAt( para )->toOneLine();
	paragraphAt( para )->append( lin );
	txtCursor->calcPos();
	int inLine = txtCursor->positionInLine();
	lin = paragraphAt( para )->lineAt( 0 );

	// delete the char in front of the cursor
	if ( lin->getInObj( inLine ) != -1 ) lin->backspaceChar( inLine );
	else if ( lin->getAfterObj( inLine ) != -1 ) lin->backspaceLastChar( lin->getAfterObj( inLine ) );

	// if the paragraph is empty now, delete it
	if ( lin->lineLength() == 0 )
	{
	    paragraphList.remove( para );
	    cellHeights.remove( para );
	    setAutoUpdate( false );
	    setNumRows( numRows()-1 );
	    setAutoUpdate( true );
	    _h = -1;
	}
    }

    // move cursor backward
    txtCursor->setMaxPosition( textLength() );
    txtCursor->charBackward();

    // recalculate everything
    recalc( false );

    if ( _h != cellHeight( para ) || _h == -1 ) return true;
    return false;
}

/*========================= key delete ===========================*/
bool KTextObject::kdelete( bool _recalc )
{
    //int i;
    int para = txtCursor->positionParagraph();
    int line = txtCursor->positionLine();

    changedParagraphs.append( ( int* )( para ) );
    int tmp = para-1;
    changedParagraphs.append( ( int* )( tmp ) );
    tmp = para+1;
    changedParagraphs.append( ( int* )( tmp ) );

    int _h = cellHeight( para );

    // if the cursor is at the last position of a paragraph -> this and the lower paragraph have to be joined
    if ( txtCursor->positionLine() == paragraphAt( para )->lines()-1 &&
	 txtCursor->positionInLine() == paragraphAt( para )->lineAt( line )->lineLength()-1 )
    {
	if ( para < paragraphs()-1 ) joinParagraphs( para, para+1 );
	_h = -1;
    }

    // if the cursor is elsewhere
    else
    {
	// concat the paragraph to one line and recalc the cursorposition
	lin = paragraphAt( para )->toOneLine();
	paragraphAt( para )->append( lin );
	txtCursor->calcPos();
	int inLine = txtCursor->positionInLine();
	lin = paragraphAt( para )->lineAt( 0 );

      // delete the char behind the cursor
	if ( lin->getInObj( inLine ) != -1 ) lin->deleteChar( inLine );
	else if ( lin->getBeforeObj( inLine ) != -1 ) lin->deleteFirstChar( lin->getBeforeObj( inLine ) );

      // if the paragraph is empty now, delete it
	if ( lin->lineLength() == 0 )
	{
	    paragraphList.remove( para );
	    cellHeights.remove( para );
	    setAutoUpdate( false );
	    setNumRows( numRows()-1 );
	    setAutoUpdate( true );
	    _h = -1;
	}
    }

    txtCursor->setMaxPosition( textLength() );

  // recalculate everything
    if ( _recalc )
	recalc( false );

    if ( _h != cellHeight( para ) || _h == -1 ) return true;
    return false;
}

/*========================= insert char ==========================*/
bool KTextObject::insertChar( QChar c )
{
    _modified = true;

    int i, objPos, w = 0;
    int para = txtCursor->positionParagraph();
    QString str;
    str = "";
    bool insertC = true;
    int cursorPlus = 0;

    if ( !autoReplace.isEmpty() ) {
	AutoReplace *ar;
	for ( ar = autoReplace.first(); ar != 0; ar = autoReplace.next() ) {
	    if ( ar->c == c ) {
		str.insert( 0, ar->replace );
		insertC = false;
		cursorPlus = ar->replace.length() - 1;
		break;
	    }
	}
    }

    if ( insertC )
	str.insert( 0, c );

    int _h = cellHeight( para );

    lin = paragraphAt( para )->toOneLine();
    paragraphAt( para )->append( lin );
    txtCursor->calcPos();
    int inLine = txtCursor->positionInLine();
    lin = paragraphAt( para )->lineAt( 0 );

    changedParagraphs.append( ( int* )( para ) );

    switch ( c ) {
    case ' ': {
	if ( lin->getInObj( inLine ) != -1 )
	    lin->splitObj( inLine );
	if ( lin->getAfterObj( inLine ) != -1 )
	    objPos = lin->getAfterObj( inLine )+1;
	else
	    objPos = lin->getBeforeObj( inLine );
	if ( static_cast<int>( objPos ) != -1 ) {
	    obj = new TxtObj( " ", currFont, currColor, TxtObj::NORMAL, TxtObj::SEPARATOR );
	    lin->insert( objPos, obj );
	}
    } break;
    default: {
	if ( lin->getInObj( inLine ) != -1 ) {
	    if ( sameEffects( lin->itemAt( lin->getInObj( inLine ) ) ) ) {
		objPos = lin->getInObj( inLine );
		for ( i = 0; i < objPos; i++ )
		    w += lin->itemAt( i )->textLength();
		lin->itemAt( objPos )->insert( inLine-w, str.data() );
		break;
	    } else
		lin->splitObj( inLine );
	}
	if ( lin->getAfterObj( inLine ) != -1 &&
	     lin->itemAt( lin->getAfterObj( inLine ) )->type() != TxtObj::SEPARATOR &&
	     sameEffects( lin->itemAt( lin->getAfterObj( inLine ) ) ) ) {
	    objPos = lin->getAfterObj( inLine );
	    lin->itemAt( objPos )->append( str.data() );
	} else if ( lin->getBeforeObj( inLine ) != -1 &&
		  lin->itemAt( lin->getBeforeObj( inLine ) )->type() != TxtObj::SEPARATOR &&
		  sameEffects( lin->itemAt( lin->getBeforeObj( inLine ) ) ) ) {
	    objPos = lin->getBeforeObj( inLine );
	    lin->itemAt( objPos )->insert( 0, str.data() );
	} else {
	    if ( lin->getAfterObj( inLine ) != -1 )
		objPos = lin->getAfterObj( inLine )+1;
	    else
		objPos = lin->getBeforeObj( inLine );
	    if ( static_cast<int>( objPos ) != -1 ) {
		obj = new TxtObj( str.data(), currFont, currColor, TxtObj::NORMAL, TxtObj::TEXT );
		lin->insert( objPos, obj );
	    }
	}
    } break;
    }

    txtCursor->setMaxPosition( textLength() );
    txtCursor->charForward();

    if ( cursorPlus > 0 )
	txtCursor->setPositionAbs( txtCursor->positionAbs() + cursorPlus );

    // recalculate everything
    recalc( false );

    if ( _h != cellHeight( para ) ) return true;
    return false;
}

/*====================== make cursor  vsisble ====================*/
bool KTextObject::makeCursorVisible()
{
    bool _update = false;

    if ( static_cast<int>( height() ) - 16 <= static_cast<int>( txtCursor->ypos() ) +
	 static_cast<int>( txtCursor->height() ) )
    {
	_update = true;
	setYOffset( ( static_cast<int>( yOffset() ) + static_cast<int>( txtCursor->height() ) <
		      static_cast<int>( totalHeight() ) - static_cast<int>( height() ) + 16 ?
		      yOffset() + txtCursor->height() : totalHeight() - height() + 16 ) );
	if ( yOffset() == totalHeight() - height() + 16 ) _update = false;
    }

    if ( !_update && txtCursor->ypos() <= 0 )
    {
	_update = true;
	setYOffset( ( yOffset() - txtCursor->height() > 0 ?
		      yOffset() - txtCursor->height() : 0 ) );
	if ( yOffset() == 0 ) _update = false;
    }

    if ( _update )
    {
	updateTableSize();
	updateScrollBars();
	_update = false;
    }

    if ( xOffset() + width() - 16 < txtCursor->xpos() )
    {
	setXOffset( txtCursor->xpos() - xOffset() );
	_update = true;
    }

    if ( !_update && xOffset() > txtCursor->xpos() )
    {
	setXOffset( yOffset() - txtCursor->xpos() );
	_update = true;
    }

    if ( _update )
    {
	updateTableSize();
	updateScrollBars();
	_update = false;
    }

    return _update;
}

/*====================== set cursor psoition ====================*/
TxtCursor KTextObject::getCursorPos( int _x, int _y, bool &changed, bool set, bool redraw )
{
    QFontMetrics fm( currFont );
    int x = _x + xOffset() - fm.width( 'x' ) / 2;
    int y = _y + yOffset(), i, h = 0, w = 0, para, line, objPos, pos, absPos = 0; ;
    TxtCursor tmpCursor( ( KTextObject* )this );

    para = paragraphs() - 1;
    for ( i = 0; i < static_cast<int>( paragraphs() ); i++ )
    {
	if ( y >= h && y <= h + static_cast<int>( paragraphAt( i )->height() ) )
	{
	    para = i;
	    break;
	}
	h += paragraphAt( i )->height();
    }

    line = paragraphAt( para )->lines() - 1;
    for ( i = 0; i < static_cast<int>( paragraphAt( para )->lines() ); i++ )
    {
	if ( y >= h && y <= h + static_cast<int>( paragraphAt( para )->lineAt( i )->height( paragraphPtr ) ) )
	{
	    line = i;
	    break;
	}
	h += paragraphAt( para )->lineAt( i )->height( paragraphPtr );
    }

    paragraphPtr = paragraphAt( para );
    linePtr = paragraphPtr->lineAt( line );
    int scrBar = 0;
    if ( tableFlags() & Tbl_vScrollBar ) scrBar = 16;
    x -= ( xstart + getLeftIndent( para ) );
    switch ( paragraphPtr->horzAlign() )
    {
    case TxtParagraph::LEFT: break;
    case TxtParagraph::CENTER: x -= ( cellWidth( 0 ) - linePtr->width() ) / 2 - scrBar / 2; break;
    case TxtParagraph::RIGHT: x -= cellWidth( 0 ) - linePtr->width() - scrBar; break;
    default: break;
    }

    int x2 = x >= 0 ? x : 0;

    objPos = paragraphAt( para )->lineAt( line )->items() - 1;
    for ( i = 0; i < static_cast<int>( paragraphAt( para )->lineAt( line )->items() ); i++ )
    {
	if ( x2 >= w && x2 <= w + static_cast<int>( paragraphAt( para )->lineAt( line )->itemAt( i )->width() ) )
	{
	    objPos = i;
	    break;
	}
	w += paragraphAt( para )->lineAt( line )->itemAt( i )->width();
    }

    pos = paragraphAt( para )->lineAt( line )->itemAt( objPos )->getPos( x - w );
    pos++;

    for ( i = 0; i < para; i++ )
	absPos += paragraphAt( i )->paragraphLength();
    for ( i = 0; i < line; i++ )
	absPos += paragraphAt( para )->lineAt( i )->lineLength();
    for ( i = 0; i < objPos; i++ )
	absPos += paragraphAt( para )->lineAt( line )->itemAt( i )->textLength();
    absPos += pos;

    if ( set )
    {
	tmpCursor.setPositionAbs( txtCursor->positionAbs() );

	txtCursor->setPositionAbs( absPos );
	cursorChanged = true;
	if ( tableFlags() & Tbl_vScrollBar || tableFlags() & Tbl_hScrollBar )
	    changed = makeCursorVisible();
	else
	    changed = false;

	if ( redraw )
	{
	    drawLine = tmpCursor.positionLine();
	    drawParagraph = tmpCursor.positionParagraph();
	    updateCell( drawParagraph, 0, false );
	    drawLine = txtCursor->positionLine();
	    drawParagraph = txtCursor->positionParagraph();
	    updateCell( drawParagraph, 0, false );
	}

	drawParagraph = -1;
	drawLine = -1;
    }

    tmpCursor.setMaxPosition(txtCursor->maxPosition());
    tmpCursor.setPositionAbs( absPos );

    return tmpCursor;
}

/*====================== convert color to hex-string ============*/
QString KTextObject::toHexString( QColor c )
{
    int r, g, b;
    QString str;

    c.rgb( &r, &g, &b );

    str.sprintf( "#%02X%02X%02X", r, g, b );

    return str;
}

/*====================== convert hex-string to color ============*/
QColor KTextObject::hexStringToQColor( QString str )
{
    //int r, g, b;

    QColor c( str );

    return c;
}

/*============ checks, if a string contains chars ===============*/
bool KTextObject::isValid( QString str )
{
    if ( str.length() == 0 ) return false;

    str = str.simplifyWhiteSpace();
    if ( str.length() == 0 ) return false;

    for ( int i = 0; i < static_cast<int>( str.length() ); i++ )
	if ( str.mid( i, 1 ) != " " ) return true;

    return false;
}

/*=================== simplify string ===========================*/
QString KTextObject::simplify( QString text )
{
    // should be rewritten
    bool hasSpace = ( text.right( 1 ) == " " );
    text = text.simplifyWhiteSpace();
    if ( hasSpace ) text += " ";

    return text;
}

/*============== check if selection is in a object ==============*/
bool KTextObject::selectionInObj( int para, int line, int item )
{
    if ( drawSelection && !drawPic )
    {
	bool start = false, stop = false;

	if ( static_cast<int>( startCursor.positionParagraph() ) < para ||
	     static_cast<int>( startCursor.positionParagraph() ) == para
	     && static_cast<int>( startCursor.positionLine() ) < line ) start = true;
	else if ( static_cast<int>( startCursor.positionParagraph() ) == para &&
		  static_cast<int>( startCursor.positionLine() ) == line )
	{
	    int startInLine = startCursor.positionInLine();
	    int startItem;

	    startItem = paragraphAt( para )->lineAt( line )->getInObj( startInLine );
	    if ( startItem == -1 )
	    {
		startItem = paragraphAt( para )->lineAt( line )->getBeforeObj( startInLine );
		if ( startItem != -1 );
		else startItem = paragraphAt( para )->lineAt( line )->getAfterObj( startInLine );
	    }

	    if ( startItem <= item ) start = true;
	}

	if ( static_cast<int>( stopCursor.positionParagraph() ) > para ||
	     static_cast<int>( stopCursor.positionParagraph() ) == para
	     && static_cast<int>( stopCursor.positionLine() ) > line ) stop = true;
	else if ( static_cast<int>( stopCursor.positionParagraph() ) == para &&
		  static_cast<int>( stopCursor.positionLine() ) == line )
	{
	    int stopInLine = stopCursor.positionInLine();
	    int stopItem;

	    stopItem = paragraphAt( para )->lineAt( line )->getInObj( stopInLine );
	    if ( stopItem == -1 )
	    {
		stopItem = paragraphAt( para )->lineAt( line )->getBeforeObj( stopInLine );
		if ( stopItem != -1 ) stopItem--;
		else stopItem = paragraphAt( para )->lineAt( line )->getAfterObj( stopInLine );
	    }

	    if ( stopItem >= item ) stop = true;
	}

	return ( start && stop );
    }
    else return false;
}

/*===================== check if select full ====================*/
bool KTextObject::selectFull( int para, int line, int item, int &sx, int &sw )
{
    sx = 0;
    sw = paragraphAt( para )->lineAt( line )->itemAt( item )->width();
    bool select_full = true;
    int i, wid;

    if ( static_cast<int>( startCursor.positionParagraph() ) == para &&
	 static_cast<int>( startCursor.positionLine() ) == line )
    {
	int startInLine = startCursor.positionInLine();
	int startItem = paragraphAt( para )->lineAt( line )->getInObj( startInLine );
	if ( startItem != -1 )
	{
	    wid = 0;
	    linePtr =  paragraphAt( para )->lineAt( line );
	    for ( i = 0; i < item; i++ )
	    {
		wid += linePtr->itemAt( i )->textLength();
		if ( wid > startInLine ) break;
	    }

	    if ( wid < startInLine )
	    {
		wid = startInLine - wid;
		QFontMetrics fm( linePtr->itemAt( item )->font() );
		sx = fm.width( linePtr->itemAt( item )->text().left( wid ) );
		sw -= sx;
		select_full = false;
	    }
	}
    }

    if ( static_cast<int>( stopCursor.positionParagraph() ) == para &&
	 static_cast<int>( stopCursor.positionLine() ) == line )
    {
	wid = 0;
	int stopInLine = stopCursor.positionInLine();
	int stopItem = paragraphAt( para )->lineAt( line )->getInObj( stopInLine );
	if ( stopItem != -1 )
	{
	    linePtr =  paragraphAt( para )->lineAt( line );
	    for ( i = 0; i < item; i++ )
	    {
		wid += linePtr->itemAt( i )->textLength();
		if ( wid > stopInLine ) break;
	    }

	    if ( wid < stopInLine )
	    {
		wid = stopInLine - wid;
		QFontMetrics fm( linePtr->itemAt( item )->font() );
		sw = fm.width( linePtr->itemAt( item )->text().left( wid ) );
		if ( sx != 0 ) sw -= sx;
		select_full = false;
	    }
	}
    }

    return select_full;
}

/*====================== redraw selection =======================*/
void KTextObject::redrawSelection( TxtCursor _startCursor, TxtCursor _stopCursor )
{
    // should be rewritten - this is too slow
    drawLine = -1;
    drawParagraph = -1;
    for ( int i = static_cast<int>( _startCursor.positionParagraph() );
	  i <= static_cast<int>( _stopCursor.positionParagraph() ); i++ )
	updateCell( i, 0, false );
}

/*======================= change attributes ====================*/
void KTextObject::changeAttribs()
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
	changeRegionAttribs( &startCursor, &stopCursor, currFont, currColor );
}

/*===================== change align =============================*/
void KTextObject::changeHorzAlign( TxtParagraph::HorzAlign ha, int p = -1 )
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() && p == -1 )
	changeRegionAlign( &startCursor, &stopCursor, ha );
    else
	_setHorzAlign( ha, p );
}

/*==================== set horizontal alignemnt ==================*/
void KTextObject::_setHorzAlign( TxtParagraph::HorzAlign ha, int p = -1 )
{
    _modified = true;

    if ( p == -1 ) p = txtCursor->positionParagraph();

    paragraphAt( p )->setHorzAlign( ha );
    updateCell( p, 0, false );
}

/*====================== create rb-menu ==========================*/
void KTextObject::createRBMenu()
{
    rbMenu = new QPopupMenu();
    CHECK_PTR( rbMenu );
    CB_CUT = rbMenu->insertItem( BarIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    CB_COPY = rbMenu->insertItem( BarIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    CB_PASTE = rbMenu->insertItem( BarIcon("editpaste"), i18n( "&Paste" ), this, SLOT( clipPaste() ) );
    rbMenu->setMouseTracking( true );
}

/*====================== select text =============================*/
void KTextObject::selectText( TxtCursor *oldCursor, CursorDirection _dir )
{
    TxtCursor *cursor_min = oldCursor->minCursor(txtCursor);
    TxtCursor *cursor_max = oldCursor->maxCursor(txtCursor);

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	TxtCursor _startCursor = startCursor;
	TxtCursor _stopCursor = stopCursor;

//	 drawSelection = false;
//	 redrawSelection( startCursor, stopCursor );

	if ( ( _dir == C_UP || _dir == C_LEFT ) && ( txtCursor->positionAbs() < startCursor.positionAbs() ) )
	    startCursor.setPositionAbs( txtCursor->positionAbs() );
	else if ( ( _dir == C_DOWN || _dir == C_RIGHT ) && ( txtCursor->positionAbs() < stopCursor.positionAbs() ) )
	    startCursor.setPositionAbs( txtCursor->positionAbs() );
	else if ( ( _dir == C_DOWN || _dir == C_RIGHT ) && ( txtCursor->positionAbs() == stopCursor.positionAbs() ) )
	{
	    stopCursor.setPositionAbs( txtCursor->positionAbs() );
	    startCursor.setPositionAbs( txtCursor->positionAbs() );
	}
	else if ( txtCursor->positionAbs() < startCursor.positionAbs() )
	    startCursor.setPositionAbs( oldCursor->positionAbs() );
	else
	    stopCursor.setPositionAbs( txtCursor->positionAbs() );

	if ( startCursor.positionAbs() != stopCursor.positionAbs() )
	    drawSelection = true;
	else drawSelection = false;

	if ( _startCursor.positionAbs() < startCursor.positionAbs() )
	    redrawSelection( _startCursor, startCursor );

	if ( _stopCursor.positionAbs() > stopCursor.positionAbs() )
	    redrawSelection( stopCursor, _stopCursor );
    }
    else
    {
	startCursor.setPositionAbs(cursor_min->positionAbs());
	stopCursor.setPositionAbs(cursor_max->positionAbs());
	if ( startCursor.positionAbs() != stopCursor.positionAbs() )
	    drawSelection = true;
	else drawSelection = false;
    }

    redrawSelection( startCursor, stopCursor );
}
/*================================================================*/
int KTextObject::getParagNum( TxtParagraph *_parag )
{
    int num = 0;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
    {
	if ( _parag == paragraphList.at( i ) ) return num;
	if ( !paragraphList.at( i )->isEmpty() ) num++;
    }

    return -1;
}

/*================================================================*/
void KTextObject::setDepth( int d )
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setDepth( d );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->setDepth( d );

    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::incDepth()
{
    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setDepth( paragraphList.at( i )->getDepth() + 1 < 16 ?
					     paragraphList.at( i )->getDepth() + 1 : 15 );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->
	    setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() + 1 < 16 ?
		      paragraphList.at( txtCursor->positionParagraph() )->getDepth() + 1 : 15 );

    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::decDepth()
{
    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setDepth( paragraphList.at( i )->getDepth() - 1 >= 0 ?
					     paragraphList.at( i )->getDepth() - 1 : 0 );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->
	    setDepth( paragraphList.at( txtCursor->positionParagraph() )->getDepth() - 1 >= 0 ?
		      paragraphList.at( txtCursor->positionParagraph() )->getDepth() - 1 : 0 );
    recalc();
    repaint( true );
}

/*================================================================*/
int KTextObject::getLeftIndent()
{
    return getLeftIndent( txtCursor->positionParagraph() ) + gap;
}

/*================================================================*/
int KTextObject::getLeftIndent( int _parag )
{
    return paragraphList.at( _parag )->getLeftIndent() + gap;
}

/*================================================================*/
int KTextObject::getLineSpacing()
{
    return paragraphList.at( txtCursor->positionParagraph() )->getLineSpacing();
}

/*================================================================*/
int KTextObject::getDistBefore()
{
    return paragraphList.at( txtCursor->positionParagraph() )->getDistBefore();
}

/*================================================================*/
int KTextObject::getDistAfter()
{
    return paragraphList.at( txtCursor->positionParagraph() )->getDistAfter();
}

/*================================================================*/
void KTextObject::setGap( int g )
{
    gap = g;
    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::setLineSpacing( int l )
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setLineSpacing( l );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->setLineSpacing( l );

    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::setDistBefore( int d )
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setDistBefore( d );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->setDistBefore( d );

    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::setDistAfter( int d )
{
    _modified = true;

    if ( drawSelection && startCursor.positionAbs() != stopCursor.positionAbs() )
    {
	int start_para = startCursor.positionParagraph();
	int stop_para = stopCursor.positionParagraph();

	for ( int i = start_para; i <= stop_para; i++ )
	    paragraphList.at( i )->setDistAfter( d );
    }
    else
	paragraphList.at( txtCursor->positionParagraph() )->setDistAfter( d );

    recalc();
    repaint( true );
}

/*================================================================*/
void KTextObject::setAllLineSpacing( int l )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
	paragraphList.at( i )->setLineSpacing( l );
    recalc();
}

/*================================================================*/
void KTextObject::setAllDistBefore( int d )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
	paragraphList.at( i )->setDistBefore( d );

    recalc();
}

/*================================================================*/
void KTextObject::setAllDistAfter( int d )
{
    _modified = true;

    for ( unsigned int i = 0; i < paragraphList.count(); i++ )
	paragraphList.at( i )->setDistAfter( d );

    recalc();
}

/*================================================================*/
void KTextObject::extendContents2Height()
{
    if ( height() <= neededSize().height() )
	return;

    _modified = true;

    if ( paragraphList.count() == 1 ) {
	setAllDistBefore( 0 );
	setAllDistAfter( 0 );
	setAllLineSpacing( 0 );

	if ( lines() < 2 ) {
	    int h = paragraphList.at( 0 )->height();
	    int dh = height() - h;
	    int ah = dh / 2;
	    if ( ah < 0 ) ah = 0;
	
	    paragraphList.at( 0 )->setDistBefore( ah );
	    paragraphList.at( 0 )->setDistAfter( ah );
	
	    recalc();
	    return;
	}

	int h = paragraphList.at( 0 )->height();
	int dh = height() - h;
	int ah = dh / ( lines() + 1 );
	if ( ah < 0 ) ah = 0;

	paragraphList.at( 0 )->setDistBefore( ah );
	paragraphList.at( 0 )->setLineSpacing( ah );
    } else {
	setAllDistBefore( 0 );
	setAllDistAfter( 0 );
	setAllLineSpacing( 0 );

	unsigned int i = 0;
	int h = 0;
	for ( i = 0; i < paragraphList.count(); i++ )
	    h += paragraphList.at( i )->height();

	int dh = height() - h;
	int ah = dh / ( paragraphList.count() + 1 );
	ah /= 2;
	if ( ah < 0 ) ah = 0;

	paragraphList.at( 0 )->setDistBefore( ah );
	for ( i = 0; i < paragraphList.count() - 1; i++ )
	    paragraphList.at( i )->setDistAfter( ah );
    }

    recalc();
}

/*================================================================*/
QSize KTextObject::neededSize()
{
    int w = 0, h = 0;
    for ( unsigned int i = 0; i < paragraphList.count(); i++ ) {
	w = QMAX( w, paragraphList.at( i )->width() + xstart + getLeftIndent( i ) );
	h += paragraphList.at( i )->height();
    }
	
    return QSize( w, h );
}

/*================================================================*/
void KTextObject::formatBold()
{
    currFont.setBold( !currFont.bold() );
    setFont( currFont );
    emit fontChanged( &currFont );
}

/*================================================================*/
void KTextObject::formatItalic()
{
    currFont.setItalic( !currFont.italic() );
    setFont( currFont );
    emit fontChanged( &currFont );
}

/*================================================================*/
void KTextObject::formatUnderline()
{
    currFont.setUnderline( !currFont.underline() );
    setFont( currFont );
    emit fontChanged( &currFont );
}

/*================================================================*/
void KTextObject::selectAll()
{
    TxtCursor *c1 = new TxtCursor( this );
    TxtCursor *c2 = new TxtCursor( this );

    c1->setPositionAbs( 0 );
    c2->setPositionAbs( textLength() - 1 );
    txtCursor->setPositionAbs( textLength() - 1 );
    startCursor = *c1;
    stopCursor = *c2;

    if ( stopCursor.positionAbs() != startCursor.positionAbs() )
	drawSelection = true;

    recalc();
    repaint( FALSE );
    redrawSelection( startCursor, stopCursor );

    delete c1;
    delete c2;
}
