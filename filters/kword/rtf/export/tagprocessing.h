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
#ifndef TAGPROCESSING_H
#define TAGPROCESSING_H

#define KDEBUG_TAGPROCESSING 30505

#include <iostream.h>
#include <qdom.h>
#include <qstring.h>
#include <qvaluelist.h>


// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If you don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.

class TagProcessing
{
    public:
        TagProcessing () {}

        TagProcessing (QString  n,
                       void     (*p)(QDomNode, void *, QString &),
                       void    *d) : name (n), processor (p), data (d) {}

        QString  name;
        void     (*processor)(QDomNode, void *, QString &);
        void    *data;
};


class AttrProcessing
{
    public:
        AttrProcessing () {}

        AttrProcessing ( QString   n,
                         QString   t,
                         void     *d  ) : name (n), type (t), data (d) {}

        QString   name;
        QString   type;
        void     *data;
};

void ProcessSubtags ( QDomNode                    parentNode,
                      QValueList<TagProcessing>  &tagProcessingList,
                      QString                    &outputText         );

void AllowNoSubtags ( QDomNode  myNode );

void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList );

void AllowNoAttributes ( QDomNode  myNode );

#endif  // TAGPROCESSING_H
