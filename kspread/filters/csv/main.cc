/******************************************************************/
/* CSVFilter - (c) 1999 by David Faure <faure@kde.org>            */
/* License: GNU GPL                                               */
/******************************************************************/

#include <main.h>
#include <main.moc>

typedef KOMAutoLoader<Factory> MyAutoLoader;

/******************************************************************/
/* MyApplication                                                  */
/******************************************************************/

MyApplication::MyApplication(int &argc,char **argv) :
    KoApplication(argc,argv,"csvfilter") {
}

void MyApplication::start() {
}

/******************************************************************/
/* Factory                                                        */
/******************************************************************/

Factory::Factory(const CORBA::ORB::ObjectTag &_tag) : 
    KOffice::FilterFactory_skel(_tag) {
}

Factory::Factory(CORBA::Object_ptr _obj) :
    KOffice::FilterFactory_skel(_obj) {
}

KOffice::Filter_ptr Factory::create() {

    Filter *f = new Filter;
    KOMShutdownManager::self()->watchObject( f );
    return KOffice::Filter::_duplicate( f );
}

/******************************************************************/
/* Filter                                                         */
/******************************************************************/

Filter::Filter() : KOMComponent(), KOffice::Filter_skel() {
}

void Filter::filter(KOffice::Filter::Data& data, const char *_from,
                    const char *_to) {

    QString to(_to);
    QString from(_from);

    if (to!="application/x-kspread" || from!="text/x-csv")
    {
        KOffice::Filter::UnsupportedFormat exc;
        exc.format = CORBA::string_dup(_to);
        mico_throw(exc);
        return;
    }

    // Get length
    CORBA::ULong len = data.length();
    if (len==0)
        return;
    // Convert to unsigned chars
    unsigned char *buffer = new unsigned char[len + 1];
    for(CORBA::ULong i=0; i<len; ++i)
        buffer[i] = static_cast<unsigned char>(data[i]);
    buffer[len] = 0;

    // Create Text Stream
    QByteArray a;
    a.setRawData( (char*) buffer, (int) len );
    QTextStream inputStream (a, IO_ReadOnly);

    // Create Filter
    myCSVFilter = new CSVFilter(inputStream);

    QString str;

    if ( myCSVFilter->filter() )
    {
        str = myCSVFilter->part();
    }
    else {
            // Ohh, something went wrong...
            // Let's tell the user that this filter is crappy.
            str="<?xml version=\"1.0\"?>\n"
            "<DOC author=\"Torben Weis\" email=\"weis@kde.org\" editor=\"KSpread\" mime=\"application/x-kspread\" >\n"
            "<PAPER format=\"A4\" orientation=\"Portrait\">\n"
            "<PAPERBORDERS left=\"20\" top=\"20\" right=\"20\" bottom=\"20\"/>\n"
            "<HEAD left=\"\" center=\"\" right=\"\"/>\n"
            "<FOOT left=\"\" center=\"\" right=\"\"/>\n"
            "</PAPER>\n"
            "<MAP>\n"
            "<TABLE name=\"Table1\">\n"
            "<CELL row=\"1\" column=\"1\">\n"
            "<FORMAT align=\"4\" precision=\"-1\" float=\"3\" floatcolor=\"2\" faktor=\"1\"/>\n"
            "Sorry :(\n"
            "</CELL>\n"
            "</TABLE>\n"
            "</MAP>\n"
            "</DOC>";
            }

    // will disappear soon?
    QCString cstr=QCString(str.utf8());
    len = cstr.length();
    data.length(len);
    
    // will we give back the name only, soon?
    for(CORBA::ULong i=0; i<len; ++i)
        data[i]=cstr[i];

    delete [] buffer;
    buffer=0L;
    delete myCSVFilter;
    myCSVFilter=0L;
}

/*================================================================*/
int main(int argc,char **argv) {
    MyApplication app(argc,argv);
    MyAutoLoader loader("IDL:KOffice/FilterFactory:1.0","KSpreadCSVFilter");
    app.exec();
}
