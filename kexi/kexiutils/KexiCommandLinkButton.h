/* This file is part of the KDE project
   Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICOMMANDLINKBUTTON_H
#define KEXICOMMANDLINKBUTTON_H

#include <kexiutils/kexiutils_export.h>

#include <QPushButton>

class KexiCommandLinkButtonPrivate;

class KEXIUTILS_EXPORT KexiCommandLinkButton : public QPushButton 
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat DESIGNABLE false)
    Q_PROPERTY(bool arrowVisible READ isArrowVisible WRITE setArrowVisible)

public:
    explicit KexiCommandLinkButton(QWidget *parent=0);
    explicit KexiCommandLinkButton(const QString &text, QWidget *parent=0);
    KexiCommandLinkButton(const QString &text, const QString &description, QWidget *parent=0);
    QString description() const;
    void setDescription(const QString &description);
    bool isArrowVisible() const;
    void setArrowVisible(bool visible);

protected:
    QSize sizeHint() const;
    int heightForWidth(int) const;
    QSize minimumSizeHint() const;
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);

private:
    Q_DISABLE_COPY(KexiCommandLinkButton)
    friend class KexiCommandLinkButtonPrivate;
    KexiCommandLinkButtonPrivate * const d;
};

#endif // KEXICOMMANDLINKBUTTON
