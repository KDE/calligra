/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_1_3_DOCUMENT_H
#define KWORD_1_3_DOCUMENT_H

class QIODevice;
class KTempFile;
class KWord13Picture;

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qstringlist.h>

#include "kword13frameset.h"

class KWord13Document
{
public:
    KWord13Document( void );
    ~KWord13Document( void );
    
public:
    void xmldump( QIODevice* io );
    /// Get the "last printed" date/time
    QDateTime lastPrintingDate( void ) const;
    /// Get the "creation" date/time
    QDateTime creationDate( void ) const;
    /// Get the "modification" date/time
    QDateTime modificationDate( void ) const;
    /**
     * Get a document info
     * \param name name of the document info
     */
    QString getDocumentInfo( const QString& name ) const;
    /**
     * Get a document property 
     * \param name name of the property
     * \param oldName alternative name of the property (in old KWord syntax)
     */
    QString getProperty( const QString& name, const QString& oldName = QString::null ) const;
protected:
    /// Get a document property \internal
    QString getPropertyInternal( const QString& name ) const;
public:
    QMap<QString, QString> m_documentProperties;
    QMap<QString, QString> m_documentInfo; ///< From documentinfo.xml
    QValueList<KWord13Layout> m_styles;
    QPtrList<KWordTextFrameset> m_normalTextFramesetList; ///< List of \<FRAMESET\> having normal text
    QPtrList<KWordTextFrameset> m_tableFramesetList; ///< List of \<FRAMESET\> being part of tables
    QPtrList<KWordTextFrameset> m_headerFooterFramesetList; ///< List of \<FRAMESET\> having footer/header
    QPtrList<KWordTextFrameset> m_footEndNoteFramesetList; ///< List of \<FRAMESET\> having footnotes or endnotes
    QPtrList<KWord13PictureFrameset> m_pictureFramesetList; ///< List of \<FRAMESET\> having pictures
    QPtrList<KWord13Frameset> m_otherFramesetList; ///< List of \<FRAMESET\> of other types
    QDict<KWord13Picture> m_pictureDict; ///< "Dictionnary" of all pictures' data
    KTempFile* m_previewFile;
    QStringList m_anchoredFramesetNames; ///< List of framesets that are anchored
};

#endif // KWORD_1_3_DOCUMENT_H
