/*
 *
 */

#include "dropboxplugin.h"

#include "controller.h"

#include <qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QApplication>

void DropboxPlugin::registerTypes(const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Dropbox" ) );
    qmlRegisterUncreatableType<Controller>("Calligra.Gemini.Dropbox", 1, 0, "Controller", "Main controller for all interaction with Dropbox");
}

void DropboxPlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Dropbox" ) );

    //engine->addImportPath(DROPBOX_QML_PLUGIN_DIR);

    Controller* controller = new Controller(qApp);

    QDeclarativeContext *context = engine->rootContext();
    context->setContextProperty("controllerMIT", controller);
    context->setContextProperty("Options", &controller->m_options);
    context->setContextProperty("folderListModel", controller->folder_model);
    context->setContextProperty("filesTransferModel", controller->filestransfer_model);
}

#include "dropboxplugin.moc"
