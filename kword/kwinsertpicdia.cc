/* This file is part of the KDE project
   Copyright (C)  2001 David Faure <faure@kde.org>

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

#include "kwinsertpicdia.h"
#include "preview.h"
#include <qwmf.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <qframe.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qpicture.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <kimageio.h>
#include <kio/netaccess.h>
#include <kdebug.h>

class KWInsertPicPreview : public QFrame
{
public:
    KWInsertPicPreview( QWidget *parent )
        : QFrame( parent )
    {
        setFrameStyle( WinPanel | Sunken );
        setMinimumSize( 300, 200 );
        m_type = IPD_IMAGE;
    }

    virtual ~KWInsertPicPreview() {}

    void setPixmap( const QString & filename )
    {
        m_type = IPD_IMAGE;
        m_pixmap.load( filename );
        //kdDebug() << "setPixmap: m_pixmap is " << m_pixmap.width() << ", " << m_pixmap.height() << endl;
        const QBitmap nullBitmap;
        m_pixmap.setMask( nullBitmap );  //don't show transparency
        repaint(false);
    }

    QSize pixmapSize() const { return m_pixmap.size(); }

    void setClipart( const QString & filename )
    {
        m_type = IPD_CLIPART;
        m_pixmap = QPixmap();
        QWinMetaFile wmf;
        wmf.load( filename );
        wmf.paint( &m_picture );
        repaint(false);
    }

    void drawContents( QPainter *p )
    {
        QFrame::drawContents( p );
        p->save();
        p->translate( contentsRect().x(), contentsRect().y() );
        if ( m_type == IPD_IMAGE )
        {
            p->drawPixmap( QPoint( 0, 0 ), m_pixmap );
        }
        else
        {
            p->drawPicture( m_picture );
        }
        p->restore();
    }
private:
    enum { IPD_IMAGE, IPD_CLIPART } m_type;
    QPixmap m_pixmap;
    QPicture m_picture;
};

//////////////

KWInsertPicDia::KWInsertPicDia( QWidget *parent, const char *name )
    : KDialogBase( Plain, i18n("Insert Picture"), Ok|Cancel, Ok, parent, name, true )
{
    setInitialSize( QSize(400, 300) );

    QWidget *page = plainPage();
    QGridLayout *grid = new QGridLayout( page, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QPushButton *pbImage = new QPushButton( i18n( "Choose &Image" ), page );
    grid->addWidget( pbImage, 0, 0 );
    connect( pbImage, SIGNAL( clicked() ), SLOT( slotChooseImage() ) );

    QPushButton *pbClipart = new QPushButton( i18n( "Choose &Clipart" ), page );
    grid->addWidget( pbClipart, 1, 0 );
    connect( pbClipart, SIGNAL( clicked() ), SLOT( slotChooseClipart() ) );

    m_cbInline = new QCheckBox( i18n( "Insert Picture Inline" ), page );
    grid->addWidget( m_cbInline, 2, 0 );

    m_preview = new KWInsertPicPreview( page );
    grid->addMultiCellWidget( m_preview, 0, 3, 1, 1 );

    // Stretch the buttons and checkboxes a little, but stretch the preview much more
    grid->setRowStretch( 0, 1 );
    grid->setRowStretch( 1, 1 );
    grid->setRowStretch( 2, 1 );
    grid->setRowStretch( 3, 10 );
    grid->setColStretch( 0, 1 );
    grid->setColStretch( 1, 10 );
    enableButtonOK( false );
}

bool KWInsertPicDia::makeInline() const
{
    return m_cbInline->isChecked();
}

void KWInsertPicDia::slotChooseImage()
{
    KFileDialog fd( QString::null, KImageIO::pattern(KImageIO::Writing), 0, 0, TRUE );
    fd.setCaption(i18n("Choose Image"));
    QString file = selectPicture( fd );
    if ( !file.isEmpty() )
    {
        m_filename = file;
        m_type = IPD_IMAGE;
        m_preview->setPixmap( m_filename );
        enableButtonOK( true );
    }
}

void KWInsertPicDia::slotChooseClipart()
{
    KFileDialog fd( QString::null, i18n( "*.wmf|Windows Metafiles (*.wmf)" ), 0, 0, true );
    fd.setCaption(i18n("Choose Clipart"));
    QString file = selectPicture( fd );
    if ( !file.isEmpty() )
    {
        m_filename = file;
        m_type = IPD_CLIPART;
        m_preview->setClipart( m_filename );
        enableButtonOK( true );
    }
}

QString KWInsertPicDia::selectPicture( KFileDialog & fd )
{
    fd.setPreviewWidget( new Preview( &fd ) );
    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() )
      return QString::null;

    QString chosen = QString::null;
    if (!KIO::NetAccess::download( url, chosen ))
        return QString::null;
    return chosen;
}

QSize KWInsertPicDia::pixmapSize() const
{
    ASSERT( m_type == IPD_IMAGE );
    return m_preview->pixmapSize();
}

#include "kwinsertpicdia.moc"
