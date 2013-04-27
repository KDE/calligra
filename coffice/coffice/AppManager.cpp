#include "AppManager.h"
#include "DocumentView.h"
#include "Document.h"
#include "Document_p.h"

#include <QPointer>
#include <QSharedPointer>
#include <QTemporaryFile>
#include <QDebug>

#include <KoStyleManager.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeBasedDocumentBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoCanvasBase.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoProgressUpdater.h>
#include <KoUpdater.h>

#include <kpluginfactory.h>
#include <kmimetype.h>

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
        qDeleteAll(m_plugins);
        qDeleteAll(m_filterPlugins);
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

    QPair<QString, KoFilter*> filterForFile(const QString &file)
    {
        KMimeType::Ptr mime = KMimeType::findByUrl(KUrl(file));
        if (!mime || mime->name().isEmpty())
            return QPair<QString, KoFilter*>(); // unknown/unsupported file format
        if (mime->name().startsWith("application/vnd.oasis.opendocument."))
            return QPair<QString, KoFilter*>(); // ODF, native buildin file format
        Q_FOREACH(KoFilter *f, m_filterPlugins) {
            //TODO not hardcode the filter mimetypes but use e.g. decorator/lookup-table
            if (f->metaObject()->className() == QLatin1String("DocxImport")) {
                if (mime->name().startsWith("application/vnd.openxmlformats-officedocument.wordprocessingml.document") ||
                    mime->name().startsWith("application/vnd.openxmlformats-officedocument.wordprocessingml.template") ||
                    mime->name().startsWith("application/vnd.ms-word.document.macroEnabled.12") ||
                    mime->name().startsWith("application/vnd.ms-word.template.macroEnabled.12") ) {
                    return QPair<QString, KoFilter*>(mime->name(), f); // Microsoft Office OOXML file format
                }
            } else if (f->metaObject()->className() == QLatin1String("MSWordOdfImport")) {
                if (mime->name().startsWith("application/msword")) {
                    return QPair<QString, KoFilter*>(mime->name(), f); // Microsoft Office binary file format
                }
            }
        }
        return QPair<QString, KoFilter*>();
    }

