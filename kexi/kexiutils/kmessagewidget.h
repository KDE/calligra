/* This file is part of the KDE libraries
 *
 * Copyright (c) 2011 Aurélien Gâteau <agateau@kde.org>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef KMESSAGEWIDGET_H
#define KMESSAGEWIDGET_H

#include "kexiutils_export.h"

#include <QFrame>

class KMessageWidgetPrivate;

/**
 * @short A widget to provide feedback or propose opportunistic interactions.
 *
 * KMessageWidget should be used to provide inline positive or negative
 * feedback, or to implement opportunistic interactions.
 *
 * See http://community.kde.org/Sprints/UX2011/KMessageWidget
 * for more information.
 *
 * @author Aurélien Gâteau <agateau@kde.org>
 * @since 4.7
 */
class KEXIUTILS_EXPORT KMessageWidget : public QFrame
{
    Q_OBJECT
public:
    enum Shape {
        LineShape,
        RectangleShape
    };

    enum MessageType {
        PositiveMessageType,
        InformationMessageType,
        WarningMessageType,
        ErrorMessageType
    };

    /**
     * Constructs a KMessageWidget with the specified parent.
     */
    explicit KMessageWidget(QWidget *parent = 0);

    ~KMessageWidget();

    QString text() const;

    KMessageWidget::Shape shape() const;

    bool showCloseButton() const;

    MessageType messageType() const;

    void addAction(QAction* action);

    void removeAction(QAction* action);

    void setDefaultAction(QAction* action);

    /**
     * @brief Sets autodeletion flag to be on or off.
     * If autodeletion is on, the widget is automatically
     * deleted after hide() or animatedHide() using QObject::deleteLater().
     * Autodeletion is false by default.
     */
    void setAutoDelete(bool set);

public Q_SLOTS:
    void setText(const QString &);

    void setShape(KMessageWidget::Shape);

    void setShowCloseButton(bool);

    void setMessageType(KMessageWidget::MessageType);

    void animatedShow();

    void animatedHide();

protected:
    void paintEvent(QPaintEvent*);

    bool event(QEvent*);

    void resizeEvent(QResizeEvent*);

    void showEvent(QShowEvent*);

private:
    KMessageWidgetPrivate *const d;
    friend class KMessageWidgetPrivate;

private Q_SLOTS:
    void slotTimeLineChanged(qreal);
    void slotTimeLineFinished();
};

#endif /* KMESSAGEWIDGET_H */
