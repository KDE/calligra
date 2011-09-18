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

#ifndef KEXIASSISTANTWIDGET_H
#define KEXIASSISTANTWIDGET_H

#include "kexiutils_export.h"

#include <QtGui/QWidget>

class KexiAssistantPage;

//! A widget for building assistants.
class KEXIUTILS_EXPORT KexiAssistantWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KexiAssistantWidget(QWidget* parent = 0);
    ~KexiAssistantWidget();

    void addPage(KexiAssistantPage* page);

    KexiAssistantPage* currentPage() const;

public slots:
    void setCurrentPage(KexiAssistantPage* page);

    virtual void previousPageRequested(KexiAssistantPage* page);

    virtual void nextPageRequested(KexiAssistantPage* page);

    virtual void cancelRequested(KexiAssistantPage* page);

private:
    class Private;
    Private* const d;
};

#endif
