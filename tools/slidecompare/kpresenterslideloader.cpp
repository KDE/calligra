#include "kpresenterslideloader.h"
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KDE/KMimeType>
#include <KDE/KParts/ComponentFactory>
#include <QtCore/QDebug>

KPresenterSlideLoader::KPresenterSlideLoader(QObject* parent) :SlideLoader(parent) {
    m_doc = 0;
    version = 0;
}

KPresenterSlideLoader::~KPresenterSlideLoader() {
    closeDocument();
}
void KPresenterSlideLoader::closeDocument() {
    if (m_doc) {
        delete m_doc;
    }
    m_doc = 0;
    version++;
    emit slidesChanged();
}

void
KPresenterSlideLoader::open(const QString& path)
{
    closeDocument();
    if (path.isEmpty()) return;
    // supported mimetypes
    // "application/vnd.oasis.opendocument.presentation"
    // "application/vnd.ms-powerpoint"
    QString mimetype = KMimeType::findByPath(path)->name();

    int errorCode = 0;
    KoDocument* kodoc
            = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>(
                mimetype, QString(),
                0, 0, QStringList(),
                &errorCode);
    KoPADocument* doc = dynamic_cast<KoPADocument*>(kodoc);
    m_doc = doc;
    if (!m_doc) {
        qDebug() << "could not create KoDocument";
        delete doc;
        closeDocument();
        return;
    }
    KUrl url;
    url.setPath(path);
    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(true);
    if (!doc->openUrl(url)) {
        qDebug() << "could not open " << path;
        closeDocument();
        return;
    }
    doc->setReadWrite(false);
    doc->setAutoSave(0);
    emit slidesChanged();
}
int
KPresenterSlideLoader::numberOfSlides() {
    return (m_doc) ?m_doc->pageCount() :0;
}
QSize KPresenterSlideLoader::slideSize() {
    if (!m_doc) return QSize();
    KoPAPageBase* page = m_doc->pages().value(0);
    if (!page) return QSize();
    return page->size().toSize();
}
QPixmap KPresenterSlideLoader::loadSlide(int number, const QSize& maxsize) {
    if (!m_doc) return QPixmap();
    KoPAPageBase* page = m_doc->pages().value(number);
    if (!page) return QPixmap();
    return page->thumbnail(maxsize);
}
