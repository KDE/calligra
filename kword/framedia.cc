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

#include "kwdoc.h"
#include "kwframe.h"
#include "kwtextframeset.h"
#include "framedia.h"
#include "framedia.moc"
#include "defs.h"
#include "kwcommand.h"
#include "kwtableframeset.h"

#include <klocale.h>
#include <kapp.h>
#include <kiconloader.h>
#include <kglobal.h>

#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qevent.h>
#include <qlistview.h>
#include <qbuttongroup.h>
#include <qhbox.h>
#include <qheader.h>
#include <kmessagebox.h>
#include <qvalidator.h>

#include <stdlib.h>
#include <limits.h>

#include <kdebug.h>

/******************************************************************/
/* Class: KWFrameDia                                              *
 *
 *  TAB Frame Options
 *      Set options dependend of frametype
 *  TAB Text Runaround
 *      Set the text behaviour of this frame
 *  TAB Frameset
 *      here the user can select from the current TEXT framesets, a new one is
 *      included in the list.
 *      Afterwards (on ok) the frame should be checked if it is already owned by a
 *      frameset, if so that connection must be disconnected (if different) and
 *      framebehaviour will be copied from the frameset
 *      then the new connection should be made.
 *
 *  TAB Geometry
 *      position/size
 ******************************************************************/

KWFrameDia::KWFrameDia( QWidget* parent, KWFrame *_frame)
    : KDialogBase( Tabbed, i18n("Frame settings"), Ok | Cancel, Ok, parent, "framedialog", true)
{
    frame = _frame;
    KWFrameSet * fs = frame->getFrameSet();
    KWTableFrameSet *table = fs->getGroupManager();
    if(table)
        frameType = table->type();
    else
        frameType = frame->getFrameSet() ? frame->getFrameSet()->type() : (FrameSetType) -1;
    // parentFs is the table in case of a table, fs otherwise
    KWFrameSet * parentFs = fs->getGroupManager() ? fs->getGroupManager() : fs;
    frameSetFloating = parentFs->isFloating();
    doc = 0;
    init();
}

/* Contructor when the dialog is used on creation of frame */
KWFrameDia::KWFrameDia( QWidget* parent, KWFrame *_frame, KWDocument *_doc, FrameSetType _ft )
    : KDialogBase( Tabbed, i18n("Frame settings"), Ok | Cancel, Ok, parent, "framedialog", true)
{
    frameType=_ft;
    doc = _doc;
    frame= _frame;
    frameSetFloating = false;
    init();
}

void KWFrameDia::init() {

    tab1 = tab2 = tab3 = tab4 = 0;
    if (frame) {
        KoRect r = frame->normalize();
        frame->setRect( r.x(), r.y(), r.width(), r.height() );
        KWFrameSet *fs = frame->getFrameSet(); // 0 when creating a frame
        if(!doc && fs)
        {
            doc = fs->kWordDocument();
        }
        if(!doc)
        {
            kdDebug() << "ERROR: KWFrameDia::init frame has no reference to doc.."<<endl;
            return;
        }
        if( fs && fs->isMainFrameset() )
        {
            setupTab2();
            setupTab4();
        }
        else if ( fs && fs->isHeaderOrFooter() )
        {
            setupTab1();
            setupTab2();
            setupTab4();
        }
        else if(frameType == FT_TEXT)
        {
            setupTab1();
            setupTab2();
            setupTab3();
            setupTab4();
            if(! frame->getFrameSet()) // first creation
                showPage(2);
        }
        else if(frameType == FT_PICTURE || frameType == FT_CLIPART)
        {
            setupTab1();
            setupTab2();
            setupTab4();
            showPage(1); // while options are not implemented..
        }
        else if(frameType == FT_PART)
        {
            setupTab2();
            setupTab4();
        }
        else if(frameType == FT_FORMULA)
        {
            setupTab1();
            setupTab2();
            setupTab4();
            showPage(1); // while options are not implemented..
        }
        else if(frameType == FT_TABLE)
        {
             setupTab4();
             grp1->setEnabled(false);
        }
    }
    else
        kdDebug() << "ERROR: KWFrameDia::init  no frame.."<<endl;
    setInitialSize( QSize(550, 400) );
}


