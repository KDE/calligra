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
  mmParagFootOffset = _layout.getPTParagFootOffset();
  mmParagHeadOffset = _layout.getPTParagHeadOffset();
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
