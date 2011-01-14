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
#include <kcmdlineargs.h>
#include <kdebug.h>

#include <QApplication>
#include <QBuffer>
#include <QDir>
#include <QFileInfo>

KoDocument* openFile(const QString &filename)
{
    const QString mimetype = KMimeType::findByPath(filename)->name();

    QString error;
    KoDocument *document = KMimeTypeTrader::self()->createPartInstanceFromQuery<KoDocument>(
                               mimetype, 0, 0, QString(),
                               QVariantList(), &error );

    if (!error.isEmpty()) {
        qWarning() << "Error cerating document" << mimetype << error;
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
            kWarning(31000)<< "openUrl failed" << filename << mimetype << error;
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
    sheetView.setPaintCellRange(range); // first page

    QRect pRect(pageRect(pageLayout, thumbSize, zoomHandler));

    p.setClipRect(pRect);
    p.translate(pRect.topLeft());
    sheetView.paintCells(p, QRect(0, 0, pageLayout.width, pageLayout.height), QPointF(0, 0));

    return thumbnail;
}

QList<QPixmap> createThumbnails(KoDocument *document, const QSize &thumbSize)
{
    QList<QPixmap> thumbnails;

    if (KoPADocument *doc = qobject_cast<KoPADocument*>(document)) {
        foreach(KoPAPageBase *page, doc->pages(false)) {
            thumbnails.append(page->thumbnail(thumbSize));
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

void saveThumbnails(const QList<QPixmap> &thumbnails, const QString &dir)
{
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = QString("%1/thumb_%2.png").arg(dir).arg(++i);
        it->save(thumbFilename, "PNG");
    }
}

bool checkThumbnails(const QList<QPixmap> &thumbnails, const QString &dir, bool verbose)
{
    bool success = true;
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = QString("%1/thumb_%2.png").arg(dir).arg(++i);

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        it->save(&buffer, "PNG");

        QFile file(thumbFilename);
        if (!file.open(QFile::ReadOnly)) {
            return false;
        }
        QByteArray baCheck(file.readAll());

        if (ba != baCheck) {
            qDebug() << "Check failed:" << dir << "Page" << i << "differ";
            success = false;
        }
        else if (verbose) {
            qDebug() << "Check successful:" << dir << "Page" << i << "identical";
        }
    }
    return success;
}

int main(int argc, char *argv[])
{
    KCmdLineArgs::init(argc, argv, "cstester", 0, KLocalizedString(), 0, KLocalizedString());

    KCmdLineOptions options;
    options.add("create", ki18n("create verification data for file"));
    options.add("verbose", ki18n("be verbose"));
    options.add("!verify", ki18n("verify the file"));
    options.add( "+file", ki18n("file to use"));
    KCmdLineArgs::addCmdLineOptions(options);

    QApplication app(argc, argv);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("create") && args->isSet("verify")) {
        kError() << "create and verify cannot be used the same time";
        exit(1);
    }

    if (!args->isSet("create") && !args->isSet("verify")) {
        kError() << "one of the options create or verify needs to be specified";
        exit(1);
    }

    bool verbose = args->isSet("verbose");

    int exitValue = 0;

    int successful = 0;
    int failed = 0;
    for (int i=0; i < args->count(); ++i) {
        QString filename(args->arg(i));
        QFileInfo file(filename);
        QString resDir(filename + ".check");
        qDebug() << "filename" << filename << "path" << file.path() << file.completeBaseName() << resDir << file.absoluteFilePath();

        // filename must be a absolute path
        KoDocument* document = openFile(file.absoluteFilePath());
        if (!document) {
            exit(2);
        }

        QList<QPixmap> thumbnails(createThumbnails(document, QSize(800,800)));

        qDebug() << "created" << thumbnails.size() << "thumbnails";
        if (args->isSet("verify")) {
            if (checkThumbnails(thumbnails, resDir, verbose)) {
                ++successful;
            }
            else {
                ++failed;
                exitValue = 2;
            }
        }
        else {
            QDir dir(file.path());
            dir.mkdir(file.fileName() + ".check");
            saveThumbnails(thumbnails, resDir);
        }
    }

    if (args->isSet("verify")) {
        qDebug() << "Totals:" << successful << "passed" << failed << "failed";
    }

    return exitValue;
}
