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
#include <atrd.h>
#include <sttbf.h>
#include <bkf.h>
#include <bkl.h>
#include <ffn.h>
#include <frd.h>

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

    /////////////////////////////////////////////////////////////
    const PCD pcd(const long &pos);
    const unsigned short numPCD() { return m_pcdCount; }

    const ATRD atrd(const long &pos);
    const unsigned short numATRD() { return m_atrdCount; }

    const BKF bkf(const long &pos);
    const unsigned short numBKF() { return m_bkfCount; }

    const BKL bkl(const long &pos);
    const unsigned short numBKL() { return m_bklCount; }

    void sttbf(STTBF &sttbf, const unsigned long &fc, const unsigned long &lcb,
               const unsigned char * const stream);
    //////////////////////////////////////////////////////////

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    void FIBInfo();
    void readFIB();

    void convertSimple();
    void convertComplex();

    const bool locatePCD();
    void locateATRD();
    void locateBKF();
    void locateBKL();

    const bool checkBinTables();
    void browseDop();
    void readCommentStuff();

    bool m_success, m_ready;
    FIB *m_fib;
    QDomDocument m_part;
    myFile m_main, m_table, m_data;

    // Stylesheet
    StyleSheet *m_styleSheet;
    // Piece table (pt), PCD stuff
    unsigned long m_pcdCount, m_pcdCPBase, m_pcdPCDBase;
    // ATRD
    unsigned long m_atrdBase, m_atrdCount;
    // BKF
    unsigned long m_bkfBase, m_bkfCount;
    // BKL
    unsigned long m_bklBase, m_bklCount;

    STTBF m_grpXst, m_atnbkmk, m_assocStrings;
};
#endif // WINWORDDOC_H
