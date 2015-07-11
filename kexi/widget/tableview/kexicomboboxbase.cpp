/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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


#include <kexi_global.h>
#include "kexicomboboxbase.h"
#include <widget/utils/kexicomboboxdropdownbutton.h>
#include "kexicomboboxpopup.h"
#include "KexiTableScrollArea.h"
#include "kexi.h"

#include <KDbTableSchema>

#include <QDebug>

KexiComboBoxBase::KexiComboBoxBase()
{
    m_internalEditorValueChanged = false; //user has text or other value inside editor
    m_slotInternalEditorValueChanged_enabled = true;
    m_mouseBtnPressedWhenPopupVisible = false;
    m_insideCreatePopup = false;
    m_setValueOrTextInInternalEditor_enabled = true;
    m_updatePopupSelectionOnShow = true;
    m_moveCursorToEndInInternalEditor_enabled = true;
    m_selectAllInInternalEditor_enabled = true;
    m_setValueInInternalEditor_enabled = true;
    m_setVisibleValueOnSetValueInternal = false;
    m_reinstantiatePopupOnShow = false;
    m_focusPopupBeforeShow = false;
}

KexiComboBoxBase::~KexiComboBoxBase()
{
}

KDbLookupFieldSchema *KexiComboBoxBase::lookupFieldSchema() const
{
    if (field() && field()->table()) {
        KDbLookupFieldSchema *lookupFieldSchema = field()->table()->lookupFieldSchema(*field());
        if (lookupFieldSchema && !lookupFieldSchema->recordSource().name().isEmpty())
            return lookupFieldSchema;
    }
    return 0;
}

int KexiComboBoxBase::recordToHighlightForLookupTable() const
{
    if (!popup())
        return -1;//err
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
    if (!lookupFieldSchema)
        return -1;
    if (lookupFieldSchema->boundColumn() == -1)
        return -1; //err
    bool ok;
    const int recordUid = origValue().toInt();
//! @todo for now we're assuming the id is INTEGER
    KDbTableViewData *tvData = popup()->tableView()->data();
    const int boundColumn = boundColumnIndex();
    int record = -1;
    for (KDbTableViewDataIterator it(tvData->begin()); it != tvData->end(); ++it) {
        record++;
        KDbRecordData* data = *it;
        if (data->at(boundColumn).toInt(&ok) == recordUid && ok)
            return record;
        if (!ok)
            break;
    }
    //item not found: highlight 1st record, if available
    return -1;
}

void KexiComboBoxBase::setValueInternal(const QVariant& add_, bool removeOld)
{
    Q_UNUSED(removeOld);
    m_mouseBtnPressedWhenPopupVisible = false;
    m_updatePopupSelectionOnShow = true;
    QString add(add_.toString());
    if (add.isEmpty()) {
        KDbTableViewData *relData = column() ? column()->relatedData() : 0;
        QVariant valueToSet;
        bool hasValueToSet = true;
        int recordToHighlight = -1;
        KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
        if (lookupFieldSchema) {
            //use 'lookup field' model
//! @todo support more RowSourceType's, not only table
            if (lookupFieldSchema->boundColumn() == -1)
//! @todo errmsg
                return;
            if (m_setVisibleValueOnSetValueInternal) {
                //only for table views
                if (!popup())
                    createPopup(false/*!show*/);
            }
            if (popup()) {
                const int recordToHighlight = recordToHighlightForLookupTable();
                popup()->tableView()->setHighlightedRecordNumber(recordToHighlight);

                const int visibleColumn = visibleColumnIndex();
                if (m_setVisibleValueOnSetValueInternal && -1 != visibleColumn) {
                    //only for table views
                    KDbRecordData *data = popup()->tableView()->highlightedRecord();
                    if (data)
                        valueToSet = data->at(visibleColumn);
                } else {
                    hasValueToSet = false;
                }
            }
        } else if (relData) {
            //use 'related table data' model
            valueToSet = valueForString(origValue().toString(), &recordToHighlight, 0, 1);
        } else {
            //use 'enum hints' model
            const int record = origValue().toInt();
            valueToSet = field()->enumHint(record).trimmed();
        }
        if (hasValueToSet)
            setValueOrTextInInternalEditor(valueToSet);
        /*impl.*/moveCursorToEndInInternalEditor();
        /*impl.*/selectAllInInternalEditor();

        if (popup()) {
            if (origValue().isNull()) {
                popup()->tableView()->clearSelection();
                popup()->tableView()->setHighlightedRecordNumber(0);
            } else {
                if (relData) {
                    if (recordToHighlight != -1)
                        popup()->tableView()->setHighlightedRecordNumber(recordToHighlight);
                } else if (!lookupFieldSchema) {
                    //popup()->tableView()->selectRecord(origValue().toInt());
                    popup()->tableView()->setHighlightedRecordNumber(origValue().toInt());
                }
            }
        }
    } else {
        //! @todo autocompl.?
        if (popup())
            popup()->tableView()->clearSelection();
        /*impl.*/setValueInInternalEditor(add); //not setLineEditText(), because 'add' is entered by user!
        //setLineEditText( add );
        /*impl.*/moveCursorToEndInInternalEditor();
    }
}