void KWFrameDia::setupTab1(){ // TAB Frame Options
    //kdDebug() << "setup tab 1 Frame options"<<endl;
    tab1 = addPage( i18n("Options") );

    int columns = 0;
    if(frameType == FT_FORMULA || frameType == FT_PICTURE)
        columns = 1;
    else if(frameType == FT_TEXT)
        columns = 2;

    grid1 = new QGridLayout( tab1, 0 /*auto create*/, columns, KDialog::marginHint(), KDialog::spacingHint() );

    // Options for all types of frames
    cbCopy = new QCheckBox( i18n("Frame is a copy of the previous frame"),tab1 );
    grid1->addWidget(cbCopy,1,0);

    cbCopy->setChecked( frame->isCopy() );
    cbCopy->setEnabled( frame->getFrameSet() && frame->getFrameSet()->getFrame( 0 ) != frame ); // First one can't be a copy
    // Well, for images, formulas etc. it doesn't make sense to deactivate 'is copy'. What else would it show ?
    if(frameType!=FT_TEXT)
        cbCopy->setEnabled( false );

    int row = 2;
    int column = 0;

    // Picture frame
    if(frameType==FT_PICTURE)
    {
        cbAspectRatio = new QCheckBox (i18n("Retain original aspect ratio"),tab1);
        if ( frame->getFrameSet() )
            cbAspectRatio->setChecked( static_cast<KWPictureFrameSet *>( frame->getFrameSet() )->keepAspectRatio() );
        else
            cbAspectRatio->setChecked( true );
        grid1->addWidget(cbAspectRatio, row, 0);
        ++row;
    }
    else
        cbAspectRatio = 0L;

    // Text frame
    if(frameType==FT_TEXT)
    {
        // AutoCreateNewFrame policy.
        endOfFrame = new QGroupBox(i18n("If text is too long for frame:"), tab1 );
        grid1->addWidget( endOfFrame, row, 0 );

        eofGrid= new QGridLayout (endOfFrame, 4, 1, KDialog::marginHint(), KDialog::spacingHint());
        rAppendFrame = new QRadioButton( i18n( "Create a new page" ), endOfFrame );
        eofGrid->addWidget( rAppendFrame, 1, 0 );

        rResizeFrame = new QRadioButton( i18n( "Resize last frame" ), endOfFrame );
        eofGrid->addWidget( rResizeFrame, 2, 0 );

        rNoShow = new QRadioButton( i18n( "Don't show the extra text" ), endOfFrame );
        eofGrid->addWidget( rNoShow, 3, 0 );
        QButtonGroup *grp = new QButtonGroup( endOfFrame );
        grp->hide();
        grp->setExclusive( true );
        grp->insert( rAppendFrame );
        grp->insert( rResizeFrame );
        grp->insert( rNoShow );

        eofGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );

        if(frame->getFrameBehaviour() == KWFrame::AutoExtendFrame) {
            rResizeFrame->setChecked(true);
        } else if (frame->getFrameBehaviour() == KWFrame::AutoCreateNewFrame) {
            rAppendFrame->setChecked(true);
        } else {
            rNoShow->setChecked(true);
        }
        column++;
    } else
    {
        rResizeFrame = 0L;
        rAppendFrame = 0L;
        rNoShow = 0L;
    }

    // NewFrameBehaviour - now for all type of frames
    onNewPage = new QGroupBox(i18n("On new page creation:"),tab1);
    grid1->addWidget( onNewPage, row, column );

    onpGrid = new QGridLayout( onNewPage, 4, 1, KDialog::marginHint(), KDialog::spacingHint() );
    reconnect = new QRadioButton (i18n ("Reconnect frame to current flow"), onNewPage);
    if ( rResizeFrame )
        connect( reconnect, SIGNAL( clicked() ), this, SLOT( setFrameBehaviourInputOn() ) );
    onpGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );
    onpGrid->addWidget( reconnect, 1, 0 );

    noFollowup = new QRadioButton (i18n ("Don't create a followup frame"), onNewPage);
    if ( rResizeFrame )
        connect( noFollowup, SIGNAL( clicked() ), this, SLOT( setFrameBehaviourInputOn() ) );
    onpGrid->addWidget( noFollowup, 2, 0 );

    copyRadio= new QRadioButton (i18n ("Place a copy of this frame"), onNewPage);
    if ( rResizeFrame )
        connect( copyRadio, SIGNAL( clicked() ), this, SLOT( setFrameBehaviourInputOff() ) );
    onpGrid->addWidget( copyRadio, 3, 0);

    enableOnNewPageOptions();

    QButtonGroup *grp2 = new QButtonGroup( onNewPage );
    grp2->hide();
    grp2->setExclusive( true );
    grp2->insert( reconnect );
    grp2->insert( noFollowup );
    grp2->insert( copyRadio );
    grid1->addRowSpacing( row, onNewPage->height());
    if(frame->getNewFrameBehaviour() == KWFrame::Reconnect) {
        reconnect->setChecked(true);
    } else if(frame->getNewFrameBehaviour() == KWFrame::NoFollowup) {
        noFollowup->setChecked(true);
    } else {
        copyRadio->setChecked(true);
        setFrameBehaviourInputOff();
    }


    // SideHeads definition - is that for text frames only ?
    if( frameType == FT_TEXT )
    {
        row++;
        sideHeads = new QGroupBox(i18n("SideHead definition"),tab1);
        sideHeads->setEnabled(false);
        grid1->addWidget(sideHeads, row, 0);

        sideGrid = new QGridLayout( sideHeads, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );
        sideTitle1 = new QLabel ( i18n("Size (%1):").arg(doc->getUnitName()),sideHeads);
        sideTitle1->resize(sideTitle1->sizeHint());
        sideGrid->addWidget(sideTitle1,1,0);
        sideWidth= new QLineEdit(sideHeads,"");
        sideWidth->setMaxLength(6);
        sideGrid->addWidget(sideWidth,1,1);
        sideTitle2 = new QLabel( i18n("Gap size (%1):").arg(doc->getUnitName()),sideHeads);
        sideTitle2->resize(sideTitle2->sizeHint());
        sideGrid->addWidget(sideTitle2,2,0);
        sideGap = new QLineEdit(sideHeads,"");
        sideGap->setMaxLength(6);
        sideGrid->addWidget(sideGap,2,1);
        sideAlign = new QComboBox (false,sideHeads);
        sideAlign->setAutoResize(false);
        sideAlign->insertItem ( i18n("Left"));
        sideAlign->insertItem ( i18n("Right"));
        sideAlign->insertItem ( i18n("Closest to binding"));
        sideAlign->insertItem ( i18n("Closest to page edge"));
        sideAlign->resize(sideAlign->sizeHint());
        sideGrid->addMultiCellWidget(sideAlign,3,3,0,1);
        sideGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );

        // init for sideheads.
        sideWidth->setText("0");
        sideWidth->setValidator( new QDoubleValidator( sideWidth) );

        sideGap->setText("0");
        sideGap->setValidator( new QDoubleValidator( sideGap) );
        // add rest of sidehead init..
    }

    for(int i=0;i < row;i++)
        grid1->setRowStretch( i, 0 );
    grid1->setRowStretch( row + 1, 1 );
}


