#include <stdio.h>
#include <math.h>

#include "fc.h"
#include "kword_doc.h"
#include "paraglayout.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

KWFormatContext::KWFormatContext( KWordDocument_impl *_doc ) : KWFormat()
{
  displayFont = 0;
  setDefaults( _doc );
  
  document = _doc;

  for ( int i = 0; i < 8; i++ )
    for ( int j = 0; j < 8; j++ )
      counters[i][j] = 0;
  
  during_vertical_cursor_movement = FALSE;

  spacingError = 0;
  ptTextLen = 0;
  textPos = 0;

  lineStartPos = 0;
  lineEndPos = 0;
  specialHeight = 0;
}

KWFormatContext::~KWFormatContext()
{
}


void KWFormatContext::init( KWParag *_parag, QPainter &_painter, bool _updateCounters = true )
{
    // Offset from the top of the page
    ptY = document->getPTTopBorder();
    column = 1;
    page = 1;

    // Enter the first paragraph
    parag = 0L;
    enterNextParag( _painter, _updateCounters );

    // Loop until we got the paragraph
    while ( parag != _parag )
    {
	// Skip the current paragraph
	skipCurrentParag( _painter );
	// Go to the next one
	enterNextParag( _painter, _updateCounters );
    }

    // gotoLine( 0, _painter );
}

void KWFormatContext::enterNextParag( QPainter &_painter, bool _updateCounters = true )
{
    // Set the context to the given paragraph
    if ( parag != 0L )
    {
      parag = parag->getNext();
      if ( parag == 0L )
      {
	warning("ERROR: Parag not found\n");
	exit(1);
      }
    }
    else
      parag = document->getFirstParag();
    // On which page are we now ...
    parag->setStartPage( page );
    // In which column ...
    parag->setStartColumn( column );
    // Vertical position ...
    parag->setPTYStart( ptY );

    if (_updateCounters)
    {
	// Initialize our paragraph counter stuff
	int cnr = parag->getParagLayout()->getCounterNr();
	int dep = parag->getParagLayout()->getCounterDepth();
	
	if ( cnr != -1 )
	  counters[cnr][dep]++;
	parag->updateCounters( this );
    }

    // We are at the beginning of our paragraph
    lineStartPos = 0;
    
    // Reset font size, color etc. to the documents default
    // setDefaults( document );
    // Change fonts & stuff to match the paragraphs layout
    // apply( parag->getParagLayout()->getFormat() );
        
    // Calculate everything about the line we are in.
    makeLineLayout( _painter );
}

void KWFormatContext::skipCurrentParag( QPainter &_painter )
{
    bool ret;

    // Iterate over all lines
    do
    {
	// returns FALSE if we are at the end of
	// the paragraph after the call returns.
	ret = makeLineLayout( _painter );
	// Skip the line
	ptY += getLineHeight();
	// next line
	lineStartPos = lineEndPos;
    } while( ret );    
}

void KWFormatContext::gotoStartOfParag( QPainter & )
{
}

bool KWFormatContext::isCursorAtLastChar()
{
  if ( textPos >= lineEndPos - 1 )
    return true;

  return false;
}

bool KWFormatContext::isCursorInFirstLine()
{
    // If we are in the first line, return TRUE
    return ( lineStartPos == 0 );
}

bool KWFormatContext::isCursorAtParagStart()
{
    return ( textPos == 0);
}

bool KWFormatContext::isCursorAtLineStart()
{
    return ( textPos == lineStartPos);
}

bool KWFormatContext::isCursorInLastLine()
{
    // If we are in the last line, return TRUE
    return ( lineEndPos == parag->getTextLen() );
}

bool KWFormatContext::isCursorAtParagEnd()
{
    if ( !isCursorInLastLine() )
	return FALSE;
    
    // Are we behind the paragraphs last character now ?
    if ( textPos == parag->getTextLen() )
	return TRUE;

    return FALSE;
}

bool KWFormatContext::isCursorAtLineEnd()
{
  //debug("pos: %d, end: %d",textPos,lineEndPos);

    // Torben: I commented this out since it looks strange
  /* if (isCursorInLastLine())
	return isCursorAtParagEnd(); */

    // Are we behind the last lines character ?
    // Torben: I commented this out since it looks strange
    return ( textPos == lineEndPos /* || textPos == lineEndPos - 1 */ );
}

