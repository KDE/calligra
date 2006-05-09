/* This file is part of the KDE project
   Copyright (C) 2005 Laurent Montel <montel@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __IMAGEEXPORT_H__
#define __IMAGEEXPORT_H__

#include <KoFilter.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3CString>

class QPixmap;
class ImageExport : public KoFilter
{
    Q_OBJECT

public:
    ImageExport(QObject* parent, const QStringList&);
    virtual ~ImageExport();

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);
    virtual bool extraImageAttribute() { return true;};
    virtual bool saveImage( QString fileName) = 0;
    virtual const char* exportFormat() = 0;
protected:
    int width;
    int height;
    QPixmap pixmap;
};

#endif // __IMAGEEXPORT_H__

