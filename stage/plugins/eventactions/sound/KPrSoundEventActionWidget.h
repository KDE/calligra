// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KPRSOUNDEVENTACTIONWIDGET_H
#define KPRSOUNDEVENTACTIONWIDGET_H

#include <KPrEventActionWidget.h>

class QComboBox;
class QString;
class KoShape;
class KoEventAction;
class KPrEventActionData;
class KPrSoundCollection;

class KPrSoundEventActionWidget : public KPrEventActionWidget
{
    Q_OBJECT
public:
    explicit KPrSoundEventActionWidget(QWidget *parent = nullptr);
    virtual ~KPrSoundEventActionWidget();

public Q_SLOTS:
    void setData(KPrEventActionData *eventActionData) override;

private Q_SLOTS:
    void soundComboChanged();

private:
    void updateCombo(const QString &title);

    KoShape *m_shape;
    KoEventAction *m_eventAction;
    KPrSoundCollection *m_soundCollection;
    QComboBox *m_soundCombo;
};

#endif /* KPRSOUNDEVENTACTIONWIDGET_H */
