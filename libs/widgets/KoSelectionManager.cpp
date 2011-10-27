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

#include "KoSelectionManager.h"

//Calligra headers
#include "KoSelectionToggle.h"
#include "KoContextBarButton.h"

//KDE headers
#include <KGlobalSettings>
#include <KIconLoader>

//Qt Headers
#include <QAbstractItemView>
#include <QModelIndex>
#include <QApplication>
#include <QHBoxLayout>
#include <QHoverEvent>

/** Space between the item outer rect and the context bar */
const int CONTEXTBAR_MARGIN = 1;

KoSelectionManager::KoSelectionManager(QAbstractItemView *parent)
    : QObject(parent)
    , m_view(parent)
    , m_appliedPointingHandCursor(false)
{
    connect(parent, SIGNAL(entered(const QModelIndex&)),
            this, SLOT(slotEntered(const QModelIndex&)));
    connect(parent, SIGNAL(viewportEntered()),
            this, SLOT(slotViewportEntered()));

    mContextBar = new QWidget(m_view->viewport());
    mContextBar->hide();
    mToggleSelectionButton = new KoContextBarButton("list-add");

    m_Layout = new QHBoxLayout(mContextBar);
    m_Layout->setMargin(2);
    m_Layout->setSpacing(2);
    m_Layout->addWidget(mToggleSelectionButton);

    connect(mToggleSelectionButton, SIGNAL(clicked()),
            this, SLOT(setItemSelected()));

    mContextBar->installEventFilter(this);
    m_view->viewport()->installEventFilter(this);
    m_view->setMouseTracking(true);
}

KoSelectionManager::~KoSelectionManager()
{
}

bool KoSelectionManager::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_view->viewport()) {
        switch (event->type()) {
        case QEvent::Leave:
            if (mContextBar->isVisible()) {
                mContextBar->hide();
            }
            break;

        case QEvent::MouseButtonPress: {
            // Set the toggle invisible, if a mouse button has been pressed
            // outside the toggle boundaries. This e.g. assures, that the toggle
            // gets invisible during dragging items.
            if (mContextBar->isVisible()) {
                const QRect mContextBarBounds(mContextBar->mapToGlobal(QPoint(0, 0)), mContextBar->size());
                mContextBar->setVisible(mContextBarBounds.contains(QCursor::pos()));
            }
            break;
        }

        default:
            break;
        }
    }
    else if (watched == mContextBar) {
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

void KoSelectionManager::slotEntered(const QModelIndex &index)
{
    const bool isSelectionCandidate = index.isValid() &&
                                      (QApplication::mouseButtons() == Qt::NoButton);

    restoreCursor();
    if (isSelectionCandidate && KGlobalSettings::singleClick()) {
        applyPointingHandCursor();
    }

    if (!mContextBar) {
        return;
    }

    mContextBar->hide();
    if (isSelectionCandidate) {
        updateHoverUi(index);
    }
    else {
        updateHoverUi(QModelIndex());
    }
}

void KoSelectionManager::updateHoverUi(const QModelIndex &index)
{
    QModelIndex oldIndex = mIndexUnderCursor;
    mIndexUnderCursor = index;
    m_view->update(oldIndex);

    const bool isSelectionCandidate = index.isValid();

    mContextBar->hide();
    if (isSelectionCandidate) {
        updateToggleSelectionButton();
        const QRect rect = m_view->visualRect(mIndexUnderCursor);
        showContextBar(rect);
        m_view->update(mIndexUnderCursor);
    } else {
        mContextBar->hide();
    }
}

void KoSelectionManager::showContextBar(const QRect &rect)
{
    mContextBar->adjustSize();
    // Center bar in FullContextBar mode, left align in
    // SelectionOnlyContextBar mode
    const int posX = 0;
    const int posY = CONTEXTBAR_MARGIN;
    mContextBar->move(rect.topLeft() + QPoint(posX, posY));
    mContextBar->show();
}

void KoSelectionManager::slotViewportEntered()
{
    mContextBar->hide();
    restoreCursor();
}

void KoSelectionManager::setItemSelected()
{
    emit selectionChanged();

    if (mIndexUnderCursor.isValid()) {
        QItemSelectionModel *selModel = m_view->selectionModel();
        if (!selModel->isSelected(mIndexUnderCursor)) {
            selModel->select(mIndexUnderCursor, QItemSelectionModel::Select);
        }
        else {
            selModel->select(mIndexUnderCursor, QItemSelectionModel::Deselect);
        }
        selModel->setCurrentIndex(mIndexUnderCursor, QItemSelectionModel::Current);
    }
}

void KoSelectionManager::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    if (mContextBar) {
        mContextBar->hide();
    }
    restoreCursor();
}

void KoSelectionManager::applyPointingHandCursor()
{
    if (!m_appliedPointingHandCursor) {
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
        m_appliedPointingHandCursor = true;
    }
}

void KoSelectionManager::restoreCursor()
{
    if (m_appliedPointingHandCursor) {
        QApplication::restoreOverrideCursor();
        m_appliedPointingHandCursor = false;
    }
}

void KoSelectionManager::updateToggleSelectionButton()
{
    mToggleSelectionButton->setIcon(SmallIcon(
            m_view->selectionModel()->isSelected(mIndexUnderCursor) ? "list-remove" : "list-add"
            ));
    mToggleSelectionButton->setToolTip( m_view->selectionModel()->isSelected(mIndexUnderCursor) ? "deselect page" : "select page");
}

QToolButton * KoSelectionManager::addContextButton(QString text, QString iconName)
{
    KoContextBarButton *newContexButton = new KoContextBarButton(iconName);
    newContexButton->setToolTip(text);
    m_Layout->addWidget(newContexButton);
    return newContexButton;
}

QModelIndex KoSelectionManager::currentIndex()
{
    return mIndexUnderCursor;
}

#include "KoSelectionManager.moc"
