/* This file is part of the KDE project
   Copyright (C) 2002-2004 David Faure <faure@kde.org>
   Copyright (C) 2002-2004 Clarence Dang <dang@kde.org>

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

#include "kofiledialog.h"
#include "koDocument.h"
#include <kfilefiltercombo.h>
#include <klocale.h>
#include <kdiroperator.h>

KoFileDialog::KoFileDialog(const QString& startDir, const QString& filter,
                           QWidget *parent, const char *name,
                           bool modal)
    : KFileDialog( startDir, filter, parent, name, modal )
{
    connect( filterWidget, SIGNAL( activated( int) ),
             this, SLOT( slotChangedfilter( int ) ) );
}

void KoFileDialog::slotChangedfilter( int index )
{
    if ( index == KoDocument::SaveAsDirectoryStore ){
        ops->setMode(KFile::Directory);
    } else {
        ops->setMode(KFile::File);
    }
}

void KoFileDialog::setSpecialMimeFilter( QStringList& mimeFilter,
                                         const QString& currentFormat, const int specialOutputFlag,
                                         const QString& nativeFormat,
                                         const QStringList& extraNativeMimeTypes )
{
    Q_ASSERT( !mimeFilter.isEmpty() );
    Q_ASSERT( mimeFilter[0] == nativeFormat );

    int numSpecialEntries = 3;

    // Insert numSpecialEntries entries with native mimetypes, for the special entries.
    QStringList::Iterator mimeFilterIt = mimeFilter.at( 1 );
    mimeFilter.insert( mimeFilterIt /* before 1 -> after 0 */, numSpecialEntries, nativeFormat );
    // Now insert entries for the extra native mimetypes
    mimeFilterIt = mimeFilter.at( 1 + numSpecialEntries );
    for( QStringList::ConstIterator it = extraNativeMimeTypes.begin(); it != extraNativeMimeTypes.end(); ++it ) {
        mimeFilterIt = mimeFilter.insert( mimeFilterIt, *it );
        ++mimeFilterIt;
    }

    // Fill in filter combo
    // Note: if currentFormat doesn't exist in mimeFilter, filterWidget
    //       will default to the first item (native format)
    setMimeFilter( mimeFilter, currentFormat.isEmpty() ? nativeFormat : currentFormat );

    // To get a different description in the combo, we need to change its entries afterwards
    KMimeType::Ptr type = KMimeType::mimeType( nativeFormat );
    filterWidget->changeItem( i18n("%1 (KOffice-1.1 Format)").arg( type->comment() ), KoDocument::SaveAsKOffice1dot1 );
    filterWidget->changeItem( i18n("%1 (Uncompressed XML Files)").arg( type->comment() ), KoDocument::SaveAsDirectoryStore );
    filterWidget->changeItem( i18n("%1 (KOffice-1.3 Format)").arg( type->comment() ), KoDocument::SaveAsKOffice1dot3 );
    //filterWidget->changeItem( i18n("%1 (OASIS Format - experimental)").arg( type->comment() ), KoDocument::SaveAsOASIS );
    // if you add an entry here, update numSpecialEntries above and specialEntrySelected() below

    // For native format...
    if (currentFormat == nativeFormat || currentFormat.isEmpty())
    {
        // KFileFilterCombo selected the _last_ "native mimetype" entry, select the correct one
        filterWidget->setCurrentItem( specialOutputFlag );
        slotChangedfilter( filterWidget->currentItem() );
    }
    // [Mainly KWord] Tell MS Office users that they can save in RTF!
    int i = 0;
    for (mimeFilterIt = mimeFilter.begin (); mimeFilterIt != mimeFilter.end (); ++mimeFilterIt, i++)
    {
        KMimeType::Ptr mime = KMimeType::mimeType (*mimeFilterIt);
        QString compatString = mime->property ("X-KDE-CompatibleApplication").toString ();
        if (!compatString.isEmpty ())
            filterWidget->changeItem (i18n ("%1 (%2 Compatible)").arg (mime->comment ()).arg (compatString), i);
    }
}

int KoFileDialog::specialEntrySelected()
{
    int i = filterWidget->currentItem();
    // This enum is the position of the special items in the filter combo.
    if ( i == KoDocument::SaveAsKOffice1dot1
         || i == KoDocument::SaveAsDirectoryStore
         || i == KoDocument::SaveAsKOffice1dot3
         /*|| i == KoDocument::SaveAsOASIS*/ )
        return i;
    return 0;
}

#include "kofiledialog.moc"
