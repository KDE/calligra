#include "AppManager.h"
#include "DocumentView.h"
#include "Document.h"
#include "Document_p.h"

#include <QDebug>

#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoCanvasBase.h>

#include <kpluginfactory.h>

#include <KWPart.h>
#include <KWView.h>
#include <KWDocument.h>
#include <Words.h>
#include <KWCanvasItem.h>
#include <KWPage.h>
#include <KWPageManager.h>

/**************************************************************************
 * PartFactory
 */

class PartFactory : public QObject
{
public:
    PartFactory() : QObject()
    {
        initPlugins();
    }
    ~PartFactory()
    {
    }

    KWPart* createPart()
    {
        //TODO reuse the KWPart-factory plugin rather then create manually

        KWPart *part = new KWPart(this);
        KWDocument *document = new KWDocument(part);
        document->setCheckAutoSaveFile(false);
        document->setAutoErrorHandlingEnabled(false);
        document->resourceManager()->setResource(KoText::StyleManager, QVariant::fromValue<KoStyleManager*>(new KoStyleManager(0)));
        part->setDocument(document);
        return part;
    }

private:
    QList<QObject*> m_plugins;

    void initPlugins()
    {
        qDebug() << "REGISTERED PLUGINS START";
        Q_FOREACH(QObject* o, QPluginLoader::staticInstances()) {
            qDebug() << ">>>>>>>>>> PLUGIN FACTORY" << o << o->metaObject()->className() << o->objectName();
            if (KPluginFactory *f = qobject_cast<KPluginFactory*>(o)) {
                QObject *plugin = f->create<QObject>(this);
                Q_ASSERT(plugin);
                if (plugin) {
                    m_plugins.append(plugin);
                    qDebug() << ">>>>>>>>>> PLUGIN INSTANCE" << plugin << plugin->metaObject()->className();
                }
            }

            /*
            if (KoShapeFactoryBase *shapeFactory = dynamic_cast<KoShapeFactoryBase*>(o)) {
                qDebug() << "Register ShapeFactory id=" << shapeFactory->id() << "name=" << shapeFactory->name();
                KoShapeRegistry::instance()->addFactory(shapeFactory); //TextShape_SHAPEID);
            }
            */
        }
        qDebug() << "REGISTERED PLUGINS END";
        Q_ASSERT_X(KoShapeRegistry::instance()->value(TextShape_SHAPEID), __FUNCTION__, "No TextShape-plugin loaded");
    }
};

Q_GLOBAL_STATIC(PartFactory, s_partFactory)

/**************************************************************************
 * Command
 */

class Command
{
public:
    Document *m_doc;
    Command(Document *doc) : m_doc(doc) {}
    virtual ~Command() {}
    virtual void run() = 0;
};

class OpenFileCommand : public Command
{
public:
    QString m_file;
    OpenFileCommand(Document *doc, const QString &file) : Command(doc), m_file(file) {}
    virtual void run()
    {
        qDebug() << Q_FUNC_INFO << m_file;

        delete m_doc->d->m_kopart;
        m_doc->d->m_kopart = 0;

        KoPart *kopart = s_partFactory()->createPart();
        if (!kopart) {
            qWarning() << Q_FUNC_INFO << "Failed to create KoPart";
            QMetaObject::invokeMethod(m_doc->d, "slotOpenFileFailed", Qt::QueuedConnection, Q_ARG(QString, QObject::tr("Unsupported Mimetype")));
            return;
        }

        m_doc->d->m_progressProxy->setValue(0);
        kopart->document()->setProgressProxy(m_doc->d->    m_progressProxy);

        QObject::connect(kopart->document(), SIGNAL(pageSetupChanged()), m_doc->d, SLOT(slotPageSetupChanged()), Qt::DirectConnection);
        QObject::connect(kopart->document(), SIGNAL(layoutFinished()), m_doc->d, SLOT(slotLayoutFinished()), Qt::DirectConnection);
        m_doc->d->m_kopart = kopart;

        bool ok = kopart->document()->openUrl(KUrl(m_file));
        if (!ok) {
            QString error = kopart->document()->errorMessage();
            if (error.isEmpty())
                error = QObject::tr("Cannot load file");
            qWarning() << Q_FUNC_INFO << "Failed to openFile" << m_file << error;
            m_doc->d->m_progressProxy->setValue(-1);
            m_doc->d->m_kopart = 0;
            QMetaObject::invokeMethod(m_doc->d, "slotOpenFileFailed", Qt::QueuedConnection, Q_ARG(QString, error));
            delete kopart;
            return;
        }

        qDebug() << Q_FUNC_INFO << "Successfully openFile=" << m_file;
        m_doc->d->m_progressProxy->setValue(-1);

        //QMetaObject::invokeMethod(m_part, "openFileSucceeded", Qt::QueuedConnection);
    }
};

