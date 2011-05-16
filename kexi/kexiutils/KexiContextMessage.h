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
    explicit KexiContextMessage(const QString& text = QString());

    explicit KexiContextMessage(const KexiContextMessage& other);

    ~KexiContextMessage();

    QString text() const;

    void setText(const QString text);

    //! Adds action. Does not take ownership.
    void addAction(QAction* action);
    
    QList<QAction*> actions() const;

    //! Sets default action, i.e. button created with this action 
    //! will be the default button of the message.
    //! Does not take ownership.
    void setDefaultAction(QAction* action);

    QAction* defaultAction() const;

private:
    class Private;
    Private * const d;
};

class QFormLayout;

//! Context message widget constructed out of context message argument.
class KEXIUTILS_EXPORT KexiContextMessageWidget : public KMessageWidget
{
    Q_OBJECT
public:
    //! Creates message widget constructed out of context message @a message.
    //! Inserts itself into layout @a layout on top of the widget @a context.
    //! If @page is not 0 and @a message has any actions added,
    //! all children of @a page widget will be visually disabled to indicate 
    //! modality of the message.
    //! The message widget will be automatically destroyed after triggering
    //! of any associated action.
    KexiContextMessageWidget(QWidget *page,
                             QFormLayout* layout, QWidget *context,
                             const KexiContextMessage& message);

    //! @overload KexiContextMessageWidget(QWidget*, QFormLayout*, QWidget*, const KexiContextMessage&);
    //! Does not enter into modal state and does not accept actions.
    KexiContextMessageWidget(QFormLayout* layout, QWidget *context,
                             const KexiContextMessage& message);

    //! @overload KexiContextMessageWidget(QFormLayout*, QWidget*, const KexiContextMessage&);
    //! Does not enter into modal state and does not accept actions.
    KexiContextMessageWidget(QFormLayout* layout, QWidget *context,
                             const QString& message);

    virtual ~KexiContextMessageWidget();

    //! Sets widget @a widget to be foused after this message closes.
    //! By default context widget passed to constructor will be focused.
    //! Useful in modal mode.
    void setNextFocusWidget(QWidget *widget);

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void actionTriggered();

private:
    void init(QWidget *page, QFormLayout* layout,
        QWidget *context, const KexiContextMessage& message);
    
    //! Made private to disable addAction().
    void addAction(QAction* action) { Q_UNUSED(action); }

    class Private;
    Private * const d;
};

#endif
