#include <stdio.h>
#include <math.h>

#include "fc.h"
#include "kword_doc.h"
#include "paraglayout.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

KWFormatContext::KWFormatContext(KWordDocument *_doc,unsigned int _frameSet)
  : KWFormat()
{
  displayFont = 0;
  setDefaults( _doc );

  document = _doc;
  doc = _doc;

  during_vertical_cursor_movement = false;

  spacingError = 0;
  ptTextLen = 0;
  textPos = 0;

  lineStartPos = 0;
  lineEndPos = 0;
  specialHeight = 0;
  ptMaxAscender = 0;
  ptMaxDescender = 0;
  frameSet = _frameSet;
  frame = 1;
  compare_formats = true;
  outOfFrame = false;
  offsetsAdded = false;
}

KWFormatContext::~KWFormatContext()
{
}


void KWFormatContext::init( KWParag *_parag, QPainter &_painter, bool _updateCounters = true, bool _fromStart = true,
			    int _frame = -1, int _page = -1 )
{
  outOfFrame = false;
  specialHeight = 0;
  ptMaxAscender = 0;
  ptMaxDescender = 0;

  if (_fromStart)
    {
      // Offset from the top of the page
      if (document->getFrameSet(frameSet - 1)->getNumFrames() > 0)
	ptY = document->getFrameSet(frameSet - 1)->getFrame(0)->top() + document->getFrameSet(frameSet - 1)->getFrame(0)->getBTop().pt();
      else
	ptY = 0;
      if (_frame != -1 && _page != -1 && doc->getFrameSet(frameSet - 1)->getFrameInfo() != FI_BODY)
	{
	  frame = _frame;
	  page = _page;
	  ptY = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->top() +
	    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBTop().pt();
	}
      else
	{
	  frame = 1;
	  page = 1;
	}

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

    }
  else
    {
      parag = _parag;
      ptY = parag->getPTYStart();

      lineStartPos = 0;
      frame = parag->getStartFrame();
      page = document->getFrameSet(frameSet - 1)->getPageOfFrame(frame - 1) + 1;

      makeLineLayout( _painter );
    }
}

void KWFormatContext::enterNextParag( QPainter &_painter, bool _updateCounters = true )
{
    // Set the context to the given paragraph
    if ( parag != 0L )
    {
      parag->setPTYEnd( ptY );
      parag = parag->getNext();
      if ( parag == 0L )
      {
	warning("ERROR: Parag not found\n");
	exit(1);
      }
    }
    else
      parag = dynamic_cast<KWTextFrameSet*>(document->getFrameSet(frameSet - 1))->getFirstParag();
    // On which page are we now ...
    parag->setStartPage( page );
    parag->setEndPage( page );
    // In which column ...
    parag->setStartFrame( frame );
    parag->setEndFrame( frame );
    // Vertical position ...
    parag->setPTYStart( ptY );

    // We are at the beginning of our paragraph
    lineStartPos = 0;

    // Reset font size, color etc. to the documents default
    setDefaults( document );
    // Change fonts & stuff to match the paragraphs layout
    compare_formats = false;
    apply( parag->getParagLayout()->getFormat() );
    compare_formats = true;

    // Calculate everything about the line we are in.
    makeLineLayout( _painter );
}

