/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "pdfimport.h"
#include "pdfimport.moc"

#include <qdom.h>

#include <koFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <koGlobal.h>
#include <koStore.h>

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
    : KoFilter()
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
    if ( !dialog->exec() ) {
        delete dialog;
        delete doc;
        return KoFilter::UserCancelled;
    }
    SelectionRange range = dialog->range();

    // if passwords : reread file
    if ( !dialog->ownerPassword().isEmpty() ||
         !dialog->userPassword().isEmpty() ) {
        delete doc;
        doc = new PdfDocument(m_chain->inputFile(), dialog->ownerPassword(),
                              dialog->userPassword(), result);
        Q_ASSERT( result==KoFilter::OK);
    }
    delete dialog;

    // document information
    QDomDocument docinfo("document-info");
    docinfo.appendChild(
        docinfo.createProcessingInstruction(
            "xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement infodoc = docinfo.createElement( "document-info" );
	docinfo.appendChild(infodoc);

	QDomElement aboutTag = docinfo.createElement("about");
    infodoc.appendChild(aboutTag);

    QDomElement authorTag = docinfo.createElement("author");
    QDomElement fullNameTag = docinfo.createElement("full-name");
	QDomText authorText = docinfo.createTextNode( doc->info("Author") );
	fullNameTag.appendChild(authorText);
	authorTag.appendChild(fullNameTag);
    infodoc.appendChild(authorTag);

	QDomElement titleTag = docinfo.createElement("title");
    QDomText titleText = docinfo.createTextNode( doc->info("Title") );
	titleTag.appendChild(titleText);
	aboutTag.appendChild(titleTag);

    // document
    QDomDocument mainDocument("DOC");
    mainDocument.appendChild(
        mainDocument.createProcessingInstruction(
            "xml","version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement elementDoc = mainDocument.createElement("DOC");
    elementDoc.setAttribute("editor", "KWord's PDF Import Filter");
    elementDoc.setAttribute("mime", "application/x-kword");
    elementDoc.setAttribute("syntaxVersion", 2);
    mainDocument.appendChild(elementDoc);

    QDomElement element = mainDocument.createElement("ATTRIBUTES");
    element.setAttribute("processing", 0);
    element.setAttribute("hasHeader", 0);
    element.setAttribute("hasFooter", 0);
    element.setAttribute("unit", "mm");
    elementDoc.appendChild(element);

    KoFormat format;
    QSize size = doc->paperSize(format);

    QDomElement elementPaper = mainDocument.createElement("PAPER");
    elementPaper.setAttribute("format", format);
    elementPaper.setAttribute("width", size.width());
    elementPaper.setAttribute("height", size.height());
    elementPaper.setAttribute("orientation", doc->paperOrientation());
    elementPaper.setAttribute("columns", 1);
    elementPaper.setAttribute("pages", doc->nbPages());
    elementPaper.setAttribute("columnspacing", 2);
    elementPaper.setAttribute("hType", 0);
    elementPaper.setAttribute("fType", 0);
    elementPaper.setAttribute("spHeadBody", 9);
    elementPaper.setAttribute("spFootBody", 9);
    elementPaper.setAttribute("zoom", 100);
    elementDoc.appendChild(elementPaper);

    // #### FIXME ?
    element = mainDocument.createElement("PAPERBORDERS");
    element.setAttribute("left", 0);
    element.setAttribute("top", 0);
    element.setAttribute("right", 0);
    element.setAttribute("bottom", 0);
    elementPaper.appendChild(element);

    QDomElement framesetsPluralElementOut =
        mainDocument.createElement("FRAMESETS");
    mainDocument.documentElement().appendChild(framesetsPluralElementOut);

    QDomElement mainFramesetElement = mainDocument.createElement("FRAMESET");
    mainFramesetElement.setAttribute("name", "Text Frameset 1");
    mainFramesetElement.setAttribute("frameType", 1);
    mainFramesetElement.setAttribute("frameInfo", 0);
    mainFramesetElement.setAttribute("autoCreateNewFrame", 1);
    mainFramesetElement.setAttribute("removable", 0);
    framesetsPluralElementOut.appendChild(mainFramesetElement);

    // #### FIXME ?
    QDomElement frameElementOut = mainDocument.createElement("FRAME");
    frameElementOut.setAttribute("left", 0);
    frameElementOut.setAttribute("top", 0);
    frameElementOut.setAttribute("bottom", size.width());
    frameElementOut.setAttribute("right", size.height());
    frameElementOut.setAttribute("runaround", 1);
    mainFramesetElement.appendChild(frameElementOut);

    QDomElement elementStylesPlural = mainDocument.createElement("STYLES");
    elementDoc.appendChild(elementStylesPlural);

    QDomElement elementStyleStandard = mainDocument.createElement("STYLE");
    elementStylesPlural.appendChild(elementStyleStandard);

    element = mainDocument.createElement("FORMAT");
    FilterFont::defaultFont->format(mainDocument, element, 0, 0, true);
    elementStyleStandard.appendChild(element);

    element = mainDocument.createElement("NAME");
    element.setAttribute("value","Standard");
    elementStyleStandard.appendChild(element);

    element = mainDocument.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    elementStyleStandard.appendChild(element);

    // treat pages
    QDomElement bookmarks = mainDocument.createElement("BOOKMARKS");
    doc->initDevice(mainDocument, mainFramesetElement);
    for (uint i=1; i<=doc->nbPages(); i++) {
        if ( !range.inside(i) ) continue;
        doc->treatPage(i);
        element = mainDocument.createElement("BOOKMARKITEM");
        element.setAttribute("name", QString("page%1").arg(i));
        element.setAttribute("cursorIndexStart", 0); // ?
        element.setAttribute("cursorIndexEnd", 0); // ?
        element.setAttribute("frameset", "Text Frameset 1");
        element.setAttribute("startparag", 0); // ?
        element.setAttribute("endparag", 0); // ?
        bookmarks.appendChild(element);
    }
    elementDoc.appendChild(bookmarks);

    // clean up
    delete doc;

    // save output
    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = mainDocument.toCString();
    out->writeBlock(cstr,cstr.length());
    out->close();

    out = m_chain->storageFile("documentinfo.xml", KoStore::Write);
    if ( !out ) {
		qWarning("WARNING: unable to write out doc info. continuing anyway");
	} else {
		cstr = docinfo.toCString();
		out->writeBlock(cstr, cstr.length());
		out->close();
	}

    return KoFilter::OK;
}
