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

#ifndef KEXIASSISTANTPAGE_H
#define KEXIASSISTANTPAGE_H

#include <QtGui/QWidget>

#include "kexiutils_export.h"

class QLabel;

//! A single page for assistant (KexiAssistantWidget).
class KEXIUTILS_EXPORT KexiAssistantPage : public QWidget
{
    Q_OBJECT
public:
    explicit KexiAssistantPage(const QString& title, const QString& description,
                               QWidget* parent = 0);
    virtual ~KexiAssistantPage();
    void setContents(QWidget* widget);
    void setContents(QLayout* layout);
    QWidget* focusWidget() const;
    void setFocusWidget(QWidget* widget);
    QLabel* backButton();
    QLabel* nextButton();
public slots:
    void setDescription(const QString& text);
    void setBackButtonVisible(bool set);
    void setNextButtonVisible(bool set);
    void back();
    void next();
    //! Sets text @a text for the 'next' button.
    //! It can be used to set e.g. "Finish" or "Create" text.
    void setNextButtonText(const QString& text);
signals:    
    void back(KexiAssistantPage* page);
    void next(KexiAssistantPage* page);
    void cancelled(KexiAssistantPage* page);

private slots:    
    void slotLinkActivated(const QString& link);
private:
    class Private;
    Private * const d;
};

#endif
