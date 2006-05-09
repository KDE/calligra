/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lukas Tinkl <lukas@kde.org>

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

#include <stdlib.h>

#include <qtextcodec.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktrader.h>
#include <kservice.h>
#include <ktempfile.h>

#include "generic_filter.h"

typedef KGenericFactory<GenericFilter> GenericFilterFactory;
K_EXPORT_COMPONENT_FACTORY( libgenerickofilter, GenericFilterFactory )


GenericFilter::GenericFilter(QObject *parent, const QStringList&) :
    KoFilter(parent) {
}

KoFilter::ConversionStatus GenericFilter::convert( const QByteArray &from, const QByteArray &to )
{

    //find the right script to use
    KTrader::OfferList offers = KTrader::self()->query("KOfficeGenericFilter",
                                "(Type == 'Service') and ('KOfficeGenericFilter' in ServiceTypes) and (exist Exec)");

    if (offers.isEmpty())
        return KoFilter::NotImplemented;

    KTrader::OfferList::ConstIterator it;
    for (it=offers.begin(); it!=offers.end(); ++it)
    {
        kDebug() << "Got a filter script, exec: " << (*it)->exec() <<
            ", imports: " << (*it)->property("X-KDE-Wrapper-Import").toString() <<
            ", exports: " << (*it)->property("X-KDE-Wrapper-Export").toString() << endl;
        if ((*it)->property("X-KDE-Wrapper-Import").toString()==from
            && (*it)->property("X-KDE-Wrapper-Export").toString()==to)
        {
            m_exec=(*it)->exec();
            m_from=from;
            m_to=to;
            break;
        }
    }

    //decide between import/export
    if( m_to == "application/x-kword" || m_to == "application/x-karbon" ||
        m_to == "application/x-kspread" || m_to == "application/x-kivio" ||
        m_to == "application/x-kchart" || m_to == "application/x-kpresenter" )
        return doImport();
    else if ( m_from == "application/x-kword" || m_from == "application/x-karbon" ||
              m_from == "application/x-kspread" || m_from == "application/x-kivio" ||
              m_from == "application/x-kchart" || m_from == "application/x-kpresenter" )
        return doExport();
    else
        return KoFilter::NotImplemented;
}

KoFilter::ConversionStatus GenericFilter::doImport()
{
    KTempFile temp(QString("genericfilter-"));
    temp.setAutoDelete(true);

    QFile tempFile(temp.name());

    m_out = KoStore::createStore(&tempFile, KoStore::Write);

    if (!m_out || !m_out->open("root"))
    {
        kError() << "Unable to create output store!" << endl;
        m_out->close();
        return KoFilter::StorageCreationError;
    }
    else
    {
        QString exec = m_exec + " " + KProcess::quote(m_chain->inputFile()) + " "
                       + KProcess::quote(m_chain->outputFile());
        system(QFile::encodeName(exec));

        kDebug() << "Executing: " << exec << endl;

        QFile outFile(m_chain->outputFile());
        outFile.open(QIODevice::ReadOnly);
        QByteArray outData = outFile.readAll();
        if (outData.size()==0) {
            m_out->close();
            return KoFilter::UnexpectedEOF;
        }
        else {
            m_out->write(outData);
            m_out->close();
        }
    }

    return KoFilter::OK;
}

KoFilter::ConversionStatus GenericFilter::doExport()
{
    return KoFilter::NotImplemented;
}

#include "generic_filter.moc"
