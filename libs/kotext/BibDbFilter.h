/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BIBDBFILTER_H
#define BIBDBFILTER_H

#include <QWidget>

#include "kotext_export.h"


class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class KPushButton;
class KComboBox;
class KLineEdit;
class KDialogButtonBox;

typedef QPair<QString, QString> ConditionPair;

class KOTEXT_EXPORT BibDbFilter : public QWidget
{
    Q_OBJECT
public:
    explicit BibDbFilter(bool hasPreCond = true);

    static const QList<ConditionPair> filterConditions;

    QString m_leftOp;
    QString m_rightOp;
    QString m_comparison;
    QString m_preOp;
    KComboBox *m_field;
    KComboBox *m_cond;
    KLineEdit *m_value;
    KComboBox *m_preCond;
    QString filterString() const;
public slots:
    void setLeftOperand(const QString &op);
    void setRightOperand(const QString &op);
    void setCondition(int index);
    void setPreCondition(const QString &preCond);
private:
    QHBoxLayout *m_layout;
};

#endif // BIBDBFILTER_H
