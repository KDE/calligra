#include <KoDocument.h>

#include <KoPADocument.h>
#include <KoPAPageBase.h>

#include <tables/part/Doc.h>
#include <tables/Sheet.h>
#include <tables/Map.h>
#include <tables/PrintSettings.h>
#include <tables/ui/SheetView.h>
#include <tables/SheetPrint.h>
#include <KoZoomHandler.h>
#include <QPainter>

#include <KMimeType>
#include <kmimetypetrader.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>

#include <QApplication>
#include <QBuffer>

KoDocument* openFile(const QString &filename)
{
    const QString mimetype = KMimeType::findByPath(filename)->name();

    QString error;
    KoDocument *document = KMimeTypeTrader::self()->createPartInstanceFromQuery<KoDocument>(
                               mimetype, 0, 0, QString(),
                               QVariantList(), &error );

    if (!error.isEmpty()) {
        kWarning()<< "Error cerating document" << mimetype << error;
        return 0;
    }

    if (0 != document) {
        KUrl url;
        url.setPath(filename);

        document->setCheckAutoSaveFile(false);
        document->setAutoErrorHandlingEnabled(true);

        if (document->openUrl(filename)) {
            document->setReadWrite(false);
        }
        else {
            delete document;
            document = 0;
        }
    }
    return document;
}

void setZoom(const KoPageLayout &pageLayout, const QSize &size, KoZoomHandler &zoomHandler)
{
    qreal zoom = size.width() / (zoomHandler.resolutionX() * pageLayout.width);
    zoom = qMin(zoom, size.height() / (zoomHandler.resolutionY() * pageLayout.height));
    zoomHandler.setZoom(zoom);
}

QRect pageRect(const KoPageLayout &pageLayout, const QSize &size, const KoZoomHandler &zoomHandler)
{
    int width = int(0.5 + zoomHandler.documentToViewX(pageLayout.width));
    int height = int(0.5 + zoomHandler.documentToViewY(pageLayout.height));
    int x = int((size.width() - width) / 2.0);
    int y = int((size.height() - height) / 2.0);
    return QRect(x, y, width, height);
}

QPixmap createSpreadsheetThumbnail(Calligra::Tables::Sheet* sheet, const QSize &thumbSize)
{
    QPixmap thumbnail(thumbSize);
    thumbnail.fill(Qt::white);
    QPainter p(&thumbnail);

    KoPageLayout pageLayout;
    pageLayout.format = KoPageFormat::IsoA4Size;
    pageLayout.leftMargin = 0;
    pageLayout.rightMargin = 0;
    pageLayout.topMargin = 0;
    pageLayout.bottomMargin = 0;
    sheet->printSettings()->setPageLayout(pageLayout);
    sheet->print()->setSettings(*sheet->printSettings(), true);

    Calligra::Tables::SheetView sheetView(sheet);

    // only paint first page for now
    KoZoomHandler zoomHandler;
    setZoom(pageLayout, thumbSize, zoomHandler);
    sheetView.setViewConverter(&zoomHandler);

    QRect range(sheet->print()->cellRange(1));
    // paint also half cells on page edge
    range.setWidth(range.width() + 1);
    //qDebug() << "Range:" << sheet->print()->cellRange(1);

    sheetView.setPaintCellRange(range); // first page
    //qDebug() << "Count:" << sheet->print()->pageCount();

    QRect pRect(pageRect(pageLayout, thumbSize, zoomHandler));
    //qDebug() << "pRect:" << pRect;
    //qDebug() << "pageLayout:" << pageLayout.width << pageLayout.height;

    p.setClipRect(pRect);
    p.translate(pRect.topLeft());
    sheetView.paintCells(p, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0, 0));

    return thumbnail;
}

QList<QPixmap> createThumbnails(KoDocument *document, const QSize &thumbSize)
{
    QList<QPixmap> thumbnails;

    int i = 0;
    if (KoPADocument *doc = qobject_cast<KoPADocument*>(document)) {
        foreach(KoPAPageBase *page, doc->pages(false)) {
            // XXX: creates a pixmap, which is converted to an image and then
            // copied to the image pointer we need for the list
            thumbnails.append(page->thumbnail(thumbSize));
            kDebug() << "create thumbnail" << ++i;
        }
    }
    else if (Calligra::Tables::Doc *doc = qobject_cast<Calligra::Tables::Doc*>(document)) {
        if (0 != doc->map()) {
            foreach(Calligra::Tables::Sheet* sheet, doc->map()->sheetList()) {
                thumbnails.append(createSpreadsheetThumbnail(sheet, thumbSize));
            }
        }
    }

    return thumbnails;
}

void saveThumbnails(const QList<QPixmap> &thumbnails, const QString &filename)
{
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = QString("%1_thumb_%2.png").arg(filename).arg(++i);
        it->save(thumbFilename, "PNG");
    }
}

bool checkThumbnails(const QList<QPixmap> &thumbnails, const QString &filename)
{
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = QString("%1_thumb_%2.png").arg(filename).arg(++i);

        QByteArray ba;
        QBuffer buffer( &ba );
        buffer.open( QIODevice::WriteOnly );
        it->save( &buffer, "PNG" );

        QFile file(thumbFilename);
        if (!file.open( QFile::ReadOnly)) {
            return false;
        }
        QByteArray baCheck(file.readAll());
        kDebug() << "file" << i << (ba == baCheck ? "are identical": "differ" );
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        kDebug() << "please provide file";
        exit(1);
    }

    QApplication app(argc, argv);

    QString filename(argv[1]);

    KoDocument* document = openFile(filename);
    if (!document) {
        exit(2);
    }

    QList<QPixmap> thumbnails(createThumbnails(document, QSize(800,800)));

    kDebug() << "created" << thumbnails.size() << "thumbnails";
    saveThumbnails(thumbnails, "test");
    //checkThumbnails(thumbnails, "test");
}
