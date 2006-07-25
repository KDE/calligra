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

#include <olefilter.h>

#include <qfile.h>
#include <ktempfile.h>
#include <kgenericfactory.h>
#include <kmimetype.h>
#include <KoFilterChain.h>
#include <KoDocumentInfo.h>

//#include <excelfilter.h>
#include <powerpointfilter.h>
//#include <wordfilter.h>
//#include <hancomwordfilter.h>

#include <myfile.h>

const int OLEFilter::s_area = 30510;

class OLEFilterFactory : KGenericFactory<OLEFilter, KoFilter>
{
public:
    OLEFilterFactory(void) : KGenericFactory<OLEFilter, KoFilter> ("olefilter")
    {}
protected:
    virtual void setupTranslations( void )
    {
        KGlobal::locale()->insertCatalogue( "kofficefilters" );
    }
};

K_EXPORT_COMPONENT_FACTORY( libolefilter, OLEFilterFactory() )

OLEFilter::OLEFilter(KoFilter *, const char *, const QStringList&) :
                     KoEmbeddingFilter(), numPic( 0 ),
                     docfile( 0 ), m_embeddeeData( 0 ),
                     m_embeddeeLength( 0 ), success( true )
{
    olefile.data=0L;
}

OLEFilter::~OLEFilter()
{
    delete [] olefile.data;
    delete docfile;
}

KoFilter::ConversionStatus OLEFilter::convert( const QCString& from, const QCString& to )
{
    if(to!="application/x-kword" &&
       to!="application/x-kspread" &&
       to!="application/x-kpresenter")
        return KoFilter::NotImplemented;
    if(from!="application/vnd.ms-word" &&
       from!="application/vnd.ms-excel" &&
       from!="application/msword" &&
       from!="application/msexcel" &&
       from!="application/mspowerpoint" &&
       from!="application/x-hancomword")
        return KoFilter::NotImplemented;

    QFile in(m_chain->inputFile());
    if(!in.open(IO_ReadOnly)) {
        kdError(s_area) << "OLEFilter::filter(): Unable to open input" << endl;
        in.close();
        return KoFilter::FileNotFound;
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
        return KoFilter::StupidError;
    }

    // Recursively convert the file
    convert( "" );
    if ( success )
        return KoFilter::OK;
    else
        return KoFilter::StupidError;
}

void OLEFilter::commSlotDelayStream( const char* delay )
{
    emit internalCommDelayStream( delay );
}

void OLEFilter::commSlotShapeID( unsigned int& shapeID )
{
    emit internalCommShapeID( shapeID );
}

void OLEFilter::slotSavePart(
    const QString &nameIN,
    QString &storageId,
    QString &mimeType,
    const QString &extension,
    unsigned int length,
    const char *data)
{
    if(nameIN.isEmpty())
        return;

    int id = internalPartReference( nameIN );

    if (id != -1)
    {
        // The part is already there, this is a lookup operation
        // -> return the part id.
        storageId = QString::number( id );
        mimeType = internalPartMimeType( nameIN );
    }
    else
    {
        // Set up the variables for the template method callback
        m_embeddeeData = data;
        m_embeddeeLength = length;

        QString srcMime( KoEmbeddingFilter::mimeTypeByExtension( extension ) );
        if ( srcMime == KMimeType::defaultMimeType() )
            kdWarning( s_area ) << "Couldn't determine the mimetype from the extension" << endl;

        KoFilter::ConversionStatus status;
        QCString destMime( mimeType.latin1() );
        storageId = QString::number( embedPart( srcMime.latin1(), destMime, status, nameIN ) );

        // copy back what the method returned
        mimeType = destMime;

        // Reset the variables to be on the safe side
        m_embeddeeData = 0;
        m_embeddeeLength = 0;

        if ( status != KoFilter::OK )
            kdDebug(s_area) << "Huh??? Couldn't convert that file" << endl;
    }
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
    author->setTelephoneHome(telephone);
    author->setFax(fax);
    author->setCountry(postalCode);
    author->setPostalCode(country);
    author->setCity(city);
    author->setStreet(street);
    about->setTitle(docTitle);
    about->setTitle(docAbstract);

    KoStoreDevice* docInfo = m_chain->storageFile( "documentinfo.xml", KoStore::Write );

    if(!docInfo)
    {
	kdError(s_area) << "OLEFilter::slotSaveDocumentInformation(): Could not open documentinfo.xml!" << endl;
    	return;
    }

    QCString data = info->save().toCString();
    // Important: don't use data.length() here. It's slow, and dangerous (in case of a '\0' somewhere)
    // The -1 is because we don't want to write the final \0.
    Q_LONG length = data.size()-1;

    if(docInfo->writeBlock(data, length) != length)
	kdError(s_area) << "OLEFilter::slotSaveDocumentInformation(): Could not write to KoStore!" << endl;
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

    QMap<QString, QString>::ConstIterator it = imageMap.find(nameIN);

    if (it != imageMap.end())
        // The key is already here - return the part id.
        storageId = it.data();
    else
    {
        // It's not here, so let's generate one.
        storageId = QString( "pictures/picture%1.%2" ).arg( numPic++ ).arg( extension );
        imageMap.insert(nameIN, storageId);
        KoStoreDevice* pic = m_chain->storageFile( storageId, KoStore::Write );
        if(!pic)
        {
            success = false;
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not open KoStore!" << endl;
            return;
        }
        // Write it to the gzipped tar file
        // Let's hope we never have to save images bigger than 2GB :-)
        bool ret = pic->writeBlock(data, length) == static_cast<int>( length );
        if (!ret)
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not write to KoStore!" << endl;
    }
}

