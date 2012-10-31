/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#include "kexidblineedit.h"
#include "kexidbautofield.h"

#include <KDebug>
#include <KNumInput>
#include <KDateTable>

#include <QMenu>
#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>

#include <kexiutils/utils.h>
#include <kexiutils/styleproxy.h>
#include <kexidb/fieldvalidator.h>
#include <db/queryschema.h>
#include <db/utils.h>

//! @internal A validator used for read only flag to disable editing
class KexiDBLineEdit_ReadOnlyValidator : public QValidator
{
public:
    KexiDBLineEdit_ReadOnlyValidator(QObject * parent)
            : QValidator(parent) {
    }
    ~KexiDBLineEdit_ReadOnlyValidator() {}
    virtual State validate(QString &input, int &pos) const {
        input = qobject_cast<KexiDBLineEdit*>(parent())->originalText();
        pos = qobject_cast<KexiDBLineEdit*>(parent())->originalCursorPosition();
        return Intermediate;
    }
};

//-----

//! A style proxy overriding KexiDBLineEdit style
class KexiDBLineEditStyle : public KexiUtils::StyleProxy
{
public:
    KexiDBLineEditStyle(QStyle* parentStyle) : KexiUtils::StyleProxy(parentStyle), indent(0)
    {
    }
    virtual ~KexiDBLineEditStyle() {
    }

    void setIndent(int indent) {
        this->indent = indent;
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const
    {
        const KFormDesigner::FormWidgetInterface *formWidget = dynamic_cast<const KFormDesigner::FormWidgetInterface*>(widget);
        if (formWidget->designMode()) {
            const KexiFormDataItemInterface *dataItemIface = dynamic_cast<const KexiFormDataItemInterface*>(widget);
            if (dataItemIface && !dataItemIface->dataSource().isEmpty() && !formWidget->editingMode()) {
                if (element == SE_LineEditContents) {
                    QRect rect = KexiUtils::StyleProxy::subElementRect(SE_LineEditContents, option, widget);
                    if (option->direction == Qt::LeftToRight)
                        return rect.adjusted(indent, 0, 0, 0);
                    else
                        return rect.adjusted(0, 0, -indent, 0);
                }
            }
        }
        return KexiUtils::StyleProxy::subElementRect(element, option, widget);
    }
    int indent;
};

//-----

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent)
        : KLineEdit(parent)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , m_readWriteValidator(0)
        , m_menuExtender(this, this)
        , m_internalReadOnly(false)
        , m_slotTextChanged_enabled(true)
        , m_paletteChangeEvent_enabled(true)
        , m_inStyleChangeEvent(false)
{
    QFont tmpFont;
    tmpFont.setPointSize(KGlobalSettings::smallestReadableFont().pointSize());
    setMinimumHeight(QFontMetrics(tmpFont).height() + 6);
    m_originalPalette = palette();
    connect(this, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(this, SIGNAL(cursorPositionChanged(int,int)),
            this, SLOT(slotCursorPositionChanged(int,int)));

    m_internalStyle = new KexiDBLineEditStyle(style());
    m_internalStyle->setParent(this);
    m_internalStyle->setIndent(KexiFormUtils::dataSourceTagIcon().width());
    m_inStyleChangeEvent = true; // do not allow KLineEdit::event() to touch the style
    setStyle(m_internalStyle);
    m_inStyleChangeEvent = false;
}

KexiDBLineEdit::~KexiDBLineEdit()
{
}

/*
void FormWidgetInterface::setDesignMode(bool design)
{
    FormWidgetInterface::setDesignMode(design);
    setCursor(design ? QCursor(Qt::ArrowCursor) : QCursor());
}*/

void KexiDBLineEdit::setInvalidState(const QString& displayText)
{
    KLineEdit::setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
    if (focusPolicy() & Qt::TabFocus)
        setFocusPolicy(Qt::ClickFocus);
    setValueInternal(displayText, true);
}

void KexiDBLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
    m_slotTextChanged_enabled = false;
    m_originalText = m_textFormatter.toString(removeOld ? QVariant() : m_origValue, add.toString());
    setText(m_originalText);
    setCursorPosition(0); //ok?
    m_slotTextChanged_enabled = true;
}

QVariant KexiDBLineEdit::value()
{
    return m_textFormatter.fromString(text());
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
    if (!m_slotTextChanged_enabled)
        return;
    signalValueChanged();
}

