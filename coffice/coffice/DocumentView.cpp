#include "DocumentView.h"

#include <QGraphicsProxyWidget>
#include <QPluginLoader>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <QDebug>

#include <kurl.h>
#include <kpluginfactory.h>

#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoCanvasBase.h>

#include <KWPart.h>
#include <KWView.h>
#include <KWDocument.h>
#include <Words.h>
#include <KWCanvasItem.h>
#include <KWPage.h>
#include <KWPageManager.h>

class DocumentPage::Private
{
public:
    KWPage m_page;
    DocumentView *m_view;
    Private(const KWPage &page, DocumentView *view) : m_page(page), m_view(view) {}
};

DocumentPage::DocumentPage(const KWPage &page, DocumentView *view)
    : QGraphicsPixmapItem(view)
    , d(new Private(page, view))
{
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setTransformationMode(Qt::SmoothTransformation);
}

DocumentPage::~DocumentPage()
{
    delete d;
}

class DocumentView::Private
{
public:
    KoPart *m_part;

    Private() : m_part(0) {}

    void initPlugins()
    {
        qDebug() << "REGISTERED PLUGINS START";
        Q_FOREACH(QObject* o, QPluginLoader::staticInstances()) {
            qDebug() << ">>>>>>>>>> PLUGIN FACTORY" << o << o->metaObject()->className();
            KPluginFactory *f = dynamic_cast<KPluginFactory*>(o);
            Q_ASSERT(f);
            QObject *plugin = f->create<QObject>("TextPlugin");
            Q_ASSERT(plugin);
            qDebug() << ">>>>>>>>>> PLUGIN INSTANCE" << plugin << plugin->metaObject()->className();

            /*
            if (KoShapeFactoryBase *shapeFactory = dynamic_cast<KoShapeFactoryBase*>(o)) {
                qDebug() << "Register ShapeFactory id=" << shapeFactory->id() << "name=" << shapeFactory->name();
                KoShapeRegistry::instance()->addFactory(shapeFactory); //TextShape_SHAPEID);
            }
            */
        }
        qDebug() << "REGISTERED PLUGINS END";
        Q_ASSERT(KoShapeRegistry::instance()->value(TextShape_SHAPEID));
    }
};

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , d(new Private())
{
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    d->initPlugins();

    KWPart *part = new KWPart(this);
    d->m_part = part;

    KWDocument *document = new KWDocument(part);
    document->setCheckAutoSaveFile(false);
    document->setAutoErrorHandlingEnabled(false);
    document->resourceManager()->setResource(KoText::StyleManager, QVariant::fromValue<KoStyleManager*>(new KoStyleManager(0)));
    part->setDocument(document);
}

DocumentView::~DocumentView()
{
    delete d;
}

bool DocumentView::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;

    KUrl url(file);
    bool ok = d->m_part->document()->openUrl(url);

    KWDocument *kwdoc = dynamic_cast<KWDocument*>(d->m_part->document());
    KWCanvasItem *canvasItem = static_cast<KWCanvasItem*>(kwdoc->documentPart()->canvasItem());
    KoZoomHandler zoomHandler;
    while (!kwdoc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();
        if (!QCoreApplication::hasPendingEvents())
            break;
    }
    KWPageManager *pageManager = kwdoc->pageManager();
    KoShapeManager *shapeManager = canvasItem->shapeManager();
    qreal margin = 10.0;
    qreal y = margin;
    qreal maxWidth = 0.0;
    foreach(const KWPage &page, pageManager->pages()) {
        qDebug()<<"PAGE="<<page.pageNumber();
        QRectF pageRect = page.rect();

        /*
        QRectF pRect(page.rect());
        KoPageLayout layout;
        layout.width = pRect.width();
        layout.height = pRect.height();

        KoPAUtil::setZoom(layout, thumbSize, zoomHandler);
        QRect pageRect = KoPAUtil::pageRect(layout, thumbSize, zoomHandler);
        */

        QSize pageSize = pageRect.size().toSize();
        QImage thumbnail(pageSize, QImage::Format_RGB32);
        thumbnail.fill(QColor(Qt::red).rgb());

        QPainter painter(&thumbnail);
        QImage img = page.thumbnail(pageSize, shapeManager);
        painter.drawImage(pageRect, img);
        painter.setPen(Qt::black);
        painter.drawRect(pageRect);
        painter.end();

        DocumentPage *pageItem = new DocumentPage(page, this);
        pageItem->setPixmap(QPixmap::fromImage(img));

        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
        effect->setBlurRadius(6);
        pageItem->setGraphicsEffect(effect);

        pageItem->setPos(margin, y);
        y += pageItem->boundingRect().height();
        y += margin;
        maxWidth = qMax(maxWidth, pageItem->boundingRect().width());
    }

    setImplicitWidth(maxWidth + margin * 2);
    setImplicitHeight(y);

    return ok;
}

bool DocumentView::openFileWithDialog()
{
    QFileDialog *dlg = new QFileDialog();
    dlg->setAcceptMode(QFileDialog::AcceptOpen);
    dlg->setFileMode(QFileDialog::ExistingFile);
    dlg->setOption(QFileDialog::ReadOnly);
    QStringList filters;
    filters << "ODT files (*.odt)"
            << "Any files (*)";
    dlg->setNameFilters(filters);
    bool ok = false;
    if (dlg->exec() && dlg && !dlg->selectedFiles().isEmpty()) {
        QString file = dlg->selectedFiles().first();
        if (!file.isEmpty())
            ok = openFile(file);
    }
    delete dlg;
    return ok;
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;

    //QRectF r(newGeometry);
    //r.moveTopLeft(QPointF(0.0, 0.0));
    //m_proxyWidget->setGeometry(r);
}
