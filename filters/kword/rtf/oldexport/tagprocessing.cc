/* This file is part of the KDE project
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
   Copurogjt (C) 2001 Michael Johnson <mikej@xnet.com>


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

#include <kdebug.h>
#include <qdir.h>
#include <qdom.h>
#include <tagprocessing.h>


// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.




void ProcessSubtags ( QDomNode                    parentNode,
                      QValueList<TagProcessing>  &tagProcessingList,
                      QString                    &outputText         )
{
    QDomNode childNode;

    for ( childNode = parentNode.firstChild (); !childNode.isNull (); childNode = childNode.nextSibling () )
    {
        if ( childNode.nodeType () == QDomNode::ElementNode )
        {
            bool found = false;

            QValueList<TagProcessing>::Iterator  tagProcessingIt;

            for ( tagProcessingIt = tagProcessingList.begin ();
                  !found && tagProcessingIt != tagProcessingList.end ();
                  tagProcessingIt++ )
            {

                if ( childNode.nodeName () == (*tagProcessingIt).name )
                {
//                kdError (KDEBUG_TAGPROCESSING) << " Tag Processing Tag " << (*tagProcessingIt).name << endl;
                    found = true;

                    if ( (*tagProcessingIt).processor != NULL )
                    {
                        ((*tagProcessingIt).processor) ( childNode, (*tagProcessingIt).data, outputText );
                    }
                    else
                    {
//                      kdError (KDEBUG_TAGPROCESSING) << "<para>ignoring " << childNode.nodeName ()
//                                                     << " tag in " << parentNode.nodeName ()
//                                                     << "!</para>" << endl;
                    }
                }
            }

            if ( !found )
            {
                kdError (KDEBUG_TAGPROCESSING) << "Unexpected tag " << childNode.nodeName ()
                                               << " in " << parentNode.nodeName () << "!" << endl;
            }
        }
    }
}


void AllowNoSubtags ( QDomNode  myNode )
{
    QString outputText;
    QValueList<TagProcessing> tagProcessingList;
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.



void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList )
{
    QDomNamedNodeMap myAttribs ( myNode.toElement ().attributes () );

    int i, n;
    n = myAttribs.length ();

    for ( i = 0; i < n; i++ )
    {
        QDomAttr myAttrib ( myAttribs.item (i).toAttr () );

        if ( !myAttrib.isNull () )
        {
            bool found = false;

            QValueList<AttrProcessing>::Iterator attrProcessingIt;

            for ( attrProcessingIt = attrProcessingList.begin ();
                  !found && attrProcessingIt != attrProcessingList.end ();
                  attrProcessingIt++ )
            {
                if ( myAttrib.name () == (*attrProcessingIt).name )
                {
                    found = true;

                    if ( (*attrProcessingIt).data != NULL )
                    {
                        if ( (*attrProcessingIt).type == "QString" )
                        {
                            *((QString *) (*attrProcessingIt).data) = myAttrib.value ();
                        }
                        else if ( (*attrProcessingIt).type == "int" )
                        {
                            *((int *) (*attrProcessingIt).data) = myAttrib.value ().toInt ();
                        }
                        else if ( (*attrProcessingIt).type == "double" )
                        {
                            *((double *) (*attrProcessingIt).data) =  myAttrib.value ().toDouble ();
                        }
                        else
                        {
                            kdError (KDEBUG_TAGPROCESSING) << "Unexpected data type "
                                                           << (*attrProcessingIt).type << " in "
                                                           << myNode.nodeName () << " attribute "
                                                           << (*attrProcessingIt).name << "!" << endl;
                        }
                    }
                    else
                    {
//                      kdError (KDEBUG_TAGPROCESSING) << "<para>ignoring " << myNode.tagName
//                                      << " attribute " << (*attrProcessingIt).name
//                                      << "!</para>" << endl;
                    }
                }
            }

            if ( !found )
            {
                kdError (KDEBUG_TAGPROCESSING) << "Unexpected attribute " << myAttrib.name ()
                                               << " in " << myNode.nodeName () << "!" << endl;
            }
        }
    }
}


void AllowNoAttributes ( QDomNode  myNode )
{
    QValueList<AttrProcessing> attrProcessingList;
    ProcessAttributes (myNode, attrProcessingList);
}