void KWFormatContext::cursorGotoRight( QPainter &_painter )
{
    during_vertical_cursor_movement = FALSE;

    // Are we at the end of a paragraph ?
    if ( isCursorAtParagEnd() )
    {
     	// The last paragraph ?
	if ( parag->getNext() == 0L )
	    return;
	// Skip the current line
	ptY += getLineHeight();
	// Enter the next paragraph
	enterNextParag( _painter );
	cursorGotoLineStart( _painter );
	return;
    }
    else if (isCursorInLastLine())
    {
      textPos++;
      cursorGotoPos( textPos, _painter );
      return;
    }

    // If the cursor is in the last line of some paragraph,
    // then we should not care here.
    if ( isCursorAtLastChar() )
    {
	lineStartPos = lineEndPos;
	ptY += getLineHeight();
	makeLineLayout( _painter );
	cursorGotoLineStart( _painter );
	return;
    }
    
    textPos++;

    cursorGotoPos( textPos, _painter );
}

void KWFormatContext::cursorGotoLeft( QPainter &_painter )
{
    during_vertical_cursor_movement = FALSE;

    if ( isCursorAtParagStart() )
    {
	// The first paragraph ?
	if ( parag->getPrev() == 0L )
	    return;
	// Enter the prev paragraph
	init( parag->getPrev(), _painter );
	int ret;
	do
	{
	    ret = makeLineLayout( _painter );
	    if (ret) {
		// Skip the line
		ptY += getLineHeight();
		// next line
		lineStartPos = lineEndPos;
	    }
	} while( ret );    

	cursorGotoLineEnd( _painter );
	return;
    }

    // If the cursor is in the first line of some paragraph
    if ( isCursorAtLineStart() )
    {
	unsigned int tmpPos = lineStartPos;
	init( parag, _painter );
	do {
	    makeLineLayout( _painter );
	    if (lineEndPos < tmpPos){
		ptY += getLineHeight();
		// next line
		lineStartPos = lineEndPos;
	    }
	} while(lineEndPos < tmpPos);    
	cursorGotoLineEnd( _painter );
	return;
    }
    
    textPos--;

    cursorGotoPos( textPos, _painter );
}

void KWFormatContext::cursorGotoUp( QPainter &_painter )
{
    if (!during_vertical_cursor_movement){
	WantedPtPos = ptPos;
    }

    if ( isCursorInFirstLine() )
    {
	// The firstparagraph ?
	if ( parag->getPrev() == 0L )
	    return;
	// Enter the prev paragraph
	
	// decrease counter here, and don't let
	// enterParagraph update the counter
	int cnr = parag->getPrev()->getParagLayout()->getCounterNr();
	int dep = parag->getPrev()->getParagLayout()->getCounterDepth();
	
	if ( cnr != -1 )
	  counters[cnr][dep]--;
	parag->getPrev()->updateCounters( this );
	
	init( parag->getPrev(), _painter, false );
	int ret;
	do
	{
	    ret = makeLineLayout( _painter );
	    if (ret) {
		// Skip the line
		ptY += getLineHeight();
		// next line
		lineStartPos = lineEndPos;
	    }
	} while( ret );    
    }
    else {
	// Re-Enter the current paragraph
	unsigned int tmpPos = lineStartPos;
	init (parag, _painter, false);
	do {
	    makeLineLayout( _painter );
	    if (lineEndPos < tmpPos){
		ptY += getLineHeight();
		// next line
		lineStartPos = lineEndPos;
	    }
	} while(lineEndPos < tmpPos);    
    }
    
    cursorGotoLineStart( _painter );
    while (ptPos < WantedPtPos && textPos < lineEndPos - 1 && !isCursorAtLineEnd()){
      cursorGotoRight( _painter);
    }
    //during_vertical_cursor_movement = TRUE;
}

