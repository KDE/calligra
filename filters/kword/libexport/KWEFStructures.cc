// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <nicog@snafu.de>
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

#include <kdebug.h>

#include "KWEFStructures.h"


TableCell::~TableCell ( void )
{
#if 0
    if ( paraList) delete paraList;
#endif
}


void Table::addCell ( int                   c,
                      int                   r,
                      QValueList<ParaData> &p  )
{
   if ( c + 1 > cols )
   {
      cols = c + 1;
   }

   cellList << TableCell ( c, r, new QValueList<ParaData> (p) );
}


void CreateMissingFormatData (QString &paraText, ValueListFormatData &paraFormatDataList)
{
    ValueListFormatData::Iterator  paraFormatDataIt;
    int lastPos=0;   // last position

    for (paraFormatDataIt = paraFormatDataList.begin ();
        paraFormatDataIt != paraFormatDataList.end ();
        paraFormatDataIt++)
    {
        if ( (*paraFormatDataIt).pos > lastPos )
        {
#if 0
            kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                            << ", pos = " << (*paraFormatDataIt).pos
                            << ", len = " << (*paraFormatDataIt).len << " (bad)" << endl;
#endif

            // We must add a FormatData
            paraFormatDataList.insert ( paraFormatDataIt,
                FormatData ( lastPos, (*paraFormatDataIt).pos - lastPos, true ) );
        }
#if 0
        else
        {
            kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                            << ", pos = " << (*paraFormatDataIt).pos
                            << ", len = " << (*paraFormatDataIt).len << " (ok)" << endl;
        }
#endif


        /* A check for pos < lastPos might be interesting at this point */


        lastPos = (*paraFormatDataIt).pos + (*paraFormatDataIt).len;

    }

    // Add the last one if needed
    if ( (int) paraText.length () > lastPos )
    {
#if 0
        kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                        << ", total len = " << paraText.length () << " (bad)" << endl;
#endif

        paraFormatDataList.append (
            FormatData ( lastPos, paraText.length () - lastPos, true ) );
    }
#if 0
    else
    {
        kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                        << ", total len = " << paraText.length () << " (ok)" << endl;
    }
#endif
}
