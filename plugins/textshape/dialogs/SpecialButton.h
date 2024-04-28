/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SPECIALBUTTON_H
#define SPECIALBUTTON_H

#include <QFrame>

class QPixmap;
class QLabel;
class StylesWidget;

class SpecialButton : public QFrame
{
    Q_OBJECT
public:
    explicit SpecialButton(QWidget *parent);
    ~SpecialButton();

    void setStylesWidget(StylesWidget *stylesWidget);
    void setStylePreview(const QPixmap &pm);

    void showPopup();
    void hidePopup();

protected:
    virtual void mousePressEvent(QMouseEvent *event);

    StylesWidget *m_stylesWidget;
    QLabel *m_preview;
    bool isPopupVisible;
};

#endif // SPECIALBUTTON_H
