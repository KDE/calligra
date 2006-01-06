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

#include "kivio_imageexport.h"

#include <qdom.h>
#include <qcstring.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qsize.h>
#include <qstringlist.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <KoStore.h>
#include <koFilterChain.h>
#include <kozoomhandler.h>

#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_screen_painter.h"

#include "kivio_imageexportdialog.h"

typedef KGenericFactory<Kivio::ImageExport, KoFilter> KivioImageExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkivioimageexport, KivioImageExportFactory("KivioImageExport") )

namespace Kivio
{

ImageExport::ImageExport(KoFilter *, const char *, const QStringList&)
  : KoFilter()
{
}

KoFilter::ConversionStatus ImageExport::convert(const QCString& from, const QCString& to)
{
  if(from != "application/x-kivio") {
    return KoFilter::BadMimeType;
  }

  QString format;

  if(to == "image/png") {
    format = "PNG";
  } else if(to == "image/jpeg") {
    format ="JPEG";
  } else {
    return KoFilter::BadMimeType;
  }

  KoStoreDevice* storeIn = m_chain->storageFile("root", KoStore::Read);

  if (!storeIn) {
    KMessageBox::error(0, i18n("Failed to read data."), i18n( "Export Error" ));
    return KoFilter::FileNotFound;
  }

  // Get the XML tree.
  QDomDocument domIn;
  domIn.setContent(storeIn);

  KivioDoc doc;

  if(!doc.loadXML(0, domIn)) {
    KMessageBox::error(0, i18n("Malformed XML data."), i18n("Export Error"));
    return KoFilter::WrongFormat;
  }

  ImageExportDialog dlg;

  QStringList pageNames;
  QPtrList<KivioPage> pageList = doc.map()->pageList();
  QPtrListIterator<KivioPage> it(pageList);

  for(; it.current() != 0; ++it) {
    pageNames.append(it.current()->pageName());
  }

  dlg.setPageList(pageNames);

  if(dlg.exec() != QDialog::Accepted) {
    return KoFilter::UserCancelled;
  }

  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
                            KoGlobal::dpiY());

  KivioPage* page = doc.map()->findPage(dlg.selectedPage());

  if(!page) {
    kdDebug() << "The page named " << dlg.selectedPage() << " wasn't found!!" << endl;
    return KoFilter::InternalError;
  }

  QSize size;

  if(dlg.usePageBorders()) {
    size = QSize(zoom.zoomItX(page->paperLayout().ptWidth), zoom.zoomItY(page->paperLayout().ptHeight));
  } else {
    size = zoom.zoomSize(page->getRectForAllStencils().size());
  }

  QPixmap pixmap = QPixmap(size);
  pixmap.fill(Qt::white);
  KivioScreenPainter kpainter;
  kpainter.start(&pixmap);

  if(!dlg.usePageBorders()) {
    QPoint point = zoom.zoomPoint(page->getRectForAllStencils().topLeft());
    kpainter.setTranslation(-point.x(), -point.y());
  }

  page->printContent(kpainter);

  if(!pixmap.save(m_chain->outputFile(), format.local8Bit())) {
    return KoFilter::CreationError;
  }

  return KoFilter::OK;
}

}

#include "kivio_imageexport.moc"
