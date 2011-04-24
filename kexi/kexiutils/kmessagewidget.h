/* This file is part of the KDE libraries
 *
 * Copyright (c) 2011 Aurélien Gâteau <agateau@kde.org>
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

#include <kdeui_export.h>

#include <QFrame>

class KMessageWidgetPrivate;

/**
 * @short An action which can alternate between two texts/icons when triggered.
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
class KDEUI_EXPORT KMessageWidget : public QFrame
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

public Q_SLOTS:
    void setText(const QString &);

    void setShape(KMessageWidget::Shape);

    void setShowCloseButton(bool);

    void setMessageType(KMessageWidget::MessageType);

    void fadeIn();

protected:
    void paintEvent(QPaintEvent*);

    bool event(QEvent*);

    void resizeEvent(QResizeEvent*);

private:
    KMessageWidgetPrivate *const d;
    friend class KMessageWidgetPrivate;

private Q_SLOTS:
    void slotTimeLineChanged(qreal);
    void slotTimeLineFinished();
};

#endif /* KMESSAGEWIDGET_H */
