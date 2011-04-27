/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICONTEXTMESSAGE_H
#define KEXICONTEXTMESSAGE_H

#include "kmessagewidget.h"

//! Information about context message.
class KEXIUTILS_EXPORT KexiContextMessage
{
public:
    KexiContextMessage();

    KexiContextMessage(const KexiContextMessage& other);

    ~KexiContextMessage();

    QString text() const;

    void setText(const QString text);

    //! Adds action. Does not take ownership.
    void addAction(QAction* action);
    
    QList<QAction*> actions() const;

    void setDefaultAction(QAction* action);

    QAction *defaultAction() const;

private:
    class Private;
    Private * const d;
};

class QFormLayout;

class KEXIUTILS_EXPORT KexiContextMessageWidget : public KMessageWidget
{
    Q_OBJECT
public:
    KexiContextMessageWidget(QWidget *page,
                             QFormLayout* layout, QWidget *context,
                             const KexiContextMessage& message);
    virtual ~KexiContextMessageWidget();
    virtual bool eventFilter(QObject* watched, QEvent* event);
private slots:
    void actionTriggered();
private:
    class Private;
    Private * const d;
};

#endif