KDbRecordData* KexiComboBoxBase::selectRecordForEnteredValueInLookupTable(const QVariant& v)
{
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
    if (!popup() || !lookupFieldSchema)
        return 0; //safety
//-not effective for large sets: please cache it!
//.trimmed() is not generic!

    const bool valueIsText = v.type() == QVariant::String || v.type() == QVariant::ByteArray; //most common case
    const QString txt(valueIsText ? v.toString().trimmed() : QString());
    KDbTableViewData *lookupData = popup()->tableView()->data();
    const int visibleColumn = visibleColumnIndex();
    if (-1 == visibleColumn)
        return 0;
    KDbTableViewDataIterator it(lookupData->begin());
    int record;
    for (record = 0;it != lookupData->end();++it, record++) {
        if (valueIsText) {
            if ((*it)->at(visibleColumn).toString().trimmed().compare(txt, Qt::CaseInsensitive) == 0)
                break;
        } else {
            if ((*it)->at(visibleColumn) == v)
                break;
        }
    }

    m_setValueOrTextInInternalEditor_enabled = false; // <-- this is the entered value,
    //     so do not change the internal editor's contents
    if (it != lookupData->constEnd())
        popup()->tableView()->selectRecord(record);
    else
        popup()->tableView()->clearSelection();

    m_setValueOrTextInInternalEditor_enabled = true;

    return it != lookupData->constEnd() ? *it : 0;
}

QString KexiComboBoxBase::valueForString(const QString& str, int* record,
        uint lookInColumn, uint returnFromColumn, bool allowNulls)
{
    Q_UNUSED(returnFromColumn);
    KDbTableViewData *relData = column() ? column()->relatedData() : 0;
    if (!relData)
        return QString(); //safety
    //use 'related table data' model
    //-not effective for large sets: please cache it!
    //.trimmed() is not generic!

    const QString txt(str.trimmed());
    KDbTableViewDataIterator it(relData->begin());
    for (*record = 0;it != relData->end();++it, (*record)++) {
        const QString s((*it)->at(lookInColumn).toString());
        if (s.trimmed().compare(txt, Qt::CaseInsensitive) == 0)
            return s;
    }

    *record = -1;

    if (column() && column()->isRelatedDataEditable())
        return str; //new value entered and that's allowed

    qWarning() << "no related record found, ID will be painted!";
    if (allowNulls)
        return QString();
    return str; //for sanity but it's weird to show id to the user
}

int KexiComboBoxBase::boundColumnIndex() const
{
    if (!lookupFieldSchema()) {
        return -1;
    }
    switch (lookupFieldSchema()->recordSource().type()) {
    case KDbLookupFieldSchema::RecordSource::Table:
        // When the record source is Table we have hardcoded columns: <visible>, <bound>
        return lookupFieldSchema()->visibleColumns().count();
    default:;
    }
    // When the record source is Query we use the lookup field's bound column index
    //! @todo Implement for other types
    return lookupFieldSchema()->boundColumn();
}

int KexiComboBoxBase::visibleColumnIndex() const
{
    if (!lookupFieldSchema() || lookupFieldSchema()->visibleColumns().isEmpty()) {
        return -1;
    }
    switch (lookupFieldSchema()->recordSource().type()) {
    case KDbLookupFieldSchema::RecordSource::Table:
        // When the record source is Table we have hardcoded columns: <visible>, <bound>
        return lookupFieldSchema()->visibleColumn(0);
    default:;
    }
    // When the record source is Query we use the lookup field's visible column index
    //! @todo Implement for multiple visible columns
    //! @todo Implement for other types
    return lookupFieldSchema()->visibleColumns().first();
}

