/* This file is part of the KDE project
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>

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

#include <docinfoExport.h>

// Every tag has its own processing function. All of those functions
// have the same parameters since the functions are passed to
// ProcessSubtags throuch the TagProcessing class.  The top level
// function is ProcessDocTag and can be called with the node returned
// by QDomDocument::documentElement (). The tagData argument can be
// used to either pass variables down to the subtags or to allow
// subtags to return values. As a bare minimum the tag processing
// functions must handle the tag's attributes and the tag's subtags
// (which it can choose to ignore). Currently implemented is
// processing for the following tags and attributes:

//
// documentinfo.xml:
// AUTHOR
//   FULL-NAME - Text Element
//   TITLE - Text Element
//   COMPANY - Text Element
//   EMAIL - Text Element
//   TELEPHONE - Text Element
//   FAX - Text Element
//   COUNTRY - Text Element
//   POSTAL-CODE - Text Element
//   CITY - Text Element
//   STREET - Text Element
// ABOUT
//   TITLE - Text Element
//   ABSTRACT - Text Element



/***************************************************************************/

// This function parses out the documentinfo about information
// called by ProcessDocumentInfoTag
/***************************************************************************/

void ProcessAboutTag ( QDomNode   myNode,
                                      void      *tagData,
                                      QString   &outputText )
{
    BookInfo *bookInfo = (BookInfo *) tagData;

    AllowNoAttributes (myNode);

    (*bookInfo).title    = "";
    (*bookInfo).abstract = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "title", ProcessTextTag, (void *) &(*bookInfo).title    )
                      << TagProcessing ( "abstract",ProcessTextTag, (void *) &(*bookInfo).abstract );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}  // end ProcessAboutTag()

/***************************************************************************/

// The following function parses out the documentinfo author information
// called by ProcessDocumentInfoTag
/***************************************************************************/

void ProcessAuthorTag ( QDomNode   myNode,
                                       void      *tagData,
                                       QString   &outputText )
{
    BookInfo *bookInfo = (BookInfo *) tagData;

    AllowNoAttributes (myNode);

    (*bookInfo).fullName   = "";
    (*bookInfo).jobTitle   = "";
    (*bookInfo).company    = "";
    (*bookInfo).email      = "";
    (*bookInfo).telephone  = "";
    (*bookInfo).fax        = "";
    (*bookInfo).country    = "";
    (*bookInfo).postalCode = "";
    (*bookInfo).city       = "";
    (*bookInfo).street     = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "full-name",   ProcessTextTag, (void *) &(*bookInfo).fullName   )
                      << TagProcessing ( "title",       ProcessTextTag, (void *) &(*bookInfo).jobTitle   )
                      << TagProcessing ( "company",     ProcessTextTag, (void *) &(*bookInfo).company    )
                      << TagProcessing ( "email",       ProcessTextTag, (void *) &(*bookInfo).email      )
                      << TagProcessing ( "telephone",   ProcessTextTag, (void *) &(*bookInfo).telephone  )
                      << TagProcessing ( "fax",         ProcessTextTag, (void *) &(*bookInfo).fax        )
                      << TagProcessing ( "country",     ProcessTextTag, (void *) &(*bookInfo).country    )
                      << TagProcessing ( "postal-code", ProcessTextTag, (void *) &(*bookInfo).postalCode )
                      << TagProcessing ( "city",        ProcessTextTag, (void *) &(*bookInfo).city       )
                      << TagProcessing ( "street",      ProcessTextTag, (void *) &(*bookInfo).street     );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}  // end ProcessAuthorTag()


/***************************************************************************/


void ProcessDocumentInfoTag ( QDomNode   myNode,
                                             void      *,
                                             QString   &outputText )
{
        kdError (KDEBUG_TAGPROCESSING) << " Starting DOCinfo " << endl;
    AllowNoAttributes (myNode);

    BookInfo bookInfo;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "log",    NULL,             NULL               )
                      << TagProcessing ( "author", ProcessAuthorTag, (void *) &bookInfo )
                      << TagProcessing ( "about",  ProcessAboutTag,  (void *) &bookInfo );
        kdError (KDEBUG_TAGPROCESSING) << "  DOCinfo list formed " << endl;
    ProcessSubtags (myNode, tagProcessingList, outputText);
        kdError (KDEBUG_TAGPROCESSING) << " DOCinfo processed " << endl;

    // Process the parsed data into markup
    ProcessDocumentData ( bookInfo );


} // end ProcessDocumrntInfoTag()

/***************************************************************************/
