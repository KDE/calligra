/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "togglebutton_p.h"
#include "togglebutton.h"


namespace Kasten2
{

ToggleButtonPrivate::ToggleButtonPrivate( ToggleButton* parent ) : p( parent )
{
    p->setCheckable( true );
    p->setAutoRaise( true );
    p->connect( p, SIGNAL(toggled(bool)), SLOT(onToggled(bool)) );
}

void ToggleButtonPrivate::setOtherState( const KIcon& icon, const QString& text, const QString& toolTip )
{
    const QFontMetrics metrics = p->fontMetrics();
    mOtherIcon = icon;
    mOtherText = text;
    mOtherToolTip = toolTip;

    if( !text.isEmpty() )
    {
        const QString currentText = p->text();

        const int currentTextWidth = p->sizeHint().width();

        p->setText( text );
        const int otherTextWidth = p->sizeHint().width();

        p->setText( currentText );

        // TODO: this breaks on new font (size) or style change
        // better would be to reimplement sizeHint()
        p->setFixedWidth( qMax(currentTextWidth,otherTextWidth) );
    }
}

void ToggleButtonPrivate::onToggled( bool )
{
    const KIcon otherIcon = mOtherIcon;
    if( !otherIcon.isNull() )
    {
        mOtherIcon = KIcon( p->icon() );
        p->setIcon( otherIcon );
    }

    const QString otherText = mOtherText;
    if( !otherText.isEmpty() )
    {
        mOtherText = p->text();
        p->setText( otherText );
    }

    const QString otherToolTip = mOtherToolTip;
    if( !otherToolTip.isEmpty() )
    {
        mOtherToolTip = p->toolTip();
        p->setToolTip( otherToolTip );
    }
}

}
