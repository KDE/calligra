/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kcharselectdia.h"
#include "kcharselectdia.moc"

#include <qlayout.h>
#include <qstrlist.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <klocale.h>
#include <kcharselect.h>

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

KCharSelectDia::KCharSelectDia( QWidget *parent, const char *name, const QChar &_chr, const QString &_font, bool _enableFont , bool _modal)
    : KDialogBase( Plain, i18n("Select a character"), Ok | Cancel, Ok , parent, name, _modal )
{
    initDialog(_chr,_font,_enableFont);

    setButtonOKText(i18n("&Insert"),
                      i18n("Insert the selected character in the text"));

}

KCharSelectDia::KCharSelectDia( QWidget *parent, const char *name, const QString &_font, const QChar &_chr, bool _modal )
    : KDialogBase( Plain, i18n("Select a character"), User1 | Close, User1 , parent, name, _modal )
{
    initDialog(_chr,_font,true);

    setButtonText( User1, i18n("&Insert") );
    setButtonTip( User1, i18n("Insert the selected character in the text") );

}

void KCharSelectDia::initDialog(const QChar &_chr, const QString &_font, bool /*_enableFont*/)
{
   QWidget *page = plainPage();

    grid = new QGridLayout( page, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    charSelect = new KCharSelect( page, "", _font, _chr );
    connect(charSelect, SIGNAL(doubleClicked()),this, SLOT(slotDoubleClicked()));
    charSelect->resize( charSelect->sizeHint() );
    charSelect->enableFontCombo( true );
    grid->addWidget( charSelect, 0, 0 );

    grid->addColSpacing( 0, charSelect->width() );
    grid->addRowSpacing( 0, charSelect->height() );
    grid->setRowStretch( 0, 0 );
    charSelect->setFocus();
}

bool KCharSelectDia::selectChar( QString &_font, QChar &_chr, bool _enableFont )
{
    bool res = false;

    KCharSelectDia *dlg = new KCharSelectDia( 0L, "Select Character", _chr, _font, _enableFont );

    if ( dlg->exec() == Accepted )
    {
        _font = dlg->font();
        _chr = dlg->chr();
        res = true;
    }

    delete dlg;

    return res;
}

QChar KCharSelectDia::chr()
{
    return charSelect->chr();
}

QString KCharSelectDia::font()
{
    return charSelect->font();
}

void KCharSelectDia::slotUser1()
{
    emit insertChar(chr(),font());
}

void KCharSelectDia::slotDoubleClicked()
{
    emit insertChar(chr(),font());
}
