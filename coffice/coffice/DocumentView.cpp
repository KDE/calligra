#include "DocumentView.h"

#include <QGraphicsProxyWidget>
#include <QPluginLoader>
#include <QLibrary>
#include <QDebug>

#include <kurl.h>
#include <kpluginfactory.h>

#include <KWPart.h>
#include <KWView.h>
#include <KWDocument.h>
#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>

typedef QObjectList (*pluginsFunc)();

DocumentView::DocumentView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_part(0)
    , m_view(0)
    , m_proxyWidget(0)
{
    //setFlag(QGraphicsItem::ItemIsMovable, true);
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemIsFocusable, true);
    //setFlag(QGraphicsItem::ItemClipsToShape, true);
    //setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
    setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    //setFlag(QGraphicsItem::ItemAcceptsInputMethod, true);
    //setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

//    KoShapeFactoryBase *textShapeFactory = new KoPathShapeFactory(QStringList());
//    KoShapeRegistry::instance()->addFactory(textShapeFactory);//TextShape_SHAPEID);

    qDebug() << "!!! REGISTERED PLUGINS START";
    Q_FOREACH(QObject* o, QPluginLoader::staticInstances()) {
        qDebug() << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>" << o << o->metaObject()->className();
        KPluginFactory *f = dynamic_cast<KPluginFactory*>(o);
        Q_ASSERT(f);
        qDebug() << ">>>" << f->create<QObject>("TextPlugin");
    }
    qDebug() << "!!! REGISTERED PLUGINS END";

    KWPart *part = new KWPart(this);
    m_part = part;

    KWDocument *document = new KWDocument(part);
    document->resourceManager()->setResource(KoText::StyleManager, QVariant::fromValue<KoStyleManager*>(new KoStyleManager(0)));

    part->setDocument(document);

    m_proxyWidget = new QGraphicsProxyWidget(this);
    QWidget *w = new QWidget();
    m_proxyWidget->setWidget(w);

    m_view = part->createView(w);
}

DocumentView::~DocumentView()
{
}

bool DocumentView::init()
{
    return true;
}

bool DocumentView::openFile(const QString &file)
{
    qDebug() << Q_FUNC_INFO << file;
    KUrl url(file);
    return m_part->document()->openUrl(url);
}

void DocumentView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    qDebug() << Q_FUNC_INFO << newGeometry;

    QRectF r(newGeometry);
    r.moveTopLeft(QPointF(0.0,0.0));
    m_proxyWidget->setGeometry(r);
}
