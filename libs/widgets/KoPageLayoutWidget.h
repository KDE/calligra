/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_PAGE_LAYOUT_WIDGET
#define KO_PAGE_LAYOUT_WIDGET

#include "kowidgets_export.h"

#include <KoPageLayout.h>
#include <KoText.h>
#include <QWidget>

class KoUnit;

/// the widget that shows the size/margins and other page settings.
class KOWIDGETS_EXPORT KoPageLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    KoPageLayoutWidget(QWidget *parent, const KoPageLayout &layout);
    ~KoPageLayoutWidget() override;

    KoPageLayout pageLayout() const;

    void setUnit(const KoUnit &unit);
    void showUnitchooser(bool on);
    void showPageSpread(bool on);
    void showPageStyles(bool on);
    void setPageStyles(const QStringList &styles);
    QString currentPageStyle() const;
    void setPageSpread(bool pageSpread);
    void showTextDirection(bool on);
    void setTextDirection(KoText::Direction direction);
    KoText::Direction textDirection() const;

Q_SIGNALS:
    void layoutChanged(const KoPageLayout &layout);
    void unitChanged(const KoUnit &unit);

public Q_SLOTS:
    void setPageLayout(const KoPageLayout &layout);
    void setTextAreaAvailable(bool available);
    void setApplyToDocument(bool apply);

private Q_SLOTS:
    void sizeChanged(int row);
    void slotUnitChanged(int row);
    void facingPagesChanged();
    void optionsChanged();
    void marginsChanged();
    void orientationChanged();

private:
    class Private;
    Private *const d;
};

#endif
