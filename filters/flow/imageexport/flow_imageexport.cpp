/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

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
   Boston, MA 02110-1301, USA.
*/

#include "flow_imageexport.h"

#include <QDomDocument>
#include <QPixmap>
#include <QPainter>
#include <QSize>
#include <QStringList>
#include <QList>

#include <kmessagebox.h>
#include <klocale.h>
#include <kpluginfactory.h>
#include <kdebug.h>

#include <KoStore.h>
#include <KoFilterChain.h>
#include <KoZoomHandler.h>

#include "flow_doc.h"
#include "flow_page.h"
#include "flow_map.h"
#include "flow_screen_painter.h"

#include "flow_imageexportdialog.h"

K_PLUGIN_FACTORY(FlowImageExportFactory, registerPlugin<Flow::ImageExport>();)
//K_EXPORT_PLUGIN(FlowImageExportFactory("FlowImageExport"))

namespace Flow
{

ImageExport::ImageExport(KoFilter *, const char *, const QStringList&)
        : KoFilter(parent)
{
}

KoFilter::ConversionStatus ImageExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/x-flow") {
        return KoFilter::BadMimeType;
    }

    QString format;

    if (to == "image/png") {
        format = "PNG";
    } else if (to == "image/jpeg") {
        format = "JPEG";
    } else if (to == "image/bmp") {
        format = "BMP";
    } else if (to == "image/x-eps") {
        format = "EPS";
    } else if (to == "image/x-portable-bitmap") {
        format = "PBM";
    } else if (to == "image/x-pcx") {
        format = "PCX";
    } else if (to == "image/x-portable-pixmap") {
        format = "PPM";
    } else if (to == "image/x-rgb") {
        format = "RGB";
    } else if (to == "image/x-xpixmap") {
        format = "XPM";
    } else if (to == "image/jpeg2000") {
        format = "JP2";
    } else {
        return KoFilter::BadMimeType;
    }

    KoStoreDevice* storeIn = m_chain->storageFile("root", KoStore::Read);

    if (!storeIn) {
        KMessageBox::error(0, i18n("Failed to read data."), i18n("Export Error"));
        return KoFilter::FileNotFound;
    }

    // Get the XML tree.
    QDomDocument domIn;
    domIn.setContent(storeIn);

    FlowDoc doc;

    if (!doc.loadXML(0, domIn)) {
        KMessageBox::error(0, i18n("Malformed XML data."), i18n("Export Error"));
        return KoFilter::WrongFormat;
    }

    ImageExportDialog dlg;

    QStringList pageNames;
    QList<FlowPage*> pageList = doc.map()->pageList();

    foreach(FlowPage* page, pageList) {
        pageNames.append(page->pageName());
    }

    KoZoomHandler zoom;

    dlg.setPageList(pageNames);
    FlowPage* page = doc.map()->firstPage();
    QSize size = QSize(zoom.zoomItX(page->paperLayout().ptWidth), zoom.zoomItY(page->paperLayout().ptHeight));
    dlg.setInitialCustomSize(size);

    if (dlg.exec() != QDialog::Accepted) {
        return KoFilter::UserCancelled;
    }

    page = doc.map()->findPage(dlg.selectedPage());

    if (!page) {
        kDebug() << "The page named" << dlg.selectedPage() << " wasn't found!!";
        return KoFilter::InternalError;
    }

    if (dlg.usePageBorders()) {
        size = QSize(zoom.zoomItX(page->paperLayout().ptWidth), zoom.zoomItY(page->paperLayout().ptHeight));
    } else {
        size = zoom.zoomSize(page->getRectForAllStencils().size());
    }

    if (dlg.useCustomSize()) {
        QSize customSize = dlg.customSize();
        float zw = (float)customSize.width() / (float)size.width();
        float zh = (float)customSize.height() / (float)size.height();
        float z = qMin(zw, zh);

	converter.setZoom( z );
	converter.setDpi( KoGlobal::dpiX(), KoGlobal::dpiY() );

        size = customSize;
    }

    int border = dlg.margin();

    size.setWidth(size.width() + (border * 2));
    size.setHeight(size.height() + (border * 2));

    QPixmap pixmap = QPixmap(size);
    pixmap.fill(Qt::white);
    FlowScreenPainter kpainter;
    kpainter.start(&pixmap);

    int translationX = border;
    int translationY = border;

    if (!dlg.usePageBorders()) {
        QPoint point = zoom.zoomPoint(page->getRectForAllStencils().topLeft());
        translationX += point.x();
        translationY += point.y();
    }

    kpainter.setTranslation(-translationX, -translationY);
    page->printContent(kpainter, &zoom);

    if (!pixmap.save(m_chain->outputFile(), format.local8Bit())) {
        return KoFilter::CreationError;
    }

    return KoFilter::OK;
}

}

#include "flow_imageexport.moc"