void KWFormatContext::skipCurrentParag( QPainter &_painter )
{
    bool ret;

    // Iterate over all lines
    do
    {
	// returns false if we are at the end of
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
    // If we are in the first line, return true
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
    // If we are in the last line, return true
    return ( lineEndPos == parag->getTextLen() );
}

bool KWFormatContext::isCursorAtParagEnd()
{
    if ( !isCursorInLastLine() )
	return false;

    // Are we behind the paragraphs last character now ?
    if ( textPos == parag->getTextLen() )
	return true;

    return false;
}

bool KWFormatContext::isCursorAtLineEnd()
{
  // Are we behind the last lines character ?
  // Torben: I commented this out since it looks strange
  return ( textPos == lineEndPos /* || textPos == lineEndPos - 1 */ );
}

void KWFormatContext::cursorGotoRight( QPainter &_painter )
{
    during_vertical_cursor_movement = false;

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

void KWFormatContext::cursorGotoRight( QPainter &_painter, int _pos )
{
    during_vertical_cursor_movement = false;

    for (int i = 0;i < _pos;i++)
      {
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
	    continue;
	  }
	else if (isCursorInLastLine())
	  {
	    cursorGotoNextChar(_painter);
	    continue;
	  }
	
	// If the cursor is in the last line of some paragraph,
	// then we should not care here.
	if ( isCursorAtLastChar() )
	  {
	    lineStartPos = lineEndPos;
	    ptY += getLineHeight();
	    makeLineLayout( _painter );
	    cursorGotoLineStart( _painter );
	    continue;
	  }
	
	cursorGotoNextChar(_painter);
      }
}

void KWFormatContext::cursorGotoLeft( QPainter &_painter )
{
    during_vertical_cursor_movement = false;

    if ( isCursorAtParagStart() )
    {
	// The first paragraph ?
	if ( parag->getPrev() == 0L )
	    return;
	// Enter the prev paragraph
	init(parag->getPrev(),_painter,true,false);
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
	init(parag,_painter,true,false);
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
	
	init(parag->getPrev(),_painter,false,false);
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
	cursorGotoLineStart( _painter );
	while (ptPos < WantedPtPos && textPos < lineEndPos && !isCursorAtLineEnd()){
	  cursorGotoRight( _painter);
	}
    }
    else {
	// Re-Enter the current paragraph
	unsigned int tmpPos = lineStartPos;
	init (parag,_painter,false,false);
	do {
	    makeLineLayout( _painter );
	    if (lineEndPos < tmpPos){
		ptY += getLineHeight();
		// next line
		lineStartPos = lineEndPos;
	    }
	} while(lineEndPos < tmpPos);

	cursorGotoLineStart( _painter );
	while (ptPos < WantedPtPos && textPos < lineEndPos - 1 && !isCursorAtLineEnd()){
	  cursorGotoRight( _painter);
	}
    }
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
}

void KWFormatContext::cursorGotoLineStart( QPainter &_painter )
{
  during_vertical_cursor_movement = false;
  spacingError = 0;
  cursorGotoPos( lineStartPos, _painter );
}

void KWFormatContext::cursorGotoLineEnd( QPainter &_painter )
{
  during_vertical_cursor_movement = false;

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
   init(parag,_painter,false,false);
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

    unsigned int pos = lineStartPos;
    spacingError = 0;
    ptPos = ptStartPos;
    compare_formats = false;
    apply(lineStartFormat);
    compare_formats = true;

    // test this to avoid crashes!!
    if (_textpos > parag->getKWString()->size())
      _textpos =  parag->getKWString()->size();

    bool _break = false;
    while ( pos < _textpos && !_break)
    {
        if ( text[ pos ].c == 0 )
	{
	  // Handle specials here
	  switch (text[pos].attrib->getClassId())
	    {
	    case ID_KWCharImage:
	      {
		ptPos += ((KWCharImage*)text[pos].attrib)->getImage()->width();
		specialHeight = max(specialHeight,(unsigned int)((KWCharImage*)text[pos].attrib)->getImage()->height());
		pos++;
	      } break;
	    case ID_KWCharVariable:
	      {
		KWCharVariable *v = dynamic_cast<KWCharVariable*>(text[pos].attrib);
		apply(*v->getFormat());
		ptPos += displayFont->getPTWidth(v->getText());
		pos++;
	      } break;
	    case ID_KWCharTab:
	      {
		unsigned int tabPos = 0;
		KoTabulators tabType;
		if (parag->getParagLayout()->getNextTab(ptPos,document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->left() +
							document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt(),
							document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->right() -
							document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt(),tabPos,tabType))
		  {
		    // next tab is in this line
		    if (tabPos > ptPos)
		      {
			switch (tabType)
			  {
			  case T_LEFT:
			    ptPos = tabPos;
			    break;
			  case T_RIGHT:
			    {
			      if (ptPos + (ptTextLen - (ptPos - ptStartPos)) < tabPos)
				ptPos = tabPos - (ptTextLen - (ptPos - ptStartPos));
 			    } break;
			  case T_CENTER:
			    {
			      if (ptPos + (ptTextLen - (ptPos - ptStartPos)) / 2 < tabPos)
				ptPos = tabPos - (ptTextLen - (ptPos - ptStartPos)) / 2;
 			    } break;
			  default: break;
			  }
		      }
		    // next tab is in the next line
		    else
		      {
			_break = true;
		      }
		  }
		pos++;
	      } break;
	    }
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
	
	    ptPos += (unsigned int)dx + displayFont->getPTWidth( text[pos].c );

	    pos++;
	  }
	  else
	  {
	    ptPos += displayFont->getPTWidth( text[ pos ].c );
	    pos++;
	  }
	}
    }

    textPos = _textpos;
}

