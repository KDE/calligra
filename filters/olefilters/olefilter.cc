/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <olefilter.h>
#include <olefilter.moc>

OLEFilter::OLEFilter(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
    olefile.data=0L;
    docfile=0L;
    store=0L;
    success=true;
}

OLEFilter::~OLEFilter() {

    delete [] olefile.data;
    olefile.data=0L;
    delete docfile;
    docfile=0L;
    delete store;
    store=0L;
}

const bool OLEFilter::filter(const QString &fileIn, const QString &fileOut,
                             const QString &from, const QString &to,
                             const QString &) {

    if(to!="application/x-kword" &&
       to!="application/x-kspread" &&
       to!="application/x-kpresenter")
        return false;
    if(from!="application/vnd.ms-word" &&
       from!="application/vnd.ms-excel" &&
       from!="application/mspowerpoint")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(30510) << "OLEFilter::filter(): Unable to open input file!" << endl;
        in.close();
        return false;
    }

    // Open the OLE 2 file. [TODO] Is it really the best way to
    // read all the stuff without buffer?
    olefile.length=in.size();
    olefile.data=new unsigned char[olefile.length];
    in.readBlock((char*)olefile.data, olefile.length);
    in.close();

    docfile=new KLaola(olefile);
    if(!docfile->isOk()) {
        kdError(30510) << "OLEFilter::filter(): Unable to read input file correctly!" << endl;
        delete [] olefile.data;
        olefile.data=0L;
        return false;
    }

    store=new KoStore(fileOut, KoStore::Write);
    if(store->bad()) {
        kdError(30510) << "OLEFilter::filter(): Unable to open output file!" << endl;
        delete [] olefile.data;
        olefile.data=0L;
        delete store;
        store=0L;
        return false;
    }

    storePath.resize(0);               // To keep track where we are :)
    partMap.insert("root", "root");    // Special treatment for the "root" element
    convert("root");                   // Recursively convert the file
    return success;
}

void OLEFilter::slotSavePic(Picture *) {
    // we'll use Reggies Collection class here, soon (at least I hope so :)
}

// Don't forget the delete [] the nameOUT string!
void OLEFilter::slotPart(const char *nameIN, char **nameOUT) {

    if(nameIN==0)
        return;

    QMap<QString, QString>::Iterator it=partMap.find(nameIN);
    QString value;

    if(it!=partMap.end())        // The "key-name" is already here - return the
        value=partMap[nameIN];   // other one
    else {
        QString key=QString(nameIN);           // It's not here, yet, so let's
        value="tar:";                          // generate one...
        for(unsigned int i=0; i<storePath.size(); ++i) {
            value+='/';
            value+=QString::number(static_cast<unsigned int>(storePath[i]));
        }
        partMap.insert(key, value);           // ...and store it
    }
    unsigned int len=value.length();
    *nameOUT=new char[len+1];
    strncpy(*nameOUT, QFile::encodeName(value), len);
    *(*nameOUT+len)='\0';
}

// Don't forget the delete [] the stream.data ptr!
void OLEFilter::slotGetStream(const int &handle, myFile &stream) {
    stream=docfile->stream(handle);
}

// I can't guarantee that you get the right stream as the names
// in a OLE 2 file are not unique! (searching only the current dir!)
// Don't forget the delete [] the stream.data ptr!
void OLEFilter::slotGetStream(const QString &name, myFile &stream) {

    QArray<int> handle;

    handle=docfile->find(name, true);  // search only in current dir!

    if(handle.size()==1)
        stream=docfile->stream(handle[0]);
    else {
        stream.data=0L;
        stream.length=0;
    }
}

// The recursive method to do all the work
void OLEFilter::convert(const QString &dirname) {

    QList<OLENode> list=docfile->parseCurrentDir();
    OLENode *node;
    bool onlyDirs=true;
    bool resized=false;
    short index=storePath.size()-1;

    // Search for the directories
    for(node=list.first(); node!=0; node=list.next()) {
        if(node->type==1) {   // It's a dir!
            if(!resized) {
                ++index;
                storePath.resize(index+1);
                storePath[index]=0;
                resized=true;
            }
            if(docfile->enterDir(node->handle)) {
                ++storePath[index];
                convert(node->name);  // Go one level deeper <----------
                docfile->leaveDir();
            }
        }
        else {
            onlyDirs=false;   // To prevent useless looping in the
        }                     // next loop
    }

    if(resized)
        storePath.resize(index);

    if(!onlyDirs) {
        FilterBase *myFilter=0L;
        node=list.first();

        // Find out the correct file type and create the appropriate filter
        do {
            if(node->name=="WordDocument" || node->name=="1Table" ||
               node->name=="0Table" || node->name=="ObjectPool") {

                myFile main, table0, table1, data;
                QArray<int> tmp;

                // WinWord
                // kdDebug(30510) << "OLEFilter::convert(): WinWord" << endl;

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
                connectCommon(&myFilter);
            }
            else if(node->name=="Workbook") {
                // Excel
                // kdDebug(30510) << "OLEFilter::convert(): Excel" << endl;

                myFile workbook;
                workbook.data=0L;

                workbook=docfile->stream(node->handle);
                myFilter=new ExcelFilter(workbook);
                connectCommon(&myFilter);
            }
            else if(node->name=="PowerPoint Document") {
                // PowerPoint
                // kdDebug(30510) << "OLEFilter::convert(): Power Point" << endl;
                myFilter=new PowerPointFilter();
                // connect SIGNALs&SLOTs
                connectCommon(&myFilter);
            }
            else
                node=list.next();
        } while(myFilter==0L && node!=0);

        if(myFilter==0L) {
            // unknown type
            kdDebug(30510) << "OLEFilter::convert(): superunknown -> black hole sun ;)" << endl;
            myFilter=new FilterBase();
            // connect SIGNALs&SLOTs
            connectCommon(&myFilter);
        }

        // Launch the filtering process...
        success=myFilter->filter();
        // ...and fetch the file
        const QDomDocument * const part=myFilter->part();
        QCString file=part->toCString();
        // Get the name of the part (dirname==key)
        char *tmp=0L;
        slotPart(QFile::encodeName(dirname), &tmp);
        if(!store->open(tmp)) {
            success=false;
            kdError(30510) << "OLEFilter::convert(): Could not open KoStore!" << endl;
            return;
        }
        // Write it to the gzipped tar file
        store->write(file.data(), file.length());
        store->close();
        delete [] tmp;
        delete myFilter;
    }
}

void OLEFilter::connectCommon(FilterBase **myFilter) {

    QObject::connect(*myFilter, SIGNAL(signalSavePic(Picture *)), this,
                     SLOT(slotSavePic(Picture *)));
    QObject::connect(*myFilter, SIGNAL(signalPart(const char *, char **)),
                     this, SLOT(slotPart(const char *, char **)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const int &, myFile &)), this,
                     SLOT(slotGetStream(const int &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const QString &, myFile &)), this,
                     SLOT(slotGetStream(const QString &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
}
