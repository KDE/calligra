// $Header$

/* This file is part of the KDE project
Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qstring.h>
#include <qmap.h>

#include <kdebug.h>

#include "ImportTags.h"

static bool AddTag(QMap<QString,ParsingTag> & mapTag, const QString& strName, const bool empty)
{
    ParsingTag* tag=new ParsingTag(empty);
    if (!tag)
    {
        kdDebug(30503)<< "Registring tag " << strName << " failed!" << endl;
        return false;
    }
    mapTag.insert(strName,*tag);
    return true;
}

bool InitMapTag(QMap<QString,ParsingTag> & mapTag)
{

    // Defulat element (must be first!)
    AddTag(mapTag,"//UNKNOWN//",false);

    // Commonly used empty elements
    AddTag(mapTag,"br",true);
    AddTag(mapTag,"meta",true);
    AddTag(mapTag,"hr",true);

    // Other empty elements
    AddTag(mapTag,"aera",true);
    AddTag(mapTag,"base",true);
    AddTag(mapTag,"basefont",true);
    AddTag(mapTag,"col",true);
    AddTag(mapTag,"frame",true);
    AddTag(mapTag,"img",true);
    AddTag(mapTag,"input",true);
    AddTag(mapTag,"isindex",true);
    AddTag(mapTag,"link",true);
    AddTag(mapTag,"param",true);

#if 1
    // Other elements (in alphabetic order)
    AddTag(mapTag,"a",false);
    AddTag(mapTag,"abbr",false);
    AddTag(mapTag,"acronym",false);
    AddTag(mapTag,"address",false);
    AddTag(mapTag,"applet",false);
    // <area>
    AddTag(mapTag,"b",false);
    // <base>
    // <basefont>
    AddTag(mapTag,"bdo",false);
    AddTag(mapTag,"big",false);
    AddTag(mapTag,"blockquote",false);
    AddTag(mapTag,"body",false);
    // <br>
    AddTag(mapTag,"button",false);
    AddTag(mapTag,"caption",false);
    AddTag(mapTag,"center",false);
    AddTag(mapTag,"cite",false);
    AddTag(mapTag,"code",false);
    // <col>
    AddTag(mapTag,"colgroup",false);
    AddTag(mapTag,"dd",false);
    AddTag(mapTag,"del",false);
    AddTag(mapTag,"dfn",false);
    AddTag(mapTag,"dir",false);
    AddTag(mapTag,"div",false);
    AddTag(mapTag,"dl",false);
    AddTag(mapTag,"dt",false);
    AddTag(mapTag,"em",false);

    AddTag(mapTag,"fieldset",false);
    AddTag(mapTag,"font",false);
    AddTag(mapTag,"form",false);
    // <frame>
    AddTag(mapTag,"frameset",false);
    AddTag(mapTag,"h1",false);
    AddTag(mapTag,"h2",false);
    AddTag(mapTag,"h3",false);
    AddTag(mapTag,"h4",false);
    AddTag(mapTag,"h5",false);
    AddTag(mapTag,"h6",false);
    AddTag(mapTag,"head",false);
    // <hr>
    AddTag(mapTag,"html",false);
    AddTag(mapTag,"i",false);
    AddTag(mapTag,"iframe",false);
    // <img>
    // <input>
    AddTag(mapTag,"ins",false);
    // <isindex>
    AddTag(mapTag,"kbd",false);
    AddTag(mapTag,"label",false);
    AddTag(mapTag,"legend",false);
    AddTag(mapTag,"li",false);
    // <link>
    AddTag(mapTag,"map",false);
    AddTag(mapTag,"menu",false);
    // <meta>
    AddTag(mapTag,"noframes",false);
    AddTag(mapTag,"noscript",false);
    AddTag(mapTag,"object",false);
    AddTag(mapTag,"ol",false);
    AddTag(mapTag,"option",false);
    AddTag(mapTag,"optiongroup",false);
    AddTag(mapTag,"p",false);
    // <param>
    AddTag(mapTag,"pre",false);
    AddTag(mapTag,"q",false);
    AddTag(mapTag,"s",false);
    AddTag(mapTag,"samp",false);
    AddTag(mapTag,"script",false);
    AddTag(mapTag,"select",false);
    AddTag(mapTag,"small",false);
    AddTag(mapTag,"span",false);
    AddTag(mapTag,"strike",false);
    AddTag(mapTag,"strong",false);
    AddTag(mapTag,"style",false);
    AddTag(mapTag,"sub",false);
    AddTag(mapTag,"sup",false);
    AddTag(mapTag,"table",false);
    AddTag(mapTag,"tbody",false);
    AddTag(mapTag,"td",false);
    AddTag(mapTag,"textarea",false);
    AddTag(mapTag,"tfoot",false);
    AddTag(mapTag,"th",false);
    AddTag(mapTag,"thead",false);
    AddTag(mapTag,"title",false);
    AddTag(mapTag,"tr",false);
    AddTag(mapTag,"tt",false);
    AddTag(mapTag,"u",false);
    AddTag(mapTag,"ul",false);
    AddTag(mapTag,"var",false);

    // Non-HTML 4.01 Elements
    // No one for now!
#endif
    return true;
}
