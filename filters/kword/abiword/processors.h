/* $Header$ */

/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
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

/* 
   19 Jan 2001  Nicolas GOUTTE <nicog@snafu.de>
        Extracting the code from file:
           /home/kde/koffice/filters/kword/ascii/asciiexport.cc
        and breaking the code into two new files:
           /home/kde/koffice/filters/kword/abiword/processors.cc 
	   /home/kde/koffice/filters/kword/abiword/processors.h

   19 Jan 2001  Nicolas GOUTTE <nicog@snafu.de>
        New functions ending with Dom
*/

// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.


#ifndef PROCESSORS_H
#define PROCESSORS_H

class TagProcessing
{
    public:
        TagProcessing ()
        {}

        TagProcessing (QString  n,
                       void     (*p)(QDomNode, void *, QString &),
                       void    *d) : name (n), processor (p), data (d)
        {}

        QString  name;
        void     (*processor)(QDomNode, void *, QString &);
        void    *data;
};

class TagProcessingDom
{
    public:
        TagProcessingDom ()
        {}

        TagProcessingDom (QString  n,
                         void     (*p)(QDomNode, void *, QDomNode&),
                         void    *d) : name (n), processor (p), data (d)
        {}

        QString  name;
        void     (*processor)(QDomNode, void *, QDomNode&);
        void    *data;
};

void ProcessSubtags     ( QDomNode                   parentNode,
                          QValueList<TagProcessing>  &tagProcessingList,
                          QString                    &outputText         );

void ProcessSubtagsDom  ( QDomNode                      parentNode,
                          QValueList<TagProcessingDom>  &tagProcessingList,
                          QDomNode                      &outputQDomNode );

void AllowNoSubtags ( QDomNode  myNode );


// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.


class AttrProcessing
{
    public:
        AttrProcessing ()
        {}

        AttrProcessing ( QString   n,
                         QString   t,
                         void     *d  ) : name (n), type (t), data (d)
        {}

        QString   name;
        QString   type;
        void     *data;
};

void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList );

void AllowNoAttributes ( QDomNode  myNode );

#endif // PROCESSORS_H
