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

#ifndef DOCINFOEXPORT_H
#define DOCINFOEXPORT_H

#include <qdom.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <kdebug.h>
#include <tagprocessing.h>

/***************************************************************************/

struct BookInfo
{
   QString title;
   QString abstract;
   QString fullName;
   QString jobTitle;
   QString company;
   QString email;
   QString telephone;
   QString fax;
   QString country;
   QString postalCode;
   QString city;
   QString street;
};  // end BookInfo

/***************************************************************************/

QString ProcessDocumentData ( BookInfo bookInfo );


void ProcessDocumentInfoTag ( QDomNode   myNode,
                              void      *,
                              QString   &outputText );


void ProcessAboutTag ( QDomNode   myNode,
                       void      *tagData,
                       QString   &outputText );

void ProcessAuthorTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText );

void ProcessTextTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         );

#endif  // DOCINFOEXPORT_H

