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

WinWordDoc::WinWordDoc(
    QDomDocument &part,
    const myFile &mainStream,
    const myFile &table0Stream,
    const myFile &table1Stream,
    const myFile &dataStream) : 
	MsWord(
            mainStream.data,
            table0Stream.data,
            table1Stream.data,
            dataStream.data),
	m_part(part), m_main(mainStream), m_data(dataStream)
{
    m_success=true;
    m_ready=false;
    m_styleSheet=0L;
    m_atrdCount=0;
    m_bkfCount=0;
    m_bklCount=0;

    if(m_fib.fWhichTblStm==0)
        m_table=table0Stream;
    else
        m_table=table1Stream;

    //m_styleSheet=new StyleSheet(m_table, &m_fib);
    m_body = QString("");
    // print some debug info
    // FIBInfo();
}

WinWordDoc::~WinWordDoc() {

    delete [] m_main.data;
    m_main.data=0L;
    delete [] m_table.data;
    m_table.data=0L;
    delete [] m_data.data;
    m_data.data=0L;
    //delete m_styleSheet;
    m_styleSheet=0L;

    m_grpXst.stringList.clear();
    for(unsigned char **tmp=m_grpXst.extraData.first(); tmp!=0L; tmp=m_grpXst.extraData.next()) {
        delete [] *tmp;
        *tmp=0L;
    }
    m_grpXst.extraData.clear();

    m_atnbkmk.stringList.clear();
    for(unsigned char **tmp=m_atnbkmk.extraData.first(); tmp!=0L; tmp=m_atnbkmk.extraData.next()) {
        delete [] *tmp;
        *tmp=0L;
    }
    m_atnbkmk.extraData.clear();

    m_assocStrings.stringList.clear();
    for(unsigned char **tmp=m_assocStrings.extraData.first(); tmp!=0L; tmp=m_assocStrings.extraData.next()) {
        delete [] *tmp;
        *tmp=0L;
    }

    m_assocStrings.extraData.clear();
}

const bool WinWordDoc::convert() {

    if(!m_success || m_ready)
        return false;
//    if(!checkBinTables())         // are the bin tables ok (==not compressed)
//        return false;
    locateATRD();
    locateBKF();
    locateBKL();
    //readCommentStuff();
    //sttbf(m_assocStrings, m_fib.fcSttbfAssoc, m_fib.lcbSttbfAssoc, m_table.data);

    // FFN, FRD - TODO(?)

    getParagraphs();

    m_success=true;

    return m_success;
}

void WinWordDoc::gotParagraph(const QString &text)
{
    m_body.append("<PARAGRAPH><TEXT>");
    m_body.append(text);
    m_body.append("</TEXT></PARAGRAPH>\n");
}

const QDomDocument * const WinWordDoc::part() {
    if(m_ready && m_success)
        return &m_part;
    else {
        QString newstr;

        newstr=QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC >\n"
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
            "   <FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n");
        if (m_success)
            newstr.append(m_body);
        else
            newstr.append(
            "   <PARAGRAPH>\n"
            "    <TEXT>This filter is still crappy and it obviously was not able to convert your document...</TEXT>\n"
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
            "   </PARAGRAPH>\n");
        newstr.append(
            "  </FRAMESET>\n"
            "  </FRAMESETS>\n"
            "</DOC>\n");
        m_part.setContent(newstr);
        m_ready=true;
        return &m_part;
    }
}

