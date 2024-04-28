/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SpecialButton.h"
#include "StylesWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QPixmap>
#include <QShowEvent>

#include <QDebug>

SpecialButton::SpecialButton(QWidget *parent)
    : QFrame(parent)
    , m_stylesWidget(0)
    , m_preview(0)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);

    setMinimumSize(50, 32);
    setMaximumHeight(25);

    m_preview = new QLabel();
    m_preview->setAutoFillBackground(true);
    m_preview->setBackgroundRole(QPalette::Base);
    m_preview->setMinimumWidth(50);
    m_preview->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->addWidget(m_preview);
    l->setMargin(0);
    setLayout(l);

    isPopupVisible = false;
}

SpecialButton::~SpecialButton()
{
    delete m_preview;
}

void SpecialButton::setStylePreview(const QPixmap &pm)
{
    m_preview->setPixmap(pm);
}

void SpecialButton::showPopup()
{
    if (!m_stylesWidget) {
        return;
    }

    QRect popupRect(mapToGlobal(QPoint(0, height())), m_stylesWidget->sizeHint());
    // Make sure the popup is not drawn outside the screen area
    QRect screenRect = QApplication::desktop()->availableGeometry(this);
    if (popupRect.right() > screenRect.right())
        popupRect.translate(screenRect.right() - popupRect.right(), 0);
    if (popupRect.left() < screenRect.left())
        popupRect.translate(screenRect.left() - popupRect.left(), 0);
    if (popupRect.bottom() > screenRect.bottom())
        popupRect.translate(0, -(height() + m_stylesWidget->height()));

    m_stylesWidget->setGeometry(popupRect);
    m_stylesWidget->raise();
    m_stylesWidget->show();
    isPopupVisible = true;
}

void SpecialButton::hidePopup()
{
    m_stylesWidget->hide();
    isPopupVisible = false;
}

void SpecialButton::setStylesWidget(StylesWidget *stylesWidget)
{
    m_stylesWidget = stylesWidget;
}

void SpecialButton::mousePressEvent(QMouseEvent *)
{
    if (!isPopupVisible) {
        showPopup();
    } else {
        hidePopup();
    }
}
