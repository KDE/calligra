#include "olefilter.h"
#include "olefilter.moc"

OLEFilter::OLEFilter(const myFile &in, const QString &nameOut, const IN i,
                     const OUT o) : QObject(), fileIn(in),
                     fileOutName(nameOut), in(i), out(o) {

    docfile=0L;
    fileOut=0L;
    numPic=0;
    numPart=0;
    success=false; // later -> true;

    docfile=new KLaola(fileIn);
    if(!docfile->isOk())
        success=false;
    else {
        fileOut=new KTar(fileOutName);
        fileOut->open(IO_WriteOnly);

        if(fileOutName.right(4)==".tgz")
            fileOutName.truncate(fileOutName.length()-4);
        else if(fileOutName.right(7)==".tar.gz")
            fileOutName.truncate(fileOutName.length()-7);

        QString tmp;
        QChar c;
        int i=1;

        c=fileOutName[fileOutName.length()];

        while(c!='/') {
            c=fileOutName[fileOutName.length()-i];
            ++i;
            tmp.prepend(static_cast<char>(c));
        }
        if(tmp.isEmpty())
            tmp="/nix";

        dir=tmp.right(tmp.length()-1);

        tmp=dir;
        dir+='/';

        fileOut->writeDir(tmp, "koffice", "users");

        kdebug(KDEBUG_INFO, 31000, (const char*)tmp);
        kdebug(KDEBUG_INFO, 31000, (const char*)dir);
    }
}

OLEFilter::~OLEFilter() {

    fileOut->close();
    delete fileOut;
    fileOut=0L;

    if(docfile) {
        delete docfile;
        docfile=0L;
    }
}

const bool OLEFilter::filter() {
    convert(fileOutName);
    return success;
}

void OLEFilter::slotSavePic(const char *data, const char *type,
                            const unsigned int size, char *nameOUT) {

    QString name;

    name+="pic";

    name+=QString::number(numPic);
    ++numPic;
    name+='.';
    name+=type;

    nameOUT=new char[name.length()+1];
    strncpy(nameOUT, (const char*)name, name.length());
    nameOUT[name.length()]='\0';

    name.prepend(dir);

    fileOut->writeFile(name, "koffice", "users", size, data);
}

void OLEFilter::slotPart(const char *, char *) {
}

void OLEFilter::convert(const QString &) {
}
