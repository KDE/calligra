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
#include <qtextstream.h>

#include <kdebug.h>

#include "ImportTags.h"
#include "htmlimportwell.h"

typedef enum
{
    stateNormal,                        // Normal character
    stateLesser,                        // Character '<' found
    stateProcessingInstruction,         // Processing instruction: <?
    stateSGML,                          // <!
    stateTagName,                       // We are reading the name of the tag
    stateBeforeAttributeName,           // We are before the name of next attribute
    stateXMLEmptyElement,               // We have found an / , we suppose that the tag will be closed now
    stateAttributeName,                 // We are reading the name of an attribute
    stateAfterAttributeName,            // After the attribute name but before the equal sign
    stateBeforeAttributeValue,          // After the equal sign but before the attribute value.
    stateQuotedAttributeValue,          // Attribute value in quote (")
    stateSingleQuotedAttributeValue,    // Attribute value in single quote (')
    stateAttributeValue                 // Unquoted attribute value.
} States;

inline bool IsWhiteSpace(const QChar& ch)
{
    return ((ch==" ") || (ch==QChar(9)) || (ch==QChar(10)) || (ch==QChar(13)));
}

bool WellFilter(QTextStream& streamIn, QTextStream& streamOut)
{
    MapTag mapTag;

    kdDebug(30503) << "Tag map has " << mapTag.count() << " elements!" << endl;
    if (!mapTag.count())
    {// Problems when creating maps!
        kdError(30503) << "Tag map is empty! Aborting!" << endl;
        return false;
    }

    QMapIterator<QString,ParsingTag> mapTagIter=mapTag.begin(); // Default to first element.

    //TODO: CDATA sections
    States state=stateNormal;

    QChar ch;

    int depth=0; // Depth of tags in <! elements
    bool tagClosing=false; // Is current tag a closing tag?
    QString tagName;
    QString attributeName;
    QString attributeValue;

    while (!streamIn.atEnd())
    {
        streamIn >> ch;

        switch (state)
        {
        case stateNormal:
        default:
            {
                if (ch=='<')
                {
                    state=stateLesser;
                }
                else
                {
                    streamOut << ch;
                }
                break;
            }
        case stateLesser:
            {
                if (ch=='?')
                {
                    state=stateProcessingInstruction;
                    streamOut << "<?";
                }
                else if (ch=='!')
                {
                    state=stateSGML;
                    streamOut << "<!";
                    depth=1; // set depth counter
                }
                else if (ch=='/')
                {
                    state=stateTagName;
                    tagClosing=true;
                    tagName=QString::null;
                }
                else if (!IsWhiteSpace(ch))
                {
                    state=stateTagName;
                    tagClosing=false;
                    tagName=ch.lower();
                }
                // Note: we ignore any white space before the element name
                break;
            }
        case stateProcessingInstruction:
            { // TODO: catch <?xml
                if (ch=='>')
                {
                    state=stateNormal;
                }
                streamOut << ch;
                break;
            }
        case stateSGML:
            {
                // TODO: do a more robust version (the characters < and > may be included in SGML comments.)
                // TODO: HTML comments including -- (not valid SGML comments)
                if (ch=='>')
                {
                    depth--;
                    if (depth<=0)
                    {
                        state=stateNormal;
                    }
                }
                else if (ch=='<')
                {
                    depth++;
                }
                streamOut << ch;
                break;
            }
        case stateTagName:
            {
                if (IsWhiteSpace(ch))
                {
                    state=stateBeforeAttributeName;
                    if (tagName.isEmpty())
                    {
                        kdError(30503) << "PARSING ERROR: tag name empty! (White Space)" << endl;
                        return false;
                    }
                    // Write tag
                    streamOut << '<';
                    if (tagClosing)
                    {
                        streamOut << '/';
                    }
                    streamOut << tagName;
                    mapTagIter=mapTag.find(tagName);
                    if (mapTagIter==mapTag.end())
                    {
                        // If not found, point to default
                        kdDebug(30503) << "Tag name " << tagName << " not found in map! (White Space)" << endl;
                        mapTagIter=mapTag.begin();
                    }
                    // TODO: treatment of mapTagIter
                }
                else if (ch=='>')
                {
                    state=stateNormal;
                    if (tagName.isEmpty())
                    {
                        kdError(30503) << "PARSING ERROR: tag name empty! (Greater)" << endl;
                        return false;
                    }
                    // Write tag
                    streamOut << '<';
                    if (tagClosing)
                    {
                        streamOut << '/';
                    }
                    mapTagIter=mapTag.find(tagName);
                    if (mapTagIter==mapTag.end())
                    {
                        // If not found, point to default
                        kdDebug(30503) << "Tag name " << tagName << " not found in map! (Greater)" << endl;
                        mapTagIter=mapTag.begin();
                    }
                    // TODO: treatment of mapTagIter
                    streamOut << tagName;
                    if (mapTagIter.data().isEmptyElement())
                    {
                        streamOut << " /";
                    }
                    streamOut << '>';
                }
                else
                { // NOTE: if an explicit XML namespace is given, it is with the tag name!
                    // Make sure that the tag name becomes lower case.
                    tagName+=ch.lower();
                }
                break;
            }
        case stateBeforeAttributeName:
            {
                if (ch=='/')
                {
                    state=stateXMLEmptyElement;
                }
                else if (ch=='>')
                {
                    if (mapTagIter.data().isEmptyElement())
                    {
                        streamOut << " /";
                    }
                    streamOut << '>';
                    state=stateNormal;
                }
                else if (!IsWhiteSpace(ch))
                {
                    attributeName=ch.lower();
                    state=stateAttributeName;
                }
                break;
            }
        case stateXMLEmptyElement:
            {
                if (ch=='>')
                {
                    streamOut << " />";
                    state=stateNormal;
                }
                else if(!IsWhiteSpace(ch))
                {
                    kdError(30503) << "PARSING ERROR: unexpected character found after / !" << endl;
                    return false;
                }
                break;
            }
        case stateAttributeName:
            {
                if (IsWhiteSpace(ch))
                {
                    state=stateAfterAttributeName;
                    if (attributeName.isEmpty())
                    {
                        kdError(30503) << "PARSING ERROR: attribute name empty!" << endl;
                        return false;
                    }
                    // Make a default attribute value (see XHTML 1.0)
                    attributeValue=attributeName;
                    // Write tag
                    streamOut << " " << attributeName;
                }
                else if (ch=='>')
                {
                    state=stateNormal;
                    // Attribute has no value but XML forces to have one.
                    streamOut << " " << attributeName << '=' << '"' << attributeName << '"';
                    if (mapTagIter.data().isEmptyElement())
                    {
                        streamOut << " /";
                    }
                    streamOut << '>';
                }
                else if (ch=='=')
                {
                    state=stateBeforeAttributeValue;
                    attributeValue=QString::null;
                    streamOut << " " << attributeName;
                }
                else
                {
                    // Make sure that the attribute name is lower-case
                    attributeName+=ch.lower();
                }
                break;
            }
        case stateAfterAttributeName:
            {
                if (ch=='/')
                {
                    state=stateXMLEmptyElement;
                }
                else if (ch=='>')
                {
                    if (mapTagIter.data().isEmptyElement())
                    {
                        streamOut << " /";
                    }
                    streamOut << '>';
                    state=stateNormal;
                }
                else if (ch=='=')
                {
                    state=stateBeforeAttributeValue;
                    attributeValue=QString::null;
                }
                else if (!IsWhiteSpace(ch))
                {
                    // We have again characters, but we have not seen any equal sign.
                    // However XML forces use to have an explicit value
                    streamOut << '=' << '"' << attributeName << '"';

                    // Having again characters means that we are starting a new attribute name!
                    attributeName=ch.lower();
                    state=stateAttributeName;
                }
                break;
            }
        case stateBeforeAttributeValue:
            {
                if (ch=='>')
                {
                    kdError(30503) << "PARSING ERROR: unexpected > before attribute value" << endl;
                    return false;
                }
                else if (ch=='"')
                {
                    state=stateQuotedAttributeValue;
                    attributeValue=QString::null;
                }
                else if (ch=="'")
                {
                    state=stateSingleQuotedAttributeValue;
                    attributeValue=QString::null;
                }
                else if (!IsWhiteSpace(ch))
                {
                    // We have a new attribute value!
                    attributeValue=ch;
                    state=stateAttributeValue;
                }
                break;
            }
        case stateAttributeValue:
            {
                if (IsWhiteSpace(ch))
                {
                    state=stateBeforeAttributeName;
                    // Write tag
                    streamOut << '=' << '"' << attributeValue << '"';
                }
                else if (ch=='>')
                {
                    state=stateNormal;
                    streamOut << '=' << '"' << attributeValue << '"';
                    if (mapTagIter.data().isEmptyElement())
                    {
                        streamOut << " /";
                    }
                    streamOut << '>';
                }
                else
                {
                    attributeValue+=ch;
                }
                break;
            }
        case stateQuotedAttributeValue:
            {
                if (ch=='"')
                {
                    state=stateBeforeAttributeName;
                    streamOut << '=' << '"' << attributeValue << '"';
                }
                else
                {
                    attributeValue+=ch;
                }
                break;
            }
        case stateSingleQuotedAttributeValue:
            {
                if (ch=="'")
                {
                    state=stateBeforeAttributeName;
                    streamOut << "='" << attributeValue << "'";
                }
                else
                {
                    attributeValue+=ch;
                }
                break;
            }
        } // end of switch
    } // end of while

    if (state!=stateNormal)
    {
            kdError(30503) << "PARSING ERROR: at end, state has not returned to normal ! (" << state << ")" << endl;
            return false;
    }

    return true;
}
