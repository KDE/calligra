/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LINK_DIALOG
#define LINK_DIALOG

#include "ActionDialog.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Dialog to insert a hyperlink.
 */
class LinkDialog : public ActionDialog
{
    Q_OBJECT
public:
    explicit LinkDialog(QWidget *parent, const QList<QString> &links);
    ~LinkDialog() override;

    QString text() const;
    QString link() const;

Q_SIGNALS:
    void applyLink(const QString &text, const QString &link);

public Q_SLOTS:
    void setText(const QString &text);
    void setLink(const QString &link);

protected:
    virtual void onApply() override;

private:
    Q_DISABLE_COPY(LinkDialog)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif /* LINK_DIALOG */
