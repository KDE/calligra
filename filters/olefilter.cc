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
        fileOut->writeDir("testOLE.kwd", "koffice", "users");  // too lazy for "playing"
                                                               // with Strings now
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

    QString name, finalName;

    name+="pic";

    name+=QString::number(numPic);
    numPic++;
    name+='.';
    name+=type;

    nameOUT=new char[name.length()+1];
    strncpy(nameOUT, (const char*)name, name.length());
    nameOUT[name.length()]='\0';

    finalName+=fileOutName;
    finalName+='/';
    finalName+=name;

    fileOut->writeFile(finalName, "koffice", "users", size, data);
}

void OLEFilter::slotPart(const char *, char *) {
}

void OLEFilter::convert(const QString &) {
}
