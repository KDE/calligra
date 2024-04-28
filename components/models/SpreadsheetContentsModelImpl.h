/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H
#define CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H

#include "ContentsModelImpl.h"

class KoDocument;
namespace Calligra
{
namespace Components
{

class SpreadsheetContentsModelImpl : public ContentsModelImpl
{
public:
    explicit SpreadsheetContentsModelImpl(KoDocument *document);
    ~SpreadsheetContentsModelImpl() override;

    int rowCount() const override;
    QVariant data(int index, Calligra::Components::ContentsModel::Role role) const override;
    void setThumbnailSize(const QSize &size) override;
    QImage thumbnail(int index, int width) const override;

private:
    class Private;
    Private *const d;
};
}
}

#endif // CALLIGRA_COMPONENTS_SPREADSHEETCONTENTSMODELIMPL_H