void KWFrameDia::setupTab2() // TAB Text Runaround
{
    //kdDebug() << "setup tab 2 text runaround"<<endl;

    tab2 = addPage( i18n( "Text run around" ) );

    grid2 = new QGridLayout( tab2, 3, 2, KDialog::marginHint(), KDialog::spacingHint() );

    runGroup = new QGroupBox( i18n( "Text in other frames will:" ), tab2 );

    runGrid = new QGridLayout( runGroup, 4, 3, KDialog::marginHint(), KDialog::spacingHint() );

    QPixmap pixmap = KWBarIcon( "run_not" );
    lRunNo = new QLabel( runGroup );
    lRunNo->setBackgroundPixmap( pixmap );
    lRunNo->setFixedSize( pixmap.size() );
    runGrid->addWidget( lRunNo, 1, 0 );
    runGrid->addColSpacing( 0, pixmap.width());

    pixmap = KWBarIcon( "run_bounding" );
    lRunBounding = new QLabel( runGroup );
    lRunBounding->setBackgroundPixmap( pixmap );
    lRunBounding->setFixedSize( pixmap.size() );
    runGrid->addWidget( lRunBounding, 2, 0 );

    pixmap = KWBarIcon( "run_skip" );
    lRunContur = new QLabel( runGroup );
    lRunContur->setBackgroundPixmap( pixmap );
    lRunContur->setFixedSize( pixmap.size() );
    runGrid->addWidget( lRunContur, 3, 0 );

    rRunNo = new QRadioButton( i18n( "&Run through this frame" ), runGroup );
    runGrid->addWidget( rRunNo, 1, 1 );
    connect( rRunNo, SIGNAL( clicked() ), this, SLOT( runNoClicked() ) );

    rRunBounding = new QRadioButton( i18n( "Run around the &boundary rectangle of this frame" ), runGroup );
    runGrid->addWidget( rRunBounding, 2, 1 );
    connect( rRunBounding, SIGNAL( clicked() ), this, SLOT( runBoundingClicked() ) );

    rRunContur = new QRadioButton( i18n( "Do&n't run around this frame" ), runGroup );
    runGrid->addWidget( rRunContur, 3, 1 );
    connect( rRunContur, SIGNAL( clicked() ), this, SLOT( runConturClicked() ) );

    runGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );

    grid2->addWidget( runGroup, 0, 0 );
    grid2->addMultiCellWidget( runGroup, 0, 0, 0, 1 );

    lRGap = new QLabel( i18n( "Run around gap (%1):" ).arg(doc->getUnitName()), tab2 );
    lRGap->resize( lRGap->sizeHint() );
    lRGap->setAlignment( AlignRight | AlignVCenter );
    grid2->addWidget( lRGap, 1, 0 );

    eRGap = new QLineEdit( tab2 );
    eRGap->setValidator( new QDoubleValidator( eRGap ) );
    eRGap->setText( "0.00" );
    eRGap->setMaxLength( 5 );
    eRGap->setEchoMode( QLineEdit::Normal );
    eRGap->setFrame( true );
    eRGap->resize( eRGap->sizeHint() );
    grid2->addWidget( eRGap, 1, 1 );

    eRGap->setEnabled( false ); // ### not implemented currently

    KWFrame::RunAround ra = KWFrame::RA_NO;
    if ( frame )
        ra = frame->runAround();
    else
    {
        KWFrame *firstFrame = doc->getFirstSelectedFrame();
        if ( firstFrame )
            ra = firstFrame->runAround();
    }

    switch ( ra ) {
    case KWFrame::RA_NO: rRunNo->setChecked( true );
        break;
    case KWFrame::RA_BOUNDINGRECT: rRunBounding->setChecked( true );
        break;
    case KWFrame::RA_SKIP: rRunContur->setChecked( true );
        break;
    }

    double ragap = 0;
    if ( frame )
        ragap = frame->runAroundGap();
    else
    {
        KWFrame *firstFrame = doc->getFirstSelectedFrame();
        if ( firstFrame )
            ragap = firstFrame->runAroundGap();
    }

    QString str;
    str.setNum( KWUnit::userValue( ragap, doc->getUnit() ) );
    eRGap->setText( str );

    enableRunAround();

    //kdDebug() << "setup tab 2 exit"<<endl;
}

