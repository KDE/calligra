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
    FilterData data;
    data.chain = m_chain;
    data.document = QDomDocument("DOC");
    data.document.appendChild(
        data.document.createProcessingInstruction(
            "xml","version=\"1.0\" encoding=\"UTF-8\""));

    data.mainElement = data.document.createElement("DOC");
    data.mainElement.setAttribute("editor", "KWord's PDF Import Filter");
    data.mainElement.setAttribute("mime", "application/x-kword");
    data.mainElement.setAttribute("syntaxVersion", 2);
    data.document.appendChild(data.mainElement);

    QDomElement element = data.document.createElement("ATTRIBUTES");
    element.setAttribute("processing", 0);
    element.setAttribute("hasHeader", 0);
    element.setAttribute("hasFooter", 0);
    element.setAttribute("unit", "mm");
    data.mainElement.appendChild(element);

    KoFormat format;
    QSize size = doc->paperSize(format);
    data.pageHeight = size.height();

    QDomElement paper = data.document.createElement("PAPER");
    paper.setAttribute("format", format);
    paper.setAttribute("width", size.width());
    paper.setAttribute("height", size.height());
    paper.setAttribute("orientation", doc->paperOrientation());
    paper.setAttribute("columns", 1);
    paper.setAttribute("pages", doc->nbPages());
    paper.setAttribute("columnspacing", 2);
    paper.setAttribute("hType", 0);
    paper.setAttribute("fType", 0);
    paper.setAttribute("spHeadBody", 9);
    paper.setAttribute("spFootBody", 9);
    paper.setAttribute("zoom", 100);
    data.mainElement.appendChild(paper);

    element = data.document.createElement("PAPERBORDERS");
    element.setAttribute("left", 0);
    element.setAttribute("top", 0);
    element.setAttribute("right", 0);
    element.setAttribute("bottom", 0);
    paper.appendChild(element);

    data.framesets = data.document.createElement("FRAMESETS");
    data.mainElement.appendChild(data.framesets);

    // Text frameset
    data.textFrameset = data.document.createElement("FRAMESET");
    data.textFrameset.setAttribute("name", "Text Frameset 1");
    data.textFrameset.setAttribute("frameType", 1);
    data.textFrameset.setAttribute("frameInfo", 0);
    data.textFrameset.setAttribute("autoCreateNewFrame", 1);
    data.textFrameset.setAttribute("removable", 0);
    data.framesets.appendChild(data.textFrameset);

    QDomElement frame = data.document.createElement("FRAME");
    frame.setAttribute("left", 0);
    frame.setAttribute("top", 0);
    frame.setAttribute("bottom", size.width());
    frame.setAttribute("right", size.height());
    frame.setAttribute("runaround", 1);
    data.textFrameset.appendChild(frame);

    // standard style
    QDomElement styles = data.document.createElement("STYLES");
    data.mainElement.appendChild(styles);

    QDomElement style = data.document.createElement("STYLE");
    styles.appendChild(style);

    element = data.document.createElement("FORMAT");
    FilterFont::defaultFont->format(data.document, element, 0, 0, true);
    style.appendChild(element);

    element = data.document.createElement("NAME");
    element.setAttribute("value","Standard");
    style.appendChild(element);

    element = data.document.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    style.appendChild(element);

    // pictures
    data.pictures = data.document.createElement("PICTURES");
    data.mainElement.appendChild(data.pictures);

    // treat pages
    QDomElement bookmarks = data.document.createElement("BOOKMARKS");
    data.mainElement.appendChild(bookmarks);
    doc->initDevice(data);
    for (uint i=1; i<=doc->nbPages(); i++) {
        if ( !range.inside(i) ) continue;
        doc->treatPage(i);
        element = data.document.createElement("BOOKMARKITEM");
        element.setAttribute("name", QString("page%1").arg(i));
        element.setAttribute("cursorIndexStart", 0); // ?
        element.setAttribute("cursorIndexEnd", 0); // ?
        element.setAttribute("frameset", "Text Frameset 1");
        element.setAttribute("startparag", 0); // ?
        element.setAttribute("endparag", 0); // ?
        bookmarks.appendChild(element);
    }

    // clean up
    delete doc;

    // save output
    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);
    if( !out ) {
        kdError(30502) << "Unable to open output file!" << endl;
        return KoFilter::StorageCreationError;
    }
    QCString cstr = data.document.toCString();
    out->writeBlock(cstr,cstr.length());
    out->close();

    out = m_chain->storageFile("documentinfo.xml", KoStore::Write);
    if ( !out ) {
		qWarning("WARNING: unable to write out doc info. continuing anyway");
	} else {
		cstr = infoDocument.toCString();
		out->writeBlock(cstr, cstr.length());
		out->close();
	}

    return KoFilter::OK;
}