void KexiDBLineEdit::slotCursorPositionChanged(int oldPos, int newPos)
{
    Q_UNUSED(oldPos);
    if (m_originalText == text()) {
        // when cursor was moved without altering the text, remember its position,
        // otherwise the change will be reverted by the validator
        m_cursorPosition = newPos;
    }
}

int KexiDBLineEdit::originalCursorPosition() const
{
    return m_cursorPosition;
}

bool KexiDBLineEdit::valueIsNull()
{
    return valueIsEmpty(); //ok??? text().isNull();
}

bool KexiDBLineEdit::valueIsEmpty()
{
    return m_textFormatter.valueIsEmpty( text() );
}

bool KexiDBLineEdit::valueIsValid()
{
    return m_textFormatter.valueIsValid( text() );
}

bool KexiDBLineEdit::isReadOnly() const
{
    return m_internalReadOnly;
}

void KexiDBLineEdit::updatePalette()
{
    m_paletteChangeEvent_enabled = false;
    setPalette(m_internalReadOnly ?
               KexiUtils::paletteForReadOnly(m_originalPalette)
              : m_originalPalette);
    m_paletteChangeEvent_enabled = true;
}

void KexiDBLineEdit::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange && m_paletteChangeEvent_enabled) {
        m_originalPalette = palette();
        updatePalette();
    }
    KLineEdit::changeEvent(e);
}

void KexiDBLineEdit::setReadOnly(bool readOnly)
{
    m_internalReadOnly = readOnly;
    updatePalette();
    if (!designMode()) {
        if (m_internalReadOnly) {
            if (m_readWriteValidator)
                disconnect(m_readWriteValidator, SIGNAL(destroyed(QObject*)),
                           this, SLOT(slotReadWriteValidatorDestroyed(QObject*)));
            m_readWriteValidator = validator();
            if (m_readWriteValidator)
                connect(m_readWriteValidator, SIGNAL(destroyed(QObject*)),
                        this, SLOT(slotReadWriteValidatorDestroyed(QObject*)));
            if (!m_readOnlyValidator)
                m_readOnlyValidator = new KexiDBLineEdit_ReadOnlyValidator(this);
            setValidator(m_readOnlyValidator);
        } else {
            //revert to r/w validator
            setValidator(m_readWriteValidator);
        }
    }
}

void KexiDBLineEdit::slotReadWriteValidatorDestroyed(QObject*)
{
    m_readWriteValidator = 0;
}

void KexiDBLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = createStandardContextMenu();
    m_menuExtender.exec(menu, e->globalPos());
    delete menu;
}

QWidget* KexiDBLineEdit::widget()
{
    return this;
}

bool KexiDBLineEdit::cursorAtStart()
{
    return cursorPosition() == 0;
}

bool KexiDBLineEdit::cursorAtEnd()
{
    return cursorPosition() == (int)text().length();
}

void KexiDBLineEdit::clear()
{
    if (!m_internalReadOnly)
        KLineEdit::clear();
}

void KexiDBLineEdit::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    m_textFormatter.setField( cinfo ? cinfo->field : 0 );

    if (!cinfo)
        return;

//! @todo handle input mask (via QLineEdit::setInputMask()) using a special KexiDB::FieldInputMask class
    delete m_readWriteValidator;
    KexiDB::FieldValidator* fieldValidator = new KexiDB::FieldValidator(*cinfo->field, this);
    if (m_internalReadOnly) {
        m_readWriteValidator = fieldValidator;
    }
    else {
        setValidator(fieldValidator);
    }

    const QString inputMask(m_textFormatter.inputMask());
    if (!inputMask.isEmpty())
        setInputMask(inputMask);

    KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);

    if (cinfo->field->isTextType()) {
        if (!designMode()) {
            if (cinfo->field->maxLength() > 0) {
                setMaxLength(cinfo->field->maxLength());
            }
        }
    }
}

/*todo
void KexiDBLineEdit::paint( QPainter *p )
{
  KexiDBTextWidgetInterface::paint( this, &p, text().isEmpty(), alignment(), hasFocus() );
}*/

