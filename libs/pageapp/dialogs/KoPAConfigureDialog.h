/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPACONFIGUREDIALOG_H
#define KOPACONFIGUREDIALOG_H

#include <kpagedialog.h>

class KoPAView;
class KoConfigDocumentPage;
class KoConfigGridPage;
class KoConfigMiscPage;
class KoConfigAuthorPage;

class KoPAConfigureDialog : public KPageDialog
{
    Q_OBJECT

public:
    explicit KoPAConfigureDialog(KoPAView *parent);

Q_SIGNALS:
    /// Emitted when the configuration may have changed.
    void changed();

public Q_SLOTS:
    void slotApply();
    void slotDefault();

private:
    KoConfigGridPage *m_gridPage;
    KoConfigDocumentPage *m_docPage;
    KoConfigMiscPage *m_miscPage;
    KoConfigAuthorPage *m_authorPage;
};

#endif // KOPACONFIGUREDIALOG_H