/*
const ATRD WinWordDoc::atrd(const long &pos) {

    ATRD atrd;
    atrd.ok=false;
    if(m_atrdCount!=0) {
        unsigned long base=m_atrdBase+pos*30;
        for(unsigned short i=0;i<10;++i)
                atrd.xstUsrInitl[i]=read16(m_table.data+base+i);
            atrd.ibst=read16(m_table.data+base+20);
            atrd.ak=read16(m_table.data+base+22);
            atrd.grfbmc=read16(m_table.data+base+24);
            atrd.lTagBkmk=read32(m_table.data+base+26);
            atrd.ok=true;
    }
    return atrd;
}

const BKF WinWordDoc::bkf(const long &pos) {

    BKF bkf;
    bkf.ok=false;
    if(m_bkfCount!=0) {
        unsigned long base=m_bkfBase+pos*4;
        unsigned short *tmp;
        bkf.ibkl=read16(m_table.data+base);
        tmp=(unsigned short*)&bkf;
        ++tmp;
        *tmp=read16(m_table.data+base+2);
        bkf.ok=true;
    }
    return bkf;
}

const BKL WinWordDoc::bkl(const long &pos) {

    BKL bkl;
    bkl.ok=false;
    if(m_bklCount!=0) {
        bkl.ibkf=read16(m_table.data+m_bklBase+pos*4);
        bkl.ok=true;
    }
    return bkl;
}
*/
void WinWordDoc::sttbf(STTBF &sttbf, const unsigned long &fc, const unsigned long &lcb,
                              const unsigned char * const stream) {

    if(lcb==0) {
        //kDebugInfo(31000, "WinWordDoc::sttbf(): empty STTBF");
        sttbf.ok=false;
        return;
    }

    QString str;
    unsigned long base=fc+4;
    unsigned short len, i, j;
    bool unicode=false;
    unsigned short numStrings=read16(stream+fc);
    sttbf.extraDataLen=read16(stream+fc+2);

    if(numStrings==0xffff) {
        kDebugInfo(31000, "WinWordDoc::sttbf(): extended...");
        unicode=true;
        base=fc+6;
        numStrings=sttbf.extraDataLen;
        sttbf.extraDataLen=read16(stream+fc+4);
    }

    // TODO: unroll this loops :)
    if(unicode) {
        for(i=0; i<numStrings; ++i) {
            len=read16(stream+base);
            base+=2;
            if(len==0) {
                sttbf.stringList.append(QString(""));
                //kDebugInfo(31000, "WinWordDoc::sttbf(): empty string");
            }
            else {
                str="";

                for(j=0; j<len*2; j+=2)
                    str+=QChar(read16(stream+base+j));

                sttbf.stringList.append(str);
                kDebugInfo(31000, str);
                base+=j;

                if(sttbf.extraDataLen!=0) {
                    kDebugInfo(31000, "WinWordDoc::sttbf(): extra data");
                    unsigned char *tmpArray=new unsigned char[sttbf.extraDataLen];
                    kDebugInfo(31000, "STILL ALIVE 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    kDebugInfo(31000, (const char*)QString::number((long)sttbf.extraDataLen));
                    for(j=0; j<sttbf.extraDataLen; ++base)
                        tmpArray[j]=*(stream+base);
                    kDebugInfo(31000, "STILL ALIVE 3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    sttbf.extraData.append(&tmpArray);
                    kDebugInfo(31000, "STILL ALIVE 4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                }
            }
        }
    }
    else {
        for(i=0; i<numStrings; ++i) {
            len=*(stream+base);
            ++base;
            if(len==0) {
                sttbf.stringList.append(QString(""));
                //kDebugInfo(31000, "WinWordDoc::sttbf(): empty string");
            }
            else {
                str="";

                for(j=0; j<len; ++j)
                    str+=QChar(char2uni(*(stream+base+j)));

                sttbf.stringList.append(str);
                kDebugInfo(31000, str);
                base+=j;
                
                if(sttbf.extraDataLen!=0) {
                    kDebugInfo(31000, "WinWordDoc::sttbf(): extra data");
                    unsigned char *tmpArray=new unsigned char[sttbf.extraDataLen];
                    kDebugInfo(31000, "STILL ALIVE 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    kDebugInfo(31000, (const char*)QString::number((long)sttbf.extraDataLen));
                    for(j=0; j<sttbf.extraDataLen; ++base)
                        tmpArray[j]=*(stream+base);
                    kDebugInfo(31000, "STILL ALIVE 3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    sttbf.extraData.append(&tmpArray);
                    kDebugInfo(31000, "STILL ALIVE 4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                }
            }
        }
    }
    sttbf.ok=true;
}

void WinWordDoc::FIBInfo() {

    kDebugInfo(31000, "WinWordDoc::FIBInfo() - start -----------------");
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.wIdent))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.nFib))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.nProduct))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.lid))));
    kDebugInfo(31000, "some bits -----------------");
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fDot))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fGlsy))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fComplex))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fHasPic))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.cQuickSaves))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fEncrypted))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fWhichTblStm))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fReadOnlyRecommended))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fWriteReservation))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fExtChar))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fLoadOverride))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fFarEast))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fCrypto))));
    kDebugInfo(31000, "--------------------------");
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.nFibBack))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.lKey))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.envr))));
    kDebugInfo(31000, "bits - bits - bits -------");
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fMac))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fEmptySpecial))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fLoadOverridePage))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fFutureSavedUndo))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fWord97Saved))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fSpare0))));
    kDebugInfo(31000, "--------------------------");
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.chs))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.chsTables))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fcMin))));
    kDebugInfo(31000, static_cast<const char*>(QString::number(static_cast<long>(m_fib.fcMac))));
    kDebugInfo(31000, "--------------------------");
}

void WinWordDoc::locateATRD() {

    if(m_fib.lcbPlcfandRef!=0) {
        m_atrdCount=static_cast<unsigned short>((m_fib.lcbPlcfandRef-4)/34); // 34 == (sizeof(ATRD)==30 + sizeof(long))
        m_atrdBase=m_fib.fcPlcfandRef+(m_atrdCount+1)*4;
    }
}

void WinWordDoc::locateBKF() {

    if(m_fib.lcbPlcfAtnbkf!=0) {
        m_bkfCount=static_cast<unsigned short>((m_fib.lcbPlcfAtnbkf-4)/8);
        m_bkfBase=m_fib.fcPlcfAtnbkf+(m_bkfCount+1)*4;
    }
}

void WinWordDoc::locateBKL() {

    if(m_fib.lcbPlcfAtnbkl!=0) {
        m_bklCount=static_cast<unsigned short>((m_fib.lcbPlcfAtnbkl-4)/6);
        m_bklBase=m_fib.fcPlcfAtnbkl+(m_bklCount+1)*4;
    }
}

const bool WinWordDoc::checkBinTables() {

    bool notCompressed=false;
    if(m_fib.pnFbpChpFirst==0xfffff && m_fib.pnFbpPapFirst==0xfffff &&
       m_fib.pnFbpLvcFirst==0xfffff)
        notCompressed=true;
    else {
        kDebugInfo(31000, "WinWordDoc::checkBinTables(): Sigh! It's compressed...");
        m_success=false;
    }
    return notCompressed;
}

void WinWordDoc::readCommentStuff() {
    sttbf(m_grpXst, m_fib.fcGrpXstAtnOwners, m_fib.lcbGrpXstAtnOwners, m_table.data);
    sttbf(m_atnbkmk, m_fib.fcSttbfAtnbkmk, m_fib.lcbSttbfAtnbkmk, m_table.data);
}