void KexiDBLineEdit::paintEvent(QPaintEvent *pe)
{
    KLineEdit::paintEvent(pe);
    KFormDesigner::FormWidgetInterface *formWidget = dynamic_cast<KFormDesigner::FormWidgetInterface*>(this);
    if (formWidget->designMode()) {
        KexiFormDataItemInterface *dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(this);
        if (dataItemIface && !dataItemIface->dataSource().isEmpty() && !formWidget->editingMode()) {
            // draw "data source tag" icon
            QPainter p(this);
            QStyleOptionFrameV2 option;
            initStyleOption(&option);
            
            int leftMargin, topMargin, rightMargin, bottomMargin;
            getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
            QRect r( style()->subElementRect(QStyle::SE_LineEditContents, &option, this) );
            r.setX(r.x() + leftMargin);
            r.setY(r.y() + topMargin);
            r.setRight(r.right() - rightMargin);
            r.setBottom(r.bottom() - bottomMargin);
            QPixmap dataSourceTagIcon;
            int x;
            if (layoutDirection() == Qt::LeftToRight) {
                dataSourceTagIcon = KexiFormUtils::dataSourceTagIcon();
                x = r.left() - dataSourceTagIcon.width() + 2;
            }
            else {
                dataSourceTagIcon = KexiFormUtils::dataSourceRTLTagIcon();
                x = r.right() - 2;
            }
            p.drawPixmap(
                x, r.top() + (r.height() - dataSourceTagIcon.height()) / 2,
                dataSourceTagIcon
            );
        }
    }
}

bool KexiDBLineEdit::event(QEvent * e)
{
    if (e->type() == QEvent::StyleChange) {
        if (m_inStyleChangeEvent) {
            return true;
        }
        // let the KLineEdit set its KLineEditStyle
        if (!KLineEdit::event(e)) {
            return false;
        }
        // move the KLineEditStyle inside our internal style as parent
        m_internalStyle->setParent(style());
        m_inStyleChangeEvent = true; // avoid recursion
        setStyle(m_internalStyle);
        m_inStyleChangeEvent = false;
        return true;
    }

    const bool ret = KLineEdit::event(e);
    KexiDBTextWidgetInterface::event(e, this, text().isEmpty());

    if (e->type() == QEvent::FocusOut) {
        QFocusEvent *fe = static_cast<QFocusEvent *>(e);
        if (fe->reason() == Qt::TabFocusReason || fe->reason() == Qt::BacktabFocusReason) {
            //display aligned to left after loosing the focus (only if this is tab/backtab event)
//! @todo add option to set cursor at the beginning
            setCursorPosition(0); //ok?
        }
    }
    return ret;
}

bool KexiDBLineEdit::appendStretchRequired(KexiDBAutoField* autoField) const
{
    return KexiDBAutoField::Top == autoField->labelPosition();
}

void KexiDBLineEdit::handleAction(const QString& actionName)
{
    if (actionName == "edit_copy") {
        copy();
    } else if (actionName == "edit_paste") {
        paste();
    } else if (actionName == "edit_cut") {
        cut();
    }
    //! @todo ?
}

void KexiDBLineEdit::setDisplayDefaultValue(QWidget *widget, bool displayDefaultValue)
{
    KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
    // initialize display parameters for default / entered value
    KexiDisplayUtils::DisplayParameters * const params = displayDefaultValue
        ? m_displayParametersForDefaultValue : m_displayParametersForEnteredValue;
    setFont(params->font);
    QPalette pal(palette());
    pal.setColor(QPalette::Active, QColorGroup::Text, params->textColor);
    setPalette(pal);
}

void KexiDBLineEdit::undo()
{
    cancelEditor();
}

void KexiDBLineEdit::moveCursorToEnd()
{
    KLineEdit::end(false/*!mark*/);
}

void KexiDBLineEdit::moveCursorToStart()
{
    KLineEdit::home(false/*!mark*/);
}

void KexiDBLineEdit::selectAll()
{
    KLineEdit::selectAll();
}

bool KexiDBLineEdit::keyPressed(QKeyEvent *ke)
{
    Q_UNUSED(ke);
    return false;
}

void KexiDBLineEdit::updateTextForDataSource()
{
    if (!designMode())
        return;
    setText(dataSource());
}

void KexiDBLineEdit::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    updateTextForDataSource();
}

void KexiDBLineEdit::setDataSourcePartClass(const QString &partClass)
{
    KexiFormDataItemInterface::setDataSourcePartClass(partClass);
    updateTextForDataSource();
}

#include "kexidblineedit.moc"
