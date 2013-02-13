/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "dialoghandler.h"

// Kasten core
#include <abstractdocument.h>
// KDE
#include <KMessageBox>
#include <KUrl>
#include <KLocale>


namespace Kasten2
{


DialogHandler::DialogHandler( QWidget* widget ) : mWidget( widget ) {}

void DialogHandler::setWidget( QWidget* widget ) { mWidget = widget; }

Answer DialogHandler::queryOverwrite( const KUrl& url, const QString& title ) const
{
    const QString message =
        i18nc( "@info",
                "There is already a file at<nl/><filename>%1</filename>.<nl/>"
                "Overwrite?", url.url() );
    const int answer = KMessageBox::warningYesNoCancel( mWidget, message, title,
                                                        KStandardGuiItem::overwrite(),
                                                        KStandardGuiItem::back() );

    return (answer == KMessageBox::Yes) ? Overwrite :
           (answer == KMessageBox::No) ?  PreviousQuestion :
                                          Cancel;
}


Answer DialogHandler::queryDiscardOnReload( const AbstractDocument* document, const QString& title ) const
{
    const QString message = i18nc( "@info \"%title\" has been modified.",
        "There are unsaved modifications to <filename>%1</filename>. "
        "They will be lost if you reload the document.<nl/>"
        "Do you want to discard them?", document->title() );

    const int answer = KMessageBox::warningContinueCancel( mWidget, message, title,
                                                           KStandardGuiItem::discard() );

    return (answer == KMessageBox::Cancel) ? Cancel : Discard;
}

Answer DialogHandler::querySaveDiscard( const AbstractDocument* document, const QString& title ) const
{
    const QString message = i18nc( "@info \"%title\" has been modified.",
        "<filename>%1</filename> has been modified.<nl/>"
        "Do you want to save your changes or discard them?", document->title() );

    const int answer = KMessageBox::warningYesNoCancel( mWidget, message, title,
                                                        KStandardGuiItem::save(), KStandardGuiItem::discard() );

    return (answer == KMessageBox::Yes) ? Save :
           (answer == KMessageBox::No) ?  Discard :
                                          Cancel;
}

Answer DialogHandler::queryDiscard( const AbstractDocument* document, const QString& title ) const
{
    const QString message = i18nc( "@info \"%title\" has been modified.",
        "<filename>%1</filename> has been modified.<nl/>"
        "Do you want to discard your changes?", document->title() );

    const int answer = KMessageBox::warningContinueCancel( mWidget, message, title,
                                                           KStandardGuiItem::discard() );

    return (answer == KMessageBox::Cancel) ? Cancel : Discard;
}

DialogHandler::~DialogHandler() {}

}
