/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).

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

#include "KexiSearchLineEdit.h"
#include <KexiSearchableModel.h>

#include <KLocale>
#include <KIcon>
#include <KDebug>

#include <kexiutils/completer/qcompleter.h>
#include <QShortcut>
#include <QKeySequence>
#include <QTreeView>
#include <QAbstractProxyModel>
#include <QInputMethodEvent>

class SearchableObject
{
public:
    KexiSearchableModel *model;
    int index;
};

class KexiSearchLineEditCompleterPopupModel : public QAbstractListModel
{
public:
    explicit KexiSearchLineEditCompleterPopupModel(QObject *parent = 0);
    ~KexiSearchLineEditCompleterPopupModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    //virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    //virtual QModelIndex parent(const QModelIndex &index) const;
    //virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    void addSearchableModel(KexiSearchableModel *model);
private:
    class Private;
    Private * const d;
};

class KexiSearchLineEditCompleterPopupModel::Private
{
public:
    Private()
     : cachedCount(-1)
    {
    }
    ~Private() {
        qDeleteAll(searchableObjects);
    }
    void updateCachedCount() {
        if (searchableModels.isEmpty()) {
            return;
        }
        cachedCount = 0;
        foreach (KexiSearchableModel* searchableModel, searchableModels) {
            cachedCount += searchableModel->searchableObjectCount();
        }
    }
    int cachedCount;
    QList<KexiSearchableModel*> searchableModels;
    QMap<int, SearchableObject*> searchableObjects;
};

KexiSearchLineEditCompleterPopupModel::KexiSearchLineEditCompleterPopupModel(QObject *parent)
 : QAbstractListModel(parent), d(new Private)
{
}

KexiSearchLineEditCompleterPopupModel::~KexiSearchLineEditCompleterPopupModel()
{
    delete d;
}

int KexiSearchLineEditCompleterPopupModel::rowCount(const QModelIndex &parent) const
{
    if (d->cachedCount < 0) {
        d->updateCachedCount();
    }
    return d->cachedCount;
}

QVariant KexiSearchLineEditCompleterPopupModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (d->cachedCount <= row) {
        return QVariant();
    }
    SearchableObject *object = static_cast<SearchableObject*>(index.internalPointer());
    QModelIndex sourceIndex = object->model->sourceIndexForSearchableObject(object->index);
    return object->model->searchableData(sourceIndex, role);
}

QModelIndex KexiSearchLineEditCompleterPopupModel::index(int row, int column,
                                                         const QModelIndex &parent) const
{
    //kDebug() << row;
    if (!hasIndex(row, column, parent)) {
        kDebug() << "!hasIndex";
        return QModelIndex();
    }

    int r = row;
    SearchableObject *sobject = d->searchableObjects.value(row);
    if (!sobject) {
        foreach (KexiSearchableModel* searchableModel, d->searchableModels) {
            const int count = searchableModel->searchableObjectCount();
            if (r < count) {
//! @todo LEAK
                sobject = new SearchableObject;
                sobject->model = searchableModel;
                sobject->index = r;
                //d->searchableObjects.insert(row, sobject);
                break;
            }
            else {
                r -= count;
            }
        }
    }
    if (!sobject) {
        return QModelIndex();
    }
    return createIndex(row, column, sobject);
}

void KexiSearchLineEditCompleterPopupModel::addSearchableModel(KexiSearchableModel *model)
{
    d->searchableModels.removeAll(model);
    d->searchableModels.append(model);
    d->updateCachedCount();
}

// ----

class KexiSearchLineEditCompleter : public KexiUtils::QCompleter
{
public:
    KexiSearchLineEditCompleter(QObject *parent = 0) : KexiUtils::QCompleter(parent) {
    }

    virtual QString pathFromIndex(const QModelIndex &index) const {
        if (!index.isValid())
            return QString();
        SearchableObject *object = static_cast<SearchableObject*>(index.internalPointer());
        QModelIndex sourceIndex = object->model->sourceIndexForSearchableObject(object->index);
        return object->model->pathFromIndex(sourceIndex);
    }
};

// ----

class KexiSearchLineEdit::Private
{
public:
    explicit Private(KexiSearchLineEdit *_q)
     : q(_q), clearShortcut(QKeySequence(Qt::Key_Escape), _q)
    {
        // make Escape key clear the search box
        QObject::connect(&clearShortcut, SIGNAL(activated()),
                         q, SLOT(clear()));
    }
    KexiSearchLineEditCompleter *completer;
    KexiSearchLineEditCompleterPopupModel *model;
private:
    KexiSearchLineEdit *q;
    QShortcut clearShortcut;
};

// ----

