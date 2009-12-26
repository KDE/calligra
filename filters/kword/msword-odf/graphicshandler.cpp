/* This file is part of the KOffice project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "graphicshandler.h"
#include "document.h"

#include <wv2/src/olestream.h>

#include <KoStoreDevice.h>
#include <KoGenStyle.h>
#include <kdebug.h>
#include <kmimetype.h>


using namespace wvWare;

#define IMG_BUF_SIZE 2048L


KWordPictureHandler::KWordPictureHandler(Document* doc, KoXmlWriter* bodyWriter,
        KoXmlWriter* manifestWriter, KoStore* store, KoGenStyles* mainStyles)
        : QObject(), m_doc(doc), m_pictureCount(0)
{
    kDebug(30513) ;
    m_bodyWriter = bodyWriter;
    m_manifestWriter = manifestWriter;
    m_store = store;
    m_mainStyles = mainStyles;
}

void KWordPictureHandler::setBodyWriter(KoXmlWriter* writer)
{
    m_bodyWriter = writer;
}

#ifdef IMAGE_IMPORT

void KWordPictureHandler::bitmapData(OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/)
{
    kDebug(30513) << "Bitmap data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size();

}

void KWordPictureHandler::escherData(OLEImageReader& reader, SharedPtr<const Word97::PICF> picf, int type)
{
    kDebug(30513) << "Escher data found";

    QString picName("Pictures/");
    ODTProcessing(&picName, picf, type);

    //write picture data to file
    m_store->open(picName);//open picture file
    long len = reader.size();
    while (len > 0)  {
        kDebug(30513) << "len = " << len;
        wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];
        size_t n = reader.read(buf, qMin(len, IMG_BUF_SIZE));
        long n1 = m_store->write((const char*)buf, n);
        kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
        len -= n;
        delete [] buf;
        //error checking
        if ((n == 0 && len != 0) ||  //endless loop
                (size_t)n1 != n) { //read/wrote different lengths
            m_store->close(); //close picture file before returning
            return; //ouch - we're in an endless loop!
        }
        //Q_ASSERT( (size_t)n1 == n );
    }
    Q_ASSERT(len == 0);
    m_store->close(); //close picture file
}

//use this version when the data had to be decompressed
//so we don't have to convert the data back to an OLEImageReader
void KWordPictureHandler::escherData(std::vector<wvWare::U8> data, SharedPtr<const Word97::PICF> picf, int type)
{
    kDebug(30513) << "Escher data found";

    QString picName("Pictures/");
    ODTProcessing(&picName, picf, type);

    //write picture data to file
    m_store->open(picName);//open picture file

    long len = data.size();
    int index = 0; //index for reading from vector
    while (len > 0)  {
        kDebug(30513) << "len = " << len;
        wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];
        //instead of a read command, we'll copy that number of bytes
        //from the vector into the buffer
        int n = qMin(len, IMG_BUF_SIZE);
        for (int i = 0; i < n; i++) {
            buf[i] = data[index];
            index++;
        }
        //size_t n = reader.read( buf, qMin( len, IMG_BUF_SIZE ) );
        long n1 = m_store->write((const char*)buf, n);
        kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
        len -= n;
        delete [] buf;
        //error checking
        if ((n == 0 && len != 0) ||  //endless loop
                (size_t)n1 != n) { //read/wrote different lengths
            m_store->close(); //close picture file before returning
            return; //ouch - we're in an endless loop!
        }
        //Q_ASSERT( (size_t)n1 == n );
    }
    Q_ASSERT(len == 0);
    m_store->close(); //close picture file
}

void KWordPictureHandler::ODTProcessing(QString* picName, SharedPtr<const Word97::PICF> picf, int type)
{

    //set up filename
    picName->append(QString::number(m_pictureCount));
    m_pictureCount++;
    //the type coming in corresponds to MSOBLIPTYPE
    //  see wv2/src/graphics.h
    if (type == 5)
        picName->append(".jpg");
    else if (type == 6)
        picName->append(".png");
    else if (type == 3) 
        picName->append(".wmf");
    else if (type == 2) 
        picName->append(".emf");
    else {
        kWarning() << "Unhandled file type (" << type << ") - pictures won't be displayed.";
        return;
    }

    //add entry in manifest file
    QString mimetype(KMimeType::findByPath(*picName, 0, true)->name());
    m_manifestWriter->addManifestEntry(*picName, mimetype);

    //create style
    QString styleName("fr");
    styleName.append(QString::number(m_pictureCount));
    KoGenStyle* style = new KoGenStyle(KoGenStyle::StyleGraphicAuto, "graphic", "Graphics");
    styleName = m_mainStyles->lookup(*style, styleName);
    delete style;

    //start frame tag for the picture
    m_bodyWriter->startElement("draw:frame");
    m_bodyWriter->addAttribute("draw:style-name", styleName.toUtf8());
    m_bodyWriter->addAttribute("text:anchor-type", "as-char");
    //mx, my = horizontal & vertical user scaling in .001 %
    double horiz_scale = picf->mx / 1000.0;
    double vert_scale = picf->my / 1000.0;
    double height = ((double) picf->dyaGoal * vert_scale) / 20.0; //twips -> pt
    double width = ((double) picf->dxaGoal * horiz_scale) / 20.0; //twips -> pt
    m_bodyWriter->addAttributePt("svg:height", height);
    m_bodyWriter->addAttributePt("svg:width", width);
    //start the actual image tag
    m_bodyWriter->startElement("draw:image");
    m_bodyWriter->addAttribute("xlink:href", *picName);
    m_bodyWriter->addAttribute("xlink:type", "simple");
    m_bodyWriter->addAttribute("xlink:show", "embed");
    m_bodyWriter->addAttribute("xlink:actuate", "onLoad");
    m_bodyWriter->endElement();//draw:image
    m_bodyWriter->endElement();//draw:frame

}

void KWordPictureHandler::wmfData(OLEImageReader& reader, SharedPtr<const Word97::PICF> picf)
{
    wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];

    kDebug(30513) << "WMF data found. Size=" << reader.size();

    QString picName("Pictures/");
    
    // Read the first bytes of the picture.  We need this to determine
    // if the contents is a WMF or an EMF.
    long len = reader.size();
    size_t  n = reader.read(buf, qMin(len, IMG_BUF_SIZE));

    // Pass 2 for emf image and 3 for wmf image
    // An EMF has the string " EMF" at the start + offset 40.
    if (len > 44 && buf[40] == ' ' && buf[41] == 'E' && buf[42] == 'M' && buf[43] == 'F') {
        kDebug(30513) << "Found an EMF file";
        ODTProcessing(&picName, picf, 2);
    }
    else {
        kDebug(30513) << "Found a WMF file";
        ODTProcessing(&picName, picf, 3);
    }

    // Write picture data to file.
    m_store->open(picName);//open picture file
    while (len > 0)  {
        kDebug(30513) << "len = " << len;

        // Write the bytes that we have in the buffer.
        ulong    n1 = m_store->write((const char*)buf, n);
        kDebug(30513) << "n=" << n << ", n1=" << n1 << "; buf contains " << (void*) buf;
        len -= n;

        // Break if something went wrong with the writing.
        if (n1 != n)
            break;

        if (len == 0)
            break;

        // Read the next batch.
        size_t  n = reader.read(buf, qMin(len, IMG_BUF_SIZE));

        //error checking
        if (n == 0 && len != 0)
            break;
    }

    delete [] buf;

    m_store->close(); //close picture file
}

void KWordPictureHandler::externalImage(const UString& name, SharedPtr<const Word97::PICF> picf)
{
    kDebug(30513);
}

#endif // IMAGE_IMPORT

#include "graphicshandler.moc"
