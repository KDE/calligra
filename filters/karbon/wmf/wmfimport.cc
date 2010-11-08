/*  This file is part of the KDE project
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    Copyright (c) 2007 Jan Hambrecht <jaham@gmx.net>

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

DESCRIPTION
*/

#include "wmfimport.h"
#include "wmfimportparser.h"

#include <KarbonPart.h>
#include <KarbonDocument.h>

#include <KoFilterChain.h>

#include <KDebug>
#include <KPluginFactory>

K_PLUGIN_FACTORY(WMFImportFactory, registerPlugin<WMFImport>();)
K_EXPORT_PLUGIN(WMFImportFactory("kofficefilters"))


WMFImport::WMFImport(QObject *parent, const QVariantList&) :
        KoFilter(parent)
{
}

WMFImport::~WMFImport()
{
}

KoFilter::ConversionStatus WMFImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/vnd.oasis.opendocument.graphics" || from != "image/x-wmf")
        return KoFilter::NotImplemented;

    WMFImportParser wmfParser;
    if (!wmfParser.load(QString(m_chain->inputFile()))) {
        return KoFilter::WrongFormat;
    }

    KarbonPart * part = dynamic_cast<KarbonPart*>(m_chain->outputDocument());
    if (! part)
        return KoFilter::CreationError;

    // Do the conversion!
    if (!wmfParser.play(part->document())) {
        return KoFilter::WrongFormat;
    }

    return KoFilter::OK;
}


#include <wmfimport.moc>