KexiSearchLineEdit::KexiSearchLineEdit(QWidget *parent)
 : KLineEdit(parent), d(new Private(this))
{
    d->completer = new KexiSearchLineEditCompleter(this);
    QTreeView *treeView = new QTreeView;
    treeView->setHeaderHidden(true);
    treeView->setRootIsDecorated(false);
    d->completer->setPopup(treeView);
    d->completer->setModel(d->model = new KexiSearchLineEditCompleterPopupModel(d->completer));
    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
    d->completer->setMaxVisibleItems(12);
    // Use unsorted model, sorting is handled in the source model itself.
    // Moreover, sorting QCompleter::CaseInsensitivelySortedModel breaks
    // filtering so only table names are displayed.
    d->completer->setModelSorting(KexiUtils::QCompleter::UnsortedModel);
    
    // forked initialiation like in QLineEdit::setCompleter:
    d->completer->setWidget(this);
    if (hasFocus()) {
        connectCompleter();
    }
    connect(d->completer, SIGNAL(highlighted(QModelIndex)),
            this, SLOT(slotCompletionHighlighted(QModelIndex)));
    connect(d->completer, SIGNAL(activated(QModelIndex)),
            this, SLOT(slotCompletionActivated(QModelIndex)));

    setFocusPolicy(Qt::ClickFocus);
    setClearButtonShown(true);
    setClickMessage(i18n("Search"));
}

KexiSearchLineEdit::~KexiSearchLineEdit()
{
    delete d;
}

void KexiSearchLineEdit::connectCompleter()
{
    connect(d->completer, SIGNAL(activated(QString)),
            this, SLOT(setText(QString)));
    connect(d->completer, SIGNAL(highlighted(QString)),
            this, SLOT(slotCompletionHighlighted(QString)));
}

void KexiSearchLineEdit::disconnectCompleter()
{
    disconnect(d->completer, 0, this, 0);
}

void KexiSearchLineEdit::slotCompletionHighlighted(const QString &newText)
{
    if (d->completer->completionMode() != KexiUtils::QCompleter::InlineCompletion) {
        setText(newText);
    }
    else {
        int p = cursorPosition();
        QString t = text();
        setText(t.left(p) + newText.mid(p));
        end(false);
        cursorBackward(text().length() - p, true);
    }
}

void KexiSearchLineEdit::addSearchableModel(KexiSearchableModel *model)
{
    d->model->addSearchableModel(model);
}

QPair<QModelIndex, KexiSearchableModel*> KexiSearchLineEdit::mapCompletionIndexToSource(const QModelIndex &index) const
{
    QModelIndex realIndex
        = qobject_cast<QAbstractProxyModel*>(d->completer->completionModel())->mapToSource(index);
    if (!realIndex.isValid()) {
        return qMakePair(QModelIndex(), static_cast<KexiSearchableModel*>(0));
    }
    SearchableObject *object = static_cast<SearchableObject*>(realIndex.internalPointer());
    if (!object) {
        return qMakePair(QModelIndex(), static_cast<KexiSearchableModel*>(0));
    }
    return qMakePair(object->model->sourceIndexForSearchableObject(object->index), object->model);
}

void KexiSearchLineEdit::slotCompletionHighlighted(const QModelIndex &index)
{
    QPair<QModelIndex, KexiSearchableModel*> source = mapCompletionIndexToSource(index);
    if (!source.first.isValid())
        return;
    kDebug() << source.second->searchableData(source.first, Qt::EditRole);
    source.second->highlightSearchableObject(source.first);
}

void KexiSearchLineEdit::slotCompletionActivated(const QModelIndex &index)
{
    QPair<QModelIndex, KexiSearchableModel*> source = mapCompletionIndexToSource(index);
    if (!source.first.isValid())
        return;
    kDebug() << source.second->searchableData(source.first, Qt::EditRole);
    source.second->activateSearchableObject(source.first);
}

// forked bits from QLineEdit::inputMethodEvent()
void KexiSearchLineEdit::inputMethodEvent(QInputMethodEvent *e)
{
    KLineEdit::inputMethodEvent(e);
    if (isReadOnly() || !e->isAccepted())
        return;
    if (!e->commitString().isEmpty()) {
        complete(Qt::Key_unknown);
    }
}

// forked bits from QLineEdit::focusInEvent()
void KexiSearchLineEdit::focusInEvent(QFocusEvent *e)
{
    KLineEdit::focusInEvent(e);
    d->completer->setWidget(this);
    connectCompleter();
    update();
}

// forked bits from QLineEdit::focusOutEvent()
void KexiSearchLineEdit::focusOutEvent(QFocusEvent *e)
{
    KLineEdit::focusOutEvent(e);
    disconnectCompleter();
    update();
}

