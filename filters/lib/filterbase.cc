#include <filterbase.h>
#include <filterbase.moc>


FilterBase::FilterBase() : QObject() {
    success=true;
    ready=false;
}

const bool FilterBase::filter() {
    success=false;
    ready=true;
    return success;
}

const QString FilterBase::part() {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<DOC author=\"Werner Trobin\" email=\"wtrobin@carinthia.com\" editor=\"OLEFilter\" mime=\"application/x-kword\" url=\"error.kwd\">\n"
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
        "    <TEXT>This filter is still crappy and it was not able to convert your document... I'm very sorry about this. Maybe you can mail the document (of course compressed) to wtrobin@carinthia.com that I can (try to) improve this filter.</TEXT>\n"
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
        "     <TABULATOR mmpos=\"64.2055\" ptpos=\"182\" inchpos=\"2.52778\" type=\"0\"/>\n"
        "     <TABULATOR mmpos=\"128.764\" ptpos=\"365\" inchpos=\"5.06944\" type=\"0\"/>\n"
        "    </LAYOUT>\n"
        "   </PARAGRAPH>\n"
        "  </FRAMESET>\n"
        "  </FRAMESETS>\n"
        "</DOC>\n";
}

void FilterBase::slotSavePic(const char *data, const char *type, const unsigned long size,
                             char **nameOUT) {
    emit signalSavePic(data, type, size, nameOUT);
}

void FilterBase::slotFilterError() {
    success=false;
}
