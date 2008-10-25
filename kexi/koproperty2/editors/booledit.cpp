/* This file is part of the KDE project
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2006-2008 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "booledit.h"
#include "Property.h"
#include "EditorDataModel.h"

#include <KIconLoader>
#include <KLocale>
#include <KComboBox>
#include <KDebug>
#include <KColorScheme>
#include <KGlobal>

#include <QApplication>
#include <QPainter>
#include <QVariant>
#include <QLayout>
#include <QBitmap>

class BoolEditGlobal
{
public:
    BoolEditGlobal()
        : yesIcon(SmallIcon("dialog-ok"))
        , noIcon(SmallIcon("button_no"))
    {
    }
    QPixmap yesIcon;
    QPixmap noIcon;
};

K_GLOBAL_STATIC(BoolEditGlobal, g_boolEdit)

BoolEdit::BoolEdit(QWidget *parent)
    : QToolButton(parent)
{
    setFocusPolicy(Qt::WheelFocus);
    setCheckable(true);
//    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setAutoFillBackground(true);
//    setFlat(false);
//    setStyle(qApp->style());
//    setPalette(qApp->palette());
//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //we're not using layout to because of problems with button size
//    m_toggle->move(0, 0);
//    m_toggle->resize(width(), height());
/*    KColorScheme cs(QPalette::Active);
    QColor focus = cs.decoration(KColorScheme::FocusColor).color();
    setStyleSheet(
        QString::fromLatin1("QToolButton { "
        "border: 1px solid %1; "
        "border-radius: 0px; "
        "padding: 0 0px; }").arg(focus.name())); */

//    setFocusWidget(m_toggle);
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged(bool)));
}

BoolEdit::~BoolEdit()
{
}

bool BoolEdit::value() const
{
    return isChecked();
}

void BoolEdit::setValue(bool value)
{
//    m_toggle->blockSignals(true);
    setChecked(value);
//    setState(value);
//    m_toggle->blockSignals(false);
//    if (emitChange)
//        emit valueChanged(this);
}

void
BoolEdit::slotValueChanged(bool state)
{
//    setState(state);
////    emit valueChanged(this);
}

void BoolEdit::draw(QPainter *p, const QRect &r, const QVariant &value,
                 const QString& nullText)
{
//    p->eraseRect(r);
    QRect r2(r);
    r2.moveLeft(r2.left() + KIconLoader::SizeSmall + 6);

    if (value.isNull() && !nullText.isEmpty()) {
        p->drawText(r2, Qt::AlignVCenter | Qt::AlignLeft, nullText);
    } else {
        const QPixmap icon( value.toBool() ? g_boolEdit->yesIcon : g_boolEdit->noIcon );
        const QString text( value.toBool() ? i18n("Yes") : i18n("No") );
        p->drawPixmap(
            r.left() + 3,
            r2.top() + (r2.height() - 1 - KIconLoader::SizeSmall) / 2,
            icon);
        p->drawText(
            r2,
            Qt::AlignVCenter | Qt::AlignLeft,
            text);
    }
}

void BoolEdit::paintEvent( QPaintEvent * event )
{
    QToolButton::paintEvent(event);
    QPainter p(this);
    BoolEdit::draw(&p, rect(), value(), QString::null);
}

/*void
BoolEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    drawViewerInternal(p, r, value, m_yesIcon, m_noIcon, "");
}*/

/*
void BoolEdit::setState(bool state)
{
    if (state) {
        setIcon(KIcon("dialog-ok")); //QIcon(g_boolEdit->yesIcon));
        setText(i18n("Yes"));
    } else {
        setIcon(KIcon("button_no")); //QIcon(g_boolEdit->noIcon));
        setText(i18n("No"));
    }
}
*/
/*void BoolEdit::resizeEvent(QResizeEvent *ev)
{
    m_toggle->resize(ev->size());
}*/

bool BoolEdit::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent* ev = static_cast<QKeyEvent*>(e);
        const int k = ev->key();
        if (k == Qt::Key_Space || k == Qt::Key_Enter || k == Qt::Key_Return) {
//            if (m_toggle)
                toggle();
            return true;
        }
    }
    return QToolButton::eventFilter(watched, e);
}

/*void
BoolEdit::setReadOnlyInternal(bool readOnly)
{
    setVisibleFlag(!readOnly);
}*/

//--------------------------------------------------

#if 0 //TODO
ThreeStateBoolEdit::ThreeStateBoolEdit(Property *property, QWidget *parent)
        : ComboBox(property, parent)
        , m_yesIcon(SmallIcon("dialog-ok"))
        , m_noIcon(SmallIcon("button_no"))
{
    m_edit->addItem(m_yesIcon, i18n("Yes"));
    m_edit->addItem(m_noIcon, i18n("No"));
    QVariant thirdState = property ? property->option("3rdState") : QVariant();
    QPixmap nullIcon(m_yesIcon.size());   //transparent pixmap of appropriate size
    nullIcon.fill(Qt::transparent);
    m_edit->addItem(nullIcon, thirdState.toString().isEmpty() ? i18n("None") : thirdState.toString());
}

ThreeStateBoolEdit::~ThreeStateBoolEdit()
{
}

QVariant
ThreeStateBoolEdit::value() const
{
    // list items: true, false, NULL
    const int idx = m_edit->currentIndex();
    if (idx == 0)
        return QVariant(true);
    else
        return idx == 1 ? QVariant(false) : QVariant();
}

void
ThreeStateBoolEdit::setProperty(Property *prop)
{
    m_setValueEnabled = false; //setValue() couldn't be called before fillBox()
    Widget::setProperty(prop);
    m_setValueEnabled = true;
    if (prop)
        setValue(prop->value(), false); //now the value can be set
}

void
ThreeStateBoolEdit::setValue(const QVariant &value, bool emitChange)
{
    if (!m_setValueEnabled)
        return;

    if (value.isNull())
        m_edit->setCurrentIndex(2);
    else
        m_edit->setCurrentIndex(value.toBool() ? 0 : 1);

    if (emitChange)
        emit valueChanged(this);
}

void
ThreeStateBoolEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    Q_UNUSED(cg);
    drawViewerInternal(p, r, value, m_yesIcon, m_noIcon, m_edit->itemText(2));
}
#endif

//---------------

BoolDelegate::BoolDelegate()
{
    options.removeBorders = false;
}

QWidget * BoolDelegate::createEditor( int type, QWidget *parent, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return new BoolEdit(parent);
}

void BoolDelegate::paint( QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
//    const EditorDataModel *editorModel = dynamic_cast<const EditorDataModel*>(index.model());
//    Property *prop = editorModel->propertyForItem(index);
    BoolEdit::draw(painter, option.rect, index.data(Qt::EditRole), QString());
    painter->restore();
}

#include "booledit.moc"
