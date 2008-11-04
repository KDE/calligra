/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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

#include "fontedit.h"

#include <KFontRequester>
#include <KLocale>
#include <KDebug>

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QModelIndex>
#include <QVariant>
#include <QStyleOptionViewItem>
#include <QFontDatabase>

using namespace KoProperty;

//! @internal
//! reimplemented to better button and label's positioning
class FontEditRequester : public KFontRequester
{
    Q_OBJECT
    Q_PROPERTY(QFont value READ value WRITE setValue USER true)
public:
    FontEditRequester(QWidget* parent)
            : KFontRequester(parent)
    {
//        setContentsMargins(0,0,0,0);
/*
        QPalette pal = label()->palette();
        pal.setColor(QPalette::Window, palette().color(QPalette::Active, QPalette::Base));
        label()->setPalette(pal);
        label()->setMinimumWidth(0);*/
//        label()->setFrameShape(QFrame::Box);
//        label()->setContentsMargins(0,0,0,0);
        label()->setFrameShape(QFrame::NoFrame);
        label()->setIndent(1);
        label()->setFocusPolicy(Qt::ClickFocus);
/*
        KAcceleratorManager::setNoAccel(label());
        layout()->removeWidget(label());
        layout()->removeWidget(button());//->reparent(this, 0, QPoint(0,0));
        delete layout();*/
        button()->setText(i18n("..."));
        button()->setToolTip(i18n("Change font"));
        button()->setWhatsThis(i18n("Changes font"));
        button()->setFocusPolicy(Qt::NoFocus);
        button()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QFontMetrics fm(button()->font());
        button()->setFixedWidth(fm.width(button()->text() + "  "));
    }
/*    virtual void resizeEvent(QResizeEvent *e)
    {
        KFontRequester::resizeEvent(e);
        label()->move(0, 0);
        label()->resize(e->size() - QSize(button()->width(), -1));
        button()->move(label()->width(), 0);
        button()->setFixedSize(button()->width(), height());
    }*/
    QFont value() const
    {
        return font();
    }

public slots:
    void setValue(const QFont& value)
    {
        kDebug() << QFontDatabase().families();
        setFont( value );
    }
};

// -----------

QWidget * FontDelegate::createEditor( int type, QWidget *parent, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return new FontEditRequester(parent);
}

void FontDelegate::paint( QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    QFont f( index.data(Qt::EditRole).value<QFont>() );
    painter->setFont( f );
    QRect rect( option.rect );
    rect.setLeft( rect.left() + 1 );
    painter->drawText( rect, Qt::AlignLeft | Qt::AlignVCenter, 
        i18n("%1 %2", f.family(), f.pointSize()) );
    painter->restore();
}

/*
FontEdit::FontEdit(QWidget *parent)
 : KLineEdit(parent)
 , m_slotTextChangedEnabled(true)
{
    setFrame(false);
    setContentsMargins(0,0,0,0);
    setClearButtonShown(true);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

FontEdit::~FontEdit()
{
}

QString FontEdit::value() const
{
    return text();
}

void FontEdit::setValue(const QString& value)
{
    m_slotTextChangedEnabled = false;
    setText(value);
    m_slotTextChangedEnabled = true;
}

void FontEdit::slotTextChanged( const QString & text )
{
    Q_UNUSED(text)
    if (!m_slotTextChangedEnabled)
        return;
    emit commitData(this);
}
*/

#if 0

#include <QLayout>
#include <QLineEdit>
#include <QVariant>
#include <QHBoxLayout>

using namespace KoProperty;

FontEdit::FontEdit(Property *property, QWidget *parent)
        : Widget(property, parent)
{
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    m_edit = new QLineEdit(this);
    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// m_edit->setMargin(1);
    m_edit->setMinimumHeight(5);
    setPlainWidgetStyle(m_edit);

    l->addWidget(m_edit);
    setFocusWidget(m_edit);

    connect(m_edit, SIGNAL(textChanged(const QString&)), this, SLOT(slotValueChanged(const QString&)));
}

FontEdit::~FontEdit()
{}

QVariant
FontEdit::value() const
{
    return m_edit->text();
}

void
FontEdit::setValue(const QVariant &value, bool emitChange)
{
    m_edit->blockSignals(true);
    m_edit->setText(value.toString());
    m_edit->blockSignals(false);
    if (emitChange)
        emit valueChanged(this);
}

void
FontEdit::slotValueChanged(const QString &)
{
    emit valueChanged(this);
}

void
FontEdit::setReadOnlyInternal(bool readOnly)
{
    m_edit->setReadOnly(readOnly);
}

#endif

#include "fontedit.moc"
