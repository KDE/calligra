/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "pdfimport.h"
#include "pdfimport.moc"

#include <qdom.h>

#include <koFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <koGlobal.h>
#include <koStore.h>
#include <kapplication.h>
#include <kprogress.h>

#include "pdfdocument.h"
#include "misc.h"
#include "dialog.h"


//-----------------------------------------------------------------------------
class PdfImportFactory : KGenericFactory<PdfImport, KoFilter>
{
 public:
    PdfImportFactory(void)
        : KGenericFactory<PdfImport, KoFilter>("kwordpdfimport") {}

 protected:
    virtual void setupTranslations() {
        KGlobal::locale()->insertCatalogue("kwordpdfimport");
    }
};

K_EXPORT_COMPONENT_FACTORY(libpdfimport, PdfImportFactory());

//-----------------------------------------------------------------------------
PdfImport::PdfImport(KoFilter *, const char *, const QStringList&)
{}

KoFilter::ConversionStatus PdfImport::convert(const QCString& from,
                                              const QCString& to)
{
    // check for proper conversion
    if ( to != "application/x-kword" || from != "application/pdf" )
        return KoFilter::NotImplemented;

    // read file
    KoFilter::ConversionStatus result;
    PdfDocument *doc = new PdfDocument(m_chain->inputFile(), QString::null,
                                       QString::null, result);
    if ( result!=KoFilter::OK ) {
        delete doc;
        return result;
    }

    // dialog
    PdfImportDialog* dialog =
        new PdfImportDialog(doc->nbPages(), doc->isEncrypted(), 0);
    dialog->exec();
    SelectionRange range = dialog->range();
    QString ownerPassword = dialog->ownerPassword();
    QString userPassword = dialog->userPassword();
    delete dialog;
    if ( dialog->result()==QDialog::Rejected ) {
        delete doc;
        return KoFilter::UserCancelled;
    }

    // progress dialog
    KProgressDialog pd(0, "progress_dialog", i18n("PDF Import"),
                       i18n("Initializing..."), true);
    pd.setMinimumDuration(0);
    pd.progressBar()->setTotalSteps(range.nbPages());
    pd.progressBar()->setValue(1);
    qApp->processEvents();

    // if passwords : reread file
    if ( !ownerPassword.isEmpty() || !userPassword.isEmpty() ) {
        delete doc;
        doc = new PdfDocument(m_chain->inputFile(), dialog->ownerPassword(),
                              dialog->userPassword(), result);
        if ( result!=KoFilter::OK ) {
            delete doc;
            return result;
        }
    }

    // document information
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
	QDomText authorText = infoDocument.createTextNode( doc->info("Author") );
	fullNameTag.appendChild(authorText);

    QDomElement titleTag = infoDocument.createElement("title");
    aboutTag.appendChild(titleTag);
    QDomText titleText = infoDocument.createTextNode( doc->info("Title") );
	titleTag.appendChild(titleText);

    // document
    KoPageLayout page;
    DRect rect = doc->paperSize(page.format);
    kdDebug(30516) << "size=[" << rect.width() << "," << rect.height() << "]"
                   << endl;
    page.orientation = doc->paperOrientation();
    FilterData data(m_chain, rect, page, doc->nbPages());
    doc->initDevice(data);

    // treat pages
    SelectionRangeIterator it(range);
    for (; it.current()!=-1; it.next()) {
        pd.setLabel(i18n("Treating page %1...").arg(it.current()));
        qApp->processEvents();
        if (pd.wasCancelled()) {
            delete doc;
            return KoFilter::UserCancelled;
        }
        kdDebug(30516) << "treat page: " << it.current() << endl;
        doc->treatPage( it.current() );
        QDomElement element = data.createElement("BOOKMARKITEM");
        element.setAttribute("name", QString("page%1").arg(it.current()));
        element.setAttribute("cursorIndexStart", 0); // ?
        element.setAttribute("cursorIndexEnd", 0); // ?
        element.setAttribute("frameset", "Text Frameset 1");
        element.setAttribute("startparag", 0); // ?
        element.setAttribute("endparag", 0); // ?
        data.bookmarks().appendChild(element);
        pd.progressBar()->advance(1);
    }

    // cleanup
    delete doc;

    // save output
    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);
    if( !out ) {
        kdError(30516) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
//    kdDebug(30516) << data.document().toCString() << endl;
    QCString cstr = data.document().toCString();
    out->writeBlock(cstr, cstr.length());
    out->close();

    out = m_chain->storageFile("documentinfo.xml", KoStore::Write);
    if ( !out )
        kdWarning(30516) << "unable to open doc info. continuing anyway\n";
	else {
		cstr = infoDocument.toCString();
		out->writeBlock(cstr, cstr.length());
		out->close();
	}

    return KoFilter::OK;
}
