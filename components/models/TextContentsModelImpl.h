/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H

#include <models/ContentsModelImpl.h>

class KoDocument;
class KoCanvasBase;
namespace Calligra
{
namespace Components
{

class TextContentsModelImpl : public QObject, public Calligra::Components::ContentsModelImpl
{
    Q_OBJECT
public:
    TextContentsModelImpl(KoDocument *document, KoCanvasBase *canvas);
    ~TextContentsModelImpl() override;

    int rowCount() const override;
    QVariant data(int index, Calligra::Components::ContentsModel::Role role) const override;
    void setThumbnailSize(const QSize &size) override;
    QImage thumbnail(int index, int width) const override;

    void setUseToC(bool newValue) override;

Q_SIGNALS:
    void listContentsCompleted();

private Q_SLOTS:
    void documentLayoutFinished();

private:
    class Private;
    Private *const d;
};

}
}

#endif // CALLIGRA_COMPONENTS_TEXTCONTENTSMODELIMPL_H
