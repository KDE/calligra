#include <asciiexport.h>
#include <asciiexport.moc>

ASCIIExport::ASCIIExport(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool ASCIIExport::filter(const QCString &fileIn, const QCString &fileOut,
                               const QCString& from, const QCString& to) {

    if(to!="text/plain" || from!="application/x-kword")
        return false;

    KoTarStore in=KoTarStore(QString(fileIn), KoStore::Read);
    if(!in.open("root", "")) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open input file!");
        in.close();
        return false;
    }
    // read the whole file - at least I hope it does :)
    QByteArray array=in.read(0xffffffff);
    QCString buf((const char*)array, array.size());

    QString str;

    int i = buf.find( "<TEXT>" );
    while ( i != -1 )
    {
        int j = buf.find( "</TEXT>", i );
        if ( j - ( i + 6 ) > 0 )
        {
            str += buf.mid( i + 6, j - ( i + 6 ) );
            str += "\n";
        }
        i = buf.find( "<TEXT>", j );
    }

    QCString cstr=QCString(str.utf8());

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kdebug(KDEBUG_ERROR, 31000, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.writeBlock((const char*)cstr, cstr.length());

    in.close();
    out.close();
    return true;
}
