/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_PAGE_LAYOUT_DIALOG
#define KO_PAGE_LAYOUT_DIALOG

#include "kowidgets_export.h"

#include <KoText.h>

#include <kpagedialog.h>

struct KoPageLayout;

/// A dialog to show the settings for one page and apply them afterwards.
class KOWIDGETS_EXPORT KoPageLayoutDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit KoPageLayoutDialog(QWidget *parent, const KoPageLayout &layout);
    ~KoPageLayoutDialog() override;

    void showTextDirection(bool on);
    KoText::Direction textDirection() const;
    void setTextDirection(KoText::Direction direction);
    void showPageSpread(bool on);
    void setPageSpread(bool pageSpread);
    KoPageLayout pageLayout() const;
    bool applyToDocument() const;
    void showApplyToDocument(bool on);

    void showUnitchooser(bool on);
    void setUnit(const KoUnit &unit);

Q_SIGNALS:
    void unitChanged(const KoUnit &unit);

public Q_SLOTS:
    void setPageLayout(const KoPageLayout &layout);

protected Q_SLOTS:
    void accept() override;
    void reject() override;

private:
    class Private;
    Private *const d;
};

#endif
