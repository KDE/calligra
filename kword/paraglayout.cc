#include "paraglayout.h"
#include "kword_doc.h"

KWParagLayout::KWParagLayout( KWordDocument_impl *_doc )
  : format(_doc), counterFormat(_doc)
{
    flow = LEFT;
    mmParagFootOffset = 0;
    mmParagHeadOffset = 0;
    mmFirstLineLeftIndent = 0;
    mmLeftIndent = 0;
    counterFlow = C_LEFT;
    counterDepth = 0;
    // counterNr = -1;
    counterNr = 0;
    counterLeftText = "";
    counterRightText = "";
    followingParagLayout = this;
    numberLikeParagLayout = 0L;
    ptLineSpacing = 0;

    left.color = white;
    left.style = SOLID;
    left.ptWidth = 0;
    right.color = white;
    right.style = SOLID;
    right.ptWidth = 0;
    top.color = white;
    top.style = SOLID;
    top.ptWidth = 0;
    bottom.color = white;
    bottom.style = SOLID;
    bottom.ptWidth = 0;

    format.setDefaults( _doc );
    
    document = _doc;
    document->paragLayoutList.append( this );
    document->paragLayoutList.setAutoDelete(true);
}

KWParagLayout::~KWParagLayout()
{
    document->paragLayoutList.removeRef( this );
}

KWParagLayout& KWParagLayout::operator=(KWParagLayout &_layout)
{
  flow = _layout.getFlow();
  mmParagFootOffset = _layout.getMMParagFootOffset();
  mmParagHeadOffset = _layout.getMMParagHeadOffset();
  mmFirstLineLeftIndent = _layout.getPTFirstLineLeftIndent();
  mmLeftIndent = _layout.getPTLeftIndent();
  counterFlow = static_cast<CounterFlow>(_layout.getCounterFlow());
  counterDepth = _layout.getCounterDepth();
  counterNr = _layout.getCounterNr();
  counterLeftText = qstrdup(_layout.getCounterLeftText());
  counterRightText = qstrdup(_layout.getCounterRightText());
  followingParagLayout = this;
  numberLikeParagLayout = 0L;
  ptLineSpacing = _layout.getPTLineSpacing();

  left = _layout.getLeftBorder();
  right = _layout.getRightBorder();
  top = _layout.getTopBorder();
  bottom = _layout.getBottomBorder();

  format.setDefaults( document );

  return *this;
}

void KWParagLayout::setFollowingParagLayout( const char *_name )
{
    KWParagLayout* p = document->findParagLayout( _name );
    if ( p == 0L )
	followingParagLayout = this;
    else
	followingParagLayout = p;
}

void KWParagLayout::setNumberLikeParagLayout( const char *_name )
{
    KWParagLayout* p = document->findParagLayout( _name );
    if ( p == 0L )
	numberLikeParagLayout = this;
    else
	numberLikeParagLayout = p;
}
