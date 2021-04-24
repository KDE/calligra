// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_BINDING
#define CALLIGRA_SHEETS_BINDING

#include <QSharedDataPointer>
#include <QVariant>

#include "Region.h"

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

class BindingModel;

/**
 * Abstracts read-only access to the ValueStorage.
 * Useful for KoChart (or other apps, that want read-only access to Calligra Sheets' data).
 *
 * If a cell in the region is updated, the BindingManager informs this Binding, which
 * in turn informs the model it holds.
 */
class CALLIGRA_SHEETS_ODF_EXPORT Binding
{
public:
    Binding();
    explicit Binding(const Region& region);
    Binding(const Binding& other);
    ~Binding();

    bool isEmpty() const;

    BindingModel* model() const;

    const Region& region() const;
    void setRegion(const Region& region);

    void update(const Region& region);

    void operator=(const Binding& other);
    bool operator==(const Binding& other) const;
    bool operator<(const Binding& other) const;

private:
    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Binding)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Binding, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_BINDING
