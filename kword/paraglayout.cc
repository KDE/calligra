#include "paraglayout.h"
#include "kword_doc.h"

KWParagLayout::KWParagLayout( KWordDocument_impl *_doc )
  : format(_doc), counterFormat(_doc)
{
    flow = BLOCK; //LEFT;
    ptParagFootOffset = 0;
    ptParagHeadOffset = 0;
    ptFirstLineLeftIndent = 0;
    ptLeftIndent = 0;
    counterFlow = C_LEFT;
    counterDepth = 0;
    // counterNr = -1;
    counterNr = 0;
    counterLeftText = "";
    counterRightText = "";
    followingParagLayout = this;
    numberLikeParagLayout = 0L;
    
    format.setDefaults( _doc );
    
    document = _doc;
    document->paragLayoutList.append( this );
    document->paragLayoutList.setAutoDelete( FALSE );
}

KWParagLayout::~KWParagLayout()
{
    document->paragLayoutList.removeRef( this );
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