void KWFrameDia::setupTab3(){ // TAB Frameset
    /*
     * here the user can select from the current TEXT framesets, a new one is
     * included in the list.
     * Afterwards (on ok) the frame should be checked if it is allready owned by a
     * frameset, if so that connection must be disconnected (if different) and
     * framebehaviour will be copied from the frameset
     * then the new connection should be made.
 */
    //kdDebug() << "setup tab 3 frameSet"<<endl;
    tab3 = addPage( i18n( "Connect text frames" ) );

    grid3 = new QGridLayout( tab3, 3, 1, KDialog::marginHint(), KDialog::spacingHint() );

    lFrameSet = new QLabel( i18n( "Choose a frameset to which the current frame should be connected:" ), tab3 );
    lFrameSet->resize( lFrameSet->sizeHint() );
    grid3->addWidget( lFrameSet, 0, 0 );

    lFrameSList = new QListView( tab3 );
    lFrameSList->addColumn( i18n( "Nr." ) );
    lFrameSList->addColumn( i18n( "Frameset name" ) );
    lFrameSList->setAllColumnsShowFocus( true );
    lFrameSList->header()->setMovingEnabled( false );

    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        KWFrameSet * fs = doc->getFrameSet( i );
        if ( i == 0 && doc->processingType() == KWDocument::WP )
            continue;
        if ( fs->type() != FT_TEXT || fs->isHeaderOrFooter() )
            continue;
        if ( fs->getGroupManager() )
            continue;
        if ( fs->getNumFrames() == 0 ) // deleted frameset
            continue;
        QListViewItem *item = new QListViewItem( lFrameSList );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, fs->getName() );
        if( frame->getFrameSet() == fs )
            lFrameSList->setSelected(item, TRUE );
    }

    if (! frame->getFrameSet()) {
        QListViewItem *item = new QListViewItem( lFrameSList );
        item->setText( 0, QString( "*%1" ).arg( doc->getNumFrameSets()+1 ) );
        item->setText( 1, i18n( "Create a new frameset with this frame" ) );
        lFrameSList->setSelected( lFrameSList->firstChild(), TRUE );
    }

    connect( lFrameSList, SIGNAL( currentChanged( QListViewItem * ) ),
             this, SLOT( connectListSelected( QListViewItem * ) ) );
    grid3->addWidget( lFrameSList, 1, 0 );

    QHBox *row = new QHBox( tab3 );
    row->setSpacing( 5 );
    if (! frame->getFrameSet())
    {
        ( void )new QLabel( i18n( "Name of new frameset:" ), row );
        oldFrameSetName = doc->generateFramesetName( i18n( "Text Frameset %1" ) );
    }
    else
    {
        ( void )new QLabel( i18n( "Name of frameset:" ), row );
        oldFrameSetName = frame->getFrameSet()->getName();
    }
    eFrameSetName = new QLineEdit( row );
    eFrameSetName->setText( oldFrameSetName );

    grid3->addWidget( row, 2, 0 );

    connectListSelected( lFrameSList->firstChild() );
}