QVariant KexiComboBoxBase::value()
{
    KDbTableViewData *relData = column() ? column()->relatedData() : 0;
    KDbLookupFieldSchema *lookupFieldSchema = 0;
    if (relData) {
        if (m_internalEditorValueChanged) {
            //we've user-entered text: look for id
//! @todo make error if matching text not found?
            int recordToHighlight;
            return valueForString(m_userEnteredValue.toString(), &recordToHighlight, 1, 0, true/*allowNulls*/);
        } else {
            //use 'related table data' model
            KDbRecordData *data = popup() ? popup()->tableView()->selectedRecord() : 0;
            return data ? data->at(0) : origValue();
        }
    } else if ((lookupFieldSchema = this->lookupFieldSchema())) {
        if (lookupFieldSchema->boundColumn() == -1)
            return origValue();
        KDbRecordData *data = popup() ? popup()->tableView()->selectedRecord() : 0;
        if (/*!record &&*/ m_internalEditorValueChanged && !m_userEnteredValue.toString().isEmpty()) { //
            //try to select a record using the user-entered text
            if (!popup()) {
                QVariant prevUserEnteredValue = m_userEnteredValue;
                createPopup(false);
                m_userEnteredValue = prevUserEnteredValue;
            }
            data = selectRecordForEnteredValueInLookupTable(m_userEnteredValue);
        }
        return data ? data->at(boundColumnIndex()) : QVariant();
    } else if (popup()) {
        //use 'enum hints' model
        const int record = popup()->tableView()->currentRecord();
        if (record >= 0)
            return QVariant(record);
    }

    if (valueFromInternalEditor().toString().isEmpty())
        return QVariant();
    /*! \todo don't return just 1st record, but use autocompletion feature
          and: show message box if entered text does not match! */
    return origValue(); //unchanged
}

QVariant KexiComboBoxBase::visibleValueForLookupField()
{
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
    if (!popup() || !lookupFieldSchema)
        return QVariant();
    const int visibleColumn = visibleColumnIndex();
    //qDebug() << "visibleColumn" << visibleColumn;
    if (-1 == visibleColumn)
        return QVariant();
    KDbRecordData *data = popup()->tableView()->selectedRecord();
    return data ? data->at(qMin(visibleColumn, data->count() - 1)/*sanity*/) : QVariant();
}

QVariant KexiComboBoxBase::visibleValue()
{
    return m_visibleValue;
}

void KexiComboBoxBase::clear()
{
    if (popup())
        popup()->hide();
    slotInternalEditorValueChanged(QVariant());
}

tristate KexiComboBoxBase::valueChangedInternal()
{
    //avoid comparing values:
    KDbTableViewData *relData = column() ? column()->relatedData() : 0;
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
    if (relData || lookupFieldSchema) {
        if (m_internalEditorValueChanged)
            return true;

        //use 'related table data' model
        KDbRecordData *data = popup() ? popup()->tableView()->selectedRecord() : 0;
        if (!data)
            return false;
    } else {
        //use 'enum hints' model
        const int record = popup() ? popup()->tableView()->currentRecord() : -1;
        if (record < 0 && !m_internalEditorValueChanged/*true if text box is cleared*/)
            return false;
    }

    return cancelled;
}

bool KexiComboBoxBase::valueIsNull()
{
// bool ok;
    QVariant v(value());
    return v.isNull();
// return !ok || v.isNull();
}

bool KexiComboBoxBase::valueIsEmpty()
{
    return valueIsNull();
}

void KexiComboBoxBase::showPopup()
{
    //qDebug();
    createPopup(true);
}

