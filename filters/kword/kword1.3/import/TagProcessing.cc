// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>
   Copyright (c) 2001 IABG mbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@IABG.de>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@GMX.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <kdebug.h>
#include <qdom.h>

#include <TagProcessing.h>

//#define DEBUG_KWORD_TAGS


// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.

void ProcessSubtags ( QDomNode                   parentNode,
                      QValueList<TagProcessing>  &tagProcessingList,
                      KWEFKWordLeader            *leader)
{
    //kdDebug(30508) << "Starting ProcessSubtags for node: " << parentNode.nodeName() << endl;

    QDomNode childNode;

    for ( childNode = parentNode.firstChild (); !childNode.isNull (); childNode = childNode.nextSibling () )
    {
        if ( childNode.isElement () )
        {
            bool found = false;

            QValueList<TagProcessing>::Iterator  tagProcessingIt;

            for ( tagProcessingIt = tagProcessingList.begin ();
                  tagProcessingIt != tagProcessingList.end ();
                  tagProcessingIt++ )
            {
                if ( childNode.nodeName () == (*tagProcessingIt).name )
                {
                    found = true;

                    if ( (*tagProcessingIt).processor != NULL )
                    {
                        ((*tagProcessingIt).processor) ( childNode, (*tagProcessingIt).data, leader );
                    }
#ifdef DEBUG_KWORD_TAGS
                    else
                    {
                        kdWarning (30508) << "Ignoring " << childNode.nodeName ()
                            << " tag in " << parentNode.nodeName () << endl;
                    }
#endif
                    break;
                }
            }

            if ( !found )
            {
                kdWarning(30508) << "Unexpected tag " << childNode.nodeName ()
                    << " in " << parentNode.nodeName () << "!" << endl;
            }
        }
    }
    //kdDebug(30508) << "Ending ProcessSubtags for node: " << parentNode.nodeName() << endl;
}

#ifdef DEBUG_KWORD_TAGS
// Version for debugging (process all sub tags)
void AllowNoSubtags ( QDomNode myNode, KWEFKWordLeader *leader )
{
    QString outputText;
    QValueList<TagProcessing> tagProcessingList;
    ProcessSubtags (myNode, tagProcessingList, leader);
}
#else
// Normal version: no subtags expected, so do not search any!
void AllowNoSubtags ( QDomNode, KWEFKWordLeader* )
{
}
#endif

// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retrieve the value in the datatype defined, and do all
// the necessary error handling.

void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList )
{
    //kdDebug(30508) << "Starting ProcessAttributes for node: " << myNode.nodeName() << endl;

    QDomNamedNodeMap myAttribs ( myNode.attributes () );
    //kdDebug(30508) << "Attributes = " << myAttribs.length () <<endl;
    for ( uint i = 0; i <  myAttribs.length (); i++ )
    {
        QDomAttr myAttrib ( myAttribs.item (i).toAttr () );

        if ( !myAttrib.isNull () )
        {
            bool found = false;

            QValueList<AttrProcessing>::Iterator attrProcessingIt;

            for ( attrProcessingIt = attrProcessingList.begin ();
                  attrProcessingIt != attrProcessingList.end ();
                  attrProcessingIt++ )
            {
              //kdDebug(30508) << "NAME: " << myAttrib.name () << " == " << (*attrProcessingIt).name <<endl;
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
                            *((double *) (*attrProcessingIt).data) = myAttrib.value ().toDouble ();
                        }
                        else if ( (*attrProcessingIt).type == "bool" )
                        {
                            QString strAttr=myAttrib.value().simplifyWhiteSpace();
                            bool flag;
                            if ((strAttr=="yes")||(strAttr=="1")||(strAttr=="true"))
                            {
                                flag=true;
                            }
                            else if ((strAttr=="no")||(strAttr=="0")||(strAttr=="false"))
                            {
                                flag=false;
                            }
                            else
                            {
                                flag=false;
                                kdWarning(30508) << "Unknown value for a boolean: " << strAttr
                                    << " in tag " << myNode.nodeName () << ", attribute "
                                    << myAttrib.name() << endl;
                            }
                            *((bool *) (*attrProcessingIt).data) = flag;
                        }
                        else
                        {
                            kdWarning(30508) << "Unexpected data type " << (*attrProcessingIt).type
                                << " in " << myNode.nodeName ()
                                << " attribute " << (*attrProcessingIt).name
                                << "!" << endl;
                        }
                    }
#ifdef DEBUG_KWORD_TAGS
                    else
                    {
                        kdWarning(30508) << "Ignoring " << myNode.nodeName()
                            << " attribute " << (*attrProcessingIt).name
                            << endl;
                    }
#endif
                    break;
                }
            }

            if ( !found )
            {
                kdWarning(30508) << "Unexpected attribute " << myAttrib.name ()
                    << " in " << myNode.nodeName () << "!" << endl;
            }
        }
    }
    //kdDebug(30508) << "Ending ProcessAttributes for node: " << myNode.nodeName() << endl;
}

#ifdef DEBUG_KWORD_TAGS
// Version for debugging (process all attributes)
void AllowNoAttributes ( QDomNode  myNode )
{
    QValueList<AttrProcessing> attrProcessingList;
    ProcessAttributes (myNode, attrProcessingList);
}
#else
// Normal version: no attributes expected, so do not process any!
void AllowNoAttributes ( QDomNode )
{
}
#endif
