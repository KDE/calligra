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
#include <qregexp.h>

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
        m_part(part)
{
    m_phase = INIT;
    m_success = TRUE;
    m_body = QString("");
    m_tableManager = 0;
}

WinWordDoc::~WinWordDoc()
{
}

const bool WinWordDoc::convert()
{
    // We do the conversion in two passes, to allow all the tables to be turned into framesets
    // after the main frameset with the text.

    if (m_phase == INIT)
    {
        m_body.append(
            "  <FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">\n"
            "   <FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n");
        m_phase = TEXT_PASS;
        m_tableManager = 0;
        parse();
        m_body.append(
            "  </FRAMESET>\n");
        if (m_success)
        {
            m_phase = TABLE_PASS;
            m_tableManager = 0;
            parse();
        }
    }

    if (m_phase != DONE)
    {
        QString newstr;

        newstr = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE DOC >\n"
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
            " <FRAMESETS>\n");
        if (!m_success)
            newstr.append(
            "  <FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">\n"
            "   <FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n"
            "   <PARAGRAPH>\n"
            "    <TEXT>This filter is still crappy and it obviously was not able to convert your document.</TEXT>\n"
            "   </PARAGRAPH>\n"
            "  </FRAMESET>\n");
        newstr.append(m_body);
        newstr.append(
            "  </FRAMESETS>\n"
            "</DOC>\n");
        m_part.setContent(newstr);
        m_phase = DONE;
    }
    return m_success;
}

void WinWordDoc::encode(QString &text)
{
  // When encoding the stored form of text to its run-time form,
  // be sure to do the conversion for "&amp;" to "&" first to avoid
  // accidentally converting user text into one of the other escape
  // sequences.

  text.replace(QRegExp("&"), "&amp;");
  text.replace(QRegExp("<"), "&lt;");
  text.replace(QRegExp(">"), "&gt;");
}

void WinWordDoc::gotError(const QString &text)
{
    if (m_phase == TEXT_PASS)
    {
        QString xml_friendly = text;

        encode(xml_friendly);
        m_body.append("<PARAGRAPH>\n<TEXT>");
        m_body.append(xml_friendly);
        m_body.append("</TEXT>\n</PARAGRAPH>\n");
    }
    m_success = false;
}

void WinWordDoc::gotParagraph(const QString &text, PAP &style)
{
    if (m_phase == TEXT_PASS)
    {
        QString xml_friendly = text;

        encode(xml_friendly);
        m_body.append("<PARAGRAPH>\n<TEXT>");
        m_body.append(xml_friendly);
        m_body.append("</TEXT>\n</PARAGRAPH>\n");
    }
}

void WinWordDoc::gotHeadingParagraph(const QString &text, PAP &style)
{
    if (m_phase == TEXT_PASS)
    {
        QString xml_friendly = text;

        encode(xml_friendly);
        m_body.append("<PARAGRAPH>\n<TEXT>");
        m_body.append(xml_friendly);
        m_body.append("</TEXT>\n"
            " <LAYOUT>\n"
            "  <NAME value=\"Head ");
        m_body.append(QString::number(style.istd));
        m_body.append("\"/>\n  <COUNTER type=\"");
        m_body.append(numberingType(style.anld.nfc));
        m_body.append("\" depth=\"");
        m_body.append(QString::number(style.istd - 1));
        m_body.append("\" bullet=\"176\" start=\"1\" numberingtype=\"1\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>\n"
            " </LAYOUT>\n"
            "</PARAGRAPH>\n");
    }
}

void WinWordDoc::gotListParagraph(const QString &text, PAP &style)
{
    if (m_phase == TEXT_PASS)
    {
        QString xml_friendly = text;

        encode(xml_friendly);
        m_body.append("<PARAGRAPH>\n<TEXT>");
        m_body.append(xml_friendly);
        m_body.append("</TEXT>\n"
            " <LAYOUT>\n"
            "  <COUNTER type=\"");
        m_body.append(numberingType(style.anld.nfc));
        m_body.append("\" depth=\"");
        m_body.append(QString::number(style.ilvl));
        m_body.append("\" bullet=\"176\" start=\"");
        m_body.append(QString::number(style.anld.iStartAt));
        m_body.append("\" numberingtype=\"0\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>\n"
            " </LAYOUT>\n"
            "</PARAGRAPH>\n");
    }
}

void WinWordDoc::gotTableBegin()
{
    // Create a unique group manager for each new table.

    m_tableManager++;
    m_tableRow = 0;

    if (m_phase == TEXT_PASS)
    {
        m_body.append("<PARAGRAPH>\n<TEXT>");
        if (m_tableManager == 1)
            m_body.append("This filter is currently unable to position tables correctly."
                " All the tables are at the end of this document. Other tables can be found by looking for strings like"
                "\"Table 1 goes here\"");
        m_body.append("Table ");
        m_body.append(QString::number(m_tableManager));
        m_body.append(" goes here.</TEXT>\n</PARAGRAPH>\n");
    }
}

void WinWordDoc::gotTableEnd()
{
}

