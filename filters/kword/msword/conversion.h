/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef CONVERSION_H
#define CONVERSION_H

#include <ustring.h>
#include <qstring.h>
#include <qcolor.h>

class QDomElement;
namespace wvWare {
    namespace Word97 {
        class LSPD;
        class BRC;
    }
}

// Static methods for simple MSWord->KWord conversions
// (enums etc.)

namespace Conversion
{
    // UString -> QConstString conversion. Use .string() to get the QString.
    // Always store the QConstString into a variable first, to avoid a deep copy.
    inline QConstString string( const wvWare::UString& str ) {
        // Let's hope there's no copying of the QConstString happening...
        return QConstString( reinterpret_cast<const QChar*>( str.data() ), str.length() );
    }

    // Prepare text for inclusion in XML
    void encodeText(QString &text);

    // Convert color code (ico) to QColor
    QColor color(int number, int defaultcolor, bool defaultWhite = false);

    // Convert alignment code to string
    QString alignment( int jc );

    // Convert linespacing struct to string
    QString lineSpacing( const wvWare::Word97::LSPD& lspd );

    // Convert number format code to KWord's COUNTER.type code
    int numberFormatCode( int nfc );

    // Set all border attributes in the XML element, from a BRC structure
    void setBorderAttributes( QDomElement& borderElement, const wvWare::Word97::BRC& brc );

    // Convert wv2's Header Type to a KWord frameInfo value
    int headerTypeToFrameInfo( unsigned char type );

    // Convert wv2's Header Type to a KWord frameset name
    QString headerTypeToFramesetName( unsigned char type );

    // Convert a mask of Header Types to the hType value for KWord
    int headerMaskToHType( unsigned char mask );

    // Convert a mask of Header Types to the fType value for KWord
    int headerMaskToFType( unsigned char mask );

    bool isHeader( unsigned char type );
};

#endif
