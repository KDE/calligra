/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

/*
 * This class is heavily inspired by QLineEdit, so here goes credit because credit is due (from klineedit.h):
 *  This class was originally inspired by Torben Weis'
 *  fileentry.cpp for KFM II.

 *  Sven Radej <sven.radej@iname.com>
 *  Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
 *  Preston Brown <pbrown@kde.org>

 *  Completely re-designed:
 *  Dawit Alemayehu <adawit@kde.org>

*/

#include "StylesComboPreview.h"

#include <KoIcon.h>

#include <QFocusEvent>
#include <QImage>
#include <QKeyEvent>
#include <QLineEdit>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QString>

#include <KLocalizedString>

#include <QDebug>

StylesComboPreview::StylesComboPreview(QWidget *parent)
    : QLineEdit(parent)
    , m_renamingNewStyle(false)
    , m_shouldAddNewStyle(false)
    , m_addButton(nullptr)
{
    init();
}

StylesComboPreview::~StylesComboPreview()
{
    delete m_addButton;
    m_addButton = nullptr;
}

void StylesComboPreview::init()
{
    setReadOnly(true);
    if (m_addButton) {
        return;
    }

    m_addButton = new QPushButton(this);
    m_addButton->setCursor(Qt::ArrowCursor);
    m_addButton->setIcon(koIcon("list-add"));
    m_addButton->setFlat(true);
    m_addButton->setMinimumSize(16, 16);
    m_addButton->setMaximumSize(16, 16);
    m_addButton->setToolTip(i18n("Create a new style with the current properties"));
    connect(m_addButton, &QAbstractButton::clicked, this, &StylesComboPreview::addNewStyle);

    updateAddButton();
}

void StylesComboPreview::updateAddButton()
{
    if (!m_addButton) {
        return;
    }

    const QSize geom = size();
    const int buttonWidth = m_addButton->size().width();
    m_addButton->move(geom.width() - buttonWidth, (geom.height() - m_addButton->size().height()) / 2);
}

void StylesComboPreview::setAddButtonShown(bool show)
{
    m_addButton->setVisible(show);
}

QSize StylesComboPreview::availableSize() const
{
    return QSize(contentsRect().width() - m_addButton->width(), contentsRect().height()); /// TODO dynamic resizing when button shown/hidden.
}

void StylesComboPreview::setPreview(const QImage &image)
{
    m_stylePreview = image;
}

bool StylesComboPreview::isAddButtonShown() const
{
    return m_addButton != nullptr;
}

void StylesComboPreview::resizeEvent(QResizeEvent *ev)
{
    QLineEdit::resizeEvent(ev);
    Q_EMIT resized();
    updateAddButton();
}

void StylesComboPreview::keyPressEvent(QKeyEvent *e)
{
    if (m_shouldAddNewStyle && e->key() == Qt::Key_Escape) {
        m_renamingNewStyle = false;
        m_shouldAddNewStyle = false;
        setReadOnly(true);
        setText(QString());
        e->accept();
    } else if (m_shouldAddNewStyle && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
        m_renamingNewStyle = false;
        m_shouldAddNewStyle = false;
        Q_EMIT newStyleRequested(text());
        setReadOnly(true);
        setText(QString());
        e->accept();
    } else {
        QLineEdit::keyPressEvent(e);
    }
}

void StylesComboPreview::focusOutEvent(QFocusEvent *e)
{
    if (e->reason() != Qt::ActiveWindowFocusReason && e->reason() != Qt::PopupFocusReason) {
        if (m_shouldAddNewStyle) {
            m_renamingNewStyle = false;
            m_shouldAddNewStyle = false;
            Q_EMIT newStyleRequested(text());
            setReadOnly(true);
            setText(QString());
            e->accept();
        }
        setReadOnly(true);
        m_renamingNewStyle = false;
        setText(QString());
    } else {
        QLineEdit::focusOutEvent(e);
    }
}

void StylesComboPreview::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!m_renamingNewStyle) {
        Q_EMIT clicked();
    }
}

void StylesComboPreview::paintEvent(QPaintEvent *ev)
{
    if (!m_renamingNewStyle) {
        QLineEdit::paintEvent(ev);
        QPainter p(this);
        p.setClipRect(ev->rect());
        p.drawImage(contentsRect().topLeft(), m_stylePreview);
    } else {
        QLineEdit::paintEvent(ev);
    }
}

void StylesComboPreview::addNewStyle()
{
    m_renamingNewStyle = true;
    m_shouldAddNewStyle = true;
    setText(i18n("New style"));
    selectAll();
    setReadOnly(false);
    this->setFocus();
}
