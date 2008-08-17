/* This file is part of the KOffice project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "graphicshandler.h"
#include "document.h"

#include <wv2/olestream.h>

#include <KoStoreDevice.h>
#include <KoGenStyle.h>
#include <kdebug.h>
#include <kmimetype.h>

using namespace wvWare;

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

#ifdef IMAGE_IMPORT

void KWordPictureHandler::bitmapData( OLEImageReader& reader, SharedPtr<const Word97::PICF> /*picf*/ )
{
    kDebug(30513) <<"Bitmap data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size();

}

void KWordPictureHandler::escherData( OLEImageReader& reader, SharedPtr<const Word97::PICF> )
{
    kDebug(30513) << "Escher data found";

    //set up filename
    QString picName("Pictures/");
    picName.append(QString::number(m_pictureCount));
    picName.append(".jpg");

    //write picture data to file
    m_store->open(picName);//open picture file
#define IMG_BUF_SIZE 2048L
    Q_LONG len = reader.size();
    while ( len > 0 )  {
        wvWare::U8* buf = new wvWare::U8[IMG_BUF_SIZE];
        size_t n = reader.read( buf, qMin( len, IMG_BUF_SIZE ) );
        Q_LONG n1 = m_store->write( (const char*)buf, n );
        kDebug(30513) << (int) buf;
        Q_ASSERT( (size_t)n1 == n );
        if ( (size_t)n1 != n )
            return; // ouch
        len -= n;
        delete [] buf;
    }
    Q_ASSERT( len == 0 );
    m_store->close(); //close picture file

    //add entry in manifest file
    QString mimetype(KMimeType::findByPath(picName, 0, true)->name());
    m_manifestWriter->addManifestEntry(picName, mimetype);

    //create style
    QString styleName("fr");
    styleName.append(QString::number(m_pictureCount));
    KoGenStyle* style = new KoGenStyle(KoGenStyle::StyleGraphicAuto, "graphic", "Graphics");
    styleName = m_mainStyles->lookup(*style, styleName);
    delete style;

    //start frame tag for the picture
    m_bodyWriter->startElement("draw:frame");
    m_bodyWriter->addAttribute("draw:style-name", styleName.toUtf8());
    //TODO these values definitely shouldn't be hardcoded
    m_bodyWriter->addAttribute("svg:height", "2.5in");
    m_bodyWriter->addAttribute("svg:width", "3.75in");
    //start the actual image tag
    m_bodyWriter->startElement("draw:image");
    m_bodyWriter->addAttribute("xlink:href", picName);
    m_bodyWriter->addAttribute("xlink:type", "simple");
    m_bodyWriter->addAttribute("xlink:show", "embed");
    m_bodyWriter->addAttribute("xlink:actuate", "onload");
    m_bodyWriter->endElement();//draw:image
    m_bodyWriter->endElement();//draw:frame

    m_pictureCount++;
}

void KWordPictureHandler::wmfData( OLEImageReader& reader, SharedPtr<const Word97::PICF> picf )
{
    kDebug(30513) <<"wmf data found ->>>>>>>>>>>>>>>>>>>>>>>>>>>>> size=" << reader.size();

    // We have two things to do here
    // 1 - Create the frameset and its frame
    // 2 - Store the picture in the store
    // We combine those two things into one call to the document
    QSizeF size( (double)picf->dxaGoal / 20.0, (double)picf->dyaGoal / 20.0 );
    kDebug(30513) <<"size=" << size;
    //fix this next line
    KoStoreDevice* dev = m_doc->createPictureFrameSet( QString("") );
    Q_ASSERT(dev);
    if ( !dev )
        return; // ouch

#define IMG_BUF_SIZE 2048L
    wvWare::U8 buf[IMG_BUF_SIZE];
    Q_LONG len = reader.size();
    while ( len > 0 )  {
        size_t n = reader.read( buf, qMin( len, IMG_BUF_SIZE ) );
        Q_LONG n1 = dev->write( (const char*)buf, n );
        Q_ASSERT( (size_t)n1 == n );
        if ( (size_t)n1 != n )
            return; // ouch
        len -= n;
    }
    Q_ASSERT( len == 0 );
    dev->close();
}

void KWordPictureHandler::tiffData( const UString& /*name*/, SharedPtr<const Word97::PICF> /*picf*/ )
{
    kDebug(30513) ;
}

#endif // IMAGE_IMPORT

#include "graphicshandler.moc"