// forked bits from QLineControl::processKeyEvent()
void KexiSearchLineEdit::keyPressEvent(QKeyEvent *event)
{
    bool inlineCompletionAccepted = false;

    KexiUtils::QCompleter::CompletionMode completionMode = d->completer->completionMode();
    if ((completionMode == KexiUtils::QCompleter::PopupCompletion
            || completionMode == KexiUtils::QCompleter::UnfilteredPopupCompletion)
        && d->completer->popup()
        && d->completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        // Ignoring the events lets the completer provide suitable default behavior
        switch (event->key()) {
        case Qt::Key_Escape:
            event->ignore();
            return;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (!QApplication::keypadNavigationEnabled())
                break;
#endif
            d->completer->popup()->hide(); // just hide. will end up propagating to parent
        default:
            break; // normal key processing
        }
    } else if (completionMode == KexiUtils::QCompleter::InlineCompletion) {
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (!QApplication::keypadNavigationEnabled())
                break;
#endif
            if (!d->completer->currentCompletion().isEmpty() && hasSelectedText()
                && textAfterSelection().isEmpty())
            {
                setText(d->completer->currentCompletion());
                inlineCompletionAccepted = true;
            }
        default:
            break; // normal key processing
        }
    }

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        KLineEdit::keyPressEvent(event); /* executes this:
                                            if (hasAcceptableInput() || fixup()) {
                                                emit returnPressed();
                                                emit editingFinished();
                                            } */
        if (inlineCompletionAccepted)
            event->accept();
        else
            event->ignore();
        return;
    }

    bool unknown = false;

    if (event == QKeySequence::MoveToNextChar) {
#if defined(Q_WS_WIN)
        if (hasSelectedText()
            && d->completer->completionMode() == KexiUtils::QCompleter::InlineCompletion)
        {
            moveCursor(selectionEnd(), false);
            event->accept();
            return;
        }
#endif
    }
    else if (event == QKeySequence::MoveToPreviousChar) {
#if defined(Q_WS_WIN)
        if (hasSelectedText()
            && d->completer->completionMode() == KexiUtils::QCompleter::InlineCompletion)
        {
            moveCursor(selectionStart(), false);
            event->accept();
            return;
        }
#endif
    }
    else {
        bool handled = false;
        if (event->modifiers() & Qt::ControlModifier) {
            switch (event->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
                complete(event->key());
                return;
            default:;
            }
        } else { // ### check for *no* modifier
            switch (event->key()) {
            case Qt::Key_Backspace:
                if (!isReadOnly()) {
                    backspace();
                    complete(Qt::Key_Backspace);
                    return;
                }
                break;
            default:;
            }
        }
    }

    if (!isReadOnly()) {
        QString t = event->text();
        if (!t.isEmpty() && t.at(0).isPrint()) {
            KLineEdit::keyPressEvent(event);
            complete(event->key());
            return;
        }
    }

    KLineEdit::keyPressEvent(event);
}

// forked bits from QLineControl::advanceToEnabledItem()
// iterating forward(dir=1)/backward(dir=-1) from the
// current row based. dir=0 indicates a new completion prefix was set.
bool KexiSearchLineEdit::advanceToEnabledItem(int dir)
{
    int start = d->completer->currentRow();
    if (start == -1)
        return false;
    int i = start + dir;
    if (dir == 0)
        dir = 1;
    do {
        if (!d->completer->setCurrentRow(i)) {
            if (!d->completer->wrapAround())
                break;
            i = i > 0 ? 0 : d->completer->completionCount() - 1;
        } else {
            QModelIndex currentIndex = d->completer->currentIndex();
            if (d->completer->completionModel()->flags(currentIndex) & Qt::ItemIsEnabled)
                return true;
            i += dir;
        }
    } while (i != start);

    d->completer->setCurrentRow(start); // restore
    return false;
}

QString KexiSearchLineEdit::textBeforeSelection() const
{
    return hasSelectedText() ? text().left(selectionStart()) : QString();
}

QString KexiSearchLineEdit::textAfterSelection() const
{
    return hasSelectedText() ? text().mid(selectionEnd()) : QString();
}

int KexiSearchLineEdit::selectionEnd() const
{
    return hasSelectedText() ?
        (selectionStart() + selectedText().length()) : -1;
}

// forked bits from QLineControl::complete()
void KexiSearchLineEdit::complete(int key)
{
    if (isReadOnly() || echoMode() != QLineEdit::Normal)
        return;

    QString text = this->text();
    if (d->completer->completionMode() == KexiUtils::QCompleter::InlineCompletion) {
        if (key == Qt::Key_Backspace)
            return;
        int n = 0;
        if (key == Qt::Key_Up || key == Qt::Key_Down) {
            if (textAfterSelection().length())
                return;
            QString prefix = hasSelectedText() ? textBeforeSelection() : text;
            if (text.compare(d->completer->currentCompletion(), d->completer->caseSensitivity()) != 0
                || prefix.compare(d->completer->completionPrefix(), d->completer->caseSensitivity()) != 0) {
                d->completer->setCompletionPrefix(prefix);
            } else {
                n = (key == Qt::Key_Up) ? -1 : +1;
            }
        } else {
            d->completer->setCompletionPrefix(text);
        }
        if (!advanceToEnabledItem(n))
            return;
    } else {
#ifndef QT_KEYPAD_NAVIGATION
        if (text.isEmpty()) {
            d->completer->popup()->hide();
            return;
        }
#endif
        d->completer->setCompletionPrefix(text);
    }

    d->completer->complete();
}
