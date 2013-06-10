#include "TestDocument.h"

#include <QImage>
#include <QUrl>
#include <QFile>
#include <QPainter>

class TestDocument::Private {
public:
    QImage image;
};

TestDocument::TestDocument(KUndo2Stack *undoStack, QObject *parent)
    : KoDocumentBase(undoStack, parent)
    , d(new Private())
{
    d->image = QImage(QSize(512, 512), QImage::Format_ARGB32);
    QPainter gc(&d->image);
    gc.fillRect(0, 0, 512, 512, Qt::white);
    gc.end();
}

TestDocument::~TestDocument()
{
}

bool TestDocument::open(const QString &fileName)
{
    if (!QFile::exists(fileName)) return false;

    bool res = d->image.load(fileName);

    if (res) {
        emit updated();
    }

    return res;

}

bool TestDocument::saveTo(const QString &fileName)
{
    return d->image.save(fileName);
}

QImage TestDocument::image() const
{
    return d->image;
}

KoDocumentBase *TestDocumentFactory::create(KUndo2Stack *undoStack, QObject *parent)
{
    return new TestDocument(undoStack, parent);
}
