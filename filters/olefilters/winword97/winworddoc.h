/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <qstring.h>
#include <qstrlist.h>
#include <paragraph.h>
#include <stylesheet.h>
#include <myfile.h>
#include <misc.h>
#include <fib.h>
#include <pcd.h>
#include <kdebug.h>

class WinWordDoc {

public:
    WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
               const myFile &table1Stream, const myFile &dataStream);
    ~WinWordDoc();

    const bool isOk() { return success; }

    const bool convert();
    const QString part();

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    void FIBInfo();
    void readFIB();
    const PCD readPCD(const long &pos);
    const bool locatePieceTbl();
    const bool checkBinTables();

    bool success, ready;
    FIB *fib;
    myFile main, table, data;
    QString _part;
    QStrList mainParas;
    StyleSheet *styleSheet;

    // piece table (pt)
    long ptCPBase, ptSize, ptCount, ptPCDBase;
};
#endif // WINWORDDOC_H