void KexiComboBoxBase::createPopup(bool show)
{
    //qDebug() << show << field() << popup() << m_updatePopupSelectionOnShow;
    if (!field())
        return;
    m_insideCreatePopup = true;
    QWidget* thisWidget = dynamic_cast<QWidget*>(this);
    QWidget *widgetToFocus = internalEditor() ? internalEditor() : thisWidget;
    //qDebug() << "widgetToFocus:" << widgetToFocus;

    if (m_reinstantiatePopupOnShow) {
        QWidget *oldPopup = popup();
        setPopup(0);
        delete oldPopup;
    }

    if (!popup()) {
        setPopup(column() ? new KexiComboBoxPopup(thisWidget, *column())
                 : new KexiComboBoxPopup(thisWidget, *field()));
        QObject::connect(popup(), SIGNAL(recordAccepted(KDbRecordData*,int)),
                         thisWidget, SLOT(slotRecordAccepted(KDbRecordData*,int)));
        QObject::connect(popup()->tableView(), SIGNAL(itemSelected(KDbRecordData*)),
                         thisWidget, SLOT(slotRecordSelected(KDbRecordData*)));

        popup()->setFocusProxy(widgetToFocus);
        popup()->tableView()->setFocusProxy(widgetToFocus);
        popup()->installEventFilter(thisWidget);

        if (origValue().isNull())
            popup()->tableView()->clearSelection();
        else {
            popup()->tableView()->selectRecord(0);
            popup()->tableView()->setHighlightedRecordNumber(0);
        }
    }
    if (show && internalEditor() && !internalEditor()->isVisible())
        /*emit*/editRequested();

    QPoint posMappedToGlobal = mapFromParentToGlobal(thisWidget->pos());
    if (posMappedToGlobal != QPoint(-1, -1)) {
//! todo alter the position to fit the popup within screen boundaries
        popup()->hide();
        popup()->move(posMappedToGlobal + QPoint(0, thisWidget->height()));
        //qDebug() << "pos:" << posMappedToGlobal + QPoint(0, thisWidget->height());
        //to avoid flickering: first resize to 0-height, then show and resize back to prev. height
        const int w = popupWidthHint();
        popup()->resize(w, 0);
        if (show) {
            popup()->show();
            //qDebug() << "SHOW!!!";
        }
        popup()->updateSize(w);
        if (m_updatePopupSelectionOnShow) {
            int recordToHighlight = -1;
            KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
            KDbTableViewData *relData = column() ? column()->relatedData() : 0;
            if (lookupFieldSchema) {
                recordToHighlight = recordToHighlightForLookupTable();
            } else if (relData) {
                (void)valueForString(origValue().toString(), &recordToHighlight, 0, 1);
            } else //enum hint
                recordToHighlight = origValue().toInt();

            /*-->*/ m_moveCursorToEndInInternalEditor_enabled = show;
            m_selectAllInInternalEditor_enabled = show;
            m_setValueInInternalEditor_enabled = show;
            if (recordToHighlight == -1) {
                recordToHighlight = qMax(popup()->tableView()->highlightedRecordNumber(), 0);
                setValueInInternalEditor(QVariant());
            }
            popup()->tableView()->selectRecord(recordToHighlight);
            popup()->tableView()->setHighlightedRecordNumber(recordToHighlight);
            popup()->tableView()->ensureCellVisible(-1, 0); // scroll to left as expected

            /*-->*/ m_moveCursorToEndInInternalEditor_enabled = true;
            m_selectAllInInternalEditor_enabled = true;
            m_setValueInInternalEditor_enabled = true;
        }
    }

    if (show) {
        moveCursorToEndInInternalEditor();
        selectAllInInternalEditor();
        if (m_focusPopupBeforeShow) {
            widgetToFocus->setFocus();
        }
        popup()->show();
        popup()->raise();
        popup()->repaint();
        if (!m_focusPopupBeforeShow) {
            widgetToFocus->setFocus();
        }
    }
    m_insideCreatePopup = false;
}

void KexiComboBoxBase::hide()
{
    if (popup())
        popup()->hide();
}

void KexiComboBoxBase::slotRecordAccepted(KDbRecordData* data, int record)
{
    Q_UNUSED(record);
    //update our value
    //..nothing to do?
    updateButton();
    slotRecordSelected(data);
    /*emit*/acceptRequested();
}

void KexiComboBoxBase::acceptPopupSelection()
{
    if (!popup())
        return;
    KDbRecordData *data = popup()->tableView()->highlightedRecord();
    if (data) {
        popup()->tableView()->selectRecord(popup()->tableView()->highlightedRecordNumber());
        slotRecordAccepted(data, -1);
    }
    popup()->hide();
}

