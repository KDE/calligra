#include "TestView.h"

#include <QLabel>
#include <QVBoxLayout>
#include "TestDocument.h"

class TestView::Private {
public:
    QLabel *display;
    QPixmap empty;
    TestDocument *document;
};

TestView::TestView(KoDocumentBase *document, QWidget *parent)
    : KoViewBase(document, parent)
    , d(new Private)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    d->display = new QLabel(this);
    layout->addWidget(d->display);

    d->document = qobject_cast<TestDocument*>(document);
    Q_ASSERT(d->document);

    d->display->setPixmap(QPixmap::fromImage(d->document->image()));

    connect(d->document, SIGNAL(updated()), this, SLOT(redisplay()));
}

void TestView::redisplay() const
{
    d->display->setPixmap(QPixmap::fromImage(d->document->image()));
}
