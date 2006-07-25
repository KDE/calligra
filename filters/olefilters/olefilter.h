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

// The OLEFilter class is the main filtering class. It manages the
// correct handling of the input and output file and provides the
// OLE 2 streams for the real filters (excel97, powerpoint97, winword97)

#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <filterbase.h>
#include <KoFilter.h>
#include <klaola.h>

class KoStore;

class OLEFilter : public KoEmbeddingFilter {

    Q_OBJECT

public:
    OLEFilter(KoFilter *parent, const char *name, const QStringList&);
    virtual ~OLEFilter();

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

public slots:
    void commSlotDelayStream( const char* delay );
    void commSlotShapeID( unsigned int& shapeID );

protected slots:
    // This slot saves the document informations to the KOffice tar storage.
    void slotSaveDocumentInformation(
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
	const QString &docAbstract);

    // This slot saves an embedded Picture to the KOffice tar storage.
    void slotSavePic(
        const QString &nameIN,
        QString &storageId,
        const QString &extension,
        unsigned int length,
        const char *data);

    // This slot saves an embedded object to the KOffice tar storage. Note that
    // this only applies to objects within an OLE stream (like embedded WMFs)
    // that we want to handle as parts rather than using slotSavePic() since OLE
    // objects are handled by us, and a filter need only call slotPart().
    void slotSavePart(
        const QString &nameIN,
        QString &storageId,
        QString &mimeType,
        const QString &extension,
        unsigned int length,
        const char *data);

    // Generate a name for a new part to store it in the KOffice tar storage,
    // or find the name and type of an existing one.
    void slotPart(
        const QString &nameIN,
        QString &storageId,
        QString &mimeType);

    // Get another OLE 2 stream for your filter.
    // Attention: You'll have to delete [] the stream.data ptr!
    void slotGetStream(const int &handle, myFile &stream);

    // Like above. Note: This method might return the wrong stream
    // as the stream names are NOT unique in the OLE 2 file!!!
    // (Therefore it's searching only in the current dir)
    // Attention: You'll have to delete [] the stream.data ptr!
    void slotGetStream(const QString &name, myFile &stream);

signals:
    // Forwarding signals for inter-filter communication
    void internalCommShapeID( unsigned int& shapeID );
    void internalCommDelayStream( const char* delay );

private:
    // Don't copy or assign me >:)
    OLEFilter(const OLEFilter &);
    const OLEFilter &operator=(const OLEFilter &);

    // Template method, triggered by embedPart calls
    virtual void savePartContents( QIODevice* file );

    void convert( const QCString& mimeTypeHint );
    void connectCommon(FilterBase **myFilter);
    QCString mimeTypeHelper();

    QMap<QString, QString> imageMap;

    myFile olefile;
    int numPic;                      // for the "unique name generation"
    KLaola *docfile;                 // used to split up the OLE 2 file

    // Needed for the template method callback savePartContents
    const char* m_embeddeeData;
    unsigned int m_embeddeeLength;

    bool success;
    static const int s_area;
};

#endif // OLEFILTER_H
