/*
 *  koffice/kspread/filters/csv/csvfilter.cc
 *
 *  Comma-separated input filter for KSpread
 *
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *  Covered by the GPL license
 */
#include <csvfilter.h>
#include <csvfilter.moc>

CSVFilter::CSVFilter(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool CSVFilter::filter(const QCString &fileIn, const QCString &fileOut,
                             const QCString& from, const QCString& to) {

    bool bSuccess=true;

    if(to!="application/x-kspread" || from!="text/x-csv")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open input file!");
        in.close();
        return false;
    }

    QTextStream inputStream(&in);
    XMLTree tree("");

    QChar decimal_point = KGlobal::locale()->decimalSymbol()[0];
    // English-speaking countries : decimal_point = '.', CSV delimiter = ','
    // France :                     decimal_point = ',', CSV delimiter = ';'
    // Germany (Austria) :          decimal_point = ',', CSV delimiter = ';'
    // I need more input !!!
    // Current hack :
    QChar csv_delimiter = (decimal_point == ',') ? ';' : ',';

    QChar x;
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_NORMAL_FIELD } state = S_START;
    QString field = "";
    while ( !inputStream.eof() && bSuccess==true )
    {
        inputStream >> x; // read one char
        if (x == '\r') inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly
        switch (state)
        {
            case S_START :
                if (x == '"') state = S_QUOTED_FIELD;
                else if (x == csv_delimiter || x == '\n') tree.emptycell();
                else
                {
                    field += x;
                    state = S_NORMAL_FIELD;
                }
                break;
            case S_QUOTED_FIELD :
                if (x == '"') state = S_MAYBE_END_OF_QUOTED_FIELD;
                else field += x;
                break;
            case S_MAYBE_END_OF_QUOTED_FIELD :
                if (x == '"')
                {
                    field += x;
                    state = S_QUOTED_FIELD;
                } else if (x == csv_delimiter || x == '\n')
                {
                    tree.cell( field );
                    field = "";
                    if (x == '\n') tree.newline();
                    state = S_START;
                } else
                { // should never happen
                    field += "*** Error : unexpected character : ";
                    field += x;
                    state = S_START;
                    bSuccess=false;
                }
                break;
            case S_NORMAL_FIELD :
                if (x == csv_delimiter || x == '\n')
                {
                    tree.cell( field );
                    field = "";
                    if (x == '\n') tree.newline();
                    state = S_START;
                }
                else field += x;
        }
    }

    QCString tmp=tree.part().utf8();

    KoTarStore out=KoTarStore(QString(fileOut), KoStore::Write);
    if(!out.open("root", "")) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.write((const char*)tmp, tmp.length());
    out.close();

    in.close();
    return bSuccess;
}
