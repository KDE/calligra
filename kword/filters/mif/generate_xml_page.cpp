/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "generate_xml_page.h"
#include "treebuild_page.h"
#include "treebuild_textrect.h"

#include <qlist.h>

TextRect* generate_xml_page::find_text_rect( int textrectid )
{
	// go through all the pages
	QListIterator<Page> pi( pages );
	Page* page = pi.current();
	while( page ) {
		++pi;
		// go through all the elements in this page
		QListIterator<PageElement> pei( *page->elements() );
		PageElement* pageelement = pei.current();
		while( pageelement ) {
			++pei;
			// If this is a TextRect and it has the correct id, we are 
			// done.
			if( pageelement->type() == PageElement::T_TextRect ) {
				TextRect* tr = pageelement->textRect();
				if( tr->id() == textrectid )
					return tr;
			}
			pageelement = pei.current();
		}
		page = pi.current();
	}
}