void KWFormatContext::cursorGotoDown( QPainter &_painter )
{
    // Save the position where we started going down
    if ( !during_vertical_cursor_movement)
    {
	WantedPtPos = ptPos;
    }

    if ( isCursorInLastLine() )
    {
	// The last paragraph ?
	if ( parag->getNext() == 0L )
	    return;
	// Skip the current line
	ptY += getLineHeight();
	// Enter the next paragraph
	enterNextParag( _painter );
    }
    else {
	lineStartPos = lineEndPos;
	ptY += getLineHeight();
	makeLineLayout( _painter );
    }
    
    cursorGotoLineStart( _painter );
    while (ptPos < WantedPtPos && 
	   !isCursorAtLineEnd() ){
	cursorGotoRight( _painter);
    }
    //during_vertical_cursor_movement = TRUE;
}

void KWFormatContext::cursorGotoLineStart( QPainter &_painter )
{
  during_vertical_cursor_movement = FALSE;

  cursorGotoPos( lineStartPos, _painter );
}

void KWFormatContext::cursorGotoLineEnd( QPainter &_painter )
{
  during_vertical_cursor_movement = FALSE;

  if ( isCursorInLastLine() )
  {
    cursorGotoPos( lineEndPos, _painter );
    return;
  }
  
  // Is the last character a space ? => it is not really displayed
  if ( lineEndPos > lineStartPos && parag->getText()[ lineEndPos - 1 ].c == ' ' )
  {
    // Go to the last character only.
    cursorGotoPos( lineEndPos - 1, _painter );
    return;
  }
  
  // The line is empty or it contains a word which is longer then a line.
  // In both cases we can go behind the last character
  cursorGotoPos( lineEndPos, _painter );
}

void KWFormatContext::cursorGotoNextLine(QPainter &_painter)
{
    during_vertical_cursor_movement = false;

    lineStartPos = lineEndPos;
    ptY += getLineHeight();
    makeLineLayout( _painter );
    cursorGotoLineStart( _painter );
    return;
}

void KWFormatContext::cursorGotoPrevLine(QPainter &_painter)
{
  // Re-Enter the current paragraph
  unsigned int tmpPos = lineStartPos;
  init (parag, _painter, false);
  do 
    {
      makeLineLayout( _painter );
      if (lineEndPos < tmpPos){
	ptY += getLineHeight();
	// next line
	lineStartPos = lineEndPos;
      }
  } 
  while(lineEndPos < tmpPos);    

  
  cursorGotoLineStart( _painter );

  during_vertical_cursor_movement = false;
}

void KWFormatContext::cursorGotoLine( unsigned int _textpos, QPainter &_painter )
{
    if ( _textpos < lineStartPos )
    {
	gotoStartOfParag( _painter );
	makeLineLayout( _painter );
    }
    else if ( _textpos >= lineStartPos && _textpos < lineEndPos )
    {
	cursorGotoPos( _textpos, _painter );
	return;
    }
    
    bool ret;
    do
    {
	if ( _textpos >= lineStartPos && _textpos < lineEndPos )
	{
	    cursorGotoPos( _textpos, _painter );
	    return;
	}
	ret = makeNextLineLayout( _painter );
    } while ( ret );

    warning("ERROR: Textpos behind content of parag\n");
    exit(1);
}

void KWFormatContext::cursorGotoPos( unsigned int _textpos, QPainter & )
{
    KWChar *text = parag->getText();
    KWParagLayout *lay = parag->getParagLayout();
    
    //unsigned int bp = 0;
    //float ddx = 0.0;
    //char buffer[_textpos + 2];
    unsigned int pos = lineStartPos;
    ptPos = ptStartPos;
    *((KWFormat*)this) = lineStartFormat;

    // test this to avoid crashes!!
    if (_textpos > parag->getKWString()->size())
      _textpos =  parag->getKWString()->size();

    while ( pos < _textpos )
    {
        if ( text[ pos ].c == 0 )
	{
	  // Handle specials here
	  ptPos += ((KWCharImage*)text[pos].attrib)->getImage()->width();
	  specialHeight = ((KWCharImage*)text[pos].attrib)->getImage()->height();
	  pos++;
	}
	else
	{
	  if ( text[ pos ].attrib )
	  {
	    // Change text format here
	    assert( text[ pos ].attrib->getClassId() == ID_KWCharFormat );
	    KWCharFormat *f = (KWCharFormat*)text[ pos ].attrib;
	    apply( *f->getFormat() );
	  }
	  
	  if ( text[ pos ].c == ' ' && lay->getFlow() == KWParagLayout::BLOCK && 
	       lineEndPos != parag->getTextLen() )
	  {
	    float sp = ptSpacing + spacingError;
	    
	    float dx = floor( sp );
	    spacingError = sp - dx;
	    
	    //ddx += dx;
	    //buffer[bp++] = text[pos].c;
	    ptPos += (unsigned int)dx + displayFont->getPTWidth( text[pos].c );

	    pos++;
	  }
	  else
	  {
	    //buffer[bp++] = text[pos].c;
	    ptPos += displayFont->getPTWidth( text[ pos ].c );
	    pos++;   
	  }
	}
    }
    //buffer[bp] = '\0';
    //ptPos += (unsigned int)ddx + displayFont->getPTWidth(buffer);
    
    textPos = _textpos;
}

