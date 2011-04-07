/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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
#include "utils.h"

/** @WARNING (DK) KFontChooser has been removed ! It seems that it's no more needed ! 
 */

#warning KFontChooser has been removed ! It seems that it's no more needed ! 
#ifdef __KDE4_LIBS__
#include <KFontChooser>
#endif

#warning (DK) QFontDialog replaced by QFontDialog
#include <QFontDialog>

#include <QLocale>
#include <QDebug>

#warning (DK) KPushButton replaced by QPushButton
#include <QPushButton>

#warning (DK) QDialog replaced by QDialog
#include <QDialog>

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QModelIndex>
#include <QVariant>
#include <QStyleOptionViewItem>
#include <QFontDatabase>
#include <QEvent>
#include <QHBoxLayout>
#include <QApplication>

#include "fontedit_p.h"

using namespace KoProperty;

FontEditRequester::FontEditRequester(QWidget* parent)
            : QWidget(parent)
            , m_paletteChangedEnabled(true)
{
    setBackgroundRole(QPalette::Base);
    QHBoxLayout *lyr = new QHBoxLayout(this);
    lyr->setContentsMargins(0,0,0,0);
    lyr->setSpacing( 1 );
    lyr->addStretch(1);
    m_button = new QPushButton(this);
    setFocusProxy(m_button);
    Utils::setupDotDotDotButton(m_button,
	tr("Click to select a font"),
	tr("Selects font"));
    connect( m_button, SIGNAL( clicked() ), SLOT( slotSelectFontClicked() ) );
    lyr->addWidget(m_button);
    setValue(qApp->font());
}

void FontEditRequester::slotSelectFontClicked()
{
#warning KFontChooser has been removed ! It seems that it's no more needed ! 
#ifdef __KDE4_LIBS__      
    KFontChooser::DisplayFlags flags = KFontChooser::NoDisplayFlags;
    if (KDialog::Accepted == KFontDialog::getFont( m_font, flags, this )) {
	setValue(m_font);
	emit commitData(this);
    }
#else
    bool ok;
    m_font = QFontDialog::getFont( &ok, m_font,this );
    if(ok) {
	// the user clicked OK and font is set to the font the user selected
	setValue(m_font);
	emit commitData(this);
    } else {
	// the user canceled the dialog; font is set to the initial
	// value, in this case Helvetica [Cronyx],
    }
#endif
}
   

QWidget * FontDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new FontEditRequester(parent);
}

void FontDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    const QFont origFont( painter->font() );
    QFont f( index.data(Qt::EditRole).value<QFont>() );
    int size = f.pointSize(); // will be needed later
    if (size == -1) {
        size = f.pixelSize();
    }
    if (option.font.pointSize() > 0)
        f.setPointSize(option.font.pointSize());
    else if (option.font.pixelSize() > 0)
        f.setPixelSize(option.font.pixelSize());
    painter->setFont( f );
    QRect rect( option.rect );
    rect.setLeft( rect.left() + 1 );
    const QString txt( QObject::tr("Abc", "Font sample for property editor item, typically \"Abc\"") );
    painter->drawText( rect, Qt::AlignLeft | Qt::AlignVCenter,
        QObject::tr("Abc", "Font sample for property editor item, typically \"Abc\"") );

    rect.setLeft(rect.left() + 5 + painter->fontMetrics().width( txt ));
    painter->setFont(origFont);
    painter->drawText( rect, Qt::AlignLeft | Qt::AlignVCenter,
        QObject::tr("%1, %2pt", "Font family and size, e.g. Arial, 2pt").arg(f.family()).arg(size) );
    painter->restore();
}

#include "fontedit_p.moc"
