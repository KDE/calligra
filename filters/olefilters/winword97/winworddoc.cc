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

#include <winworddoc.h>

WinWordDoc::WinWordDoc(QDomDocument &part, const myFile &mainStream,
                       const myFile &table0Stream, const myFile &table1Stream,
                       const myFile &dataStream) : m_part(part), m_main(mainStream),
                       m_data(dataStream) {

    m_success=true;
    m_ready=false;
    m_fib=0L;
    m_styleSheet=0L;
    m_ptSize=-1;      // it's safer that way
    readFIB();

    if(m_fib->fEncrypted==1) {
        kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::WinWordDoc(): Sorry - the document is encrypted.");
        m_success=false;
    }
    if(m_fib->fWhichTblStm==0)
        m_table=table0Stream;
    else
        m_table=table1Stream;

    m_styleSheet=new StyleSheet(m_table, m_fib);
    // print some debug info
    FIBInfo();
}

WinWordDoc::~WinWordDoc() {

    delete [] m_main.data;
    m_main.data=0L;
    delete [] m_table.data;
    m_table.data=0L;
    delete [] m_data.data;
    m_data.data=0L;
    delete m_fib;
    m_fib=0L;
    delete m_styleSheet;
    m_styleSheet=0L;
}

const bool WinWordDoc::convert() {

    if(!m_success || m_ready)
        return false;
    if(!locatePieceTbl())
        return false;
    if(!checkBinTables())
        return false;

    // Read & process the DOP [TODO],...

    if(m_fib->fComplex==0) {
        convertSimple();
    }
    else {
        convertComplex();
    }
    m_ready=true;
    return m_success;
}

const QDomDocument * const WinWordDoc::part() {
    if(m_ready && m_success)
        return &m_part;
    else {
        m_part.setContent(QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC >\n"
            "<DOC author=\"Reginald Stadlbauer and Torben Weis\" email=\"reggie@kde.org and weis@kde.org\" editor=\"KWord\" mime=\"application/x-kword\">\n"
            " <PAPER format=\"1\" ptWidth=\"595\" ptHeight=\"841\" mmWidth =\"210\" mmHeight=\"297\" inchWidth =\"8.26772\" inchHeight=\"11.6929\" orientation=\"0\" columns=\"1\" ptColumnspc=\"2\" mmColumnspc=\"1\" inchColumnspc=\"0.0393701\" hType=\"0\" fType=\"0\" ptHeadBody=\"9\" ptFootBody=\"9\" mmHeadBody=\"3.5\" mmFootBody=\"3.5\" inchHeadBody=\"0.137795\" inchFootBody=\"0.137795\">\n"
            "  <PAPERBORDERS mmLeft=\"10\" mmTop=\"15\" mmRight=\"10\" mmBottom=\"15\" ptLeft=\"28\" ptTop=\"42\" ptRight=\"28\" ptBottom=\"42\" inchLeft=\"0.393701\" inchTop=\"0.590551\" inchRight=\"0.393701\" inchBottom=\"0.590551\"/>\n"
            " </PAPER>\n"
            " <ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>\n"
            " <FOOTNOTEMGR>\n"
            "  <START value=\"1\"/>\n"
            "  <FORMAT superscript=\"1\" type=\"1\"/>\n"
            "  <FIRSTPARAG ref=\"(null)\"/>\n"
            " </FOOTNOTEMGR>\n"
            " <FRAMESETS>\n"
            "  <FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">\n"
            "   <FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n"
            "   <PARAGRAPH>\n"
            "    <TEXT>This filter is still crappy and it was not able to convert your document...</TEXT>\n"
            "    <INFO info=\"0\"/>\n"
            "    <HARDBRK frame=\"0\"/>\n"
            "    <FORMATS>\n"
            "    </FORMATS>\n"
            "    <LAYOUT>\n"
            "     <NAME value=\"Standard\"/>\n"
            "     <FOLLOWING name=\"Standard\"/>\n"
            "     <FLOW value=\"0\"/>\n"
            "     <OHEAD pt=\"0\" mm=\"0\" inch=\"0\"/>\n"
            "     <OFOOT pt=\"0\" mm=\"0\" inch=\"0\"/>\n"
            "     <IFIRST pt=\"0\" mm=\"0\" inch=\"0\"/>\n"
            "     <ILEFT pt=\"0\" mm=\"0\" inch=\"0\"/>\n"
            "     <LINESPACE pt=\"0\" mm=\"0\" inch=\"0\"/>\n"
            "     <COUNTER type=\"0\" depth=\"0\" bullet=\"176\" start=\"1\" numberingtype=\"1\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>\n"
            "     <LEFTBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>\n"
            "     <RIGHTBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>\n"
            "     <TOPBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>\n"
            "     <BOTTOMBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>\n"
            "     <FORMAT>\n"
            "      <COLOR red=\"0\" green=\"0\" blue=\"0\"/>\n"
            "      <FONT name=\"times\"/>\n"
            "      <SIZE value=\"12\"/>\n"
            "      <WEIGHT value=\"50\"/>\n"
            "      <ITALIC value=\"0\"/>\n"
            "      <UNDERLINE value=\"0\"/>\n"
            "      <VERTALIGN value=\"0\"/>\n"
            "     </FORMAT> \n"
            "    </LAYOUT>\n"
            "   </PARAGRAPH>\n"
            "  </FRAMESET>\n"
            "  </FRAMESETS>\n"
            "</DOC>\n"));
        return &m_part;
    }
}