class UpdatePageCommand : public Command
{
public:
    QSharedPointer<Page> m_page;
    UpdatePageCommand(const QSharedPointer<Page> &page) : Command(page->doc()), m_page(page) {}
    virtual void run()
    {
        qDebug() << Q_FUNC_INFO << "Starting updating page" << m_page->pageNumber() << "document=" << m_doc->file();

        KoPart *kopart = m_doc->d->m_kopart;
        KWDocument *kwdoc = dynamic_cast<KWDocument*>(kopart->document());
        Q_ASSERT(kwdoc);

        KWPageManager *kwPageManager = kwdoc->pageManager();
        Q_ASSERT(kwPageManager);

        KWPage kwpage = kwPageManager->page(m_page->pageNumber());
        Q_ASSERT_X(kwpage.isValid(), __FUNCTION__, qPrintable(QString("No such page number %1 (valid are 1-%2) in document %3").arg(m_page->pageNumber()).arg(kwPageManager->pageCount()).arg(kopart->url().toString())));

        KWCanvasItem *canvasItem = static_cast<KWCanvasItem*>(kwdoc->documentPart()->canvasItem());
        Q_ASSERT(canvasItem);

        KoShapeManager *shapeManager = canvasItem->shapeManager();

        QRectF pageRect = kwpage.rect();

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
        thumbnail.fill(QColor(Qt::white).rgb());

        QImage img = kwpage.thumbnail(pageSize, shapeManager);

        QPainter painter(&thumbnail);
        painter.drawImage(QRectF(QPointF(0.0, 0.0), pageRect.size()), img);
        painter.end();

        qDebug() << Q_FUNC_INFO << "Finished updating page" << m_page.data();
        QMetaObject::invokeMethod(m_doc->d, "slotThumbnailFinished", Qt::QueuedConnection, Q_ARG(int, m_page->pageNumber()), Q_ARG(QImage, thumbnail));
    }
};

/**************************************************************************
 * AppManager
 */

class AppManager::Private
{
public:
    bool m_kill;
    QMutex m_waitMutex, m_workMutex;
    QWaitCondition m_waitCondition;
    QQueue<Command*> m_prendingCommands;
    Private() : m_kill(false) {}
};

AppManager::AppManager()
    : QThread()
    , d(new Private())
{
    QTimer::singleShot(0, this, SLOT(start()));
}

AppManager::~AppManager()
{
    {
        QMutexLocker locker(&d->m_workMutex);
        d->m_kill = true;
        qDeleteAll(d->m_prendingCommands);
        d->m_prendingCommands.clear();
    }
    d->m_waitCondition.wakeAll();
    bool threadFinished = wait();
    Q_ASSERT(threadFinished);
    delete d;
}

bool AppManager::openFile(Document *doc, const QString &file)
{
    OpenFileCommand *command = new OpenFileCommand(doc, file);
    {
        QMutexLocker locker(&d->m_workMutex);
        // abort all pending commands
        qDeleteAll(d->m_prendingCommands);
        d->m_prendingCommands.clear();
        // add the new command
        d->m_prendingCommands.enqueue(command);
    }
    d->m_waitCondition.wakeAll();
    return true;
}

void AppManager::update(const QSharedPointer<Page> &page)
{
    UpdatePageCommand *command = new UpdatePageCommand(page);
    QMutexLocker locker(&d->m_workMutex);
    d->m_prendingCommands.enqueue(command);
    d->m_waitCondition.wakeAll();
}

void AppManager::run()
{
    Q_FOREVER {
        QSharedPointer<Command> command;
        {
            QMutexLocker locker(&d->m_workMutex);
            if (d->m_kill) {
                qDebug() << Q_FUNC_INFO << "Got killed";
                return;
            }
            if (!d->m_prendingCommands.isEmpty()) {
                command = QSharedPointer<Command>(d->m_prendingCommands.dequeue());
            }
        }

        if (command) {
            if (OpenFileCommand *openFileCommand = dynamic_cast<OpenFileCommand*>(command.data())) {
                openFileCommand->run();
            } else if (UpdatePageCommand *updatePageCommand = dynamic_cast<UpdatePageCommand*>(command.data())) {
                updatePageCommand->run();
            } else {
                Q_ASSERT_X(false, __FUNCTION__, "Unknown Command");
            }
        } else {
            // No command in the queue. Wait till a new one arrives.
            d->m_waitCondition.wait(&d->m_waitMutex);
        }
    }
}
