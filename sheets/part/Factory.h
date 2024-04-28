/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FACTORY_H
#define CALLIGRA_SHEETS_FACTORY_H

#include <KPluginFactory>

#include "sheets_part_export.h"

class KAboutData;
class KoComponentData;

namespace Calligra
{
namespace Sheets
{

class CALLIGRA_SHEETS_PART_EXPORT Factory : public KPluginFactory
{
    Q_OBJECT
public:
    explicit Factory();
    ~Factory() override;

    QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args) override;

    static const KoComponentData &global();

    static KAboutData *aboutData();

private:
    static KoComponentData *s_global;
    static KAboutData *s_aboutData;
};

} // namespace Sheets
} // namespace Calligra

#endif
