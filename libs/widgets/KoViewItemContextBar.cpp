/* This file is part of the KDE project
*
* Copyright (C) 2008 Peter Penz <peter.penz19@gmail.com>
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "KoViewItemContextBar.h"

//Calligra headers
#include "KoContextBarButton.h"

//KDE headers
#include <KGlobalSettings>
#include <KIconLoader>
#include <klocale.h>

//Qt Headers
#include <QAbstractItemView>
#include <QModelIndex>
#include <QApplication>
#include <QHBoxLayout>
#include <QHoverEvent>

/** Space between the item outer rect and the context bar */
const int CONTEXTBAR_MARGIN = 1;

KoViewItemContextBar::KoViewItemContextBar(QAbstractItemView *parent)
    : QObject(parent)
    , m_view(parent)
    , m_enabled(true)
    , m_appliedPointingHandCursor(false)
{
    connect(parent, SIGNAL(entered(const QModelIndex&)),
            this, SLOT(slotEntered(const QModelIndex&)));
    connect(parent, SIGNAL(viewportEntered()),
            this, SLOT(slotViewportEntered()));

    m_ContextBar = new QWidget(m_view->viewport());
    m_ContextBar->hide();
    m_ToggleSelectionButton = new KoContextBarButton("list-add");

    m_Layout = new QHBoxLayout(m_ContextBar);
    m_Layout->setMargin(2);
    m_Layout->setSpacing(2);
    m_Layout->addWidget(m_ToggleSelectionButton);

    connect(m_ToggleSelectionButton, SIGNAL(clicked()),
            this, SLOT(setItemSelected()));
    connect(m_view->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(slotRowsRemoved(const QModelIndex&, int, int)));

    m_ContextBar->installEventFilter(this);
    m_view->viewport()->installEventFilter(this);
    m_view->setMouseTracking(true);
}

KoViewItemContextBar::~KoViewItemContextBar()
{
}

bool KoViewItemContextBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_view->viewport()) {
        switch (event->type()) {
        case QEvent::Leave:
            if (m_ContextBar->isVisible()) {
                m_ContextBar->hide();
            }
            break;

        case QEvent::MouseButtonPress: {
            // Set the toggle invisible, if a mouse button has been pressed
            // outside the toggle boundaries. This e.g. assures, that the toggle
            // gets invisible during dragging items.
            if (m_ContextBar->isVisible()) {
                const QRect mContextBarBounds(m_ContextBar->mapToGlobal(QPoint(0, 0)), m_ContextBar->size());
                m_ContextBar->setVisible(mContextBarBounds.contains(QCursor::pos()));
            }
            break;
        }

        default:
            break;
        }
    }
    else if (watched == m_ContextBar) {
            switch (event->type()) {
            case QEvent::Enter:
                QApplication::changeOverrideCursor(Qt::PointingHandCursor);
                break;

            case QEvent::Leave:
                QApplication::changeOverrideCursor(Qt::ArrowCursor);
                break;

            default:
                break;
            }
        }
    return QObject::eventFilter(watched, event);
}

void KoViewItemContextBar::slotEntered(const QModelIndex &index)
{
    const bool isSelectionCandidate = index.isValid() &&
                                      (QApplication::mouseButtons() == Qt::NoButton);

    restoreCursor();
    if (isSelectionCandidate && KGlobalSettings::singleClick()) {
        applyPointingHandCursor();
    }

    if (!m_ContextBar || !m_enabled) {
        return;
    }

    m_ContextBar->hide();
    if (isSelectionCandidate) {
        updateHoverUi(index);
    }
    else {
        updateHoverUi(QModelIndex());
    }
}

void KoViewItemContextBar::updateHoverUi(const QModelIndex &index)
{
    QModelIndex oldIndex = m_IndexUnderCursor;
    m_IndexUnderCursor = index;
    m_view->update(oldIndex);

    const bool isSelectionCandidate = index.isValid();

    m_ContextBar->hide();
    if (isSelectionCandidate) {
        updateToggleSelectionButton();
        const QRect rect = m_view->visualRect(m_IndexUnderCursor);
        showContextBar(rect);
        m_view->update(m_IndexUnderCursor);
    } else {
        m_ContextBar->hide();
    }
}

void KoViewItemContextBar::showContextBar(const QRect &rect)
{
    m_ContextBar->adjustSize();
    // Center bar in FullContextBar mode, left align in
    // SelectionOnlyContextBar mode
    const int posX = 0;
    const int posY = CONTEXTBAR_MARGIN;
    m_ContextBar->move(rect.topLeft() + QPoint(posX, posY));
    m_ContextBar->show();
}

void KoViewItemContextBar::slotViewportEntered()
{
    m_ContextBar->hide();
    restoreCursor();
}

void KoViewItemContextBar::setItemSelected()
{
    emit selectionChanged();

    if (m_IndexUnderCursor.isValid()) {
        QItemSelectionModel *selModel = m_view->selectionModel();
        if (!selModel->isSelected(m_IndexUnderCursor)) {
            selModel->select(m_IndexUnderCursor, QItemSelectionModel::Select);
        }
        else {
            selModel->select(m_IndexUnderCursor, QItemSelectionModel::Deselect);
        }
        selModel->setCurrentIndex(m_IndexUnderCursor, QItemSelectionModel::Current);
    }
}

void KoViewItemContextBar::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    if (m_ContextBar) {
        m_ContextBar->hide();
    }
    restoreCursor();
}

void KoViewItemContextBar::applyPointingHandCursor()
{
    if (!m_appliedPointingHandCursor) {
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
        m_appliedPointingHandCursor = true;
    }
}

void KoViewItemContextBar::restoreCursor()
{
    if (m_appliedPointingHandCursor) {
        QApplication::restoreOverrideCursor();
        m_appliedPointingHandCursor = false;
    }
}

void KoViewItemContextBar::updateToggleSelectionButton()
{
    m_ToggleSelectionButton->setIcon(SmallIcon(
            m_view->selectionModel()->isSelected(m_IndexUnderCursor) ? "list-remove" : "list-add"));
    m_ToggleSelectionButton->setToolTip( m_view->selectionModel()->isSelected(m_IndexUnderCursor) ? i18n("deselect item") : i18n("select item"));
}

QToolButton * KoViewItemContextBar::addContextButton(QString text, QString iconName)
{
    KoContextBarButton *newContexButton = new KoContextBarButton(iconName);
    newContexButton->setToolTip(text);
    m_Layout->addWidget(newContexButton);
    return newContexButton;
}

QModelIndex KoViewItemContextBar::currentIndex()
{
    return m_IndexUnderCursor;
}

void KoViewItemContextBar::reset()
{
    if (m_ContextBar) {
        m_ContextBar->hide();
    }
    restoreCursor();
}

void KoViewItemContextBar::enableContextBar()
{
    m_enabled = true;
}

void KoViewItemContextBar::disableContextBar()
{
    m_enabled = false;
}

#include "KoViewItemContextBar.moc"