void KWFormatContext::cursorGotoPixelLine(unsigned int mx,unsigned int my,QPainter &_painter)
{
  textPos = 0;

  init(document->getFirstParag(),_painter);
  
  if (ptY <= my && ptY + getLineHeight() >= my &&
      ptLeft <= mx && ptLeft + ptWidth >= mx)
    {
      textPos = lineStartPos;
      cursorGotoLineStart(_painter);
      return;
    }

  while (makeNextLineLayout(_painter))
    {
      if (ptY <= my && ptY + getLineHeight() >= my &&
	  ptLeft <= mx && ptLeft + ptWidth >= mx)
	break;
    }

  textPos = lineStartPos;
  cursorGotoLineStart(_painter);
}

void KWFormatContext::cursorGotoPixelInLine(unsigned int mx,unsigned int my,QPainter &_painter)
{
  if (isCursorAtLineEnd()) return;
  unsigned int oldPTPos;

  while (!isCursorAtLineEnd())
    {
      oldPTPos = ptPos;
      cursorGotoRight(_painter);
      if (mx >= oldPTPos && mx <= ptPos || textPos == lineStartPos)
	{
	  cursorGotoLeft(_painter);
	  return;
	}
    }      
}

int KWFormatContext::cursorGotoNextChar(QPainter & _painter)
{
  // If we are already at lineend, then we wont move further
  if ( isCursorAtLineEnd() )
    return -2;
  
  cursorGotoPos( textPos + 1, _painter ); //!! HACK !! This is DOG SLOW
  if ( isCursorAtLineEnd() )
    return -2;
  
  if ( parag->getText()[ textPos ].c != 0 && parag->getText()[ textPos ].attrib == 0 ||
       textPos - 1 >= 0 && parag->getText()[ textPos - 1 ].c != 0 && 
       *((KWCharFormat*)parag->getText()[ textPos - 1 ].attrib) == *((KWCharFormat*)parag->getText()[ textPos ].attrib))
    {
      return 1;
    }
  
  if ( parag->getText()[ textPos ].c != 0 )
    return 0;
  
  assert( parag->getText()[ textPos ].attrib != 0L );
  return -1;
}


bool KWFormatContext::makeNextLineLayout( QPainter &_painter )
{
    if ( lineEndPos == parag->getTextLen() )
    {
	if ( parag->getNext() == 0L )
	    return FALSE;
	ptY += getLineHeight();
	enterNextParag( _painter );
    }
    else
    {
	lineStartPos = lineEndPos;
	ptY += getLineHeight();
	makeLineLayout( _painter );
    }

    return TRUE;
}

