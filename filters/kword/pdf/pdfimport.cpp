/*
 * Copyright (c) 2002-2003 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "pdfimport.h"
#include "pdfimport.moc"

#include <qdom.h>
#include <qdatetime.h> // debug
//Added by qt3to4:
#include <Q3CString>

#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <KoGlobal.h>
#include <KoStore.h>
#include <kapplication.h>
#include <kprogressdialog.h>

#include "data.h"


using namespace PDFImport;

//-----------------------------------------------------------------------------
class PdfImportFactory : KGenericFactory<PdfImport>
{
 public:
    PdfImportFactory()
        : KGenericFactory<PdfImport>("kwordpdfimport") {}

 protected:
    virtual void setupTranslations() {
        KGlobal::locale()->insertCatalog("kofficefilters");
    }
};

K_EXPORT_COMPONENT_FACTORY(libpdfimport, PdfImportFactory())

//-----------------------------------------------------------------------------
PdfImport::PdfImport(QObject* parent, const QStringList&)
    : KoFilter(parent)
{}

KoFilter::ConversionStatus PdfImport::convert(const QByteArray& from,
                                              const QByteArray& to)
{
    // check for proper conversion
    if ( to!="application/x-kword" || from!="application/pdf" )
        return KoFilter::NotImplemented;

    // read file
    KoFilter::ConversionStatus result
        = _doc.init(m_chain->inputFile(), QString::null, QString::null);
    if ( result!=KoFilter::OK ) return result;

    // options dialog
    {
        Dialog dialog(_doc.nbPages(), _doc.isEncrypted(), 0);
        dialog.exec();
        if ( dialog.result()==QDialog::Rejected )
            return KoFilter::UserCancelled;
        _options = dialog.options();
    }

    // progress dialog
    KProgressDialog pd(0, i18n("PDF Import"),
                       i18n("Initializing..."), true);
    pd.setMinimumDuration(0);
    pd.progressBar()->setMaximum( _options.range.nbPages()*2 );
    pd.progressBar()->setValue(1);
    qApp->processEvents();

    // if passwords : reread file
    if ( !_options.ownerPassword.isEmpty()
         || !_options.userPassword.isEmpty() ) {
        result = _doc.init(m_chain->inputFile(), _options.ownerPassword,
                           _options.userPassword);
        if ( result!=KoFilter::OK ) return result;
    }

    // data
    KoPageLayout page;
    DRect rect = _doc.paperSize(page.format);
    kDebug(30516) << "paper size: " << rect.toString() << endl;
    page.orientation = _doc.paperOrientation();
    Data data(m_chain, rect, page, _options);
    _doc.initDevice(data);

    // treat pages
    QTime time;
    time.start();
    SelectionRangeIterator it(_options.range);
    for (uint k=0; k<2; k++) {
        bool first = ( k==0 );
        data.pageIndex = 0;
        if ( !first ) _doc.init();
        for (it.toFirst(); it.current()!=it.end(); it.next()) {
            QString s = (first ? i18n("First pass: page #%1...")
                         : i18n("Second pass: page #%1..."));
            pd.setLabel( s.arg(it.current()) );
            qApp->processEvents();
            if (pd.wasCancelled()) return KoFilter::UserCancelled;
            kDebug(30516) << "-- " << "pass #" << k
                           << "  treat page: " << it.current()
                           << "----------------" << endl;
            if (first) _doc.treatPage( it.current() );
            else _doc.dumpPage(data.pageIndex);
            pd.progressBar()->advance(1);
            data.pageIndex++;
        }
    }
    data.endDump();
    kDebug(30516) << "treatement elapsed=" << time.elapsed() << endl;

    // output
    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);
    if( !out ) {
        kError(30516) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
//    kDebug(30516) << data.document().toByteArray() << endl;
    QByteArray cstr = data.document().toByteArray();
    out->write(cstr);
    out->close();

    treatInfoDocument();

    return KoFilter::OK;
}

void PdfImport::treatInfoDocument()
{
    QDomDocument infoDocument("document-info");
    infoDocument.appendChild(
        infoDocument.createProcessingInstruction(
            "xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement infoElement = infoDocument.createElement( "document-info" );
	infoDocument.appendChild(infoElement);

	QDomElement aboutTag = infoDocument.createElement("about");
    infoElement.appendChild(aboutTag);

    QDomElement authorTag = infoDocument.createElement("author");
    infoElement.appendChild(authorTag);
    QDomElement fullNameTag = infoDocument.createElement("full-name");
    authorTag.appendChild(fullNameTag);
	QDomText authorText = infoDocument.createTextNode( _doc.info("Author") );
	fullNameTag.appendChild(authorText);

    QDomElement titleTag = infoDocument.createElement("title");
    aboutTag.appendChild(titleTag);
    QDomText titleText = infoDocument.createTextNode( _doc.info("Title") );
	titleTag.appendChild(titleText);

    // output
    KoStoreDevice *out =
        m_chain->storageFile("documentinfo.xml", KoStore::Write);
    if ( !out )
        kWarning(30516) << "unable to open doc info. continuing anyway\n";
	else {
		QByteArray cstr = infoDocument.toByteArray();
		out->write(cstr);
		out->close();
	}
}
