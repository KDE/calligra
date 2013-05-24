/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbcombobox.h"
#include "kexidblineedit.h"
#include "../kexiformscrollview.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <kapplication.h>

#include <QMetaObject>
#include <QPainter>
#include <QStyle>
#include <qdrawutil.h>
#include <QCursor>
#include <QList>

#include <db/queryschema.h>
#include <widget/tableview/kexicomboboxpopup.h>
#include <widget/tableview/kexicelleditorfactory.h>
#include <kexiutils/utils.h>

//! @internal
class KexiDBComboBox::Private
{
public:
    Private()
            : popup(0)
            , visibleColumnInfo(0)
            , isEditable(false)
            , buttonPressed(false)
            , mouseOver(false)
            , dataEnteredByHand(true) {
    }
    ~Private() {
    }

    KexiComboBoxPopup *popup;
    KComboBox *paintedCombo; //!< fake combo used only to pass it as 'this' for QStyle (because styles use <static_cast>)
    QSize sizeHint; //!< A cache for KexiDBComboBox::sizeHint(),
    //!< rebuilt by KexiDBComboBox::fontChange() and KexiDBComboBox::styleChange()
    KexiDB::QueryColumnInfo* visibleColumnInfo;
    //! used for collecting subwidgets and their childrens (if isEditable is false)
    QList<QWidget*> subWidgetsWithDisabledEvents;
    bool isEditable : 1; //!< true is the combo box is editable
    bool buttonPressed : 1;
    bool mouseOver : 1;
    bool dataEnteredByHand : 1;
//2.0    bool designMode : 1;
};

//-------------------------------------

KexiDBComboBox::KexiDBComboBox(QWidget *parent)
        : KexiDBAutoField(parent, NoLabel)
        , KexiComboBoxBase()
        , d(new Private())
{
#ifdef __GNUC__
#warning TODO fix creating popup for forms instead; remove KexiComboBoxBase::m_setReinstantiatePopupOnShow
#else
#pragma WARNING( fix creating popup for forms instead; remove KexiComboBoxBase::m_setReinstantiatePopupOnShow )
#endif
    m_reinstantiatePopupOnShow = true;

    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    installEventFilter(this);
//2.0    d->designMode = designMode;
    d->paintedCombo = new KComboBox(this);
    d->paintedCombo->hide();
    d->paintedCombo->move(0, 0);
}

KexiDBComboBox::~KexiDBComboBox()
{
    delete d;
}

KexiComboBoxPopup *KexiDBComboBox::popup() const
{
    return d->popup;
}

void KexiDBComboBox::setPopup(KexiComboBoxPopup *popup)
{
    d->popup = popup;
}

void KexiDBComboBox::setEditable(bool set)
{
    if (d->isEditable == set)
        return;
    d->isEditable = set;
    d->paintedCombo->setEditable(set);
    if (set)
        createEditor();
    else {
        delete subwidget();
        setSubwidget(0);
    }
    update();
}

bool KexiDBComboBox::isEditable() const
{
    return d->isEditable;
}

void KexiDBComboBox::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(palette().color(QPalette::Text));
//    QColorGroup cg(palette().active());
// if ( hasFocus() )
//  cg.setColor(QColorGroup::Base, cg.highlight());
// else
    QPalette pal(palette());
    pal.setColor(QColorGroup::Base, paletteBackgroundColor()); //update base color using (reimplemented) bg color

    if (width() < 5 || height() < 5) {
        qDrawShadePanel(&p, rect(), pal, false /* !sunken */,
                        2 /*line width*/, &pal.brush(QPalette::Button)/*fill*/);
        return;
    }

#ifdef __GNUC__
#warning TODO KexiDBComboBox::paintEvent()
#else
#pragma WARNING( TODO KexiDBComboBox::paintEvent() )
#endif

    QStyleOptionComboBox option;
    option.palette = pal;
    option.initFrom(d->paintedCombo);

    if (isEnabled())
        option.state |= QStyle::State_Enabled;
    if (hasFocus())
        option.state |= QStyle::State_HasFocus;
    if (d->mouseOver)
        option.state |= QStyle::State_MouseOver;

    style()->drawComplexControl(QStyle::CC_ComboBox, &option, &p, d->paintedCombo);

