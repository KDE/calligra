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

#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <qobject.h>
#include <qstring.h>
#include <myfile.h>

class Picture;

// Attention: The nameOUT Strings are allocated with new[] in the
// slots!!! Therefore you have to delete [] them!
class FilterBase : public QObject {

    Q_OBJECT

public:
    FilterBase();
    virtual ~FilterBase() {}

    virtual const bool filter();
    virtual const QString part() { return QString(""); }

signals:
    void signalSavePic(Picture *pic);
    void signalPart(const char *nameIN, char **nameOUT);
    void signalGetStream(const long &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void signalGetStream(const QString &name, myFile &stream);

protected slots:
    void slotSavePic(Picture *pic);
    void slotPart(const char *nameIN, char **nameOUT);
    void slotFilterError();

protected:
    bool success;      // ok, the filtering process was successful
    bool ready;        // filtering took place, you may fetch the file now

private:
    FilterBase(const FilterBase &);
    const FilterBase &operator=(const FilterBase &);
};
#endif // FILTERBASE_H
