/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWCONFIGUREDIALOG_H
#define KWCONFIGUREDIALOG_H

#include <kpagedialog.h>

class KWView;
class KoConfigDocumentPage;
class KoConfigGridPage;
class KoConfigMiscPage;
class KoConfigAuthorPage;

class KWConfigureDialog : public KPageDialog
{
    Q_OBJECT

public:
    explicit KWConfigureDialog(KWView *parent);

Q_SIGNALS:
    /// Emitted when the configuration may have changed.
    void changed();

public Q_SLOTS:
    void slotApply();
    void slotDefault();

protected Q_SLOTS:
    void handleButtonClicked(QAbstractButton *button);

private:
    KoConfigGridPage *m_gridPage;
    KoConfigDocumentPage *m_docPage;
    KoConfigMiscPage *m_miscPage;
    KoConfigAuthorPage *m_authorPage;
};

#endif // KWCONFIGUREDIALOG_H