#if 0 //TODO
//! @todo support reverse layout
//bool reverse = QApplication::reverseLayout();
    style()->drawComplexControl(QStyle::CC_ComboBox, &option, &p, d->paintedCombo  /*this*/
                                flags, (uint)QStyle::SC_All,
                                (d->buttonPressed ? QStyle::SC_ComboBoxArrow : QStyle::SC_None)
                               );

    if (d->isEditable) {
        //if editable, editor paints itself, nothing to do
    } else { //not editable: we need to paint the current item
        QRect editorGeometry(this->editorGeometry());
        if (hasFocus()) {
            if (0 == qstrcmp(style()->name(), "windows")) //a hack
                p.fillRect(editorGeometry, cg.brush(QColorGroup::Highlight));
            QRect r(QStyle::visualRect(style()->subRect(QStyle::SR_ComboBoxFocusRect, d->paintedCombo), this));
            r = QRect(r.left() - 1, r.top() - 1, r.width() + 2, r.height() + 2); //enlare by 1 pixel each side to avoid covering by the subwidget
            style()->drawPrimitive(QStyle::PE_FocusRect, &p,
                                   r, cg, flags | QStyle::Style_FocusAtBorder, QStyleOption(cg.highlight()));
        }
        //todo
    }
#endif
}

QRect KexiDBComboBox::editorGeometry() const
{
#ifdef __GNUC__
#warning KexiDBComboBox::editorGeometry() OK?
#else
#pragma WARNING( KexiDBComboBox::editorGeometry() OK? )
#endif

#if 0 //20080316, sebsauer; crashes here with;
    QRect r(QStyle::visualRect(
                qApp->layoutDirection(),
                d->paintedCombo->geometry(),
                style()->subControlRect(QStyle::CC_ComboBox, 0, QStyle::SC_ComboBoxEditField, d->paintedCombo)));
#else
    QRect r = d->paintedCombo->geometry();
#endif

    //if ((height()-r.bottom())<6)
    // r.setBottom(height()-6);
    return r;
}

void KexiDBComboBox::createEditor()
{
    KexiDBAutoField::createEditor();
    if (subwidget()) {
        subwidget()->setGeometry(editorGeometry());
        if (!d->isEditable) {
            QStyleOptionComboBox option;
            option.initFrom(subwidget());
            const QRect comboRect = subwidget()->style()->subControlRect(
                QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, subwidget());
            kDebug() << "comboRect:" << comboRect;
            subwidget()->setContentsMargins(comboRect.left(), comboRect.top(),
                width() - comboRect.right(), height() - comboRect.bottom());
            int l, t, r, b;
            subwidget()->getContentsMargins(&l, &t, &r, &b);
            kDebug() << "altered margins:" << l << t << r << b;

            subwidget()->setFocusPolicy(Qt::NoFocus);
            subwidget()->setCursor(QCursor(Qt::ArrowCursor)); // widgets like listedit have IbeamCursor, we don't want that
            QPalette subwidgetPalette(subwidget()->palette());
            subwidgetPalette.setColor(QColorGroup::Base, Qt::transparent);
            subwidget()->setPalette(subwidgetPalette);
            d->subWidgetsWithDisabledEvents.clear();
            d->subWidgetsWithDisabledEvents << subwidget();
            subwidget()->installEventFilter(this);
            QList<QWidget*> widgets(subwidget()->findChildren<QWidget*>());
            foreach(QWidget *widget, widgets) {
                d->subWidgetsWithDisabledEvents << widget;
                widget->installEventFilter(this);
            }
        }
    }
    updateGeometry();
}

void KexiDBComboBox::setLabelPosition(LabelPosition position)
{
    if (subwidget()) {
        if (-1 != subwidget()->metaObject()->indexOfProperty("frameShape")) {
            subwidget()->setProperty("frameShape", QVariant((int)QFrame::NoFrame));
        }
        subwidget()->setGeometry(editorGeometry());
    }
//  KexiSubwidgetInterface *subwidgetInterface = dynamic_cast<KexiSubwidgetInterface*>((QWidget*)m_subwidget);
    // update size policy
//  if (subwidgetInterface && subwidgetInterface->subwidgetStretchRequired(this)) {
    QSizePolicy sizePolicy(this->sizePolicy());
    if (position == Left)
        sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    else
        sizePolicy.setVerticalPolicy(QSizePolicy::Minimum);
    //m_subwidget->setSizePolicy(sizePolicy);
    setSizePolicy(sizePolicy);
    //}
// }
}

QRect KexiDBComboBox::buttonGeometry() const
{
    QRect arrowRect(
        style()->subControlRect(
            QStyle::CC_ComboBox, 0, QStyle::SC_ComboBoxArrow, d->paintedCombo));
#ifdef __GNUC__
#warning KexiDBComboBox::buttonGeometry() OK?
#else
#pragma WARNING( KexiDBComboBox::buttonGeometry() OK? )
#endif
    arrowRect = QStyle::visualRect(
                    qApp->layoutDirection(), d->paintedCombo->geometry(), arrowRect);
    arrowRect.setHeight(qMax(height() - (2 * arrowRect.y()), arrowRect.height()));      // a fix for Motif style
    return arrowRect;
}