// ##### Only used for lookup now!
void OLEFilter::slotPart(
    const QString& nameIN,
    QString &storageId,
    QString &mimeType)
{
    if (nameIN.isEmpty())
        return;

    int id = internalPartReference( nameIN );

    if (id != -1)
    {
        // The key is already here - return the part id.
        storageId = QString::number( id );
        mimeType = internalPartMimeType( nameIN );
    }
    else
        kdWarning( s_area ) << "slotPart() can be used for lookup operations only" << endl;
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

void OLEFilter::savePartContents( QIODevice* file )
{
    if ( m_embeddeeData != 0 && m_embeddeeLength != 0 )
        file->writeBlock( m_embeddeeData, m_embeddeeLength );
}

// The recursive method to do all the work
void OLEFilter::convert( const QCString& mimeTypeHint )
{
    KLaola::NodeList list=docfile->parseCurrentDir();
    KLaola::OLENode *node;
    bool onlyDirs=true;

    // Search for the directories
    for(node=list.first(); node!=0; node=list.next()) {
        if(node->isDirectory()) {   // It's a dir!
            if(docfile->enterDir(node)) {
                // Go one level deeper, but don't increase the depth
                // for ObjectPools.
                if (node->name() == "ObjectPool")
                    convert( "" );
                else {
                    // Get the storage name of the part (dirname==key), and associate the
                    // mimeType with it for later use.
                    QCString mimeHint( mimeTypeHelper() );
                    if ( mimeHint.isEmpty() )
                        mimeHint = "application/x-kword"; // will be converted to a dummy KWord part
                    startInternalEmbedding( node->name(), mimeHint );
                    convert( mimeHint );
                    endInternalEmbedding();
                }
                docfile->leaveDir();
            }
        }
        else
            onlyDirs=false;   // To prevent useless looping in the next loop
    }

    if(!onlyDirs) {
        QStringList nodeNames;
        QCString mimeType;
        if ( !mimeTypeHint.isEmpty() )
            mimeType = mimeTypeHint;
        else
            mimeType = mimeTypeHelper();

        FilterBase *myFilter=0L;

	#if 0
        if ( mimeType == "application/x-kword" ) {
            // WinWord (or dummy).

            myFile main;
            KLaola::NodeList tmp;
            tmp=docfile->find("WordDocument", true);

            if(tmp.count()==1) {
                // okay, not a dummy
                main=docfile->stream(tmp.at(0));

                myFile table0, table1, data;
                tmp=docfile->find("0Table", true);
                if(tmp.count()==1)
                    table0=docfile->stream(tmp.at(0));

                tmp=docfile->find("1Table", true);
                if(tmp.count()==1)
                    table1=docfile->stream(tmp.at(0));

                tmp=docfile->find("Data", true);
                if(tmp.count()==1)
                    data=docfile->stream(tmp.at(0));

                myFilter=new WordFilter(main, table0, table1, data);

                // forward the internal communication calls
                connect( this, SIGNAL( internalCommShapeID( unsigned int& ) ), myFilter, SIGNAL( internalCommShapeID( unsigned int& ) ) );
                connect( this, SIGNAL( internalCommDelayStream( const char* ) ), myFilter, SIGNAL( internalCommDelayStream( const char* ) ) );
            }
        }
        else if ( mimeType == "application/x-kspread" ) {
            // Excel.

            myFile workbook;
            KLaola::NodeList tmp;

            tmp = docfile->find( "Workbook", true );
            if ( tmp.count() == 1 )
                workbook = docfile->stream( tmp.at( 0 ) );
            else {
                tmp = docfile->find( "Book", true );
                if ( tmp.count() == 1 )
                    workbook = docfile->stream( tmp.at( 0 ) );
            }
            myFilter=new ExcelFilter(workbook);
        }
        else
	#endif       
	if ( mimeType == "application/x-kpresenter" ) {
            // Powerpoint.

            myFile main, currentUser, pictures, summary, documentSummary;
            KLaola::NodeList tmp;

            tmp=docfile->find("PowerPoint Document", true);
            if(tmp.count()==1)
                main=docfile->stream(tmp.at(0));

            tmp=docfile->find("Current User", true);
            if(tmp.count()==1)
                currentUser=docfile->stream(tmp.at(0));

            tmp=docfile->find("Pictures", true);
            if(tmp.count()==1)
                pictures=docfile->stream(tmp.at(0));

            tmp=docfile->find("SummaryInformation", true);
            if(tmp.count()==1)
                summary=docfile->stream(tmp.at(0));

            tmp=docfile->find("DocumentSummaryInformation", true);
            if(tmp.count()==1)
                documentSummary=docfile->stream(tmp.at(0));

            myFilter=new PowerPointFilter(main, currentUser, pictures);
        }
	#if 0
        else if ( mimeType == "application/x-hancomword" ) {
            // HancomWord 6
            myFile prvText;
            KLaola::NodeList tmp;

            tmp = docfile->find( "PrvText", true );
            if( tmp.count() == 1 ) prvText = docfile->stream( tmp.at( 0 ) );

            myFilter = new HancomWordFilter( prvText );
        }
	#endif

        if(!myFilter) {
            // Unknown type. We turn it into a dummy kword document...
            node = list.first();
            do {
                nodeNames.prepend(node->name());
                node = list.next();
            } while ( node );

            kdWarning(s_area) << "cannot convert \"" << nodeNames.join(",") << "\"" << endl;
            myFilter=new FilterBase(nodeNames);
        }

        // connect SIGNALs&SLOTs
        connectCommon(&myFilter);

        // Launch the filtering process...
        success=myFilter->filter();
        // ...and fetch the file
        QCString file;
        if(!myFilter->plainString()) {
            const QDomDocument * const part=myFilter->part();
            file=part->toCString();
        }
        else
            file=myFilter->CString();

        KoStoreDevice* dev = m_chain->storageFile( "root", KoStore::Write );
        if(!dev) {
            success=false;
            kdError(s_area) << "OLEFilter::convert(): Could not open KoStore!" << endl;
            return;
        }

        // Write it to the gzipped tar file
        bool ret = dev->writeBlock(file.data(), file.size()-1) == static_cast<Q_LONG>( file.size() - 1 );
        if (!ret)
            kdError(s_area) << "OLEFilter::slotSavePic(): Could not write to KoStore!" << endl;
        delete myFilter;
    }
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
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SLOT(slotSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)));
    QObject::connect(*myFilter, SIGNAL(signalPart(const QString&, QString &, QString &)),
                     this, SLOT(slotPart(const QString&, QString &, QString &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const int &, myFile &)), this,
                     SLOT(slotGetStream(const int &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const QString &, myFile &)), this,
                     SLOT(slotGetStream(const QString &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(sigProgress(int)), this, SIGNAL(sigProgress(int)));
}

QCString OLEFilter::mimeTypeHelper()
{
    KLaola::NodeList list = docfile->parseCurrentDir();
    KLaola::OLENode* node = list.first();

    // ###### FIXME: Shaheed, please add additional mimetypes here
    while ( node ) {
        if ( node->name() == "WordDocument" )
            return "application/x-kword";
        else if ( node->name() == "Workbook" || node->name() == "Book" )
            return "application/x-kspread";
        else if ( node->name() == "PowerPoint Document" )
            return "application/x-kpresenter";
        else if ( node->name() == "PrvText" || node->name() == "BodyText" )
            return "application/x-hancomword";
        else
            node = list.next();
    }
    kdWarning( s_area ) << "No known mimetype detected" << endl;
    return "";
}

#include <olefilter.moc>
