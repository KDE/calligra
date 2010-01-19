/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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
#include <KIconEffect>

#include <QMenu>
#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>

#include <kexiutils/utils.h>
#include <kexiutils/styleproxy.h>
#include <kexidb/queryschema.h>
#include <kexidb/fieldvalidator.h>
#include <kexiutils/utils.h>

//! @todo reenable as an app aption
//#define USE_KLineEdit_setReadOnly

//! @internal A validator used for read only flag to disable editing
class KexiDBLineEdit_ReadOnlyValidator : public QValidator
{
public:
    KexiDBLineEdit_ReadOnlyValidator(QObject * parent)
            : QValidator(parent) {
    }
    ~KexiDBLineEdit_ReadOnlyValidator() {}
    virtual State validate(QString &, int &) const {
        return Invalid;
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

    void setIndent(int indent) {
        this->indent = indent;
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
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
{
#ifdef USE_KLineEdit_setReadOnly
//! @todo reenable as an app aption
    QPalette p(widget->palette());
    p.setColor(KexiFormUtils::lighterGrayBackgroundColor(palette()));
    widget->setPalette(p);
#endif

    QFont tmpFont;
    tmpFont.setPointSize(KGlobalSettings::smallestReadableFont().pointSize());
    setMinimumHeight(QFontMetrics(tmpFont).height() + 6);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));

    KexiDBLineEditStyle *ks = new KexiDBLineEditStyle(style());
    ks->setParent(this);
    ks->setIndent(KexiFormUtils::dataSourceTagIcon().width());
    setStyle( ks );
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
    setText(displayText);
}

void KexiDBLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
    m_slotTextChanged_enabled = false;
    setText( m_textFormatter.valueToText(removeOld ? QVariant() : m_origValue, add.toString()) );
    setCursorPosition(0); //ok?
    m_slotTextChanged_enabled = true;
}

QVariant KexiDBLineEdit::value()
{
    return m_textFormatter.textToValue( text() );
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
    if (!m_slotTextChanged_enabled)
        return;
    signalValueChanged();
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

void KexiDBLineEdit::setReadOnly(bool readOnly)
{
#ifdef USE_KLineEdit_setReadOnly
//! @todo reenable as an app aption
    return KLineEdit::setReadOnly(readOnly);
#else
    m_internalReadOnly = readOnly;
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
    m_menuExtender.updatePopupMenuActions();
#endif
}

void KexiDBLineEdit::slotReadWriteValidatorDestroyed(QObject*)
{
    m_readWriteValidator = 0;
}

QMenu * KexiDBLineEdit::createPopupMenu()
{
    QMenu *contextMenu = KLineEdit::createStandardContextMenu();
    m_menuExtender.createTitle(contextMenu);
    return contextMenu;
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
    setValidator(new KexiDB::FieldValidator(*cinfo->field, this));

    const QString inputMask(m_textFormatter.inputMask());
    if (!inputMask.isEmpty())
        setInputMask(inputMask);

    KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
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
