#include "paragvisitors.h"
#include "kwanchor.h"

bool KWCollectFramesetsVisitor::visit( KoTextParag *parag, int start, int end )
{
  KoTextString* str = parag->string();
  for ( int i = start ; i < end ; ++i )
  {
    const KoTextStringChar& ch = str->at( i );
    if ( ch.isCustom() )
    {
      KoTextCustomItem *customitem = ch.customItem();
      KWAnchor *anchor = static_cast<KWAnchor *>(customitem);
      if (anchor)
      {
	m_framesets.append(anchor->frameSet());
      }
    }
  }
  return true; //always return true, i.e. keep going
}
