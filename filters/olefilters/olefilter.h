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
#include <koFilter.h>
#include <koStore.h>
#include <klaola.h>
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

class myFile;

class OLEFilter : public KoFilter {

    Q_OBJECT

public:
    OLEFilter(KoFilter *parent, const char *name);
    virtual ~OLEFilter();

/*
    virtual bool filter(const QString &fileIn, const QString &fileOut,
                        const QString &from, const QString &to,
                        const QString &config=QString::null);
*/
    virtual bool filter(const QString &fileIn,
                        const QString &fileOut, const QString &prefixOut,
                        const QString &from, const QString &to,
                        const QString &config);
    virtual bool supportsEmbedding() { return true; }

protected slots:
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
        const QString &config,
        unsigned int length,
        const char *data);

    // Generate a name for a new part to store it in the KOffice tar storage,
    // or find the name and type of an existing one.
    void slotPart(
        const char *nameIN,
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

private:
    // Don't copy or assign me >:)
    OLEFilter(const OLEFilter &);
    const OLEFilter &operator=(const OLEFilter &);

    unsigned convert(const QString &parentPath, const QString &dirname);
    void connectCommon(FilterBase **myFilter);

    QMap<QString, QString> partMap;
    QMap<QString, QString> mimeMap;
    QMap<QString, QString> imageMap;
    QString m_path;

    myFile olefile;
    QString m_prefixOut;
    int numPic;                      // for the "unique name generation"
    int m_nextPart;
    KLaola *docfile;                 // used to split up the OLE 2 file
    KoStore *store;               // KOffice Storage structure
    bool success;
    static const int s_area = 30510;
};
#endif // OLEFILTER_H
