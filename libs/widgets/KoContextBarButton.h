// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/* This file is part of the KDE project
SPDX-FileCopyrightText: 2011 Aurélien Gâteau <agateau@kde.org>
SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOCONTEXTBARBUTTON_H
#define KOCONTEXTBARBUTTON_H

// Qt
#include <QTimeLine>
#include <QToolButton>

class QTimeLine;
/**
 * A button with a special look, appears when hovering over thumbnails
 */
class KoContextBarButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KoContextBarButton(const QString &iconName, QWidget *parent = nullptr);
    ~KoContextBarButton() override = default;

public Q_SLOTS:
    void setFadingValue(int value);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    /** Starts button fading animation */
    void startFading();

    /** Stops button fading animation */
    void stopFading();
    bool m_isHovered;
    int m_fadingValue;
    QTimeLine *m_fadingTimeLine;
};

#endif /* KOCONTEXTBARBUTTON_H */
