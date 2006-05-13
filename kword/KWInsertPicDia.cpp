/* This file is part of the KDE project
   Copyright (C)  2001 David Faure <faure@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <goutte@kde.org>

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

#include <QPainter>
#include <QPushButton>
#include <QBitmap>
#include <QLayout>
#include <QCheckBox>
#include <q3scrollview.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <kdebug.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kio/netaccess.h>

#include <KoPicture.h>
#include <KoPictureFilePreview.h>

#include "KWDocument.h"
#include "KWInsertPicDia.h"
#include <QTimer>

/**
 * This is the preview that appears on the right of the "Insert picture" dialog.
 * Not to be confused with KoPictureFilePreview, the one inside the file dialog!
 * (Note: this one has to remain separate, for the day we add options like flipping, rotating, etc.)
 */
class KWInsertPicPreview : public Q3ScrollView
{
public:
    KWInsertPicPreview( QWidget *parent )
        : Q3ScrollView( parent )
    {
        viewport()->setBackgroundMode( Qt::PaletteBase );
        setMinimumSize( 300, 200 );
    }

    virtual ~KWInsertPicPreview() {}

    bool setPicture( const KoPicture & picture )
    {
        if (!picture.isNull())
        {
            m_size = picture.getOriginalSize();
            m_picture = picture;
            resizeContents( m_size.width(), m_size.height() );
            repaint( false );
            return true;
        }
        else
            return false;
    }

    void drawContents( QPainter *p, int, int, int, int )
    {
        p->setBackgroundColor( Qt::white );
        // Be sure that the background is white (for transparency)
        p->fillRect(0, 0, m_size.width(), m_size.height(), QBrush( Qt::white ));
        m_picture.draw( *p, 0 ,0, m_size.width(), m_size.height());
    }

private:
    KoPicture m_picture;
    QSize m_size;
};

//////////////

KWInsertPicDia::KWInsertPicDia( QWidget *parent, bool _inline, bool _keepRatio, KWDocument *_doc, const char *name )
    : KDialogBase( Plain, i18n("Insert Picture"), Ok|Cancel, Ok, parent, name, true ),
    m_bFirst ( true ), m_doc ( _doc )
{
    setInitialSize( QSize(400, 300) );
    QWidget *page = plainPage();
    Q3GridLayout *grid = new Q3GridLayout( page, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QPushButton *pbImage = new QPushButton( i18n( "Choose &Picture..." ), page );
    grid->addWidget( pbImage, 0, 0 );
    connect( pbImage, SIGNAL( clicked() ), SLOT( slotChooseImage() ) );

    m_cbInline = new QCheckBox( i18n( "Insert picture inline" ), page );
    grid->addWidget( m_cbInline, 1, 0 );

    m_cbKeepRatio= new QCheckBox( i18n("Retain original aspect ratio"),page);
    grid->addWidget( m_cbKeepRatio, 2, 0);

    m_preview = new KWInsertPicPreview( page );
    grid->addMultiCellWidget( m_preview, 0, 3, 1, 1 );

    // Stretch the buttons and checkboxes a little, but stretch the preview much more
    grid->setRowStretch( 0, 1 );
    grid->setRowStretch( 1, 1 );
    grid->setRowStretch( 2, 1 );
    grid->setRowStretch( 3, 10 );
    grid->setColStretch( 0, 1 );
    grid->setColStretch( 1, 10 );
    m_cbKeepRatio->setChecked(_keepRatio);
    m_cbInline->setChecked( _inline );
    enableButtonOK( false );
    setFocus();
    slotChooseImage(); // save the user time, directly open the dialog
}

bool KWInsertPicDia::makeInline() const
{
    return m_cbInline->isChecked();
}

bool KWInsertPicDia::keepRatio() const
{
    return m_cbKeepRatio->isChecked();
}

void KWInsertPicDia::slotChooseImage()
{
    KoPicture tmppicture = KWInsertPicDia::selectPictureDia( ":picture", this );
    if ( !tmppicture.isNull() ) // if canceled, keep current picture (#72827)
        m_picture = tmppicture;
    if ( m_picture.isNull() && m_bFirst)
    {
        kDebug() << "KWInsertPicDia::slotChooseImage cancelled by user." << endl;
        // Close, but delayed, otherwise it won't work (we only return from the ctor)
        QTimer::singleShot( 0, this, SLOT( cancel() ) );
        return;
    }
    enableButtonOK ( m_preview->setPicture( m_picture ) );
    m_bFirst = false;
}

KoPicture KWInsertPicDia::selectPictureDia( const QString & _path, QWidget* parent )
{
    QStringList mimetypes ( KImageIO::mimeTypes( KImageIO::Reading ) );
    mimetypes += KoPictureFilePreview::clipartMimeTypes();

    KFileDialog fd( _path, QString::null, parent );
    fd.setMimeFilter( mimetypes );
    fd.setCaption(i18n("Choose Picture"));
    return selectPicture( fd, parent );
}

KoPicture KWInsertPicDia::selectPicture( KFileDialog & fd, QWidget *parent )
{
    KoPicture picture;

    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );
    KUrl url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( !url.isEmpty() )
        picture.setKeyAndDownloadPicture( url, parent );

    return picture;
}

KoPicture KWInsertPicDia::picture ( void ) const
{
    kDebug() << m_picture.getKey().toString() << " selected in KWInsertPicDia" << endl;
    return m_picture;
}

#include "KWInsertPicDia.moc"
