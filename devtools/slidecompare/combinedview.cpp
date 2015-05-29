/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "combinedview.h"
#include "dirslideloader.h"
#include "kpresenterslideloader.h"
#include "slideview.h"
#include "oothread.h"
#include <PptToOdp.h>
#include <QGridLayout>
#include <QDragEnterEvent>
#include <QCoreApplication>
#include <kmessagebox.h>
#include <kmimetype.h>

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
    setAcceptDrops(true);
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
    QDir d(QDir::temp().filePath("slidecompare-" + QDir::home().dirName()));
    QString dirpath = d.absolutePath();
    d.mkpath(dirpath);
    QString to = dirpath + QDir::separator()
                 + QFileInfo(from).baseName() + ".odp";
    QFile::remove(to);
    PptToOdp ppttoodp(0, 0);
    ppttoodp.convert(from, to, KoStore::Zip);
    return to;
}
void
CombinedView::openFile(const QString& path) {
    bool odp = path.endsWith(QLatin1String(".odp"), Qt::CaseInsensitive);
    oopptview->setVisible(!odp);
    kopptview->setVisible(!odp);
    layout->setRowStretch(1, (odp)?0:1);
    // update view now for more pleasing user experience, later renderings
    // may be slow
    qApp->processEvents();
    oopptloader->setSlideDir("");
    kopptloader->close();

    if (!QFileInfo(path).exists()) {
         KMessageBox::error(this,
                       QString("File %1 does not exist.").arg(path));
         return;
    }

    if (!odp) {
        nextodpfile = koppttoodp(path);
        if (!QFileInfo(nextodpfile).exists()) {
            KMessageBox::error(this, QString(
                    "File %1 cannot be converted by PptToOdp.").arg(path));
            return;
        }
        koodploader->open(nextodpfile);
    } else {
        nextodpfile.clear();
        koodploader->open(path);
    }
    quint32 nslides = koodploader->numberOfSlides();
    if (nslides == 0) {
        return;
    }
    ooodploader->setSlideSize(koodploader->slideSize());
    ooodploader->setNumberOfSlides(nslides);

    if (!odp) {
        // start conversion to odp
        ooodpresult = oothread->toOdp(path);
        oopptloader->setSlideSize(koodploader->slideSize());
        oopptloader->setNumberOfSlides(nslides);
    } else {
        oopptloader->setNumberOfSlides(0);
    }
    QString dir = oothread->toPng(path, koodploader->slideSize().width());
    ooodploader->setSlideDir(dir);
    // if ppt, convert to odp with calligra and put in queue for conversion to
    // png

    // adapt zoom level to number of slides
    qreal zoomlevel = (nslides > 3 || nslides == 0) ?0.25 :1.0/nslides;
    ooodpview->setView(zoomlevel, 0, 0);
    koodpview->setView(zoomlevel, 0, 0);
    oopptview->setView(zoomlevel, 0, 0);
    kopptview->setView(zoomlevel, 0, 0);
}
void
CombinedView::slotHandleOoOdp(const QString& path) {
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
        nextodpfile.clear();
    }
}
void CombinedView::dragEnterEvent(QDragEnterEvent *event)
{
    foreach (const QUrl& url, event->mimeData()->urls()) {
        const QString path(url.toLocalFile());
        if (path.isEmpty()) { // url is not local
            event->acceptProposedAction();
        } else {
            QString mimetype = KMimeType::findByUrl(url)->name();
            if (mimetype == "application/vnd.oasis.opendocument.presentation"
                || mimetype == "application/vnd.ms-powerpoint") {
                event->acceptProposedAction();
            }
        }
    }
}
void CombinedView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->urls().size()) {
        openFile(event->mimeData()->urls().first().toLocalFile());
    }
}
