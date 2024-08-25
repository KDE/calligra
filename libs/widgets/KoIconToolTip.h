/* This file is part of the KDE project
  SPDX-FileCopyrightText: 1999 Carsten Pfeiffer (pfeiffer@kde.org)
  SPDX-FileCopyrightText: 2002 Igor Jansen (rm@kde.org)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "KoItemToolTip.h"

class KoIconToolTip final : public KoItemToolTip
{
public:
    KoIconToolTip() = default;
    ~KoIconToolTip() override = default;

protected:
    [[nodiscard]] QString createDocument(const QModelIndex &index) const override;

private:
    using super = KoItemToolTip;
};
