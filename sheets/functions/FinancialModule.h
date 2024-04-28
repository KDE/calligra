// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KSPREAD_FINANCIAL_MODULE
#define KSPREAD_FINANCIAL_MODULE

#include "FunctionModule.h"

#include <QVariantList>

namespace Calligra
{
namespace Sheets
{

class FinancialModule : public FunctionModule
{
    Q_OBJECT
public:
    explicit FinancialModule(QObject *parent, const QVariantList &args = QVariantList());

    QString descriptionFileName() const override;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_FINANCIAL_MODULE
