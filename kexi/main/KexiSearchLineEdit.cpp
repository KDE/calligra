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
#include <KDebug>

#include <kexiutils/completer/KexiCompleter.h>
#include <QShortcut>
#include <QKeySequence>
#include <QTreeView>
#include <QAbstractProxyModel>
#include <QInputMethodEvent>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QPainter>

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
    Q_UNUSED(parent);
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
                sobject = new SearchableObject;
                sobject->model = searchableModel;
                sobject->index = r;
                d->searchableObjects.insert(row, sobject);
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

class KexiSearchLineEditCompleter : public KexiCompleter
{
public:
    KexiSearchLineEditCompleter(QObject *parent = 0) : KexiCompleter(parent) {
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

class KexiSearchLineEditPopupItemDelegate;

class KexiSearchLineEdit::Private
{
public:
    explicit Private(KexiSearchLineEdit *_q)
     : q(_q), clearShortcut(QKeySequence(Qt::Key_Escape), _q),
       recentlyHighlightedModel(0)
    {
        // make Escape key clear the search box
        QObject::connect(&clearShortcut, SIGNAL(activated()),
                         q, SLOT(slotClearShortcutActivated()));
    }

    void highlightSearchableObject(const QPair<QModelIndex, KexiSearchableModel*> &source)
    {
        source.second->highlightSearchableObject(source.first);
        recentlyHighlightedModel = source.second;
    }

    void removeHighlightingForSearchableObject()
    {
        if (recentlyHighlightedModel) {
            recentlyHighlightedModel->highlightSearchableObject(QModelIndex());
            recentlyHighlightedModel = 0;
        }
    }

    KexiSearchLineEditCompleter *completer;
    KexiSearchLineEditCompleterPopupModel *model;
    KexiSearchLineEditPopupItemDelegate *delegate;
    QPointer<QWidget> previouslyFocusedWidget;

private:
    KexiSearchLineEdit *q;
    QShortcut clearShortcut;
    KexiSearchableModel *recentlyHighlightedModel;
};

// ----

static QSizeF viewItemTextLayout(QTextLayout &textLayout, int lineWidth)
{
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

class KexiSearchLineEditPopupItemDelegate : public QStyledItemDelegate
{
public:
    KexiSearchLineEditPopupItemDelegate(QObject *parent, KexiCompleter *completer) 
     : QStyledItemDelegate(parent), highlightMatchingSubstrings(true), m_completer(completer)
    {
    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
    {
        QStyledItemDelegate::paint(painter, option, index);
        QStyleOptionViewItemV4 v4 = option;
        QStyledItemDelegate::initStyleOption(&v4, index);
        // like in QCommonStyle::paint():
        if (!v4.text.isEmpty()) {
            painter->save();
            painter->setClipRect(v4.rect);
            QPalette::ColorGroup cg = v4.state & QStyle::State_Enabled
                                    ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(v4.state & QStyle::State_Active)) {
                cg = QPalette::Inactive;
            }
            if (v4.state & QStyle::State_Selected) {
                painter->setPen(v4.palette.color(cg, QPalette::HighlightedText));
            }
            else {
                painter->setPen(v4.palette.color(cg, QPalette::Text));
            }
            QRect textRect = v4.widget->style()->subElementRect(QStyle::SE_ItemViewItemText,
                                                                &v4, v4.widget);
            /*if (v4->state & QStyle::State_Editing) {
                p->setPen(v4->palette.color(cg, QPalette::Text));
                p->drawRect(textRect.adjusted(0, 0, -1, -1));
            }*/
            viewItemDrawText(painter, &v4, textRect);
            painter->restore();
        }
    }
    bool highlightMatchingSubstrings;

protected:
    // bits from qcommonstyle.cpp
    void viewItemDrawText(QPainter *p, const QStyleOptionViewItemV4 *option, const QRect &rect) const
    {
        const QWidget *widget = option->widget;
        const int textMargin = widget->style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;

        QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
        const bool wrapText = option->features & QStyleOptionViewItemV2::WrapText;
        QTextOption textOption;
        textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
        textOption.setTextDirection(option->direction);
        textOption.setAlignment(QStyle::visualAlignment(option->direction, option->displayAlignment));
        QTextLayout textLayout;
        textLayout.setTextOption(textOption);
        QFont f(option->font);
        if (highlightMatchingSubstrings) {
            f.setWeight(QFont::Black);
        }
        textLayout.setFont(f);
        QString text = option->text;
        textLayout.setText(text);

        viewItemTextLayout(textLayout, textRect.width());

        if (highlightMatchingSubstrings) {
            QList<QTextLayout::FormatRange> formats;
            QString substring = m_completer->completionPrefix();

            for (int i = 0; i < text.length();) {
                i = text.indexOf(substring, i, Qt::CaseInsensitive);
                if (i == -1)
                    break;
                QTextLayout::FormatRange formatRange;
                formatRange.format.setFontWeight(QFont::Normal);
                formatRange.length = substring.length();
                formatRange.start = i;
                formats.append(formatRange);
                i += formatRange.length;
            }
            textLayout.setAdditionalFormats(formats);
        }
        const int lineCount = textLayout.lineCount();
/*        const QRect layoutRect = QStyle::alignedRect(option->direction, option->displayAlignment,
                                                    QSize(int(width), int(height)), textRect);*/
        QPointF position = textRect.topLeft(); /*layoutRect.topLeft();*/
        for (int i = 0; i < lineCount; ++i) {
            const QTextLine line = textLayout.lineAt(i);
            line.draw(p, position);
            position.setY(position.y() + line.y() + line.ascent());
        }
        //textLayout.draw(p, position, QVector<QTextLayout::FormatRange>(), textRect);
    }

    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
    {
        QStyledItemDelegate::initStyleOption(option, index);
        QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4*>(option);
        if (v4) {
            v4->text.clear();
        }
    }
    KexiCompleter *m_completer;
};

// ----

KexiSearchLineEdit::KexiSearchLineEdit(QWidget *parent)
 : KLineEdit(parent), d(new Private(this))
{
    d->completer = new KexiSearchLineEditCompleter(this);
    QTreeView *treeView = new QTreeView;
    d->completer->setPopup(treeView);
    d->completer->setModel(d->model = new KexiSearchLineEditCompleterPopupModel(d->completer));
    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
    d->completer->setSubstringCompletion(true);
    d->completer->setMaxVisibleItems(12);
    // Use unsorted model, sorting is handled in the source model itself.
    // Moreover, sorting KexiCompleter::CaseInsensitivelySortedModel breaks
    // filtering so only table names are displayed.
    d->completer->setModelSorting(KexiCompleter::UnsortedModel);
    
    treeView->setHeaderHidden(true);
    treeView->setRootIsDecorated(false);
    treeView->setItemDelegate(
        d->delegate = new KexiSearchLineEditPopupItemDelegate(treeView, d->completer));
    
    // forked initialization like in QLineEdit::setCompleter:
    d->completer->setWidget(this);
    if (hasFocus()) {
        connectCompleter();
    }

    setFocusPolicy(Qt::NoFocus); // We cannot focus set any policy here.
                                 // Qt::ClickFocus would make it impossible to find
                                 // previously focus widget in KexiSearchLineEdit::setFocus().
                                 // We need this information to focus back when pressing Escape key.
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
    connect(d->completer, SIGNAL(activated(QModelIndex)),
            this, SLOT(slotCompletionActivated(QModelIndex)));
    connect(d->completer, SIGNAL(highlighted(QString)),
            this, SLOT(slotCompletionHighlighted(QString)));
    connect(d->completer, SIGNAL(highlighted(QModelIndex)),
            this, SLOT(slotCompletionHighlighted(QModelIndex)));
}

void KexiSearchLineEdit::disconnectCompleter()
{
    disconnect(d->completer, 0, this, 0);
}

void KexiSearchLineEdit::slotClearShortcutActivated()
{
    //kDebug() << (QWidget*)d->previouslyFocusedWidget << text();
    d->removeHighlightingForSearchableObject();
    if (text().isEmpty() && d->previouslyFocusedWidget) {
        // after second Escape, go back to previously focused widget
        d->previouslyFocusedWidget->setFocus();
        d->previouslyFocusedWidget = 0;
    }
    else {
        clear();
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

void KexiSearchLineEdit::slotCompletionHighlighted(const QString &newText)
{
    if (d->completer->completionMode() != KexiCompleter::InlineCompletion) {
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

void KexiSearchLineEdit::slotCompletionHighlighted(const QModelIndex &index)
{
    QPair<QModelIndex, KexiSearchableModel*> source = mapCompletionIndexToSource(index);
    if (!source.first.isValid())
        return;
    //kDebug() << source.second->searchableData(source.first, Qt::EditRole);
    d->highlightSearchableObject(source);
}

void KexiSearchLineEdit::slotCompletionActivated(const QModelIndex &index)
{
    QPair<QModelIndex, KexiSearchableModel*> source = mapCompletionIndexToSource(index);
    if (!source.first.isValid())
        return;
    //kDebug() << source.second->searchableData(source.first, Qt::EditRole);
    
    d->highlightSearchableObject(source);
    d->removeHighlightingForSearchableObject();
    if (source.second->activateSearchableObject(source.first)) {
        clear();
    }
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

void KexiSearchLineEdit::setFocus()
{
    //kDebug() << "d->previouslyFocusedWidget:" << (QWidget*)d->previouslyFocusedWidget
    //         << "window()->focusWidget():" << window()->focusWidget();
    if (!d->previouslyFocusedWidget && window()->focusWidget() != this) {
        d->previouslyFocusedWidget = window()->focusWidget();
    }
    KLineEdit::setFocus();
}

// forked bits from QLineEdit::focusInEvent()
void KexiSearchLineEdit::focusInEvent(QFocusEvent *e)
{
    //kDebug() << "d->previouslyFocusedWidget:" << (QWidget*)d->previouslyFocusedWidget
    //         << "window()->focusWidget():" << window()->focusWidget();
    if (!d->previouslyFocusedWidget && window()->focusWidget() != this) {
        d->previouslyFocusedWidget = window()->focusWidget();
    }
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
    if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
        // go back to previously focused widget
        d->previouslyFocusedWidget->setFocus();
        e->accept();
    }
    d->previouslyFocusedWidget = 0;
    d->removeHighlightingForSearchableObject();
}

// forked bits from QLineControl::processKeyEvent()
void KexiSearchLineEdit::keyPressEvent(QKeyEvent *event)
{
    bool inlineCompletionAccepted = false;

    //kDebug() << event->key() << (QWidget*)d->previouslyFocusedWidget;

    KexiCompleter::CompletionMode completionMode = d->completer->completionMode();
    if ((completionMode == KexiCompleter::PopupCompletion
            || completionMode == KexiCompleter::UnfilteredPopupCompletion)
        && d->completer->popup()
        && d->completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        // Ignoring the events lets the completer provide suitable default behavior
        switch (event->key()) {
        case Qt::Key_Escape:
            event->ignore();
            return;
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (!QApplication::keypadNavigationEnabled())
                break;
#endif
            d->completer->popup()->hide(); // just hide. will end up propagating to parent
        default:
            break; // normal key processing
        }
    } else if (completionMode == KexiCompleter::InlineCompletion) {
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

    if (d->completer->popup() && !d->completer->popup()->isVisible()
        && (event->key() == Qt::Key_F4 || event->key() == Qt::Key_Down))
    {
        // go back to completing when popup is closed and F4/Down pressed
        d->completer->complete();
    }
    else if (d->completer->popup() && d->completer->popup()->isVisible()
        && event->key() == Qt::Key_F4)
    {
        // hide popup if F4 pressed
        d->completer->popup()->hide();
    }

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (d->completer->popup() && !d->completer->popup()->isVisible()) {
            d->completer->setCompletionPrefix(text());
        }
        if (d->completer->completionCount() == 1) {
            // single item on the completion list, select it automatically
            d->completer->setCurrentRow(0);
            slotCompletionActivated(d->completer->currentIndex());
            event->accept();
            if (d->completer->popup()) {
                d->completer->popup()->hide();
            }
            return;
        }
        //kDebug() << "currentRow:" << d->completer->currentRow();
        //kDebug() << "currentIndex:" << d->completer->currentIndex().isValid();
        //kDebug() << "currentCompletion:" << d->completer->currentCompletion();
        if (d->completer->popup() && d->completer->completionCount() > 1) {
            //kDebug () << "11111" << d->completer->completionPrefix()
            //          << d->completer->completionCount();
            
            // more than one item on completion list, find exact match, if found, accept
            for (int i = 0; i < d->completer->completionCount(); i++) {
                //kDebug() << d->completer->completionModel()->index(i, 0, QModelIndex()).data(Qt::EditRole).toString();
                if (d->completer->completionPrefix()
                    == d->completer->completionModel()->index(i, 0, QModelIndex()).data(Qt::EditRole).toString())
                {
                    d->completer->setCurrentRow(i);
                    slotCompletionActivated(d->completer->currentIndex());
                    event->accept();
                    d->completer->popup()->hide();
                    return;
                }
            }
            // exactly matching item not found
            bool selectedItem = !d->completer->popup()->selectionModel()->selectedIndexes().isEmpty();
            if (!selectedItem || !d->completer->popup()->isVisible()) {
                if (!d->completer->popup()->isVisible()) {
                    // there is no matching text, go back to completing
                    d->completer->complete();
                }
                // do not hide
                event->accept();
                return;
            }
        }
        // applying completion since there is item selected
        d->completer->popup()->hide();
        connectCompleter();
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

    if (event == QKeySequence::MoveToNextChar) {
#if defined(Q_WS_WIN)
        if (hasSelectedText()
            && d->completer->completionMode() == KexiCompleter::InlineCompletion)
        {
            int selEnd = selectionEnd();
            if (selEnd >= 0) {
                setCursorPosition(selEnd);
            }
            event->accept();
            return;
        }
#endif
    }
    else if (event == QKeySequence::MoveToPreviousChar) {
#if defined(Q_WS_WIN)
        if (hasSelectedText()
            && d->completer->completionMode() == KexiCompleter::InlineCompletion)
        {
            int selStart = selectionStart();
            if (selStart >= 0) {
                setCursorPosition(selStart);
            }
            event->accept();
            return;
        }
#endif
    }
    else {
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
            case Qt::Key_Delete:
                if (!isReadOnly()) {
                    KLineEdit::keyPressEvent(event);
                    complete(Qt::Key_Delete);
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
    if (d->completer->completionMode() == KexiCompleter::InlineCompletion) {
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

bool KexiSearchLineEdit::highlightMatchingSubstrings() const
{
    return d->delegate->highlightMatchingSubstrings;
}

void KexiSearchLineEdit::setHighlightMatchingSubstrings(bool highlight)
{
    d->delegate->highlightMatchingSubstrings = highlight;
}