void WinWordDoc::FIBInfo() {

    kdebug(KDEBUG_INFO, 31000, "WinWordDoc::FIBInfo() - start -----------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->wIdent))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->nFib))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->nProduct))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->lid))));
    kdebug(KDEBUG_INFO, 31000, "some bits -----------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fDot))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fGlsy))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fComplex))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fHasPic))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->cQuickSaves))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fEncrypted))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fWhichTblStm))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fReadOnlyRecommended))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fWriteReservation))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fExtChar))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fLoadOverride))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fFarEast))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fCrypto))));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->nFibBack))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->lKey))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->envr))));
    kdebug(KDEBUG_INFO, 31000, "bits - bits - bits -------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fMac))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fEmptySpecial))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fLoadOverridePage))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fFutureSavedUndo))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fWord97Saved))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fSpare0))));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->chs))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->chsTables))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fcMin))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib->fcMac))));
    kdebug(KDEBUG_INFO, 31000, "T-e-x-t-------------------");

    /*char *str=new char[m_fib->fcMac - m_fib->fcMin];
    int i, j;

    for(i=m_fib->fcMin, j=0;i<m_fib->fcMac;++i, ++j)
        str[j]=*(m_main.data+i);

    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(str));
    delete [] str;
    kdebug(KDEBUG_INFO, 31000, "WinWordDoc::FIBInfo() - end -----------------");
    */
}

void WinWordDoc::readFIB() {

    m_fib=new FIB;
    unsigned short *tmpS;
    unsigned long *tmpL;
    int i;

    m_fib->wIdent=read16(m_main.data);
    m_fib->nFib=read16(m_main.data+2);
    m_fib->nProduct=read16(m_main.data+4);
    m_fib->lid=read16(m_main.data+6);
    m_fib->pnNext=read16(m_main.data+8);
    tmpS=(unsigned short*)m_fib;
    tmpS+=5;
    *tmpS=read16(m_main.data+10);
    m_fib->nFibBack=read16(m_main.data+12);

    m_fib->lKey=read32(m_main.data+14);

    m_fib->envr=*(m_main.data+18);
    *((unsigned char*)m_fib+19)=*(m_main.data+19);

    m_fib->chs=read16(m_main.data+20);
    m_fib->chsTables=read16(m_main.data+22);

    m_fib->fcMin=read32(m_main.data+24);
    m_fib->fcMac=read32(m_main.data+28);

    m_fib->csw=read16(m_main.data+32);

    tmpS=(unsigned short*)&m_fib->wMagicCreated;
    for(i=0; i<15; ++i, ++tmpS)
        *tmpS=read16(m_main.data+34+2*i);

    tmpL=(unsigned long*)&m_fib->cbMac;
    for(i=0; i<22; ++i, ++tmpL)
        *tmpL=read32(m_main.data+64+4*i);

    m_fib->cfclcb=read16(m_main.data+152);

    tmpL=(unsigned long*)&m_fib->fcStshfOrig;
    for(i=0; i<186; ++i, ++tmpL)
        *tmpL=read32(m_main.data+154+4*i);
}

void WinWordDoc::convertSimple() {
    m_success=false;
}

void WinWordDoc::convertComplex() {
    m_success=false;
}

const PCD WinWordDoc::readPCD(const long &pos) {

    PCD ret;

    if(m_ptSize==-1)
        return ret;       // undefined, so don't call this one
                          // before you call locatePieceTbl()!

    long tmpPos=m_ptPCDBase+pos*8;
    unsigned short *tmp=(unsigned short*)&ret;

    *tmp=read16(m_table.data+tmpPos);
    ret.fc=read32(m_table.data+tmpPos+2);
    if((ret.fc & 0x40000000) == 0x40000000) {
        ret.fc=(ret.fc & 0xBFFFFFFF)/2;
        ret.unicode=false;
    }
    else
        ret.unicode=true;
    ret.prm=read16(m_table.data+tmpPos+6);
    return ret;
}

const bool WinWordDoc::locatePieceTbl() {

    long tmp=m_fib->fcClx;
    QString r;
    bool found=false;

    while(*(m_table.data+tmp)==1 && tmp<static_cast<long>(m_fib->fcClx+m_fib->lcbClx))
        tmp+=read16(m_table.data+tmp+1)+3;

    if(*(m_table.data+tmp)==2) {
        found=true;
        m_ptCPBase=tmp+1;
        m_ptSize=read32(m_table.data+m_ptCPBase);
        m_ptCPBase+=4;
        if((m_ptSize-4)%12!=0) {
            kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::locatePieceTbl(): Sumting Wong (inside joke(tm))");
            found=false;
        }
        m_ptCount=static_cast<long>((m_ptSize-4)/12);
        m_ptPCDBase=(m_ptCount+1)*4+m_ptCPBase;
    }
    else {
        m_success=false;
        kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::locatePieceTbl(): Can't locate the piece table");
    }
    return found;
}

const bool WinWordDoc::checkBinTables() {

    bool notCompressed=false;
    if(m_fib->pnFbpChpFirst==0xfffff && m_fib->pnFbpPapFirst==0xfffff &&
       m_fib->pnFbpLvcFirst==0xfffff)
        notCompressed=true;
    else {
        kdebug(KDEBUG_INFO, 31000, "WinWordDoc::checkBinTables(): Sigh! It's compressed...");
        m_success=false;
    }
    return notCompressed;
}
