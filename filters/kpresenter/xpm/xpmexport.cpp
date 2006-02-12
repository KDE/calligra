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

#include <qpixmap.h>
#include <qpainter.h>

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <kgenericfactory.h>
#include <KoDocument.h>

#include "xpmexport.h"
#include "exportsizedia.h"

typedef KGenericFactory<XpmExport, KoFilter> xpmExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkpresenterxpmexport, xpmExportFactory( "xpmexport" ) )

XpmExport::XpmExport(KoFilter *fil, const char *name, const QStringList&lst)
    : ImageExport(fil,name,lst)
{
}

XpmExport::~XpmExport()
{
}

bool XpmExport::extraImageAttribute()
{
    bool ret = false;
    ExportSizeDia  *exportDialog = new ExportSizeDia( width, height,
						   0, "exportdialog");
    if (exportDialog->exec()) {
	width  = exportDialog->width();
	height = exportDialog->height();
	ret = true;
	kdDebug() << "Xpm Export: size = [" << width << "," << height << "]" << endl;
    }
    delete exportDialog;
    return ret;
}


bool XpmExport::saveImage( QString fileName)
{
    bool ret = pixmap.save( fileName, "XPM" );
    // Save the image.
    if ( !ret ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ),
                            i18n( "Xpm Export Error" ) );
    }
    return ret;
}

const char * XpmExport::exportFormat()
{
	        return "image/x-xpm";
}

#include "xpmexport.moc"