bool KexiDBComboBox::handleMousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton || designMode())
        return true;
    /*todo if ( m_discardNextMousePress ) {
        d->discardNextMousePress = FALSE;
        return;
        }*/

    if (/*count() &&*/ (!isEditable() || buttonGeometry().contains(e->pos()))) {
        d->buttonPressed = false;

        /* if ( d->usingListBox() ) {
              listBox()->blockSignals( TRUE );
              qApp->sendEvent( listBox(), e ); // trigger the listbox's autoscroll
              listBox()->setCurrentItem(d->current);
              listBox()->blockSignals( FALSE );
              popup();
              if ( arrowRect.contains( e->pos() ) ) {
            d->arrowPressed = TRUE;
            d->arrowDown    = TRUE;
            repaint( FALSE );
              }
          } else {*/
        showPopup();
        return true;
    }
    return false;
}

bool KexiDBComboBox::handleKeyPressEvent(QKeyEvent *ke)
{
    const int k = ke->key();
    const bool dropDown = (ke->modifiers() == Qt::NoModifier && ((k == Qt::Key_F2 && !d->isEditable) || k == Qt::Key_F4))
                          || (ke->modifiers() == Qt::AltModifier && k == Qt::Key_Down);
    const bool escPressed = ke->modifiers() == Qt::NoModifier && k == Qt::Key_Escape;
    const bool popupVisible =  popup() && popup()->isVisible();
    if ((dropDown || escPressed) && popupVisible) {
        popup()->hide();
        return true;
    } else if (dropDown && !popupVisible) {
        d->buttonPressed = false;
        showPopup();
        return true;
    } else if (popupVisible) {
        const bool enterPressed = k == Qt::Key_Enter || k == Qt::Key_Return;
        if (enterPressed/* && m_internalEditorValueChanged*/) {
            acceptPopupSelection();
            return true;
        }
        return handleKeyPressForPopup(ke);
    }

    return false;
}

bool KexiDBComboBox::keyPressed(QKeyEvent *ke)
{
    if (KexiDBAutoField::keyPressed(ke))
        return true;

    const int k = ke->key();
    const bool popupVisible =  popup() && popup()->isVisible();
    const bool escPressed = ke->modifiers() == Qt::NoModifier && k == Qt::Key_Escape;
    if (escPressed && popupVisible) {
        popup()->hide();
        return true;
    }
    if (ke->modifiers() == Qt::NoModifier
            && (k == Qt::Key_PageDown || k == Qt::Key_PageUp)
            && popupVisible) {
        return true;
    }
    return false;
}

void KexiDBComboBox::mousePressEvent(QMouseEvent *e)
{
    if (handleMousePressEvent(e))
        return;

// QTimer::singleShot( 200, this, SLOT(internalClickTimeout()));
// d->shortClick = TRUE;
//  }
    KexiDBAutoField::mousePressEvent(e);
}

void KexiDBComboBox::mouseDoubleClickEvent(QMouseEvent *e)
{
    mousePressEvent(e);
}

bool KexiDBComboBox::eventFilter(QObject *o, QEvent *e)
{
    if (o == this) {
        if (e->type() == QEvent::Resize) {
            d->paintedCombo->resize(size());
            if (subwidget())
                subwidget()->setGeometry(editorGeometry());
        } else if (e->type() == QEvent::Enter) {
            if (!d->isEditable
                    || /*over button if editable combo*/buttonGeometry().contains(static_cast<QMouseEvent*>(e)->pos())) {
                d->mouseOver = true;
                update();
            }
        } else if (e->type() == QEvent::MouseMove) {
            if (d->isEditable) {
                const bool overButton = buttonGeometry().contains(static_cast<QMouseEvent*>(e)->pos());
                if (overButton != d->mouseOver) {
                    d->mouseOver = overButton;
                    update();
                }
            }
        } else if (e->type() == QEvent::Leave) {
            d->mouseOver = false;
            update();
        } else if (e->type() == QEvent::KeyPress) {
            // handle F2/F4
            if (handleKeyPressEvent(static_cast<QKeyEvent*>(e)))
                return true;
        } else if (e->type() == QEvent::FocusOut) {
            if (popup() && popup()->isVisible()) {
//                popup()->hide();
//                undoChanges();
            }
        }
    } else if (!d->isEditable && d->subWidgetsWithDisabledEvents.contains(dynamic_cast<QWidget*>(o))) {
        kDebug() << "**********************####" << e->type() << o;
        if (e->type() == QEvent::MouseButtonPress) {
            // clicking the subwidget should mean the same as clicking the combo box (i.e. show the popup)
            if (handleMousePressEvent(static_cast<QMouseEvent*>(e)))
                return true;
        } else if (e->type() == QEvent::KeyPress) {
            if (handleKeyPressEvent(static_cast<QKeyEvent*>(e)))
                return true;
        }
        if (e->type() != QEvent::Paint)
            return true;
    }
    return KexiDBAutoField::eventFilter(o, e);
}

