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

DESCRIPTION

   When reading, the point of this module is toperform a depth-first traversal
   of an OLE file. This ensures that a parent object is processed only after
   its child objects have been processed.
*/

#include <excelfilter.h>
#include <koFilterManager.h>
#include <koQueryTrader.h>
#include <koDocumentInfo.h>
#include <ktempfile.h>
#include <myfile.h>
#include <olefilter.h>
#include <olefilter.moc>
#include <powerpointfilter.h>
#include <unistd.h>
#include <wordfilter.h>

OLEFilter::OLEFilter(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
    olefile.data=0L;
    docfile=0L;
    store=0L;
    success=true;
    numPic=0;
    m_nextPart=0;
}

OLEFilter::~OLEFilter() {

    delete [] olefile.data;
    olefile.data=0L;
    delete docfile;
    docfile=0L;
    delete store;
    store=0L;
}

bool OLEFilter::filter(
    const QString &fileIn,
    const QString &fileOut, const QString &prefixOut,
    const QString &from, const QString &to,
    const QString &) {

    if(to!="application/x-kword" &&
       to!="application/x-kspread" &&
       to!="application/x-kpresenter")
        return false;
    if(from!="application/vnd.ms-word" &&
       from!="application/vnd.ms-excel" &&
       from!="application/msword" &&
       from!="application/msexcel" &&
       from!="application/mspowerpoint")
        return false;

    m_prefixOut = prefixOut;
    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdError(s_area) << "OLEFilter::filter(): Unable to open input: " << fileIn << endl;
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
        kdError(s_area) << "OLEFilter::filter(): Unable to read input file correctly!" << endl;
        delete [] olefile.data;
        olefile.data=0L;
        return false;
    }

    store=new KoStore(fileOut, KoStore::Write, to.latin1());
    if(store->bad()) {
        kdError(s_area) << "OLEFilter::filter(): Unable to open output file! " << fileOut << endl;
        delete [] olefile.data;
        olefile.data=0L;
        delete store;
        store=0L;
        return false;
    }

    // Special treatment for the "root" element: add a precanned name
    // conversion for what is expected by koStore.
    partMap.insert("tar:root", "root");

    // Recursively convert the file
    convert(prefixOut, "root");
    kdDebug(s_area) << "OLEFilter::filter(): created " << fileOut << endl;
    delete store;
    store=0L;
    return success;
}

void OLEFilter::slotSavePart(
    const QString &nameIN,
    QString &storageId,
    QString &mimeType,
    const QString &extension,
    const QString &config,
    unsigned int length,
    const char *data)
{
    if(nameIN.isEmpty())
        return;

    QString key = m_path + nameIN;
    QString id;
    QMap<QString, QString>::Iterator it = partMap.find(key);

    if (it != partMap.end())
    {
        // The key is already here - return the part id.
        id = partMap[key];
        mimeType = mimeMap[key];
    }
    else
    {
        KoFilterManager *mgr = KoFilterManager::self();
        KTempFile tempFile(QString::null, "." + extension);

        // It's not here, so let's generate one.
        id = m_path + '/' + QString::number(m_nextPart);
        m_nextPart++;

        // Save the data supplied into a temporary file, then run the filter
        // on it.

        tempFile.file()->writeBlock(data, length);
        tempFile.close();
        QString result = mgr->import(tempFile.name(), mimeType, config, m_prefixOut + id.mid(sizeof("tar:") - 1));
        tempFile.unlink();
        partMap.insert(key, id);
        mimeMap.insert(key, mimeType);

        // Now fetch out the elements from the resulting KoStore and embed them in our KoStore.

        KoStore storedPart(result, KoStore::Read);
        if (!store->embed(id, storedPart))
            kdError(s_area) << "Could not embed in KoStore!" << endl;
        unlink(result.local8Bit());
    }
    //storageId = QFile::encodeName(id);
    storageId = (id);
}

void OLEFilter::slotSaveDocumentInformation(
    const QString &fullName,
    const QString &title,
    const QString &company,
    const QString &email,
    const QString &telephone,
    const QString &fax,
    const QString &postalCode,
    const QString &country,
    const QString &city,
    const QString &street,
    const QString &docTitle,
    const QString &docAbstract)
{
    KoDocumentInfo *info = new KoDocumentInfo();
    KoDocumentInfoAuthor *author = static_cast<KoDocumentInfoAuthor *>(info->page("author"));
    KoDocumentInfoAbout *about = static_cast<KoDocumentInfoAbout *>(info->page("about"));
    author->setFullName(fullName);
    author->setTitle(title);
    author->setCompany(company);
    author->setEmail(email);
    author->setTelephone(telephone);
    author->setFax(fax);
    author->setCountry(postalCode);
    author->setPostalCode(country);
    author->setCity(city);
    author->setStreet(street);
    about->setTitle(docTitle);
    about->setTitle(docAbstract);

    QString id = m_path + "/documentinfo.xml";

    if(!store->open(id))
    {
	kdError(s_area) << "OLEFilter::slotSaveDocumentInformation(): Could not open KoStore!" << endl;
    	return;
    }

    QString data = info->save().toString();
    int length = data.utf8().length();
    
    if(!store->write(data.utf8(), length))
	kdError(s_area) << "OLEFilter::slotSaveDocumentInformation(): Could not write to KoStore!" << endl;
    store->close();
}

