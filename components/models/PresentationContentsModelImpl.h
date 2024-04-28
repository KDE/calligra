/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_PRESENTATIONCONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_PRESENTATIONCONTENTSMODELIMPL_H

#include "ContentsModelImpl.h"

class KoDocument;
namespace Calligra
{
namespace Components
{

class PresentationContentsModelImpl : public ContentsModelImpl
{
public:
    explicit PresentationContentsModelImpl(KoDocument *document);
    ~PresentationContentsModelImpl() override;

    int rowCount() const override;
    QVariant data(int index, ContentsModel::Role role) const override;
    void setThumbnailSize(const QSize &size) override;
    QImage thumbnail(int index, int width) const override;

private:
    class Private;
    Private *const d;
};
}
}

#endif // CALLIGRA_COMPONENTS_PRESENTATIONCONTENTSMODELIMPL_H