void KWFrameDia::setupTab4(){ // TAB Geometry
    //kdDebug() << "setup tab 4 geometry"<<endl;

    tab4 = addPage( i18n( "Geometry" ) );
    grid4 = new QGridLayout( tab4, 4, 1, KDialog::marginHint(), KDialog::spacingHint() );

    floating = new QCheckBox (i18n("Frame is inline"), tab4);

    connect( floating, SIGNAL( toggled(bool) ), this, SLOT( slotFloatingToggled(bool) ) );
    int row = 0;
    grid4->addMultiCellWidget( floating, row, row, 0, 1 );

    /* ideally the following properties could be given to any floating frame:
       Position: (y)
        Top of frame
        Top of paragraph
        Above current line
        At insertion point
        Below current line
        Bottom of paragraph
        Bottom of frame
        Absolute
       Alignment: (x)
        Left
        Right
        Center
        Closest to binding
        Further from binding
        Absolute
    */


    grp1 = new QGroupBox( i18n("Position in %1").arg(doc->getUnitName()), tab4 );
    pGrid = new QGridLayout( grp1, 5, 2, KDialog::marginHint(), KDialog::spacingHint() );

    lx = new QLabel( i18n( "Left:" ), grp1 );
    lx->resize( lx->sizeHint() );
    pGrid->addWidget( lx, 1, 0 );

    sx = new QLineEdit( grp1 );

    sx->setText( "0.00" );
    sx->setMaxLength( 16 );
    sx->setEchoMode( QLineEdit::Normal );
    sx->setFrame( true );
    sx->resize( sx->sizeHint() );
    pGrid->addWidget( sx, 2, 0 );

    ly = new QLabel( i18n( "Top:" ), grp1 );
    ly->resize( ly->sizeHint() );
    pGrid->addWidget( ly, 1, 1 );

    sy = new QLineEdit( grp1 );

    sy->setText( "0.00" );
    sy->setMaxLength( 16 );
    sy->setEchoMode( QLineEdit::Normal );
    sy->setFrame( true );
    sy->resize( sy->sizeHint() );
    pGrid->addWidget( sy, 2, 1 );

    lw = new QLabel( i18n( "Width:" ), grp1 );
    lw->resize( lw->sizeHint() );
    pGrid->addWidget( lw, 3, 0 );

    sw = new QLineEdit( grp1 );

    sw->setText( "0.00" );
    sw->setMaxLength( 16 );
    sw->setEchoMode( QLineEdit::Normal );
    sw->setFrame( true );
    sw->resize( sw->sizeHint() );
    pGrid->addWidget( sw, 4, 0 );

    lh = new QLabel( i18n( "Height:" ), grp1 );
    lh->resize( lh->sizeHint() );
    pGrid->addWidget( lh, 3, 1 );

    sh = new QLineEdit( grp1 );

    sh->setText( "0.00" );
    sh->setMaxLength( 16 );
    sh->setEchoMode( QLineEdit::Normal );
    sh->setFrame( true );
    sh->resize( sh->sizeHint() );
    pGrid->addWidget( sh, 4, 1 );

    pGrid->addRowSpacing( 0, KDialog::spacingHint() + 5 );

    grid4->addWidget( grp1, ++row, 0 );

    grp2 = new QGroupBox( i18n("Margins in %1").arg(doc->getUnitName()), tab4 );
    mGrid = new QGridLayout( grp2, 5, 2, KDialog::marginHint(), KDialog::spacingHint() );

    lml = new QLabel( i18n( "Left:" ), grp2 );
    lml->resize( lml->sizeHint() );
    mGrid->addWidget( lml, 1, 0 );

    sml = new QLineEdit( grp2 );

    sml->setText( "0.00" );
    sml->setMaxLength( 5 );
    sml->setEchoMode( QLineEdit::Normal );
    sml->setFrame( true );
    sml->resize( sml->sizeHint() );
    mGrid->addWidget( sml, 2, 0 );

    lmr = new QLabel( i18n( "Right:" ), grp2 );
    lmr->resize( lmr->sizeHint() );
    mGrid->addWidget( lmr, 1, 1 );

    smr = new QLineEdit( grp2 );

    smr->setText( "0.00" );
    smr->setMaxLength( 5 );
    smr->setEchoMode( QLineEdit::Normal );
    smr->setFrame( true );
    smr->resize( smr->sizeHint() );
    mGrid->addWidget( smr, 2, 1 );

    lmt = new QLabel( i18n( "Top:" ), grp2 );
    lmt->resize( lmt->sizeHint() );
    mGrid->addWidget( lmt, 3, 0 );

    smt = new QLineEdit( grp2 );

    smt->setText( "0.00" );
    smt->setMaxLength( 5 );
    smt->setEchoMode( QLineEdit::Normal );
    smt->setFrame( true );
    smt->resize( smt->sizeHint() );
    mGrid->addWidget( smt, 4, 0 );

    lmb = new QLabel( i18n( "Bottom:" ), grp2 );
    lmb->resize( lmb->sizeHint() );
    mGrid->addWidget( lmb, 3, 1 );

    smb = new QLineEdit( grp2 );

    smb->setText( "0.00" );
    smb->setMaxLength( 5 );
    smb->setEchoMode( QLineEdit::Normal );
    smb->setFrame( true );
    smb->resize( smb->sizeHint() );
    mGrid->addWidget( smb, 4, 1 );

    //// ### frame margins are currently not implemented
    sml->setEnabled( false );
    smr->setEnabled( false );
    smt->setEnabled( false );
    smb->setEnabled( false );
    // margins are not implemented yet
    grp2->hide();

    mGrid->addRowSpacing( 0, KDialog::spacingHint() + 5 );

    /// ##### grid4->addWidget( grp2, ++row, 0 );

    double l, r, t, b;
    doc->getFrameMargins( l, r, t, b );
    sml->setText( QString::number( QMAX(0.00,KWUnit::userValue( l, doc->getUnit() ) ) ));
    smr->setText( QString::number( QMAX(0.00,KWUnit::userValue( r, doc->getUnit() ) ) ));
    smt->setText( QString::number( QMAX(0.00,KWUnit::userValue( t, doc->getUnit() ) ) ));
    smb->setText( QString::number( QMAX(0.00,KWUnit::userValue( b, doc->getUnit() ) ) ));

    sx->setValidator( new QDoubleValidator( sx ) );
    sy->setValidator( new QDoubleValidator( sy ) );
    smb->setValidator( new QDoubleValidator( smb ) );
    sml->setValidator( new QDoubleValidator( sml ) );
    smr->setValidator( new QDoubleValidator( smr ) );
    smt->setValidator( new QDoubleValidator( smt ) );
    sh->setValidator( new QDoubleValidator( sh ) );
    sw->setValidator( new QDoubleValidator( sw ) );

    bool disable = false;
    if ( doc->isOnlyOneFrameSelected() )
    {
        KWFrame * theFrame = doc->getFirstSelectedFrame();

        oldX = KWUnit::userValue( theFrame->x(), doc->getUnit() );
        oldY = KWUnit::userValue( (theFrame->y() - (theFrame->pageNum() * doc->ptPaperHeight())), doc->getUnit() );
        oldW = KWUnit::userValue( theFrame->width(), doc->getUnit() );
        oldH = KWUnit::userValue( theFrame->height(), doc->getUnit() );

        sx->setText( QString::number( oldX ) );
        sy->setText( QString::number( oldY ) );
        sw->setText( QString::number( oldW ) );
        sh->setText( QString::number( oldH ) );

        // QString::number leads to some rounding !
        oldX = sx->text().toDouble();
        oldY = sy->text().toDouble();
        oldW = sw->text().toDouble();
        oldH = sh->text().toDouble();

        if ( theFrame->getFrameSet()->getGroupManager() )
            floating->setText( i18n( "Table is inline" ) );

        floating->setChecked( frameSetFloating );

        if ( frameSetFloating )
            slotFloatingToggled( true );

        // Can't change geometry of main WP frame or headers/footers
        if ( theFrame->getFrameSet()->isHeaderOrFooter() ||
             theFrame->getFrameSet()->isMainFrameset() )
            disable = true;
    }
    else
        disable = true;

    if ( disable )
    {
        grp2->hide( );
        sx->setEnabled( false );
        sy->setEnabled( false );
        sw->setEnabled( false );
        sh->setEnabled( false );
        floating->setEnabled( false );
    }

    //kdDebug() << "setup tab 4 exit"<<endl;
}




