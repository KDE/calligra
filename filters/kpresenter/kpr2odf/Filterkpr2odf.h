/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301  USA.
*/

#ifndef FILTERKPR2ODF_H
#define FILTERKPR2ODF_H

//Qt's includes
#include <QHash>

//KOffice includes
#include <KoDom.h>
#include <KoFilter.h>
#include <KoGenStyles.h>

class KoXmlWriter;
class KoGenStyles;

class Filterkpr2odf : public KoFilter {
    Q_OBJECT

public:
    Filterkpr2odf(QObject* parent, const QStringList&);

    virtual ~Filterkpr2odf() {}

    //reimplemented from KoFilter
    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

private:
    //Helper functions
    void createImageList( KoStore* output, KoStore* input, KoXmlWriter* manifest );
    void convertContent( KoXmlWriter* content );
    void convertObjects( KoXmlWriter* content, const KoXmlNode& objects, const int currentPage );
    void appendPicture( KoXmlWriter* content, KoXmlElement objectElement );
    const QString getPictureNameFromKey( const KoXmlElement& key );

     //Styles functions
    const QString createPageStyle( const KoXmlElement& page );
    const QString createGradientStyle( const KoXmlElement& page );
    const QString createPageLayout();//we use more than one tag, better load them from m_mainDoc
    const QString createMasterPageStyle();//same as above

    KoXmlDocument m_mainDoc;//from KPR
    KoXmlDocument m_documentInfo;//from KPR

    int m_pageHeight;//needed to find out where's every object
    QHash<QString,QString> m_pictures;//store the <filename, name> pair of the keys

    KoGenStyles m_styles;//style collector
};

#endif //FILTERKPR2ODF_H