void OLEFilter::slotSavePic(
    const QString &nameIN,
    QString &storageId,
    const QString &extension,
    unsigned int length,
    const char *data)
{
    if(nameIN.isEmpty())
        return;

    QString key = m_path + nameIN;
    QString id;
    QMap<QString, QString>::Iterator it = imageMap.find(key);

    if (it != imageMap.end())
    {
        // The key is already here - return the part id.
        id = imageMap[key];
    }
    else
    {
        // It's not here, so let's generate one.
        id = m_path + "/pictures/picture" + QString::number(numPic++) + '.' + extension;
        imageMap.insert(key, id);
        if(!store->open(id))
        {
            success = false;
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not open KoStore!" << endl;
            return;
        }
        // Write it to the gzipped tar file
        bool ret = store->write(data, length);
        if (!ret)
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not write to KoStore!" << endl;
        store->close();
    }
    //storageId = QFile::encodeName(id);
    storageId = (id);
}

void OLEFilter::slotPart(
    const char *nameIN,
    QString &storageId,
    QString &mimeType)
{
    if (!nameIN)
        return;

    QString key = m_path + nameIN;
    QString id;
    QMap<QString, QString>::Iterator it = partMap.find(key);

    if (it != partMap.end())
    {
        // The key is already here - return the part id.
        id = partMap[key];
        mimeType = mimeMap[key];
    }
    else
    {
        // It's not here, so let's generate one. Note that since any
        // references to the part will be from one level above, we trim
        // the key by one level first!
        id = m_path;
        key = m_path.left(m_path.findRev('/')) + nameIN;
        partMap.insert(key, id);
        mimeMap.insert(key, mimeType);
    }
    storageId = QFile::encodeName(id);
}

// Don't forget the delete [] the stream.data ptr!
void OLEFilter::slotGetStream(const int &handle, myFile &stream) {
    stream=docfile->stream(handle);
}

// I can't guarantee that you get the right stream as the names
// in a OLE 2 file are not unique! (searching only the current dir!)
// Don't forget the delete [] the stream.data ptr!
void OLEFilter::slotGetStream(const QString &name, myFile &stream) {

    KLaola::NodeList handle;

    handle=docfile->find(name, true);  // search only in current dir!

    if (handle.count()==1)
        stream=docfile->stream(handle.at(0));
    else {
        stream.data=0L;
        stream.length=0;
    }
}

