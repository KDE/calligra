/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXINAMEDIALOG_H
#define KEXINAMEDIALOG_H

#include <QPixmap>
#include <QLabel>
#include <KDialog>

#include "kexinamewidget.h"

class KEXIMAIN_EXPORT KexiNameDialog : public KDialog
{
    Q_OBJECT

public:
    KexiNameDialog(const QString& message, QWidget * parent = 0);

    KexiNameDialog(const QString& message,
                   const QString& nameLabel, const QString& nameText,
                   const QString& captionLabel, const QString& captionText,
                   QWidget * parent = 0);

    virtual ~KexiNameDialog();

    KexiNameWidget* widget() const {
        return m_widget;
    }

public slots:
    virtual void setDialogIcon(const QPixmap& icon);

protected slots:
    void slotTextChanged();
    virtual void accept();
    void updateSize();

protected:
    void init();
    virtual void showEvent(QShowEvent * event);

    QLabel *m_icon;
    KexiNameWidget* m_widget;
};

#endif
