#include <olefilter.h>
#include <olefilter.moc>

OLEFilter::OLEFilter(const myFile &in, const IN i, const OUT o) : QObject(),
                     fileIn(in), in(i), out(o) {

    docfile=0L;
    myFilter=0L;
    numPic=0;
    success=true;

    docfile=new KLaola(fileIn);
    if(!docfile->isOk())
        success=false;
}

OLEFilter::~OLEFilter() {

    if(docfile) {
        delete docfile;
        docfile=0L;
    }
    if(myFilter) {
        delete myFilter;
        myFilter=0L;
    }
}

const bool OLEFilter::filter() {
    convert();
    return success;
}

void OLEFilter::slotSavePic(const char *data, const char *type,
                            const unsigned long size, char **nameOUT) {

    QString name;

    name+="/tmp/koffice-pic";
    name+=QString::number(numPic);
    ++numPic;
    if(type[0]!='.')
        name+='.';
    name+=type;

    QFile f(name);
    if(f.open(IO_WriteOnly) && f.writeBlock(data, size)!=-1) {
        int len=name.length();
        *nameOUT=new char[len+1];
        strncpy(*nameOUT, static_cast<const char*>(name), len);
        *nameOUT[len]='\0';
    }
    f.close();
    *nameOUT=0L;  // error
}

void OLEFilter::slotGetStream(const long &handle, myFile &stream) {
    stream=docfile->stream(handle);
}

// I can't guarantee that you get the right stream as the names
// are not unique! (searching only the current dir!)
void OLEFilter::slotGetStream(const QString &name, myFile &stream) {

    QArray<long> handle;

    handle=docfile->find(name, true);  // search only in current dir!

    if(handle.size()==1)
        stream=docfile->stream(handle[0]);
    else {
        stream.data=0L;
        stream.length=0;
    }
}

void OLEFilter::convert() {

    QList<OLENode> list=docfile->parseCurrentDir();
    OLENode *node;

    node=list.first();

    do {
        if(node->name=="WordDocument" || node->name=="1Table" ||
           node->name=="0Table" || node->name=="ObjectPool") {

            myFile main, table0, table1, data;
            QArray<long> tmp;

            kdebug(KDEBUG_INFO, 31000, "WinWord");

            main.data=0L;
            table0.data=0L;
            table1.data=0L;
            data.data=0L;

            tmp=docfile->find("WordDocument", true);
            if(tmp.size()==1)
                main=docfile->stream(tmp[0]);

            tmp=docfile->find("0Table", true);
            if(tmp.size()==1)
                table0=docfile->stream(tmp[0]);

            tmp=docfile->find("1Table", true);
            if(tmp.size()==1)
                table1=docfile->stream(tmp[0]);

            tmp=docfile->find("Data", true);
            if(tmp.size()==1)
                data=docfile->stream(tmp[0]);

            myFilter=new WordFilter(main, table0, table1, data);
            connectCommon();
        }
        else if(node->name=="Workbook") {
            // Excel
            kdebug(KDEBUG_INFO, 31000, "Excel");

            myFile workbook;
            workbook.data=0L;

            workbook=docfile->stream(node->handle);
            myFilter=new ExcelFilter(workbook);
            connectCommon();
        }
        else if(node->name=="PowerPoint Document") {
            // PowerPoint
            kdebug(KDEBUG_INFO, 31000, "Power Point");
            myFilter=new FilterBase();
            // connect SIGNALs&SLOTs
            connectCommon();
        }

        // some more will be here, soon
        // I'll have to read some additional OLE-Streams
        // and look for information in them as the names
        // are not unique!
        else
            node=list.next();
    } while(myFilter==0L && node!=0);

    if(myFilter==0L) {
        // unknown
        kdebug(KDEBUG_INFO, 31000, "superunknown");
        myFilter=new FilterBase();
        // connect SIGNALs&SLOTs
        connectCommon();
    }
    success=myFilter->filter();      // do we really need that flag?
}

// Reggie: Wenn Du diese Kommentare entfernst, tritt der Bug auf :(
// Ich war mir nicht sicher, ob ein "connect" ausreichend ist - es sollte
// eigentlich genügen. Ich habe nur in einer Datei in den libs "QObject::
// connect" gesehen und dachte mir - schaden kann das nicht...
void OLEFilter::connectCommon() {

    QObject::connect(myFilter, SIGNAL(signalSavePic(const char *, const char *,
                     const unsigned long, char **)), this, SLOT(slotSavePic(const char *,
                     const char *, const unsigned long, char **)));
    QObject::connect(myFilter, SIGNAL(signalGetStream(const long &, myFile &)), this,
                     SLOT(slotGetStream(const long &, myFile &)));
    QObject::connect(myFilter, SIGNAL(signalGetStream(const QString &, myFile &)), this,
                     SLOT(slotGetStream(const QString &, myFile &)));
}
