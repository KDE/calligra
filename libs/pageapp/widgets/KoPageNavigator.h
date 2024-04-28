/*  This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAGENAVIGATOR_H
#define KOPAGENAVIGATOR_H

// Qt
#include <QStackedWidget>

class KoPAView;
class KoPAPageBase;

class KoPageNavigator : public QStackedWidget
{
    Q_OBJECT

    enum State { Display = 0, Edit = 1 };

public:
    explicit KoPageNavigator(KoPAView *view);
    ~KoPageNavigator() override;

    void initActions();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private Q_SLOTS:
    void updateDisplayLabel();
    void onPageNumberEntered();
    void slotPageRemoved(KoPAPageBase *page, int index);

private:
    class Private;
    Private *const d;
};

#endif // KOPAGENAVIGATOR_H
