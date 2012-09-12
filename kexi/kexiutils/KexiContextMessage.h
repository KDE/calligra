/* This file is part of the KDE project
   Copyright (C) 2011-2012 Jarosław Staniek <staniek@kde.org>

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

    explicit KexiContextMessage(QWidget *contentsWidget);

    explicit KexiContextMessage(const KexiContextMessage& other);

    ~KexiContextMessage();

    QString text() const;

    void setText(const QString text);

    //! Alignment of button corresponding to action added in addAction()
    enum ButtonAlignment {
        AlignLeft,
        AlignRight
    };

    //! Adds action. Does not take ownership.
    void addAction(QAction* action, ButtonAlignment alignment = AlignRight);
    
    QList<QAction*> actions() const;

    //! @return alignment of button for action @a action.
    ButtonAlignment buttonAlignment(QAction* action) const;

    //! Sets default action, i.e. button created with this action 
    //! will be the default button of the message.
    //! Does not take ownership.
    void setDefaultAction(QAction* action);

    QAction* defaultAction() const;
    
    QWidget* contentsWidget() const;

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
    /*! Inserts itself into layout @a layout on top of the widget @a context.
       If @page is not 0 and @a message has any actions added,
       all children of @a page widget will be visually disabled to indicate 
       modality of the message.
       The message widget will be automatically destroyed after triggering
       of any associated action.
       If @a layout is provided, direction of callout pointer is set by default
       to KMessageWidget::Down. This can be changed using setCalloutPointerDirection(). */
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

    //! Sets global position for callout pointer
    //! @overload KMessageWidget::setCalloutPointerPosition(const QPoint&)
    //! Also sets tracked widget @a trackedWidget.
    //! If the widget changes its local position, the pointer position
    //! is moved by the same delta.
    void setCalloutPointerPosition(const QPoint& globalPos,
                                   QWidget *trackedWidget = 0);

    //! Sets tracking policy for resize of the parent widget.
    //! When parent is resized in any way, size of the message box
    //! can be changed in one or two orientations. It is disabled by default
    //! and does not affect position of the callout pointer.
    //! Works only when tracked widget is set in setCalloutPointerPosition().
    void setResizeTrackingPolicy(Qt::Orientations orientations);

    //! @return tracking policy for resize of the parent widget.
    Qt::Orientations resizeTrackingPolicy() const;

    //! Sets palette of the contents widget inheriting the message palette (background).
    //! Calling it is needed after delayed insering of the child contents widgets.
    void setPaletteInherited();

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
