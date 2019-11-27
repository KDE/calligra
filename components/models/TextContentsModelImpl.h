/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *
 */

#ifndef CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H

#include <models/ContentsModelImpl.h>

class KoDocument;
class KoCanvasBase;
namespace Calligra {
namespace Components {

class TextContentsModelImpl : public QObject, public Calligra::Components::ContentsModelImpl
{
    Q_OBJECT
public:
    TextContentsModelImpl(KoDocument* document, KoCanvasBase* canvas);
    ~TextContentsModelImpl() override;

    int rowCount() const override;
    QVariant data(int index, Calligra::Components::ContentsModel::Role role) const override;
    void setThumbnailSize(const QSize& size) override;
    QImage thumbnail(int index, int width) const override;

    void setUseToC(bool newValue) override;

Q_SIGNALS:
    void listContentsCompleted();

private Q_SLOTS:
    void documentLayoutFinished();

private:
    class Private;
    Private* const d;
};

}
}

#endif // CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H
