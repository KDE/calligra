/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "KisFlipbookSelector.h"
#include <kis_doc2.h>
#include <kis_part2.h>

#include <KoIcon.h>

#include <KGlobal>
#include <KStandardDirs>
#include <KFileDialog>

#include <QDesktopServices>
#include <QListWidget>
#include <QListWidgetItem>

KisFlipbookSelector::KisFlipbookSelector(QWidget *parent, KisDoc2 *document)
    : QWidget(parent)
    , m_document(document)
{
    setupUi(this);

    bnGetFlipbookName->setIcon(koIcon("document-open"));
    connect(bnGetFlipbookName, SIGNAL(clicked()), SLOT(loadFlipbook()));
    connect(bnLoadFlipbook, SIGNAL(clicked()), SLOT(createImage()));

    KConfigGroup config(document->documentPart()->componentData().config(), "RecentFlipbooks");
    QString path;
    int i = 0;
    do {
        path = config.readPathEntry(QString("File%1").arg(i), QString());

        if (!path.isEmpty()) {
            QString name = config.readPathEntry(QString("Name%1").arg(i), QString());

            KUrl url(path);

            if (name.isEmpty())
                name = url.fileName();

            if (QFile::exists(url.toLocalFile())) {
                QListWidgetItem *item = new QListWidgetItem(name, listFlipbooks);
                item->setData(Qt::UserRole, path);
                qDebug() << name << path;
            }
        }
        i++;
    } while (!path.isEmpty() || i <= 100);


    bnLoadFlipbook->setEnabled(listFlipbooks->count() > 0);

}

void KisFlipbookSelector::loadFlipbook()
{
    QString flipbook = KFileDialog::getSaveFileName(QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    "flipbook",
                                                    this, i18n("Open Flipbook Definitions"));
    txtFlipbookName->setText(flipbook);
    bnCreateNewFlipbook->setEnabled(true);
}


void KisFlipbookSelector::createImage()
{
    QListWidgetItem *item = listFlipbooks->currentItem();
    if (!item) {
        item = listFlipbooks->item(0);
    }

    if (item) {
        static_cast<KisPart2*>(m_document->documentPart())->setFlipbook(item->data(Qt::UserRole).toString());
        emit documentSelected();
    }
}