void KWFormatContext::cursorGotoPixelLine(unsigned int mx,unsigned int my,QPainter &_painter)
{
  textPos = 0;

  init(dynamic_cast<KWTextFrameSet*>(document->getFrameSet(frameSet - 1))->getFirstParag(),_painter);

  if (ptY <= my && ptY + getLineHeight() >= my &&
      ptLeft <= mx && ptLeft + ptWidth >= mx)
    {
      textPos = lineStartPos;
      cursorGotoLineStart(_painter);
      return;
    }

  KWParag *_p = dynamic_cast<KWTextFrameSet*>(document->getFrameSet(frameSet - 1))->getFirstParag();
  while (_p->getPTYEnd() < my && _p->getNext())
    _p = _p->getNext();

  if (_p->getPrev()) _p = _p->getPrev();

  init(_p,_painter,false,false);

  bool found = false;
  while (makeNextLineLayout(_painter))
    {
      if (ptY <= my && ptY + getLineHeight() >= my &&
	  ptLeft <= mx && ptLeft + ptWidth >= mx)
	{
	  found = true;
	  break;
	}
    }

  if (!found) ptY -= getLineHeight();

  textPos = lineStartPos;
  cursorGotoLineStart(_painter);
}

void KWFormatContext::cursorGotoPixelInLine(unsigned int mx,unsigned int my,QPainter &_painter)
{
  if (isCursorAtLineEnd()) return;
  unsigned int oldPTPos,oldDist = 0,dist = 0;

  while (!isCursorAtLineEnd())
    {
      oldPTPos = ptPos;
      cursorGotoRight(_painter);
      dist = (ptPos - oldPTPos) / 3;
      if (mx >= oldPTPos - oldDist && mx <= ptPos - dist || textPos == lineStartPos)
	{
	  cursorGotoLeft(_painter);
	  return;
	}
      oldDist = dist;
    }
}

int KWFormatContext::cursorGotoNextChar(QPainter & _painter)
{
  // If we are already at lineend, then we wont move further
  if ( isCursorAtLineEnd() )
    return -2;

  KWChar *text = parag->getText();
  KWParagLayout *lay = parag->getParagLayout();
  int pos = textPos;
  if ( text[ pos ].c == 0 )
    {
      // Handle specials here
      switch (text[pos].attrib->getClassId())
	{
	case ID_KWCharImage:
	  {
	    ptPos += ((KWCharImage*)text[pos].attrib)->getImage()->width();
	    specialHeight = max(specialHeight,(unsigned int)((KWCharImage*)text[pos].attrib)->getImage()->height());
	    pos++;
	  } break;
	case ID_KWCharVariable:
	  {
	    KWCharVariable *v = dynamic_cast<KWCharVariable*>(text[pos].attrib);
	    apply(*v->getFormat());
	    ptPos += displayFont->getPTWidth(v->getText());
	    pos++;
	  } break;
	case ID_KWCharTab:
	  {
	    unsigned int tabPos = 0;
	    KoTabulators tabType;
	    if (parag->getParagLayout()->getNextTab(ptPos,document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->left() +
						    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt(),
						    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->right() -
						    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt(),tabPos,tabType))
	      {
		// next tab is in this line
		if (tabPos > ptPos)
		  {
		    switch (tabType)
		      {
		      case T_LEFT:
			ptPos = tabPos;
			break;
		      case T_RIGHT:
			{
			  //if (!_checkTabs) break;
			  if (ptPos + (ptTextLen - (ptPos - ptStartPos)) < tabPos)
			    ptPos = tabPos - (ptTextLen - (ptPos - ptStartPos));
			} break;
		      case T_CENTER:
			{
			  //if (!_checkTabs) break;
			  if (ptPos + (ptTextLen - (ptPos - ptStartPos)) / 2 < tabPos)
			    ptPos = tabPos - (ptTextLen - (ptPos - ptStartPos)) / 2;
			} break;
		      default: break;
		      }
		  }
		// next tab is in the next line
		else
		  {
		    pos++;
		    return -2;
		  }
	      }
	    pos++;
	  } break;
	}
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
	  ptPos += (unsigned int)dx + displayFont->getPTWidth( text[pos].c );
	  pos++;
	}
      else
	{
	  ptPos += displayFont->getPTWidth( text[ pos ].c );
	  pos++;
	}
    }
  textPos = pos;

  if ( isCursorAtLineEnd() )
    return -2;

  if ( parag->getText()[ textPos ].c != 0 && parag->getText()[ textPos ].attrib == 0 ||
       textPos - 1 >= 0 && parag->getText()[ textPos - 1 ].c != 0 &&
       *((KWCharFormat*)parag->getText()[ textPos - 1 ].attrib) == *((KWCharFormat*)parag->getText()[ textPos ].attrib))
    return 1;

  if ( parag->getText()[ textPos ].c != 0 )
    return 0;

  assert( parag->getText()[ textPos ].attrib != 0L );
  return -1;
}