bool KWFormatContext::makeLineLayout( QPainter &_painter )
{
//   if (parag->getParagLayout()->getFlow() == KWParagLayout::RIGHT || 
//       parag->getParagLayout()->getFlow() == KWParagLayout::CENTER)
//     calcTextLen();
  
    ptTextLen = 0;
    specialHeight = 0;

    ptPos = 0;
    spaces = 0;
    textPos = lineStartPos;
    lineEndPos = lineStartPos;
    ptMaxAscender = 0;
    ptMaxDescender = 0;
    
    unsigned int tmpPTWidth = 0;
    unsigned int tmpPTAscender;
    unsigned int tmpPTDescender;
    unsigned int tmpSpaces = 0;
    
    unsigned left = 0;
    unsigned int right = 0;

    KWChar *text = parag->getText();

    makeCounterLayout(_painter); // !!! HACK !!!

    // The indentation of the line. This is only the indentation
    // the user selected.
    unsigned int indent;
    if ( lineStartPos == 0 )
    {
      // Reset font size, color etc. to the documents default
      setDefaults( document );
      // Change fonts & stuff to match the paragraphs layout
      apply( parag->getParagLayout()->getFormat() );
      
      indent = parag->getParagLayout()->getPTFirstLineLeftIndent();
    }
    else
      indent = parag->getParagLayout()->getPTLeftIndent();
    
    // Calculate the shift for the first visible character. This may be the counter, too
    unsigned int xShift = document->getPTLeftBorder() + ( column - 1 ) * ( document->getPTColumnWidth() + document->getPTColumnSpacing() )
      + indent;

    ptLeft = xShift;
    ptWidth = document->getPTColumnWidth() - indent;

    // First line ? Draw the couter ?
    if ( lineStartPos == 0 && parag->getParagLayout()->getCounterNr() != -1 )
    {
	KWFormat counterfm(doc, *this );
	counterfm.apply( parag->getParagLayout()->getCounterFormat() );
	_painter.setFont( *(counterfm.loadFont( document )) );
	_painter.setPen( counterfm.getColor() );
	
	// Is the counter fixed to the left side ?
	if ( parag->getParagLayout()->getCounterFlow() == KWParagLayout::C_LEFT ){
	    left += ptCounterWidth;
	}
	else { // the counter is fixed to the right side
	    right += ptCounterWidth;
	}
	
    }


    ptPos = 0;
    
    // Calculate the first characters position in screen coordinates
//     if ( parag->getParagLayout()->getFlow() == KWParagLayout::LEFT )
//       ptPos = xShift + left;
//     else if ( parag->getParagLayout()->getFlow() == KWParagLayout::BLOCK )
      ptPos = xShift + left;

    //debug("%d %d %d",ptPos,ptTextLen,xShift + document->getPTColumnWidth());

    ptStartPos = ptPos;

    // Assume the counter to have the maximum ascender/descender
    ptMaxAscender = ptCounterAscender;
    ptMaxDescender = ptCounterDescender;

    // Calculate the counter position
    // Is the counter fixed to the left side ?
    if ( parag->getParagLayout()->getCounterFlow() == KWParagLayout::C_LEFT )
    {
	ptCounterPos = ptStartPos - ptCounterWidth;
    }
    else
    { // the counter is fixed to the right side
	ptCounterPos = xShift + document->getPTColumnWidth() - ptCounterWidth; // Attention!
    }
    
    // Get the correct font
    tmpFormat.apply( *this );
    KWDisplayFont *font = tmpFormat.loadFont( document );
    displayFont = font;
    

    // Get ascender/descender of the font we are starting with
    tmpPTAscender = font->getPTAscender();
    tmpPTDescender = font->getPTDescender();

    lineStartFormat = *this;
    
    // Loop until we reach the end of line
    while( ptPos < xShift + document->getPTColumnWidth() - indent  && textPos < parag->getTextLen() )
    {
	char c = text[ textPos ].c;

	if ( c != 0 && text[ textPos ].attrib )
	{
	  // Handle font formats here.
	  assert( text[ textPos ].attrib->getClassId() == ID_KWCharFormat );
	  KWCharFormat *f = (KWCharFormat*)text[ textPos ].attrib;
	  apply( *f->getFormat() );
	}
	
	// if we will not fit into the line anymore, let us leave the loop
	if (c != 0)
	  {
	    if (ptPos + font->getPTWidth(c) >= xShift + document->getPTColumnWidth() - indent)
	      break;
	  }
	else
	  {
	    if (((KWCharImage*)text[textPos].attrib)->getImage()->width() + ptPos >= xShift + document->getPTColumnWidth() - indent)
	      break;
	  }

	// Is it a space character
	if ( c == ' ' )
	{
	    // This is the correct point to make a line break
	    lineEndPos = textPos + 1;
	    // If we break here, then the line has the following width ...
	    ptTextLen = tmpPTWidth;
	    ptAscender = tmpPTAscender;
	    ptDescender = tmpPTDescender;
	    if ( ptAscender > ptMaxAscender )
		ptMaxAscender = ptAscender;
	    if ( ptDescender > ptMaxDescender )
		ptMaxDescender = ptDescender;
	    // The amount of spaces in the line if
	    // we do a line break here ...
	    spaces = tmpSpaces;
	    // ... or one more space if we dont break the line here.
	    tmpSpaces++;
	}

	// Do we have some format definition here ?
	if ( c == 0 )
	{
	  ptPos += ((KWCharImage*)text[textPos].attrib)->getImage()->width();
	  tmpPTWidth += ((KWCharImage*)text[textPos].attrib)->getImage()->width();
	  specialHeight = ((KWCharImage*)text[textPos].attrib)->getImage()->height();
	  textPos++;
	}
	else // A usual character ...
	{ 
	    // Go right ...
	    ptPos += font->getPTWidth(c);
	    // Increase the lines width
	    tmpPTWidth += font->getPTWidth(c);
	    // One more character
	    textPos++;
	}
    }

    // Are we at the paragraphs end ?
    if ( textPos == parag->getTextLen() )
    {
	// We have to take the last possible linebreak
	lineEndPos = textPos;
	ptTextLen = tmpPTWidth;
	ptAscender = tmpPTAscender;
	ptDescender = tmpPTDescender;
	if ( ptAscender > ptMaxAscender )
	    ptMaxAscender = ptAscender;
	if ( ptDescender > ptMaxDescender )
	    ptMaxDescender = ptDescender;	    
	spaces = tmpSpaces;
    }
    
    if ( parag->getParagLayout()->getFlow() == KWParagLayout::CENTER )
      {
	ptPos = xShift + ( document->getPTColumnWidth() - indent - left - right - ptTextLen ) / 2;
	ptStartPos = ptPos;
      }
    else if ( parag->getParagLayout()->getFlow() == KWParagLayout::RIGHT )
      {      
	ptPos = xShift + document->getPTColumnWidth() - right - ptTextLen;
	ptStartPos = ptPos;
      }

    // Calculate the space between words if we have "block" formating.
    if ( parag->getParagLayout()->getFlow() == KWParagLayout::BLOCK && spaces > 0)
      ptSpacing = (float)( document->getPTColumnWidth() - ptTextLen - indent) / (float)spaces;
    else
      ptSpacing = 0;

    // Does this line still fit on this column/page ?
    if ( !document->isPTYIn( page, ptY + getLineHeight() ) )
    {
	// Can we jump to the next column ?
	if ( column < document->getColumns() )
	{
	    ptY = document->getPTPaperHeight() * ( page - 1 ) + document->getPTTopBorder();
	    column++;
	    return makeLineLayout(_painter);
	}
	else // We chnage to another page
	{
	    column = 1;
	    page++;
	    ptY = document->getPTPaperHeight() * ( page - 1 ) + document->getPTTopBorder();
	    return makeLineLayout(_painter);
	}
    }

    //debug("start: %d, end: %d",lineStartPos,lineEndPos);

    // If we are in the last line, return FALSE
    if ( lineEndPos == parag->getTextLen() )
	return FALSE;

    return TRUE;
}