void KWFrameDia::uncheckAllRuns()
{
    rRunNo->setChecked( false );
    rRunBounding->setChecked( false );
    rRunContur->setChecked( false );
}

void KWFrameDia::runNoClicked()
{
    uncheckAllRuns();
    rRunNo->setChecked( true );
}

void KWFrameDia::runBoundingClicked()
{
    uncheckAllRuns();
    rRunBounding->setChecked( true );
}

void KWFrameDia::runConturClicked()
{

    uncheckAllRuns();
    rRunContur->setChecked( true );
}

// Called when "reconnect" or "no followup" is checked
void KWFrameDia::setFrameBehaviourInputOn() {
    if ( tab4 && floating->isChecked() )
        return;
    if( rAppendFrame && rResizeFrame && rNoShow && !rAppendFrame->isEnabled() ) {
        if(frameBehaviour== KWFrame::AutoExtendFrame) {
            rResizeFrame->setChecked(true);
        } else if (frameBehaviour== KWFrame::AutoCreateNewFrame) {
            rAppendFrame->setChecked(true);
        } else {
            rNoShow->setChecked(true);
        }
        rResizeFrame->setEnabled(true);
        rAppendFrame->setEnabled(true);
        rNoShow->setEnabled(true);
    }
}

// Called when "place a copy" is checked
void KWFrameDia::setFrameBehaviourInputOff() {
    if ( tab4 && floating->isChecked() )
        return;
    if( rAppendFrame && rResizeFrame && rNoShow && rAppendFrame->isEnabled() ) {
        if(rResizeFrame->isChecked()) {
            frameBehaviour=KWFrame::AutoExtendFrame;
        } else if ( rAppendFrame->isChecked()) {
            frameBehaviour=KWFrame::AutoCreateNewFrame;
        } else {
            frameBehaviour=KWFrame::Ignore;
        }
        // In "Place a copy" mode, we can't have "create new page if text too long"
        if ( rAppendFrame->isChecked() )
            rNoShow->setChecked(true);
        rAppendFrame->setEnabled(false);
        rResizeFrame->setEnabled(true);
        rNoShow->setEnabled(true);
    }
}

void KWFrameDia::slotFloatingToggled(bool b)
{
    grp1->setEnabled( !b ); // Position doesn't make sense for a floating frame
    if (tab1 && rAppendFrame && rResizeFrame && rNoShow ) {
        cbCopy->setEnabled( !b ); // 'copy' irrelevant for floating frames.
        if ( rAppendFrame )
        {
            rAppendFrame->setEnabled( !b ); // 'create new page' irrelevant for floating frames.
            if ( b && rAppendFrame->isChecked() )
                rNoShow->setChecked( true );
        }
        enableOnNewPageOptions();
        if ( b ) {
            noFollowup->setChecked( true );
            cbCopy->setChecked( false );
        } else {
            // Revert to non-inline frame stuff
            rResizeFrame->setEnabled(true);
            rAppendFrame->setEnabled(true);
            rNoShow->setEnabled(true);
        }
    }

    enableRunAround();
}

// Enable or disable the "on new page" options
void KWFrameDia::enableOnNewPageOptions()
{
    if ( tab1 )
    {
        bool f = tab4 && floating->isChecked();
        // 'what happens on new page' is irrelevant for floating frames
        reconnect->setEnabled( !f );
        noFollowup->setEnabled( !f );
        copyRadio->setEnabled( !f );

        if( frameType != FT_TEXT )
            reconnect->setEnabled( false );
        else
        {
            KWFrameSet *fs = frame->getFrameSet(); // 0 when creating a frame
            if ( fs && fs->isHeaderOrFooter() )
            {
                reconnect->setEnabled( false );
                noFollowup->setEnabled( false );
            }
        }
    }
}

void KWFrameDia::enableRunAround()
{
    if ( tab2 )
    {
        if ( tab4 && floating->isChecked() )
            runGroup->setEnabled( false ); // Runaround options don't make sense for floating frames
        else
        {
            KWFrameSet *fs = frame->getFrameSet();
            if ( fs )
                runGroup->setEnabled( !frameSetFloating && !fs->isMainFrameset() && !fs->isHeaderOrFooter() );
            else
                runGroup->setEnabled( true );
        }
    }
}

