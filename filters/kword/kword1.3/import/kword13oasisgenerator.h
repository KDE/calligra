//

/* This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef KWORD_1_3_OASIS_GENERATOR
#define KWORD_1_3_OASIS_GENERATOR

#define STRICT_OOWRITER_VERSION_1

class QString;
class QTextStream;
class KZip;
class KoStore;
class KoGenStyle;
class KWord13Document;

#include <koGenStyles.h>

class KWord13OasisGenerator
{
public:
    KWord13OasisGenerator( void );
    ~KWord13OasisGenerator( void );
    
    bool prepare( KWord13Document& kwordDocument );
    bool generate( const QString& fileName, KWord13Document& kwordDocument );
    
protected: //deprecated
    QString escapeOOText(const QString& strText) const;
    QString escapeOOSpan(const QString& strText) const;
protected: // deprecated
    bool zipPrepareWriting(const QString& name);
    bool zipDoneWriting(void);
    bool zipWriteData(const char* str);
    bool zipWriteData(const QByteArray& array);
    bool zipWriteData(const QCString& cstr);
    bool zipWriteData(const QString& str); ///< Assumes UTF-8
protected: // Generating phase
    /// \deprecated
    void writeStartOfFile( const QString& type );
    /// @param main Is it the main frameset?
    void generateTextFrameset( KoXmlWriter& writer, KWordTextFrameset* frameset, bool main );
    void writeStylesXml( void );
    void writeContentXml( void );
    void writeMetaXml( void );
protected: // Preparing phase
    void prepareTextFrameset( KWordTextFrameset* frameset );
    /// Also sets the name of the automatical style
    void declareLayout( KWord13Layout& layout );
    /// Also sets the name of the automatical style
    void declareStyle( KWord13Layout& layout );
    double numberOrNull( const QString& str ) const;
    void fillGenStyleWithFormatOne( const KWord13FormatOneData& one, KoGenStyle& gs, const bool style ) const;
    void fillGenStyleWithLayout( const KWord13Layout& layout, KoGenStyle& gs, const bool style ) const;
protected:
    KWord13Document* m_kwordDocument;
    KZip* m_zip; ///< Output OOWriter file
    KoStore* m_store;
    uint m_size; ///< Size of ZIP entry
    QTextStream* m_streamOut;
    QByteArray m_contentBody; ///< office:body element of content.xml
    QString m_contentAutomaticStyles; ///< Automatic styles for content.xml (in OO format)
    KoGenStyles m_oasisGenStyles; ///< OASIS styles
};

#endif // KWORD_1_3_OASIS_GENERATOR