unsigned short KWFormatContext::getCounter( unsigned int _counternr, unsigned int _depth )
{
    return counters[ _counternr ][ _depth ];
}

unsigned int KWFormatContext::getLineHeight()
{ 
  unsigned int hei = ptMaxAscender + ptMaxDescender;

  return max(hei,specialHeight) + getParag()->getParagLayout()->getPTLineSpacing(); 
}

void KWFormatContext::makeCounterLayout( QPainter &_painter )
{
    KWFormat format( doc,parag->getParagLayout()->getFormat() );
    format.apply( parag->getParagLayout()->getCounterFormat() );
    KWDisplayFont *font = loadFont( document );    

    parag->makeCounterText( counterText );
    
    ptCounterWidth = 30; //font->getPTWidth( counterText.data() );
    ptCounterAscender = font->getPTAscender();
    ptCounterDescender = font->getPTDescender();
}


void KWFormatContext::apply( KWFormat &_format )
{
  KWFormat::apply(_format);
  if (displayFont)
    {
      displayFont->setFamily(_format.getUserFont()->getFontName());
      if (_format.getPTFontSize() != -1)
	displayFont->setPTSize(_format.getPTFontSize());
      if (_format.getWeight() != -1)
	displayFont->setWeight(_format.getWeight());
      if (_format.getItalic() != -1)
	displayFont->setItalic(_format.getItalic());
      if (_format.getUnderline() != -1)
	displayFont->setUnderline(_format.getUnderline());

      ptAscender = displayFont->getPTAscender();
      ptDescender = displayFont->getPTDescender();
      ptMaxAscender = max(ptAscender,ptMaxAscender);
      ptMaxDescender = max(ptDescender,ptMaxDescender);
    }
}

