/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_CONVERSION_MODULE
#define KSPREAD_CONVERSION_MODULE

#include "FunctionModule.h"

#include <QVariantList>

namespace Calligra
{
namespace Sheets
{

class ConversionModule : public FunctionModule
{
    Q_OBJECT
public:
    explicit ConversionModule(QObject *parent, const QVariantList &args = QVariantList());

    QString descriptionFileName() const override;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_CONVERSION_MODULE
