/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISMALLTOOLBUTTON_H
#define KEXISMALLTOOLBUTTON_H

#include <QToolButton>
#include "kexiutils_export.h"

class QAction;
class KIcon;

//! @short A small tool button with icon and optional text
class KEXIUTILS_EXPORT KexiSmallToolButton : public QToolButton
{
    Q_OBJECT
public:
    KexiSmallToolButton(QWidget* parent = 0);

    KexiSmallToolButton(const QString& text, QWidget* parent = 0);

    KexiSmallToolButton(const KIcon& icon, const QString& text, QWidget* parent = 0);

    KexiSmallToolButton(QAction *action, QWidget* parent = 0);

    virtual ~KexiSmallToolButton();

    void updateAction();

    virtual void setIcon(const QIcon& icon);
    virtual void setIcon(const QString &iconName);
    virtual void setText(const QString& text);
    void setToolButtonStyle(Qt::ToolButtonStyle style);
    virtual QSize sizeHint() const;
    QAction* action() const;

protected slots:
    void slotActionChanged();
    void slotButtonToggled(bool checked);
    void slotActionToggled(bool checked);

protected:
    void update(const QString& text, const QIcon& icon, bool tipToo = false);
    void init();
//  virtual void paintEvent(QPaintEvent *pe);

    class Private;
    Private * const d;
};

class QStyleOption;

//! @short separator for custom toolbars
class KEXIUTILS_EXPORT KexiToolBarSeparator : public QWidget
{
    Q_OBJECT
public:
    KexiToolBarSeparator(QWidget *parent);
    virtual ~KexiToolBarSeparator();

    QSize sizeHint() const;
    Qt::Orientation orientation() const;
public slots:
    void setOrientation(Qt::Orientation o);
protected:
    virtual void paintEvent(QPaintEvent *e);
    void initStyleOption(QStyleOption *o) const;

private:
    Qt::Orientation m_orientation;
};

#endif