void KWFormatContext::selectWord(KWFormatContext &_fc1,KWFormatContext &_fc2,QPainter &painter)
{
  KWChar *text = parag->getText();
  bool goLeft = false;

  KWFormatContext fc(document);
  fc = *this;

  if (text[textPos].c == ' ')
    {
      _fc1 = *this;
      cursorGotoRight(painter);
    }
  else
    {
      unsigned int i = textPos;
      while (i > 0 && i > lineStartPos && text[i].c != ' ')
	{
	  cursorGotoLeft(painter);
	  i = textPos;
	}
      if (i == lineStartPos)
	goLeft = true;
      cursorGotoRight(painter);
      _fc1 = *this;
    }

  if (text[textPos].c == ' ')
    _fc2 = *this;
  else
    {
      unsigned int i = textPos;
      while (i < parag->getTextLen() && i < lineEndPos && text[i].c != ' ')
	{
	  cursorGotoRight(painter);
	  i = textPos;
	}
      _fc2 = *this;
    }
  if (goLeft) _fc1.cursorGotoLeft(painter);
}

void KWFormatContext::calcTextLen()
{
  // Reggie: this is DOG SLOW and has to be done better

  ptTextLen = 0;
  
  unsigned int pos = lineStartPos,tmpPTPos = 0,_ptPos = 0;
  KWChar *text = parag->getText();
  unsigned int indent;
  if ( lineStartPos == 0 )
    {
      // Reset font size, color etc. to the documents default
      setDefaults( document );
      // Change fonts & stuff to match the paragraphs layout
      apply( parag->getParagLayout()->getFormat() );
      
      indent = parag->getParagLayout()->getPTFirstLineLeftIndent();
    }
    else
      indent = parag->getParagLayout()->getPTLeftIndent();
    
  // Calculate the shift for the first visible character. This may be the counter, too
  unsigned int xShift = document->getPTLeftBorder() + ( column - 1 ) * ( document->getPTColumnWidth() + document->getPTColumnSpacing() )
    + indent;
  
  tmpFormat.apply( *this );
  KWDisplayFont *font = tmpFormat.loadFont( document );
  displayFont = font;


  while ( _ptPos < xShift + document->getPTColumnWidth() - indent && pos < getParag()->getTextLen())
    {
      char c = text[ pos ].c;
      
      if ( c != 0 && text[ pos ].attrib )
	{
	  // Handle font formats here.
	  assert( text[ pos ].attrib->getClassId() == ID_KWCharFormat );
	  KWCharFormat *f = (KWCharFormat*)text[ pos ].attrib;
	  apply( *f->getFormat() );
	}
      
      if (c == ' ')
	{
	  ptTextLen += tmpPTPos;
	  tmpPTPos = 0;
	}
	
      // Do we have some format definition here ?
      if ( c == 0 )
	;
      else // A usual character ...
	{ 
	  _ptPos += font->getPTWidth( c );
	  tmpPTPos += font->getPTWidth( c );
	  pos++;
	}
    }

  if ( textPos >= parag->getTextLen() - 1 )
    ptTextLen += tmpPTPos - 16;

  debug("%d %d",lineStartPos,ptTextLen);
}
