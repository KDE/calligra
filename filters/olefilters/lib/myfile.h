/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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

    A data-structure to hold the file data for an OLE stream. Shared storage is
    used to implement reference counting and storage reclamation.
*/

#ifndef MYFILE_H
#define MYFILE_H

#include <qcstring.h>

class myFile: public QByteArray
{
public:
    myFile() { data = 0L; length = 0; }

    // NOTE: this implementation may look completely ugly, but its features are
    // mostly for backwards compatibility. If you feel like cleaning up, be my
    // guest!

    const unsigned char *data;
    unsigned int length;

    void setRawData(const unsigned char *data, unsigned length)
    {
        this->data = data;
        this->length = length;
        QByteArray::setRawData((const char *)data, length);
    }
};
#endif // MYFILE_H
