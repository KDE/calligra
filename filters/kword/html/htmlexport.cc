#include <htmlexport.h>
#include <htmlexport.moc>

void mainFunc(const char*);

HTMLExport::HTMLExport(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool HTMLExport::filter(const QCString &fileIn, const QCString &fileOut,
                               const QCString& from, const QCString& to) {

    if(to!="text/html" || from!="application/x-kword")
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

    int begin = buf.find( "<DOC" );
    buf.remove( 0, begin - 1 );

    mainFunc( (const char*)buf );

    QFile f( "/tmp/kword2html" );
    if ( !f.open( IO_ReadOnly ) ) {
        in.close();
        return false;
    }

    QTextStream s( &f );
    QString str = s.read();
    f.close();

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