private:
    QList<QObject*> m_plugins;
    QList<KoFilter*> m_filterPlugins;

    bool loadLibrary(const QString &lib)
    {
        QLibrary loader(lib);
        bool ok = loader.load();
        if (!ok)
            qWarning() << Q_FUNC_INFO << "Failed loading library=" << lib << "error=" << loader.errorString();
        return ok;
    }

    void initPlugins()
    {
#ifndef Q_OS_ANDROID
        bool calligraFiltersLibraryLoaded = loadLibrary("calligrafilters");
        Q_ASSERT_X(calligraFiltersLibraryLoaded, __FUNCTION__, "Failed to load calligrafilters library");

        bool calligraPluginsLibraryLoaded = loadLibrary("calligraplugins");
        Q_ASSERT_X(calligraPluginsLibraryLoaded, __FUNCTION__, "Failed to load calligraplugins library");
#endif

        qDebug() << "REGISTERED PLUGINS START";
        Q_FOREACH(QObject* o, QPluginLoader::staticInstances()) {
            qDebug() << ">>>>>>>>>> PLUGIN FACTORY" << o << o->metaObject()->className() << o->objectName();
            if (KPluginFactory *f = qobject_cast<KPluginFactory*>(o)) {
                QObject *plugin = f->create<QObject>(this);
                Q_ASSERT(plugin);
                if (!plugin)
                    continue;
                qDebug() << ">>>>>>>>>> PLUGIN INSTANCE" << plugin << plugin->metaObject()->className();
                if (KoFilter *kofilter = dynamic_cast<KoFilter*>(plugin)) {
                    m_filterPlugins.append(kofilter);
                } else {
                    m_plugins.append(plugin);
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
        kopart->document()->setProgressProxy(m_doc->d->m_progressProxy);

        QObject::connect(kopart->document(), SIGNAL(pageSetupChanged()), m_doc->d, SLOT(slotPageSetupChanged()), Qt::DirectConnection);
        QObject::connect(kopart->document(), SIGNAL(layoutFinished()), m_doc->d, SLOT(slotLayoutFinished()), Qt::DirectConnection);
        m_doc->d->m_kopart = kopart;

        QString inputFile = m_file;
        QSharedPointer<QTemporaryFile> outputTempFile;
        QPair<QString, KoFilter*> filter = s_partFactory()->filterForFile(inputFile);
        if (filter.second) {
            QString error;
            if (!importFile(filter, inputFile, outputTempFile, error)) {
                m_doc->d->m_progressProxy->setValue(-1);
                delete m_doc->d->m_kopart;
                m_doc->d->m_kopart = 0;
                if (outputTempFile)
                    outputTempFile->remove();
                if (error.isEmpty())
                    error = QObject::tr("Cannot import file");
                qWarning() << Q_FUNC_INFO << inputFile << error;
                QMetaObject::invokeMethod(m_doc->d, "slotOpenFileFailed", Qt::QueuedConnection, Q_ARG(QString, error));
                return;
            }
        }

        bool ok = kopart->document()->openUrl(KUrl(inputFile));

        if (outputTempFile)
            outputTempFile->remove();

        if (!ok) {
            m_doc->d->m_progressProxy->setValue(-1);
            delete m_doc->d->m_kopart;
            m_doc->d->m_kopart = 0;
            QString error = kopart->document()->errorMessage();
            if (error.isEmpty())
                error = QObject::tr("Cannot load file");
            qWarning() << Q_FUNC_INFO << inputFile << error;
            QMetaObject::invokeMethod(m_doc->d, "slotOpenFileFailed", Qt::QueuedConnection, Q_ARG(QString, error));
            return;
        }

        qDebug() << Q_FUNC_INFO << "Successfully openFile=" << inputFile;
        m_doc->d->m_progressProxy->setValue(-1);

        //QMetaObject::invokeMethod(m_part, "openFileSucceeded", Qt::QueuedConnection);
    }

private:

    bool importFile(const QPair<QString, KoFilter*> &filter, QString &inputFile, QSharedPointer<QTemporaryFile> &outputTempFile, QString &error)
    {
        QString outputFile;

        const QByteArray from = filter.first.toUtf8();
        const QByteArray to = "application/vnd.oasis.opendocument.text";

        const QString tempFileName = QFileInfo(inputFile).baseName() + ".XXXXXX.odt";
        outputTempFile = QSharedPointer<QTemporaryFile>(new QTemporaryFile(tempFileName));
        outputTempFile->setAutoRemove(false);
        if (!outputTempFile->open()) { // needed to open to get the full fileName
            error = QObject::tr("Cannot create temporary file %1").arg(tempFileName);
            return false;
        }
        outputTempFile->close();
        outputFile = outputTempFile->fileName();

        KoFilterManager filterManager(to);
        //KoFilterManager filterManager(kopart->document());
        //KoFilterManager filterManager(inputFile, to);

        filterManager.setImportFile(inputFile);
        filterManager.setImportFile(outputFile);

        KoFilterChain filterchain(&filterManager);
        filterchain.setInputFile(inputFile);
        filterchain.setOutputFile(outputFile);
        filter.second->setFilterChain(&filterchain);

        qDebug()<<"FilterPlugin from="<<filterchain.inputFile()<<"to="<<outputFile;

        QSharedPointer<KoProgressUpdater> progressUpdater(new KoProgressUpdater(m_doc->d->m_progressProxy, KoProgressUpdater::Unthreaded));
        QPointer<KoUpdater> updater = progressUpdater->startSubtask(1, "Filter");
        updater->setProgress(0);
        m_doc->d->m_progressProxy->beginSubTask();

        filter.second->setUpdater(updater);
        KoFilter::ConversionStatus status = filter.second->convert(from, to);

        updater->setProgress(100);
        m_doc->d->m_progressProxy->endSubTask();

        filter.second->setFilterChain(0);

        //Q_ASSERT_X(status == KoFilter::OK, __FUNCTION__, qPrintable(QString("FiFilter::convert failed with status=%1").arg(status)));
        if (status != KoFilter::OK) {
            error = QObject::tr("Cannot import %1: %2").arg(inputFile).arg(KoFilterManager::statusText(status));
            return false;
        }

        inputFile = outputFile;
        return true;
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

        QRectF pageRect = m_page->rect();
        //QRectF pageRect = kwpage.rect();

        QSizeF pageSizeF = pageRect.size();
        QSize pageSize = pageSizeF.toSize();
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
    QMutex m_waitMutex, m_workMutex;
    QWaitCondition m_waitCondition;
    QQueue<Command*> m_prendingCommands;
    bool m_kill;

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
    {
        QMutexLocker locker(&d->m_workMutex);
        d->m_prendingCommands.enqueue(command);
    }
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
            // Run the command. The call will block (this thread only) till the command is done.
            command->run();
        } else {
            // No command in the queue. Wait till a new one arrives.
            d->m_waitCondition.wait(&d->m_waitMutex);
        }
    }
}
