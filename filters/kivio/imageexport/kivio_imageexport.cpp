/* This file is part of the KDE project
   Copyright (C) 2005-2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Kåre Särs <kare.sars@kolumbus.fi>

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
#include <KoFilterChain.h>
#include <KoZoomHandler.h>

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
    format = "JPEG";
  } else if(to == "image/x-bmp") {
    format = "BMP";
  } else if(to == "image/x-eps") {
    format = "EPS";
  } else if(to == "image/x-portable-bitmap") {
    format = "PBM";
  } else if(to == "image/x-pcx") {
    format = "PCX";
  } else if(to == "image/x-portable-pixmap") {
    format = "PPM";
  } else if(to == "image/x-rgb") {
    format = "RGB";
  } else if(to == "image/x-xpm") {
    format = "XPM";
  } else if(to == "image/jp2") {
    format = "JP2";
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

  dlg.setInitialDPI(300);
  dlg.setInitialmargin(10);

  if(dlg.exec() != QDialog::Accepted) {
    return KoFilter::UserCancelled;
  }

  KivioPage* page = doc.map()->findPage(dlg.selectedPage());

  if(!page) {
    kdDebug() << "The page named " << dlg.selectedPage() << " wasn't found!!" << endl;
    return KoFilter::InternalError;
  }

  float z = (float)dlg.imageDPI()/(float)KoGlobal::dpiX();
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(qRound(z * 100), KoGlobal::dpiX(), KoGlobal::dpiY());

  QSize size;
  if(dlg.usePageBorders()) {
    size = QSize(zoom.zoomItX(page->paperLayout().ptWidth), zoom.zoomItY(page->paperLayout().ptHeight));
  } else {
    size = zoom.zoomSize(page->getRectForAllStencils().size());
  }

  kdDebug() << "KoGlobal::dpiX() " << KoGlobal::dpiX() << " KoGlobal::dpiY() " << KoGlobal::dpiY() << endl;

  int border = dlg.margin();

  size.setWidth(size.width() + (border * 2));
  size.setHeight(size.height() + (border * 2));

  QPixmap pixmap = QPixmap(size);
  pixmap.fill(Qt::white);
  KivioScreenPainter kpainter;
  kpainter.start(&pixmap);

  float translationX = border;
  float translationY = border;

  if(!dlg.usePageBorders()) {
    QPoint point = zoom.zoomPoint(page->getRectForAllStencils().topLeft());
    translationX -= point.x();
    translationY -= point.y();
  }

  kpainter.setTranslation(translationX, translationY);
  page->printContent(kpainter, &zoom);

  if(!pixmap.save(m_chain->outputFile(), format.local8Bit())) {
    return KoFilter::CreationError;
  }

  return KoFilter::OK;
}

}

#include "kivio_imageexport.moc"