bool KWFrameDia::applyChanges()
{
    //kdDebug() << "KWFrameDia::applyChanges"<<endl;
    ASSERT(frame);
    if ( !frame )
        return false;

    if ( tab1 )
    {
        // Copy
        frame->setCopy( cbCopy->isChecked() );

        // FrameBehaviour
        if ( frameType == FT_TEXT )
        {
            if(rResizeFrame->isChecked())
                frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
            else if ( rAppendFrame->isChecked())
                frame->setFrameBehaviour(KWFrame::AutoCreateNewFrame);
            else
                frame->setFrameBehaviour(KWFrame::Ignore);
        }

        // NewFrameBehaviour
        if( reconnect && reconnect->isChecked() )
            frame->setNewFrameBehaviour(KWFrame::Reconnect);
        else if ( noFollowup->isChecked() )
            frame->setNewFrameBehaviour(KWFrame::NoFollowup);
        else
            frame->setNewFrameBehaviour(KWFrame::Copy);

        if ( cbAspectRatio && frame->getFrameSet() )
            static_cast<KWPictureFrameSet *>( frame->getFrameSet() )->setKeepAspectRatio( cbAspectRatio->isChecked() );
    }

    if ( tab3 )
    {
        // Frame/Frameset belonging, and frameset naming
        // We have basically three cases:
        // * Creating a frame (fs==0), and creating a frameset ('*' selected)
        // * Creating a frame (fs==0), and attaching to an existing frameset (other)
        // * Editing a frame (fs!=0), possibly changing the frameset attachment, possibly renaming the frameset...

        QString str = lFrameSList->currentItem() ? lFrameSList->currentItem()->text( 0 ) : QString::null;
        bool createFrameset = ( str[ 0 ] == '*' );
        if ( createFrameset )
            str.remove( 0, 1 );
        int _num = str.toInt() - 1;
        KWFrameSet * fs = frame->getFrameSet();

        QString name = eFrameSetName->text();
        if ( name.isEmpty() ) // Don't allow empty names
            name = doc->generateFramesetName( i18n( "Text Frameset %1" ) );

        if ( fs || createFrameset ) // Last or first case -> check frameset name unicity
        {
            // Note: this isn't recursive, so it won't find table cells.
            QListIterator<KWFrameSet> fit = doc->framesetsIterator();
            for ( ; fit.current() ; ++fit )
                if ( fit.current()->getName() == name &&
                     fs /*which is 0L when creating*/ != fit.current() &&
                     !fit.current()->isDeleted() ) // Allow to reuse a deleted frameset's name
                {
                    if ( createFrameset )
                        KMessageBox::sorry( this,
                                            i18n( "A new frameset with the name '%1'\n"
                                                  "can not be made because a frameset with that name\n"
                                                  "already exists. Please enter another name or select\n"
                                                  "an existing frameset from the list.").arg(name));
                    else
                        KMessageBox::sorry( this,
                                            i18n( "A frameset with the name '%1'\n"
                                                  "already exists. Please enter another name.\n" ).arg(name) );
                    eFrameSetName->setText(oldFrameSetName);
                    return false;
                }
        }

        // Third case (changing frame attachment)
        if ( fs &&
             ! (static_cast<unsigned int>( _num ) < doc->getNumFrameSets() &&
                fs == doc->getFrameSet(_num)))
        {
            // Check if last frame
            if ( fs->getNumFrames() == 1 )
            {
                kdDebug() << "KWFrameDia::applyChanges " << fs->getName() << " has only one frame" << endl;
                ASSERT( fs->type() == FT_TEXT );
                KWTextFrameSet * textfs = static_cast<KWTextFrameSet*>( fs );
                QTextParag * parag = textfs->textDocument()->firstParag();
                bool isEmpty = parag->next() == 0L && parag->length() == 1;
                if ( !isEmpty )
                {
                    int result = KMessageBox::warningContinueCancel(this,
                                                                    i18n( "You are about to reconnect the last Frame of the\n"
                                                                          "Frameset '%1'.\n"
                                                                          "The contents of this Frameset will not appear\n"
                                                                          "anymore!\n\n"
                                                                          "Are you sure you want to do that?").arg(fs->getName()),
                                                                    i18n("Reconnect Frame"), i18n("&Reconnect"));
                    if (result != KMessageBox::Continue)
                        return false;
                }
            }

            kdDebug() << "KWFrameDia::applyChanges detaching frame from frameset " << fs->getName() << endl;
            // Detach frame from its frameset (re-attach is done afterwards)
            fs->delFrame( frame, FALSE );
            // TODO undo/redo ?
        }
        // Do not use 'fs' past this, the above might have changed the frame's frameset

        if(frame->getFrameSet() == 0L) { // if there is no frameset (anymore)
            if ( createFrameset )
            {
                kdDebug() << "KWFrameDia::applyChanges creating a new frameset" << endl;
                KWTextFrameSet *_frameSet = new KWTextFrameSet( doc, name );
                _frameSet->addFrame( frame );
                doc->addFrameSet( _frameSet );
                KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create text frame"), frame) ;
                doc->addCommand(cmd);
            }
            else
            {
                kdDebug() << "KWFrameDia::applyChanges attaching to frameset " << _num << endl;
                // attach frame to frameset number _num
                KWFrameSet * newFs = doc->getFrameSet( _num );
                ASSERT( newFs );
                newFs->addFrame( frame );
                // TODO undo/redo ?
            }
        }
        else
        {
            // Rename frameset
            frame->getFrameSet()->setName( name );
        }
    }

    if ( tab2 )
    {
        // Run around
        if ( rRunNo->isChecked() )
            frame->setRunAround( KWFrame::RA_NO );
        else if ( rRunBounding->isChecked() )
            frame->setRunAround( KWFrame::RA_BOUNDINGRECT );
        else if ( rRunContur->isChecked() )
            frame->setRunAround( KWFrame::RA_SKIP );

        frame->setRunAroundGap( KWUnit::fromUserValue( eRGap->text().toDouble(), doc->getUnit() ) );
    }

    if ( tab4 )
    {
        // The floating attribute applies to the whole frameset...
        KWFrameSet * fs = frame->getFrameSet();
        KWFrameSet * parentFs = fs->getGroupManager() ? fs->getGroupManager() : fs;

        // Floating
        if ( floating->isChecked() && !parentFs->isFloating() )
        {
            KMacroCommand * macroCmd = new KMacroCommand( i18n("Make FrameSet Inline") );

            QList<FrameIndex> frameindexList;
            QList<FrameResizeStruct> frameindexMove;

            FrameIndex *index=new FrameIndex( frame );
            FrameResizeStruct *move=new FrameResizeStruct;

            move->sizeOfBegin=frame->normalize();

            // turn non-floating frame into floating frame
            KWFrameSetFloatingCommand *cmd = new KWFrameSetFloatingCommand( i18n("Make FrameSet Inline"), parentFs, true );
            cmd->execute();

            move->sizeOfEnd=frame->normalize();

            frameindexList.append(index);
            frameindexMove.append(move);

            KWFrameMoveCommand *cmdMoveFrame = new KWFrameMoveCommand( i18n("Move Frame"), frameindexList, frameindexMove );

            macroCmd->addCommand(cmdMoveFrame);
            macroCmd->addCommand(cmd);
            doc->addCommand(macroCmd);

        }
        else if ( !floating->isChecked() && parentFs->isFloating() )
        {
            // turn floating-frame into non-floating frame
            KWFrameSetFloatingCommand *cmd = new KWFrameSetFloatingCommand( i18n("Make FrameSet Non-Inline"), parentFs, false );
            doc->addCommand(cmd);
            cmd->execute();
        }

        if ( doc->isOnlyOneFrameSelected() && ( doc->processingType() == KWDocument::DTP ||
                                                ( doc->processingType() == KWDocument::WP &&
                                                  doc->getFrameSetNum( frame->getFrameSet() ) > 0 ) ) ) {
            if ( oldX != sx->text().toDouble() || oldY != sy->text().toDouble() || oldW != sw->text().toDouble() || oldH != sh->text().toDouble() ) {
                //kdDebug() << "Old geom: " << oldX << ", " << oldY<< " " << oldW << "x" << oldH << endl;
                //kdDebug() << "New geom: " << sx->text().toDouble() << ", " << sy->text().toDouble()
                //          << " " << sw->text().toDouble() << "x" << sh->text().toDouble() << endl;

                double px = KWUnit::fromUserValue( QMAX( sx->text().toDouble(), 0 ), doc->getUnit() );
                double py = KWUnit::fromUserValue( (QMAX( sy->text().toDouble(),0)) + (frame->pageNum() * doc->ptPaperHeight()), doc->getUnit());
                double pw = KWUnit::fromUserValue( QMAX( sw->text().toDouble(), 0 ), doc->getUnit() );
                double ph = KWUnit::fromUserValue( QMAX( sh->text().toDouble(), 0 ), doc->getUnit() );

                FrameIndex index( frame );
                FrameResizeStruct tmpResize;
                tmpResize.sizeOfBegin = frame->normalize();
                KoRect rect( px, py, pw, ph );
                 if( !doc->isOutOfPage( rect , 0 ) )
                        frame->setRect( px, py, pw, ph );
                        // else TODO message box after 1.1

                // TODO apply page limits?
                tmpResize.sizeOfEnd = frame->normalize();
                KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), index, tmpResize ) ;
                doc->addCommand(cmd);
                doc->frameChanged( frame );
            }
        }

        double u1, u2, u3, u4;
        u1=KWUnit::fromUserValue( QMAX(sml->text().toDouble(),0), doc->getUnit() );
        u2=KWUnit::fromUserValue( QMAX(smr->text().toDouble(),0), doc->getUnit() );
        u3=KWUnit::fromUserValue( QMAX(smt->text().toDouble(),0), doc->getUnit() );
        u4=KWUnit::fromUserValue( QMAX(smb->text().toDouble(),0), doc->getUnit() );
        doc->setFrameMargins( u1, u2, u3, u4 );
    }

    updateFrames();
    return true;
}

void KWFrameDia::updateFrames()
{
    QList<KWFrame> frames=doc->getSelectedFrames();

    doc->updateAllFrames();
    doc->layout();

    if(frames.count()==1)
    {
        KWFrame *theFrame = frames.first();
        if(theFrame->isSelected())
            theFrame->updateResizeHandles();
    }
    doc->repaintAllViews();
}

void KWFrameDia::slotOk()
{
    if (applyChanges())
    {
        KDialogBase::slotOk();
    }
}

void KWFrameDia::connectListSelected( QListViewItem *item )
{
    if ( !item )
        return;

    QString str = item->text( 0 );
    bool createFrameset = ( str[ 0 ] == '*' );
    if ( createFrameset )
    {
        // Allow naming new frameset
        eFrameSetName->setEnabled( TRUE );
    }
    else if ( frame && frame->getFrameSet() )
    {
        int _num = str.toInt() - 1;
        // Allow renaming an existing frameset
        // ( Disabled when changing the frameset connection )
        eFrameSetName->setEnabled( doc->getFrameSet( _num ) == frame->getFrameSet() );
    }
    else
        eFrameSetName->setEnabled( FALSE );
}

