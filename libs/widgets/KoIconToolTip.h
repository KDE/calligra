/* This file is part of the KDE project
  SPDX-FileCopyrightText: 1999 Carsten Pfeiffer (pfeiffer@kde.org)
  SPDX-FileCopyrightText: 2002 Igor Jansen (rm@kde.org)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOICONTOOLTIP_H
#define KOICONTOOLTIP_H

#include "KoItemToolTip.h"

class KoIconToolTip: public KoItemToolTip
{
Q_OBJECT
    public:
        KoIconToolTip() = default;
        ~KoIconToolTip() override = default;

    protected:
        QTextDocument *createDocument( const QModelIndex &index ) override;

    private:
        typedef KoItemToolTip super;
};

#endif // KOICONTOOLTIP_H
