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
        int i=1;
        QChar c=fileOutName[fileOutName.length()];

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
        fileOut->writeDir(tmp, "", "");

        partMap.insert("root-dir", tmp);
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
    convert("root-dir");
    return success;
}

void OLEFilter::slotSavePic(const char *data, const char *type,
                            const unsigned int size, char **nameOUT) {

    QString name;

    name+="pic";
    name+=QString::number(numPic);
    ++numPic;
    name+='.';
    name+=type;
    *nameOUT=new char[name.length()+1];
    strncpy(*nameOUT, (const char*)name, name.length());
    *nameOUT[name.length()]='\0';
    name.prepend(dir);
    fileOut->writeFile(name, "", "", size, data);
}

void OLEFilter::slotPart(const char *nameIN, const char *type,
                         char **nameOUT) {

    if(nameIN!=0) {
        QMap<QString, QString>::Iterator it=partMap.find(nameIN);
        QString value;

        if(it!=partMap.end())
            value=partMap[nameIN];
        else {
            QString key=QString(nameIN);
            value="part";
            value+=QString::number(numPart);
            numPart++;
            value+='.';
            value+=type;
            partMap.insert(key, value);
        }
        *nameOUT=new char[value.length()+1];
        strncpy(*nameOUT, (const char*)value, value.length());
        *nameOUT[value.length()]='\0';
    }
}

void OLEFilter::convert(const QString &dirname) {

    QList<OLENode> list=docfile->parseCurrentDir();
    OLENode *node;
    bool onlyDirs=true;

    for(node=list.first(); node!=0; node=list.next()) {
        if(node->type==1) {         // it is a dir!
            docfile->enterDir(node->handle);
            convert(node->name);
            docfile->leaveDir();
        }
        else {
            onlyDirs=false;
        }
    }
    kdebug(KDEBUG_INFO, 31000, (const char*)dirname);

    if(!onlyDirs) {
        FilterBase *myFilter=0L;
        node=list.first();

        do {
            if(node->name=="WordDocument" || node->name=="1Table" ||
               node->name=="0Table" || node->name=="ObjectPool") {
                // Word
                kdebug(KDEBUG_INFO, 31000, "WinWord");
                myFilter=new FilterBase();
            }
            else if(node->name=="Workbook") {
                // Excel
                kdebug(KDEBUG_INFO, 31000, "Excel");
                myFilter=new FilterBase();
            }
            else if(node->name=="PowerPoint Document") {
                // PowerPoint
                kdebug(KDEBUG_INFO, 31000, "Power Point");
                myFilter=new FilterBase();
            }
            else if(node->name=="Equation Native") {
                // MS Equation
                kdebug(KDEBUG_INFO, 31000, "Formel");
                myFilter=new FilterBase();
            }
            node=list.next();
        } while(myFilter==0L && node!=0);

        if(myFilter==0L) {
            // unknown
            kdebug(KDEBUG_INFO, 31000, "keine Ahnung");
        }

        delete myFilter;
    }
}
