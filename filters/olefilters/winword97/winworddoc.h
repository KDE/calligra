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
#include <qdom.h>

#include <paragraph.h>
#include <stylesheet.h>
#include <section.h>
#include <myfile.h>
#include <misc.h>
#include <fib.h>
#include <pcd.h>

#include <kdebug.h>

class WinWordDoc {

public:
    WinWordDoc(QDomDocument &part, const myFile &mainStream,
               const myFile &table0Stream, const myFile &table1Stream,
               const myFile &dataStream);
    ~WinWordDoc();

    const bool isOk() const { return m_success; }

    const bool convert();
    const QDomDocument * const part();

    const PCD readPCD(const long &pos);

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    void FIBInfo();
    void readFIB();

    void convertSimple();
    void convertComplex();

    const bool locatePieceTbl();
    const bool checkBinTables();
    bool m_success, m_ready;
    FIB *m_fib;
    QDomDocument m_part;
    myFile m_main, m_table, m_data;
    StyleSheet *m_styleSheet;

    // piece table (pt)
    long m_ptCPBase, m_ptSize, m_ptCount, m_ptPCDBase;
};
#endif // WINWORDDOC_H
