/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

#ifndef OLEFILTER_H
#define OLEFILTER_H

#include <string.h>
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qarray.h>
#include <qfile.h>
#include <koFilter.h>
#include <koTarStore.h>
#include <klaola.h>
#include <filterbase.h>
#include <wordfilter.h>
#include <excelfilter.h>
#include <picture.h>
#include <myfile.h>


class OLEFilter : public KoFilter {

    Q_OBJECT

public:
    OLEFilter(KoFilter *parent, QString name);
    virtual ~OLEFilter();

    virtual const bool filter(const QCString &fileIn, const QCString &fileOut,
                              const QCString& from, const QCString& to);
    
protected slots:
    void slotSavePic(Picture *pic);
    void slotPart(const char *nameIN, char **nameOUT);
    void slotGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void slotGetStream(const QString &name, myFile &stream);

private:
    OLEFilter(const OLEFilter &);
    const OLEFilter &operator=(const OLEFilter &);

    void convert(const QString &dirname);
    void connectCommon(FilterBase **myFilter);

    QMap<QString, QString> partMap;
    QArray<unsigned short> storePath;

    myFile olefile;
    int numPic;                      // unique name generation 
    KLaola *docfile;                 // used to split up the OLE-file
    KoTarStore *store;               // KOffice Storage structure
    bool success;
};
#endif // OLEFILTER_H
