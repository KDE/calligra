/* This file is part of the KDE project
 * Copyright (C) 2020 Dag Andersen <dag.andersen@kdemail.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TableShapeConfigWidget.h"
#include "TableShape.h"
#include "SheetsDebug.h"
#include <Sheet.h>

#include <KoImageData.h>
#include <KoImageCollection.h>

#include <KUrlRequester>

#include <QVBoxLayout>
#include <QUrl>

using namespace Calligra::Sheets;

TableShapeConfigWidget::TableShapeConfigWidget()
    : m_shape(nullptr)
{
    ui.setupUi(this);
    connect(ui.createSpreadsheet, SIGNAL(toggled(bool)), ui.url, SLOT(setDisabled(bool)));
}

TableShapeConfigWidget::~TableShapeConfigWidget()
{
}

void TableShapeConfigWidget::open(KoShape *shape)
{
    m_shape = dynamic_cast<TableShape*>(shape);
    Q_ASSERT(m_shape);
    ui.createSpreadsheet->setChecked(true);
}

void TableShapeConfigWidget::save()
{
    if (!m_shape) {
        return;
    }
    if (ui.linkSpreadsheet->isChecked() || ui.importSpreadsheet->isChecked()) {
        QUrl url = ui.url->url();
        if (!url.isEmpty()) {
            connect(m_shape->document(), SIGNAL(completed()), this, SLOT(documentLoaded()));
            m_shape->clear();
            m_shape->document()->openUrl(url);
        }
        m_shape->document()->setStoreInternal(ui.importSpreadsheet->isChecked());
        if (!m_shape->document()->storeInternal()) {
            m_shape->document()->setUrl(url);
            m_shape->updateUrl();
        }
    }
}

bool TableShapeConfigWidget::showOnShapeCreate()
{
    return true;
}

bool TableShapeConfigWidget::showOnShapeSelect()
{
    return false;
}

void TableShapeConfigWidget::documentLoaded()
{
    m_shape->setMap();
    if (Sheet* sheet = m_shape->sheet()) {
        QRect area = sheet->usedArea();
        QRectF darea = sheet->cellCoordinatesToDocument(area);
        QSizeF size = m_shape->size();
        if (darea.size().width() > size.width()) {
            QRect rect = area;
            qreal right = 0.0;
            for (int c = area.left(); c <= area.right(); ++c) {
                rect.setRight(c);
                qreal r = sheet->cellCoordinatesToDocument(rect).right();
                if (r <= size.width()) {
                    right = r;
                } else {
                    break;
                }
            }
            darea.setRight(right);
        }
        if (darea.size().height() > size.height()) {
            QRect rect = area;
            qreal bottom = 0.0;
            for (int c = area.top(); c <= area.bottom(); ++c) {
                rect.setBottom(c);
                qreal b = sheet->cellCoordinatesToDocument(rect).bottom();
                if (b <= size.height()) {
                    bottom = b;
                } else {
                    break;
                }
            }
            darea.setBottom(bottom);
        }
        m_shape->resize(darea.size());
    }
    disconnect(m_shape->document(), SIGNAL(completed()), this, SLOT(documentLoaded()));
}