void WinWordDoc::gotTableRow(const QString texts[], const PAP styles[], TAP &row)
{
    if (m_phase == TABLE_PASS)
    {
        int offset = -row.rgdxaCenter[0];
        QString xml_friendly;

        for (unsigned i = 0; i < row.itcMac; i++)
        {
            m_body.append("<FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" grpMgr=\"grpmgr_");
            m_body.append(QString::number(m_tableManager));
            m_body.append("\" row=\"");
            m_body.append(QString::number(m_tableRow));
            m_body.append("\" col=\"");
            m_body.append(QString::number(i));
            m_body.append("\" rows=\"1\" cols=\"1\" removeable=\"0\" visible=\"1\">\n"
                " <FRAME left=\"");
            m_body.append(QString::number((row.rgdxaCenter[i] + offset)/20));
            m_body.append("\" right=\"");
            m_body.append(QString::number((row.rgdxaCenter[i+1] + offset)/20));
            m_body.append("\" top=\"");
            m_body.append(QString::number(400 + m_tableRow * 30));
            m_body.append("\" bottom=\"");
            m_body.append(QString::number(500 + m_tableRow * 30));
            m_body.append("\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>\n");
            m_body.append("<PARAGRAPH>\n<TEXT>");
            xml_friendly = texts[i];
            encode(xml_friendly);
            m_body.append(xml_friendly);
            m_body.append("</TEXT>\n </PARAGRAPH>\n");
            m_body.append("</FRAMESET>\n");

            offset++;
        }
    }
    m_tableRow++;
}

char WinWordDoc::numberingType(unsigned nfc)
{
    // Word number formats are:
    //
    // 0 Arabic numbering
    // 1 Upper case Roman
    // 2 Lower case Roman
    // 3 Upper case Letter
    // 4 Lower case letter
    // 5 Ordinal

    static unsigned numberingTypes[6] =
    {
        '1', '5', '4', '3', '2', '6'
    };

    return numberingTypes[nfc];
}

const QDomDocument * const WinWordDoc::part()
{
    if (m_phase == DONE)
        return &m_part;
    else
        return NULL;
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
        //kdDebug(30513) << "WinWordDoc::sttbf(): empty STTBF" << endl;
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
        kdDebug(30513) << "WinWordDoc::sttbf(): extended..." << endl;
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
                //kdDebug(30513) << "WinWordDoc::sttbf(): empty string" << endl;
            }
            else {
                str="";

                for(j=0; j<len*2; j+=2)
                    str+=QChar(read16(stream+base+j));

                sttbf.stringList.append(str);
                kdDebug(30513) << str << endl;
                base+=j;

                if(sttbf.extraDataLen!=0) {
                    kdDebug(30513) << "WinWordDoc::sttbf(): extra data" << endl;
                    unsigned char *tmpArray=new unsigned char[sttbf.extraDataLen];
                    kdDebug(30513) << "STILL ALIVE 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    kdDebug(30513) << QString::number((long)sttbf.extraDataLen) << endl;
                    for(j=0; j<sttbf.extraDataLen; ++base)
                        tmpArray[j]=*(stream+base);
                    kdDebug(30513) << "STILL ALIVE 3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    sttbf.extraData.append(&tmpArray);
                    kdDebug(30513) << "STILL ALIVE 4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
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
                //kdDebug(30513) << "WinWordDoc::sttbf(): empty string" << endl;
            }
            else {
                str="";

                for(j=0; j<len; ++j)
                    str+=QChar(char2uni(*(stream+base+j)));

                sttbf.stringList.append(str);
                kdDebug(30513) << str << endl;
                base+=j;

                if(sttbf.extraDataLen!=0) {
                    kdDebug(30513) << "WinWordDoc::sttbf(): extra data" << endl;
                    unsigned char *tmpArray=new unsigned char[sttbf.extraDataLen];
                    kdDebug(30513) << "STILL ALIVE 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    kdDebug(30513) << QString::number((long)sttbf.extraDataLen) << endl;
                    for(j=0; j<sttbf.extraDataLen; ++base)
                        tmpArray[j]=*(stream+base);
                    kdDebug(30513) << "STILL ALIVE 3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    sttbf.extraData.append(&tmpArray);
                    kdDebug(30513) << "STILL ALIVE 4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                }
            }
        }
    }
    sttbf.ok=true;
}

void WinWordDoc::FIBInfo() {

    kdDebug(30513) << "WinWordDoc::FIBInfo() - start -----------------" << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.wIdent)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.nFib)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.nProduct)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.lid)) << endl;
    kdDebug(30513) << "some bits -----------------" << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fDot)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fGlsy)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fComplex)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fHasPic)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.cQuickSaves)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fEncrypted)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fWhichTblStm)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fReadOnlyRecommended)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fWriteReservation)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fExtChar)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fLoadOverride)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fFarEast)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fCrypto)) << endl;
    kdDebug(30513) << "--------------------------" << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.nFibBack)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.lKey)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.envr)) << endl;
    kdDebug(30513) << "bits - bits - bits -------" << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fMac)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fEmptySpecial)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fLoadOverridePage)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fFutureSavedUndo)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fWord97Saved)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fSpare0)) << endl;
    kdDebug(30513) << "--------------------------" << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.chs)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.chsTables)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fcMin)) << endl;
    kdDebug(30513) << QString::number(static_cast<long>(m_fib.fcMac)) << endl;
    kdDebug(30513) << "--------------------------" << endl;
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
        kdDebug(30513) << "WinWordDoc::checkBinTables(): Sigh! It's compressed..." << endl;
        m_success=false;
    }
    return notCompressed;
}

void WinWordDoc::readCommentStuff() {
//    sttbf(m_grpXst, m_fib.fcGrpXstAtnOwners, m_fib.lcbGrpXstAtnOwners, m_table.data);
//    sttbf(m_atnbkmk, m_fib.fcSttbfAtnbkmk, m_fib.lcbSttbfAtnbkmk, m_table.data);
}





