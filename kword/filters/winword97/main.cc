/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Werner Trobin                                          */
/* E-Mail: wtrobin@carinthia.com, reggie@kde.org, weis@kde.org    */
/* Homepage: http://members.carinthia.com/wtrobin/                */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Filter                                                 */
/******************************************************************/

#include "main.h"
#include "main.moc"


typedef KOMAutoLoader<Factory> MyAutoLoader;

/******************************************************************/
/* MyApplication                                                  */
/******************************************************************/

/*================================================================*/
MyApplication::MyApplication(int &argc,char **argv) :
    KoApplication(argc,argv,"kword_winword97") {
}

/*================================================================*/
void MyApplication::start() {
}

/******************************************************************/
/* Factory                                                        */
/******************************************************************/

/*================================================================*/
Factory::Factory(const CORBA::ORB::ObjectTag &_tag) : 
    KOffice::FilterFactory_skel(_tag) {
}

/*================================================================*/
Factory::Factory(CORBA::Object_ptr _obj) :
    KOffice::FilterFactory_skel(_obj) {
}

/******************************************************************/
/* Filter                                                         */
/******************************************************************/

/*================================================================*/
KOffice::Filter_ptr Factory::create() {

    Filter *f = new Filter;
    KOMShutdownManager::self()->watchObject( f );
    return KOffice::Filter::_duplicate( f );
}

/*================================================================*/
Filter::Filter() : KOMComponent(), KOffice::Filter_skel() {
}

/*================================================================*/
void Filter::filter(KOffice::Filter::Data& data, const char *_from,
                    const char *_to) {

    if (QString(_to) != "application/x-kword") {
        KOffice::Filter::UnsupportedFormat exc;
        exc.format = CORBA::string_dup(_to);
        mico_throw(exc);
        return;
    }
    if (QString(_from) != "text/winword97") {
        KOffice::Filter::UnsupportedFormat exc;
        exc.format = CORBA::string_dup(_from);
        mico_throw(exc);
        return;
    }

    CORBA::ULong len = data.length();
    if (len == 0)
        return;

    unsigned char *buffer = new unsigned char[len + 1];
    for(CORBA::ULong i = 0;i < len;++i)
        buffer[i] = static_cast<unsigned char>(data[i]);
    buffer[len] = 0;
    
    docFile.data=buffer;   // see myfile.h
    docFile.length=len;
    myWordFilter=new WordFilter(docFile);

    QString str;

    if(myWordFilter->filterIt())
        str=myWordFilter->kwdFile();
    else 
        // Ohh, something went wrong - Sorry...
        // Let´s tell the user that this filter is crappy.

        str+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<DOC author=\"Reginald Stadlbauer and Torben Weis\" email=\"reggie@kde.org and weis@kde.org\" editor=\"KWord\" mime=\"application/x-kword\" url=\"/home/koffice/empty.kwd\">
 <PAPER format=\"1\" ptWidth=\"595\" ptHeight=\"841\" mmWidth =\"210\" mmHeight=\"297\" inchWidth =\"8.26772\" inchHeight=\"11.6929\" orientation=\"0\" columns=\"1\" ptColumnspc=\"2\" mmColumnspc=\"1\" inchColumnspc=\"0.0393701\" hType=\"0\" fType=\"0\" ptHeadBody=\"9\" ptFootBody=\"9\" mmHeadBody=\"3.5\" mmFootBody=\"3.5\" inchHeadBody=\"0.137795\" inchFootBody=\"0.137795\">
  <PAPERBORDERS mmLeft=\"10\" mmTop=\"15\" mmRight=\"10\" mmBottom=\"15\" ptLeft=\"28\" ptTop=\"42\" ptRight=\"28\" ptBottom=\"42\" inchLeft=\"0.393701\" inchTop=\"0.590551\" inchRight=\"0.393701\" inchBottom=\"0.590551\"/>
 </PAPER>
 <ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>
 <FOOTNOTEMGR>
  <START value=\"1\"/>
  <FORMAT superscript=\"1\" type=\"1\"/>
  <FIRSTPARAG ref=\"(null)\"/>
 </FOOTNOTEMGR>
 <FRAMESETS>
  <FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">
   <FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0\"/>
   <PARAGRAPH>
    <TEXT>This filter is still crappy and it was not able to convert your document... IÂ´m very sorry about this. Maybe you can mail this document (of course compressed) to wtrobin@carinthia.com that I can improve this filter.</TEXT>
    <INFO info=\"0\"/>
    <HARDBRK frame=\"0\"/>
    <FORMATS>
    </FORMATS>
    <LAYOUT>
     <NAME value=\"Standard\"/>
     <FOLLOWING name=\"Standard\"/>
     <FLOW value=\"0\"/>
     <OHEAD pt=\"0\" mm=\"0\" inch=\"0\"/>
     <OFOOT pt=\"0\" mm=\"0\" inch=\"0\"/>
     <IFIRST pt=\"0\" mm=\"0\" inch=\"0\"/>
     <ILEFT pt=\"0\" mm=\"0\" inch=\"0\"/>
     <LINESPACE pt=\"0\" mm=\"0\" inch=\"0\"/>
     <COUNTER type=\"0\" depth=\"0\" bullet=\"176\" start=\"1\" numberingtype=\"1\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>
     <LEFTBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>
     <RIGHTBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>
     <TOPBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>
     <BOTTOMBORDER red=\"255\" green=\"255\" blue=\"255\" style=\"0\" width=\"0\"/>
     <FORMAT>
      <COLOR red=\"0\" green=\"0\" blue=\"0\"/>
      <FONT name=\"times\"/>
      <SIZE value=\"12\"/>
      <WEIGHT value=\"50\"/>
      <ITALIC value=\"0\"/>
      <UNDERLINE value=\"0\"/>
      <VERTALIGN value=\"0\"/>
     </FORMAT> 
     <TABULATOR mmpos=\"64.2055\" ptpos=\"182\" inchpos=\"2.52778\" type=\"0\"/>
     <TABULATOR mmpos=\"128.764\" ptpos=\"365\" inchpos=\"5.06944\" type=\"0\"/>
    </LAYOUT>
   </PARAGRAPH>
  </FRAMESET>
  </FRAMESETS>
</DOC>";
    
    len = str.length();
    data.length(len);
    for(CORBA::ULong i = 0;i < len;++i)
        data[i] = QChar(str[i]);

    delete [] buffer;
    buffer=0L;
    delete myWordFilter;
    myWordFilter=0L;
}

/*================================================================*/
void main(int argc,char **argv) {

    MyApplication app(argc,argv);
    MyAutoLoader loader("IDL:KOffice/FilterFactory:1.0","KWordWinword97");
    app.exec();
}