bool KexiDBComboBox::subwidgetStretchRequired(KexiDBAutoField* autoField) const
{
    Q_UNUSED(autoField);
    return true;
}

void KexiDBComboBox::setPaletteBackgroundColor(const QColor & color)
{
    KexiDBAutoField::setPaletteBackgroundColor(color);
/*    QPalette pal(palette());
    QColorGroup cg(pal.active());
    pal.setActive(cg);
    QWidget::setPalette(pal);*/
    update();
}

bool KexiDBComboBox::valueChanged()
{
    kDebug() << KexiDataItemInterface::originalValue().toString() << " ? " << value().toString();
    return KexiDataItemInterface::originalValue() != value();
}

void
KexiDBComboBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
}

void KexiDBComboBox::setVisibleColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    d->visibleColumnInfo = cinfo;
    // we're assuming we already have columnInfo()
    setColumnInfoInternal(columnInfo(), d->visibleColumnInfo);
}

KexiDB::QueryColumnInfo* KexiDBComboBox::visibleColumnInfo() const
{
    return d->visibleColumnInfo;
}

void KexiDBComboBox::moveCursorToEndInInternalEditor()
{
    if (d->isEditable && m_moveCursorToEndInInternalEditor_enabled)
        moveCursorToEnd();
}

void KexiDBComboBox::selectAllInInternalEditor()
{
    if (d->isEditable && m_selectAllInInternalEditor_enabled)
        selectAll();
}

void KexiDBComboBox::setValueInternal(const QVariant& add, bool removeOld)
{
    //// use KexiDBAutoField instead of KexiComboBoxBase::setValueInternal
    //// expects existing popup(), but we want to have delayed creation
    if (popup())
        popup()->hide();
    KexiComboBoxBase::setValueInternal(add, removeOld);
}

void KexiDBComboBox::setVisibleValueInternal(const QVariant& value)
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->setValue(value, QVariant(), false /*!removeOld*/);
}

QVariant KexiDBComboBox::visibleValue()
{
    return KexiComboBoxBase::visibleValue();
}

void KexiDBComboBox::setValueInInternalEditor(const QVariant& value)
{
    if (!m_setValueInInternalEditor_enabled)
        return;
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->setValue(value, QVariant(), false/*!removeOld*/);
}

QVariant KexiDBComboBox::valueFromInternalEditor()
{
    return KexiDBAutoField::value();
}

QPoint KexiDBComboBox::mapFromParentToGlobal(const QPoint& pos) const
{
// const KexiFormScrollView* view = KexiUtils::findParentConst<const KexiFormScrollView>(this, "KexiFormScrollView");
    if (!parentWidget())
        return QPoint(-1, -1);
    return parentWidget()->mapToGlobal(pos);
// return view->viewport()->mapToGlobal(pos);
}

int KexiDBComboBox::popupWidthHint() const
{
    return width(); //popup() ? popup()->width() : 0;
}

void KexiDBComboBox::fontChange(const QFont & oldFont)
{
    d->sizeHint = QSize(); //force rebuild the cache
    KexiDBAutoField::fontChange(oldFont);
}

void KexiDBComboBox::styleChange(QStyle& oldStyle)
{
    KexiDBAutoField::styleChange(oldStyle);
    d->sizeHint = QSize(); //force rebuild the cache
    if (subwidget())
        subwidget()->setGeometry(editorGeometry());
}

QSize KexiDBComboBox::sizeHint() const
{
    if (isVisible() && d->sizeHint.isValid())
        return d->sizeHint;

    const int maxWidth = 7 * fontMetrics().width(QChar('x')) + 18;
    const int maxHeight = qMax(fontMetrics().lineSpacing(), 14) + 2;
    QStyleOptionComboBox option;
    option.initFrom(d->paintedCombo);
    d->sizeHint = (style()->sizeFromContents(
                       QStyle::CT_ComboBox, &option, QSize(maxWidth, maxHeight), d->paintedCombo).expandedTo(QApplication::globalStrut()));

    return d->sizeHint;
}

void KexiDBComboBox::editRequested()
{
}

void KexiDBComboBox::acceptRequested()
{
    signalValueChanged();
}

void KexiDBComboBox::slotRowAccepted(KexiDB::RecordData *record, int row)
{
    d->dataEnteredByHand = false;
    KexiComboBoxBase::slotRowAccepted(record, row);
    d->dataEnteredByHand = true;
}

void KexiDBComboBox::beforeSignalValueChanged()
{
    if (d->dataEnteredByHand) {
        KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
        if (iface) {
            slotInternalEditorValueChanged(iface->value());
        }
    }
}

void KexiDBComboBox::undoChanges()
{
    KexiDBAutoField::undoChanges();
    KexiComboBoxBase::undoChanges();
}

#include "kexidbcombobox.moc"
