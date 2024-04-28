/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONCONFIGUREDIALOG_H
#define KARBONCONFIGUREDIALOG_H

#include <kpagedialog.h>

class KarbonView;
class KarbonConfigInterfacePage;
class KoConfigMiscPage;
class KoConfigGridPage;
class KoConfigDocumentPage;
class KoConfigAuthorPage;

class KarbonConfigureDialog : public KPageDialog
{
    Q_OBJECT

public:
    explicit KarbonConfigureDialog(KarbonView *parent);

public Q_SLOTS:
    void slotApply();
    void slotDefault();

private:
    KarbonConfigInterfacePage *m_interfacePage;
    KoConfigMiscPage *m_miscPage;
    KoConfigGridPage *m_gridPage;
    KoConfigDocumentPage *m_defaultDocPage;
    KoConfigAuthorPage *m_authorPage;
};

#endif // KARBONCONFIGUREDIALOG_H