void KexiComboBoxBase::slotRecordSelected(KDbRecordData*)
{
    //qDebug() << "m_visibleValue=" << m_visibleValue;

    QVariant valueToSet;
    KDbTableViewData *relData = column() ? column()->relatedData() : 0;
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();

    m_visibleValue = lookupFieldSchema ? visibleValueForLookupField() : QVariant();

    if (relData) {
        //use 'related table data' model
        KDbRecordData *data = popup()->tableView()->selectedRecord();
        if (data)
            valueToSet = data->at(1);
    } else if (lookupFieldSchema) {
        KDbRecordData *data = popup()->tableView()->selectedRecord();
        const int visibleColumn = visibleColumnIndex();
        if (data && visibleColumn != -1 /* && (int)item->size() >= visibleColumn --already checked*/) {
            valueToSet = data->at(qMin(visibleColumn, data->count() - 1)/*sanity*/);
        }
    } else {
        //use 'enum hints' model
        valueToSet = field()->enumHint(popup()->tableView()->currentRecord());
        if (valueToSet.toString().isEmpty() && !m_insideCreatePopup) {
            clear();
            QWidget* thisWidget = dynamic_cast<QWidget*>(this);
            thisWidget->parentWidget()->setFocus();
            return;
        }
    }
    setValueOrTextInInternalEditor(valueToSet);
    QWidget* thisWidget = dynamic_cast<QWidget*>(this);
    thisWidget->setFocus();
    if (m_setValueOrTextInInternalEditor_enabled) {
        moveCursorToEndInInternalEditor();
        selectAllInInternalEditor();
    }
    // a new (temp) popup table index is selected: do not update selection next time:
    m_updatePopupSelectionOnShow = false;
}

void KexiComboBoxBase::slotInternalEditorValueChanged(const QVariant& v)
{
    if (!m_slotInternalEditorValueChanged_enabled)
        return;
    m_userEnteredValue = v;
    m_internalEditorValueChanged = true;
    if (v.toString().isEmpty()) {
        if (popup()) {
            popup()->tableView()->clearSelection();
        }
        return;
    }
}

void KexiComboBoxBase::setValueOrTextInInternalEditor(const QVariant& value)
{
    if (!m_setValueOrTextInInternalEditor_enabled)
        return;
    setValueInInternalEditor(value);
    //this text is not entered by hand:
    m_userEnteredValue = QVariant();
    m_internalEditorValueChanged = false;
}

bool KexiComboBoxBase::handleKeyPressForPopup(QKeyEvent *ke)
{
    const int k = ke->key();
    int highlightedOrSelectedRecord = popup() ? popup()->tableView()->highlightedRecordNumber() : -1;
    if (popup() && highlightedOrSelectedRecord < 0)
        highlightedOrSelectedRecord = popup()->tableView()->currentRecord();

    const bool enterPressed = k == Qt::Key_Enter || k == Qt::Key_Return;

    // The editor may be active but the pull down menu not existent/visible,
    // e.g. when the user has pressed a normal button to activate the editor
    // Don't handle the event here in that case.
    if (!popup() || (!enterPressed && !popup()->isVisible())) {
        return false;
    }

    switch (k) {
    case Qt::Key_Up:
        popup()->tableView()->setHighlightedRecordNumber(
            qMax(highlightedOrSelectedRecord - 1, 0));
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_Down:
        popup()->tableView()->setHighlightedRecordNumber(
            qMin(highlightedOrSelectedRecord + 1, popup()->tableView()->recordCount() - 1));
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_PageUp:
        popup()->tableView()->setHighlightedRecordNumber(
            qMax(highlightedOrSelectedRecord - popup()->tableView()->recordsPerPage(), 0));
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_PageDown:
        popup()->tableView()->setHighlightedRecordNumber(
            qMin(highlightedOrSelectedRecord + popup()->tableView()->recordsPerPage(),
                 popup()->tableView()->recordCount() - 1));
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_Home:
        popup()->tableView()->setHighlightedRecordNumber(0);
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_End:
        popup()->tableView()->setHighlightedRecordNumber(popup()->tableView()->recordCount() - 1);
        updateTextForHighlightedRecord();
        return true;
    case Qt::Key_Enter:
    case Qt::Key_Return: //accept
        //select record that is highlighted
        if (popup()->tableView()->highlightedRecordNumber() >= 0) {
            popup()->tableView()->selectRecord(popup()->tableView()->highlightedRecordNumber());
            acceptPopupSelection();
            return true;
        }
    default: ;
    }
    return false;
}

void KexiComboBoxBase::updateTextForHighlightedRecord()
{
    KDbRecordData* data = popup() ? popup()->tableView()->highlightedRecord() : 0;
    if (data)
        slotRecordSelected(data);
}

void KexiComboBoxBase::undoChanges()
{
    KDbLookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
    if (lookupFieldSchema) {
//  qDebug() << "m_visibleValue BEFORE=" << m_visibleValue;
        if (popup())
            popup()->tableView()->selectRecord(popup()->tableView()->highlightedRecordNumber());
        m_visibleValue = visibleValueForLookupField();
//  qDebug() << "m_visibleValue AFTER=" << m_visibleValue;
        setValueOrTextInInternalEditor(m_visibleValue);
    }
}
