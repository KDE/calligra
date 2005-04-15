/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <magickexport.h>
#include <kgenericfactory.h>
#include <koDocument.h>
#include <koFilterChain.h>
#include <kis_doc.h>

typedef KGenericFactory<MagickExport, KoFilter> MagickExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkritamagickexport, MagickExportFactory("kofficefilters"))

MagickExport::MagickExport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
}

MagickExport::~MagickExport()
{
}

KoFilter::ConversionStatus MagickExport::convert(const QCString& from, const QCString& to)
{
	kdDebug() << "magick export!\n";
	if (from != "application/x-krita")
		return KoFilter::NotImplemented;

	// XXX: Add dialog about flattening layers here

	KisDoc *output = dynamic_cast<KisDoc*>(m_chain -> inputDocument());
	QString outputFilename = m_chain -> outputFile();
	
	if (!output)
		return KoFilter::CreationError;
	
	return output -> exportImage(outputFilename) ? KoFilter::OK : KoFilter::BadMimeType;
}

#include <magickexport.moc>

