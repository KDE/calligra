/******************************************************************/
/* CSVFilter - (c) 1999 by David Faure <faure@kde.org>            */
/* License: GNU GPL                                               */
/******************************************************************/

#include <main.h>
#include <main.moc>

#include <klocale.h>

#include <stdio.h> // for stdin

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

void Filter::filter(KOffice::Filter::Data& data, const QCString & _from,
                    const QCString & _to) {

    QString to(_to);
    QString from(_from);

    if (to!="application/x-kspread" || from!="text/x-csv")
    {
        KOffice::Filter::UnsupportedFormat exc;
        exc.format = CORBA::string_dup(_to);
        mico_throw(exc);
        return;
    }

    // Create Text Stream from QByteArray
    QTextIStream inputStream( data );

    // Create Filter
    // "CSV File" will be the table name. I would have preferred to set
    // it to the input filename, but how to get hold of it here ? (David)
    CSVFilter * myCSVFilter = new CSVFilter(inputStream, i18n( "CSV File") );

    QString str;

    // Go for it, boy !
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

    // return the XML string
    QCString cstr=QCString(str.utf8());
    char * ret = strdup( cstr.data() );
    data.assign( ret, cstr.length() );

    // cleanups
    delete myCSVFilter;
}

/*================================================================*/
int main(int argc,char **argv) {

#undef TEST_ONLY // define this to enable using the filter on standard input/output

#ifdef TEST_ONLY
    KApplication a(argc,argv); // needed for klocale
    QTextIStream inputStream( stdin );
    CSVFilter * myCSVFilter = new CSVFilter(inputStream, "standard input");

    if ( myCSVFilter->filter() )
      (void) myCSVFilter->part(); // will be shown on debug output
    else
      debug( "error ! filter() returned false" );

    delete myCSVFilter;
#else
    MyApplication app(argc,argv);
    MyAutoLoader loader("IDL:KOffice/FilterFactory:1.0","KSpreadCSVFilter");
    app.exec();
#endif
    return 0;
}