bool KWFormatContext::makeNextLineLayout( QPainter &_painter )
{
  if ( lineEndPos == parag->getTextLen() )
    {
      if ( parag->getNext() == 0L || outOfFrame)
	{
	  ptY += getLineHeight();
	  parag->setPTYEnd( ptY );
	  outOfFrame = false;
	  return false;
	}

      ptY += getLineHeight();
      enterNextParag( _painter );
    }
  else
    {
      lineStartPos = lineEndPos;
      ptY += getLineHeight();
      makeLineLayout( _painter );
    }

  return true;
}

bool KWFormatContext::makeLineLayout( QPainter &_painter, bool _checkIntersects = true, bool _checkTabs = true )
{
    int _left = 0,_right = 0;

    // Reggie: DAMN SLOW HACK !!!!!!!
    if (parag->getParagLayout()->hasSpecialTabs() && _checkTabs && _checkIntersects)
      makeLineLayout(_painter,true,false);

    if (_checkIntersects)
      {
	if (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->hasIntersections())
	  makeLineLayout(_painter,false);
      }

    if (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->hasIntersections())
      {	
	_left = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getLeftIndent(ptY,getLineHeight());
	_right = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getRightIndent(ptY,getLineHeight());
      }

    compare_formats = false;
    unsigned int _ptTextLen = ptTextLen;
    ptTextLen = 0;
    specialHeight = 0;

    ptPos = 0;
    spaces = 0;
    textPos = lineStartPos;
    lineEndPos = lineStartPos;
    ptMaxAscender = 0;
    ptMaxDescender = 0;
    ptAscender = 0;
    ptDescender = 0;
    offsetsAdded = true;

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

      indent = parag->getParagLayout()->getFirstLineLeftIndent().pt();
    }
    else
      indent = parag->getParagLayout()->getLeftIndent().pt();

    indent += _left;

    // Calculate the shift for the first visible character. This may be the counter, too
    unsigned int xShift = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->left() +
      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() + indent;

    ptLeft = xShift;
    ptWidth = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt() - indent - _right;

    // First line ? Draw the couter ?
    if (lineStartPos == 0 && parag->getParagLayout()->getCounterType() != KWParagLayout::CT_NONE)
    {
      KWFormat counterfm(doc,*this);
      counterfm.apply(parag->getParagLayout()->getFormat());
      if (parag->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET)
	counterfm.setUserFont(document->findUserFont(parag->getParagLayout()->getBulletFont()));
      _painter.setFont(*(counterfm.loadFont(document)));
      _painter.setPen(counterfm.getColor());
	
      left += ptCounterWidth;
    }


    ptPos = 0;

    // Calculate the first characters position in screen coordinates
    ptPos = xShift + left;

    ptStartPos = ptPos;

    // Calculate the counter position
    // Is the counter fixed to the left side ?
    ptCounterPos = ptStartPos - ptCounterWidth;

    apply(*this);

    // Get ascender/descender of the font we are starting with
    tmpPTAscender = displayFont->getPTAscender();
    tmpPTDescender = displayFont->getPTDescender();

    lineStartFormat = *this;

    if (static_cast<int>(ptWidth) < document->getRastX())
      {
	ptY = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getNextFreeYPos(ptY,getLineHeight()) + 2;
	return makeLineLayout(_painter);
      }
	
    bool _broken = false;
    bool _break = false;

    // Loop until we reach the end of line
    while (!_break && (ptPos < xShift + (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
					 document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
					 document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - indent - _right ||
		       !_broken) &&
	   textPos < parag->getTextLen())
      {
	char c = text[textPos].c;
	
	if (c != 0 && text[ textPos ].attrib)
	  {
	    // Handle font formats here.
	    assert( text[ textPos ].attrib->getClassId() == ID_KWCharFormat );
	    KWCharFormat *f = (KWCharFormat*)text[ textPos ].attrib;
	    apply( *f->getFormat() );
	    if (textPos == lineStartPos)
	      lineStartFormat = *this;
	  }
	
	// if we will not fit into the line anymore, let us leave the loop
	if (c != 0)
	  {
	    if (ptPos + displayFont->getPTWidth(c) >=
		xShift + (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - indent - _right && _broken)
	      break;
	  }
	else if (c == 0 && text[textPos].attrib->getClassId() == ID_KWCharImage)
	  {
	    if (((KWCharImage*)text[textPos].attrib)->getImage()->width() + ptPos >=
		xShift + (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - indent - _right && _broken)
	      break;
	  }
	else if (c == 0 && text[textPos].attrib->getClassId() == ID_KWCharVariable)
	  {
	    if (displayFont->getPTWidth(dynamic_cast<KWCharVariable*>(text[textPos].attrib)->getText()) + ptPos >=
		xShift + (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - indent - _right && _broken)
	      break;
	  }
	
	// Is it a space character
	if (c == ' ')
	  {
	    // This is the correct point to make a line break
	    lineEndPos = textPos + 1;
	    // If we break here, then the line has the following width ...
	    ptTextLen = tmpPTWidth;
	    spaces = tmpSpaces;
	    // ... or one more space if we dont break the line here.
	    tmpSpaces++;
	    _broken = true;
	  }

	// Do we have some format definition here ?
	if (c == 0)
	  {
	    switch (text[textPos].attrib->getClassId())
	      {
	      case ID_KWCharImage:
		{
		  ptPos += ((KWCharImage*)text[textPos].attrib)->getImage()->width();
		  tmpPTWidth += ((KWCharImage*)text[textPos].attrib)->getImage()->width();
		  specialHeight = max(specialHeight,(unsigned int)((KWCharImage*)text[textPos].attrib)->getImage()->height());
		  textPos++;
		} break;
	      case ID_KWCharVariable:
		{
		  KWCharVariable *v = dynamic_cast<KWCharVariable*>(text[textPos].attrib);
		  apply(*v->getFormat());
		  ptPos += displayFont->getPTWidth(v->getText());
		  textPos++;
		} break;
	      case ID_KWCharTab:
		{
		  unsigned int tabPos = 0;
		  KoTabulators tabType;
		  if (parag->getParagLayout()->getNextTab(ptPos,document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->left() +
							  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt(),
							  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->right() -
							  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt(),
							  tabPos,tabType))
		    {
		      // next tab is in this line
		      if (tabPos > ptPos)
			{
			  switch (tabType)
			    {
			    case T_LEFT:
			      ptPos = tabPos;
			      break;
			    case T_RIGHT:
			      {
				if (!_checkTabs) break;
				if (ptPos + (_ptTextLen - (ptPos - ptStartPos)) < tabPos)
				  ptPos = tabPos - (_ptTextLen - (ptPos - ptStartPos));
			      } break;
			    case T_CENTER:
			      {
				if (!_checkTabs) break;
				if (ptPos + (_ptTextLen - (ptPos - ptStartPos)) / 2 < tabPos)
				  ptPos = tabPos - (_ptTextLen - (ptPos - ptStartPos)) / 2;
			      } break;
			    default: break;
			    }
			}
		      // next tab is in the next line
		      else
			{
			  lineEndPos = textPos;
			  textPos--;
			  ptTextLen = tmpPTWidth;
			  spaces = tmpSpaces;
			  tmpSpaces++;
			  _broken = true;
			  _break = true;
			}
		    }
		  textPos++;
		} break;
	      }
	  }
	else // A usual character ...
	  {
	    // Go right ...
	    ptPos += displayFont->getPTWidth(c);
	    // Increase the lines width
	    tmpPTWidth += displayFont->getPTWidth(c);
	    // One more character
	    textPos++;
	  }
      }

    // Are we at the paragraphs end ?
    if (textPos == parag->getTextLen())
      {
	// We have to take the last possible linebreak
	lineEndPos = textPos;
	ptTextLen = tmpPTWidth;
	ptAscender = tmpPTAscender;
	ptDescender = tmpPTDescender;
	if (ptAscender > ptMaxAscender)
	    ptMaxAscender = ptAscender;
	if (ptDescender > ptMaxDescender)
	  ptMaxDescender = ptDescender;	
	spaces = tmpSpaces;
      }

    if (parag->getParagLayout()->getFlow() == KWParagLayout::CENTER)
      {
	ptPos = xShift + ((document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
			   document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
			   document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) -
			   indent - left - right - ptTextLen - _right) / 2;
	ptStartPos = ptPos;
      }
    else if (parag->getParagLayout()->getFlow() == KWParagLayout::RIGHT)
      {
	ptPos = xShift + (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
			  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - right - ptTextLen - indent - _right;
	ptStartPos = ptPos;
      }

    // Calculate the space between words if we have "block" formating.
    if (parag->getParagLayout()->getFlow() == KWParagLayout::BLOCK && spaces > 0)
      ptSpacing = static_cast<float>((document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->width() -
				      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBLeft().pt() -
				      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBRight().pt()) - ptTextLen -
				     indent - _right) / static_cast<float>(spaces);
    else
      ptSpacing = 0;

    offsetsAdded = false;
    compare_formats = false;
    apply(*((KWFormat*)this));
    compare_formats = true;

    // Does this line still fit in this frame or do we have to make a hard break?
    if (!document->isPTYInFrame(frameSet - 1,frame - 1,ptY + getLineHeight()) ||
	(parag->hasHardBreak() && isCursorInFirstLine()) && parag->getPrev() && parag->getPrev()->getEndPage() == page)
      {
	// Are we a header or footer?
	if (isAHeader(document->getFrameSet(frameSet - 1)->getFrameInfo()) || isAFooter(document->getFrameSet(frameSet - 1)->getFrameInfo()))
	  {
	    int diff = (ptY + getLineHeight()) - (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->bottom() -
						  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBBottom().pt());
		
	    if (document->canResize(document->getFrameSet(frameSet - 1),document->getFrameSet(frameSet - 1)->getFrame(frame - 1),
				    document->getFrameSet(frameSet - 1)->getPageOfFrame(frame - 1),diff + 1))
	      {
		document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->
		  setHeight(document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->height() + diff + 1);
		
		if (document->getFrameSet(frameSet - 1)->getGroupManager())
		  {
		    document->getFrameSet(frameSet - 1)->getGroupManager()->deselectAll();
		    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->setSelected(true);
		    document->getFrameSet(frameSet - 1)->getGroupManager()->recalcRows(_painter);
		  }
		
		QPaintDevice *dev = _painter.device();
		_painter.end();

		document->recalcFrames(false,true);
		document->updateAllFrames();
		document->setNeedRedraw(true);
		
		_painter.begin(dev);
	      }
	    else
	      {		
		outOfFrame = true;
		return false;
	      }
	  }
	// Can we jump to the next frame ?
	else if (frame < document->getFrameSet(frameSet - 1)->getNumFrames())
	  {
	    frame++;
	    if (static_cast<int>(document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->top() +
		 document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBTop().pt()) >
		static_cast<int>(page) * static_cast<int>(document->getPTPaperHeight()))
	      page++;
	    parag->setEndPage(page);
	    parag->setEndFrame(frame);
	    ptY = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->top() +
	      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBTop().pt();
	    return makeLineLayout(_painter);
	  }
	else // append a page or resize frame
	  {
	    if (!dynamic_cast<KWTextFrameSet*>(document->getFrameSet(frameSet - 1))->getAutoCreateNewFrame())
	      {
		int diff = (ptY + getLineHeight()) - (document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->bottom() -
						      document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBBottom().pt());
		
		if (document->canResize(document->getFrameSet(frameSet - 1),document->getFrameSet(frameSet - 1)->getFrame(frame - 1),
					document->getFrameSet(frameSet - 1)->getPageOfFrame(frame - 1),diff + 1))
		  {
		    document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->
		      setHeight(document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->height() + diff + 1);
		
		    if (document->getFrameSet(frameSet - 1)->getGroupManager())
		      {
			document->getFrameSet(frameSet - 1)->getGroupManager()->deselectAll();
			document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->setSelected(true);
			document->getFrameSet(frameSet - 1)->getGroupManager()->recalcRows(_painter);
		      }

		    QPaintDevice *dev = _painter.device();
		    _painter.end();

 		    document->recalcFrames(false,true);
 		    document->updateAllFrames();
 		    document->setNeedRedraw(true);

		    _painter.begin(dev);
		  }
		else
		  {		
		    outOfFrame = true;
		    return false;
		  }
	      }
	    else
	      {
		document->appendPage(page - 1,_painter);
		page++;
		frame++;
		parag->setEndPage(page);
		parag->setEndFrame(frame);
		ptY = document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->top() +
		  document->getFrameSet(frameSet - 1)->getFrame(frame - 1)->getBTop().pt();
		return makeLineLayout(_painter);
	
	      }
	  }
      }


    compare_formats = true;
    // If we are in the last line, return false
    if (lineEndPos == parag->getTextLen())
	return false;

    return true;
}

unsigned int KWFormatContext::getLineHeight()
{
  unsigned int hei = ptMaxAscender + ptMaxDescender;
  unsigned int plus = 0;

  return max(hei,specialHeight) + getParag()->getParagLayout()->getLineSpacing().pt() + plus;
}

void KWFormatContext::makeCounterLayout(QPainter &_painter)
{
  if (parag->getParagLayout()->getCounterType() == KWParagLayout::CT_NONE) return;

  KWFormat format(doc,parag->getParagLayout()->getFormat());
  format.apply(parag->getParagLayout()->getFormat());
  if (parag->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET)
    format.setUserFont(document->findUserFont(parag->getParagLayout()->getBulletFont()));
  KWDisplayFont *font = format.loadFont(document);

  counterText = parag->getCounterText();

  ptCounterWidth = max(font->getPTWidth(parag->getCounterWidth().data()),font->getPTWidth(parag->getCounterText().data()));
  ptCounterAscender = font->getPTAscender();
  ptCounterDescender = font->getPTDescender();
}


void KWFormatContext::apply( KWFormat &_format )
{
  if (compare_formats && _format == *((KWFormat*)this)) return;

  KWFormat::apply(_format);
  if (_format.getVertAlign() != VA_NORMAL)
    displayFont = document->findDisplayFont(userFont,(2 * _format.getPTFontSize()) / 3,_format.getWeight(),
					    _format.getItalic(),_format.getUnderline());
  else
    displayFont = loadFont(document);

  ptAscender = displayFont->getPTAscender();
  ptDescender = displayFont->getPTDescender();
  ptMaxAscender = max(ptAscender,ptMaxAscender);
  ptMaxDescender = max(ptDescender,ptMaxDescender);

  if (_format.getVertAlign() == KWFormat::VA_SUB)
    ptMaxDescender = max(ptMaxDescender,(unsigned int)((2 * _format.getPTFontSize()) / 3) / 2);
  else if (_format.getVertAlign() == KWFormat::VA_SUPER)
    ptMaxAscender = max(ptMaxAscender,(unsigned int)((2 * _format.getPTFontSize()) / 3) / 2);

  if (!offsetsAdded)
    {
      if (isCursorInLastLine()  && getParag() && getParag()->getParagLayout()->getParagFootOffset().pt() != 0)
	ptMaxDescender += getParag()->getParagLayout()->getParagFootOffset().pt();
      if (isCursorInFirstLine() && getParag() && getParag()->getParagLayout()->getParagHeadOffset().pt() != 0)
	ptMaxAscender += getParag()->getParagLayout()->getParagHeadOffset().pt();
      offsetsAdded = true;
    }
}

void KWFormatContext::selectWord(KWFormatContext &_fc1,KWFormatContext &_fc2,QPainter &painter)
{
  KWChar *text = parag->getText();
  bool goLeft = false;

  KWFormatContext fc(document,frameSet);
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