// The recursive method to do all the work
unsigned OLEFilter::convert(const QString &parentPath, const QString &dirname) {

    KLaola::NodeList list=docfile->parseCurrentDir();
    KLaola::OLENode *node;
    bool onlyDirs=true;
    unsigned part=0;
    unsigned nextPart=0;

    // Search for the directories
    for(node=list.first(); node!=0; node=list.next()) {
        if(node->isDirectory()) {   // It's a dir!
            if(docfile->enterDir(node)) {

                // Go one level deeper, but don't increase the depth
                // for ObjectPools.
                if (node->name() == "ObjectPool")
                    nextPart=convert(parentPath, node->name());
                else
                    nextPart=convert(parentPath + '/' + QString::number(part), node->name());
                part++;

                docfile->leaveDir();
            }
        }
        else {
            onlyDirs=false;   // To prevent useless looping in the
        }                     // next loop
    }

    QString mimeType;
    m_path = parentPath;
    if(!onlyDirs) {
        FilterBase *myFilter=0L;
        QStringList nodeNames;
        node=list.first();

        // Find out the correct file type and create the appropriate filter
        do {
            nodeNames.prepend(node->name());
            if(node->name()=="WordDocument") {

                // WinWord.

                myFile main, table0, table1, data;
                KLaola::NodeList tmp;

                tmp=docfile->find("WordDocument", true);
                if(tmp.count()==1)
                    main=docfile->stream(tmp.at(0));

                tmp=docfile->find("0Table", true);
                if(tmp.count()==1)
                    table0=docfile->stream(tmp.at(0));

                tmp=docfile->find("1Table", true);
                if(tmp.count()==1)
                    table1=docfile->stream(tmp.at(0));

                tmp=docfile->find("Data", true);
                if(tmp.count()==1)
                    data=docfile->stream(tmp.at(0));

                mimeType = "application/x-kword";
                myFilter=new WordFilter(main, table0, table1, data);
            }
            else if(node->name()=="Workbook" || node->name()=="Book") {

                // Excel.

                myFile workbook;

                workbook=docfile->stream(node->handle());
                mimeType = "application/x-kspread";
                myFilter=new ExcelFilter(workbook);
            }
/*
            else if(node->name()=="Ole10Native") {
                myFile native, pres, objinfo, compobj;
                KLaola::NodeList tmp;

                tmp=docfile->find("Ole10Native", true);
                if(tmp.count()==1)
                    native=docfile->stream(tmp.at(0));

                tmp=docfile->find("OlePres000", true);
                if(tmp.count()==1)
                    pres=docfile->stream(tmp.at(0));

                tmp=docfile->find("ObjInfo", true);
                if(tmp.count()==1)
                    objinfo=docfile->stream(tmp.at(0));

                tmp=docfile->find("CompObj", true);
                if(tmp.count()==1)
                    compobj=docfile->stream(tmp.at(0));
kdDebug(s_area) << "converting XXXXXXXXXXXX" << endl;
QFile file;
file.setName("Ole10Native");
file.open(IO_WriteOnly );
file.writeBlock((char*)native.data, native.length);
file.close();
file.setName("OlePres000");
file.open(IO_WriteOnly );
file.writeBlock((char*)pres.data, pres.length);
file.close();
file.setName("ObjInfo");
file.open(IO_WriteOnly );
file.writeBlock((char*)objinfo.data, objinfo.length);
file.close();
file.setName("CompObj");
file.open(IO_WriteOnly );
file.writeBlock((char*)compobj.data, compobj.length);
file.close();
                node=list.next();
            }
*/
            else if(node->name()=="PowerPoint Document") {

                // Powerpoint.

                myFile main, currentUser, summary, documentSummary;
                KLaola::NodeList tmp;

                tmp=docfile->find("PowerPoint Document", true);
                if(tmp.count()==1)
                    main=docfile->stream(tmp.at(0));

                tmp=docfile->find("Current User", true);
                if(tmp.count()==1)
                    currentUser=docfile->stream(tmp.at(0));

                tmp=docfile->find("SummaryInformation", true);
                if(tmp.count()==1)
                    summary=docfile->stream(tmp.at(0));

                tmp=docfile->find("DocumentSummaryInformation", true);
                if(tmp.count()==1)
                    documentSummary=docfile->stream(tmp.at(0));

                mimeType = "application/x-kpresenter";
                myFilter=new PowerPointFilter(main, currentUser);
            }
            else
                node=list.next();
        } while(!myFilter && node);

        if(!myFilter) {

            // Unknown type. We turn it into a dummy kword document...

            kdWarning(s_area) << "cannot convert \"" << nodeNames.join(",") << "\"" << endl;
            mimeType = "application/x-kword";
            myFilter=new FilterBase(nodeNames);
        }

        // connect SIGNALs&SLOTs
        connectCommon(&myFilter);

        // Launch the filtering process...
        m_nextPart = nextPart;
        success=myFilter->filter();
        // ...and fetch the file
        QCString file;
        if(!myFilter->plainString()) {
            const QDomDocument * const part=myFilter->part();
            file=part->toCString();
        }
        else
            file=myFilter->CString();

        // Get the storage name of the part (dirname==key), and associate the
        // mimeType with it for later use.
        QString tmp;
        slotPart(QFile::encodeName(dirname), tmp, mimeType);
        if(!store->open(tmp)) {
            success=false;
            kdError(s_area) << "OLEFilter::convert(): Could not open KoStore!" << endl;
            return part;
        }

        // Write it to the gzipped tar file
        bool ret = store->write(file.data(), file.length());
        if (!ret)
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not write to KoStore!" << endl;
        store->close();
        delete myFilter;
    }

    // Track the last part number used at this level.
    return part;
}

void OLEFilter::connectCommon(FilterBase **myFilter) {
    QObject::connect(
        *myFilter,
        SIGNAL(signalSaveDocumentInformation(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)),
        this,
        SLOT(slotSaveDocumentInformation(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)));

    QObject::connect(
        *myFilter,
        SIGNAL(signalSavePic(const QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SLOT(slotSavePic(const QString &, QString &, const QString &, unsigned int, const char *)));
    QObject::connect(
        *myFilter,
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, const QString &, unsigned int, const char *)),
        this,
        SLOT(slotSavePart(const QString &, QString &, QString &, const QString &, const QString &, unsigned int, const char *)));
    QObject::connect(*myFilter, SIGNAL(signalPart(const char *, QString &, QString &)),
                     this, SLOT(slotPart(const char *, QString &, QString &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const int &, myFile &)), this,
                     SLOT(slotGetStream(const int &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const QString &, myFile &)), this,
                     SLOT(slotGetStream(const QString &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
}
