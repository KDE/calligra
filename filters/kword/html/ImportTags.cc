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

bool MapTag::AddTag(const QString& strName,const ParsingTag* tag)
{
    if (!tag)
    {
        kdDebug(30503)<< "Registring tag " << strName << " failed!" << endl;
        return false;
    }
    insert(strName,*tag);
    return true;
}

bool MapTag::AddTag(const QString& strName, const bool empty)
{
    return AddTag(strName,new ParsingTag(modeDisplayUnknown,empty,QString::null));
}

bool MapTag::AddNoneTag(const QString& strName)
{ // Note: do NOT use for empty tags (e.g. <br>)
    return AddTag(strName,new ParsingTag(modeDisplayNone,false,"display: none;"));
}

bool MapTag::AddBlockTag(const QString& strName,const QString& strStyle)
{
    return AddTag(strName,new ParsingTag(modeDisplayBlock,false,strStyle));
}

bool MapTag::AddInlineTag(const QString& strName,const QString& strStyle)
{
    return AddTag(strName,new ParsingTag(modeDisplayInline,false,strStyle));
}

bool MapTag::InitMapTag(void)
{

    // Default element (must be first!)
    AddTag("//UNKNOWN//",false);

    // Commonly used empty elements
    AddTag("br",true);      // TODO: how can we implement this one?
    AddTag("meta",true);    // "display: none"
    AddTag("hr",true);

    // Other empty elements
    AddTag("aera",true);
    AddTag("base",true);
    AddTag("basefont",true);
    AddTag("col",true);
    AddTag("frame",true); // "display: none" as we do not support frames
    AddTag("img",true);
    AddTag("input",true);
    AddTag("isindex",true);
    AddTag("link",true);
    AddTag("param",true);

    // Other elements (in alphabetic order)
    AddTag("a",false);
    AddTag("abbr",false);
    AddTag("acronym",false);
    AddInlineTag("address","font-style:italic;");
    AddNoneTag("applet"); // We do not support scripts!
    // <area>
    AddInlineTag("b","font-weight:bold;");
    // <base>
    // <basefont>
    AddTag("bdo",false);
    AddTag("big",false);
    AddTag("blockquote",false);
    AddTag("body",false);
    // <br>
    AddTag("button",false);
    AddTag("caption",false);
    AddTag("center",false);
    AddInlineTag("cite","font-style:italic;");
    AddTag("code",false);
    // <col>
    AddTag("colgroup",false);
    AddTag("dd",false);
    AddInlineTag("del","text-decoration:line-through;"); // May need to be changed when a new KWord does know what "deleted" text is!
    AddTag("dfn",false);
    AddTag("dir",false);
    AddTag("div",false);
    AddTag("dl",false);
    AddTag("dt",false);
    AddInlineTag("em","font-style:italic;");

    AddTag("fieldset",false);
    AddTag("font",false);
    AddTag("form",false);  // Candidate for display none?
    // <frame>
    AddNoneTag("frameset"); // We do not support framesets
    AddBlockTag("h1","font-weight:bold;");
    AddBlockTag("h2","font-weight:bold;");
    AddBlockTag("h3","font-weight:bold;");
    AddBlockTag("h4","font-weight:bold;");
    AddBlockTag("h5","font-weight:bold;");
    AddBlockTag("h6","font-weight:bold;");
    AddNoneTag("head"); // <head> is always "display: none"
    // <hr>
    AddTag("html",false);
    AddInlineTag("i","font-style:italic;");
    AddTag("iframe",false);
    // <img>
    // <input>
    AddInlineTag("ins","text-decoration:underline;"); // May need to be changed when a new KWord does know what "inserted" text is!
    // <isindex>
    AddTag("kbd",false);
    AddTag("label",false);
    AddTag("legend",false);
    AddTag("li",false);
    // <link>
    AddTag("map",false);
    AddTag("menu",false);
    // <meta>
    AddTag("noframes",false);
    AddTag("noscript",false);
    AddTag("object",false);
    AddTag("ol",false);
    AddTag("option",false);
    AddTag("optiongroup",false);
    AddBlockTag("p",QString::null);
    // <param>
    AddTag("pre",false);
    AddTag("q",false);
    AddInlineTag("s","text-decoration:line-through;");
    AddTag("samp",false);
    AddNoneTag("script"); // We do not support scripts
    AddTag("select",false);
    AddTag("small",false);
    AddInlineTag("span",QString::null);
    AddInlineTag("strike","text-decoration:line-through;");
    AddInlineTag("strong","font-weight:bold;");
    AddNoneTag("style"); // "display: none" but has to be treated! (TODO)
    AddInlineTag("sub","text-position:subscript;");
    AddInlineTag("sup","text-position:superscript;");
    AddTag("table",false);
    AddTag("tbody",false);
    AddTag("td",false);
    AddTag("textarea",false);
    AddTag("tfoot",false);
    AddTag("th",false);
    AddTag("thead",false);
    AddNoneTag("title");
    AddTag("tr",false);
    AddTag("tt",false);
    AddInlineTag("u","text-decoration:underline;");
    AddTag("ul",false);
    AddInlineTag("var","font-style:italic;");

    // Non-HTML 4.01 Elements
    // No one for now!

    return true;
}
