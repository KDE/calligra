/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION

    This file implements a simplified abstraction of Microsoft Powerpoint documents.
*/

#ifndef PPTDOC_H
#define PPTDOC_H

#include <kdebug.h>
#include <myfile.h>
#include <powerpoint.h>
#include <qstring.h>
#include <qstringlist.h>

class myFile;

class PptDoc:
    protected Powerpoint
{
protected:

    // Construction. Invoke with the OLE streams that comprise the Powerpoint document.

    PptDoc(
        const myFile &mainStream,
        const myFile &currentUser,
        const myFile &pictures);
    virtual ~PptDoc();

    // Metadata.

    QString getAuthor(void) const;

    // Call the parse() function to process the document. The callbacks return
    // the text along with any relevant attributes.

    bool parse();

    virtual void gotDrawing(
        unsigned id,
        QString type,
        unsigned length,
        const char *data) = 0;
    virtual void gotSlide(PptSlide &slide) = 0;

private:
    PptDoc(const PptDoc &);
    const PptDoc &operator=(const PptDoc &);

    // Error handling and reporting support.

    static const int s_area;

    myFile m_mainStream;
    myFile m_currentUser;
    myFile m_pictures;

    // Override the base class functions.

    //void gotSlide(
    //    PptSlide &slide);
};
#endif
