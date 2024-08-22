/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LISTLEVELWIDGET_H
#define LISTLEVELWIDGET_H

#include <ui_ListLevelWidget.h>

class KoListLevelProperties;
class LabelDrawingWidget;
class KCharSelect;

class ListLevelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListLevelWidget(QWidget *parent = nullptr);
    ~ListLevelWidget() override = default;

    void setDisplay(const KoListLevelProperties &props);
    void save(KoListLevelProperties &props) const;

protected:
    void paintEvent(QPaintEvent *ev) override;

private Q_SLOTS:
    void labelFollowedByChanged(int);
    void alignmentChanged(int);
    void numberFormatChanged(int);

private:
    Ui::ListLevelWidget widget;
    LabelDrawingWidget *m_label;
    bool m_labelFollowedByModified;
    bool m_alignmentModified;
    KCharSelect *m_charSelect;
};

#endif
