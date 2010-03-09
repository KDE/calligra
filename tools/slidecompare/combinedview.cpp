#include "combinedview.h"
#include "dirslideloader.h"
#include "kpresenterslideloader.h"
#include "slideview.h"
#include "oothread.h"
#include <PptToOdp.h>
#include <QtGui/QGridLayout>
#include <QtCore/QCoreApplication>

CombinedView::CombinedView(QWidget* parent) :QWidget(parent),
        ooodploader(new DirSlideLoader(this)),
        koodploader(new KPresenterSlideLoader(this)),
        oopptloader(new DirSlideLoader(this)),
        kopptloader(new KPresenterSlideLoader(this)),
        ooodpview(new SlideView(ooodploader, this)),
        koodpview(new SlideView(koodploader, this)),
        oopptview(new SlideView(oopptloader, this)),
        kopptview(new SlideView(kopptloader, this)),
        oothread(new OoThread(this)),
        layout(new QGridLayout(this)) {

    ooodploader->setSlideNamePattern("img%1.png");
    oopptloader->setSlideNamePattern("img%1.png");

    addSlideView(ooodpview);
    addSlideView(koodpview);
    addSlideView(oopptview);
    addSlideView(kopptview);

    layout->addWidget(ooodpview, 0, 0);
    layout->addWidget(koodpview, 0, 1);
    layout->addWidget(oopptview, 1, 0);
    layout->addWidget(kopptview, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(oothread, SIGNAL(toOdpDone(QString)),
        this, SLOT(slotHandleOoOdp(QString)));
    connect(oothread, SIGNAL(toPngDone(QString)),
        this, SLOT(slotHandleOoPng(QString)));
}

CombinedView::~CombinedView() {
    oothread->stop();
    oothread->wait();
}

void CombinedView::slotSetView(qreal zoomFactor, int h, int v) {
    for (int i=0; i<slideViews.size(); ++i) {
        slideViews[i]->setView(zoomFactor, h, v);
    }
}
void CombinedView::addSlideView(SlideView* slideview) {
    slideViews.push_back(slideview);
    connect(slideview, SIGNAL(viewChanged(qreal,int,int)),
        this, SLOT(slotSetView(qreal,int,int)));
}
QString
koppttoodp(const QString& from) {
    QString to = "/tmp/out.odp"; // maybe use tmp name ;-)
    QFile::remove(to);
    PptToOdp ppttoodp;
    ppttoodp.convert(from, to, KoStore::Zip);
    return to;
}
void
CombinedView::openFile(const QString& path) {
    bool odp = path.toLower().endsWith(".odp");
    oopptview->setVisible(!odp);
    kopptview->setVisible(!odp);
    layout->setRowStretch(1, (odp)?0:1);
    // update view now for more pleasing user experience, later renderings
    // may be slow
    qApp->processEvents();
    oopptloader->setSlideDir("");
    kopptloader->open("");

    if (!odp) {
        qDebug() << "PptToOdp " << path;
        nextodpfile = koppttoodp(path);
        koodploader->open(nextodpfile);
    } else {
        nextodpfile = "";
        koodploader->open(path);
    }
    ooodploader->setSlideSize(koodploader->slideSize());
    ooodploader->setNumberOfSlides(koodploader->numberOfSlides());

    if (!odp) {
        // start conversion to odp
        ooodpresult = oothread->toOdp(path);
        oopptloader->setSlideSize(koodploader->slideSize());
        oopptloader->setNumberOfSlides(koodploader->numberOfSlides());
    } else {
        oopptloader->setNumberOfSlides(0);
    }
    QString dir = oothread->toPng(path, koodploader->slideSize().width());
    ooodploader->setSlideDir(dir);
    // if ppt, convert to odp with koffice and put in queue for conversion to
    // png
}
void
CombinedView::slotHandleOoOdp(const QString& path) {
    qDebug() << "got " << path;
    if (path == ooodpresult) {
        kopptloader->open(path);
    }
}
void
CombinedView::slotHandleOoPng(const QString& /*path*/) {
    if (!nextodpfile.isEmpty()) {
        QString dir = oothread->toPng(nextodpfile,
            koodploader->slideSize().width());
        oopptloader->setSlideDir(dir);
        nextodpfile = "";
    }
}
