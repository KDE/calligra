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

#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"
#include "jpegexport.h"
#include "exportsizedia.h"

typedef KGenericFactory<JpegExport, KoFilter> jpegExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkpresenterjpegexport, jpegExportFactory( "jpegexport" ) )

JpegExport::JpegExport(KoFilter *fil, const char *name, const QStringList&lst)
    : ImageExport(fil,name,lst)
{
}

JpegExport::~JpegExport()
{
}

bool JpegExport::extraImageAttribute()
{
    bool ret = false;
    ExportSizeDia  *exportDialog = new ExportSizeDia( width, height,
						   0, "exportdialog");
    if (exportDialog->exec()) {
	width  = exportDialog->width();
	height = exportDialog->height();

	kdDebug() << "MNG Export: size = [" << width << "," << height << "]" << endl;
    	ret = true;
    }
    delete exportDialog;
    return ret;
}


bool JpegExport::saveImage( QString fileName)
{
    bool ret = pixmap.save( fileName, "JPEG" );
    // Save the image.
    if ( !ret ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ),
                            i18n( "JPEG Export Error" ) );
    }
    return ret;
}

const char * JpegExport::exportFormat()
{
	        return "image/jpeg";
}

#include "jpegexport.moc"

