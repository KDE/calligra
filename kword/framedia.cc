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
#include "kwtextframeset.h"
#include "framedia.h"
#include "framedia.moc"
#include "defs.h"
#include "kwcommand.h"
#include "kwtableframeset.h"

#include <koSetPropCommand.h>

#include <knuminput.h>
#include <klocale.h>
#include <kiconloader.h>

#include <qwhatsthis.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qlistview.h>
#include <qbuttongroup.h>
#include <qhbox.h>
#include <qheader.h>
#include <kmessagebox.h>
#include <knumvalidator.h>
#include <kcolorbutton.h>

#include <stdlib.h>
#include <limits.h>

#include <kdebug.h>

/******************************************************************/
/* Class KWBrushStylePreview - only used by kwframestylemanager   */
/******************************************************************/
KWBrushStylePreview::KWBrushStylePreview( QWidget* parent )
    : QFrame(parent)
{
}

void KWBrushStylePreview::drawContents( QPainter* painter )
{
    painter->save();
    painter->translate( contentsRect().x(), contentsRect().y() );
    painter->fillRect( contentsRect(), colorGroup().base() ); // in case of a transparent brush
    painter->fillRect( contentsRect(), brush );
    painter->restore();
}


/******************************************************************/
/* Class: KWFrameDia                                              *
 *
 *  TAB Frame Options
 *      Set options dependent of frametype
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
    : KDialogBase( Tabbed, QString::null, Ok | Apply| Cancel, Ok, parent, "framedialog", true)
{
    noSignal=false;
    frame = _frame;
    if(frame==0) {
        kdDebug() << "ERROR: KWFrameDia::constructor no frame.."<<endl;
        return;
    }
    setCaption( i18n( "Frame Properties for %1" ).arg( frame->frameSet()->getName() ) );
    KWFrameSet *fs = frame->frameSet()->getGroupManager();
    if(fs==0L) fs=frame->frameSet();
    frameType = fs->type();
    frameSetFloating = fs->isFloating();
    frameSetProtectedSize = fs->isProtectSize();
    m_mainFrameSetIncluded = fs->isMainFrameset();
    m_defaultFrameSetIncluded = fs->isMainFrameset() || fs->isHeaderOrFooter() || fs->isFootEndNote();

    doc = 0;
    init();
}

/* Contructor when the dialog is used on creation of frame */
KWFrameDia::KWFrameDia( QWidget* parent, KWFrame *_frame, KWDocument *_doc, FrameSetType _ft )
    : KDialogBase( Tabbed, i18n("Frame Properties for New Frame"), Ok | Cancel, Ok, parent, "framedialog", true)
{
    noSignal=false;
    frameType=_ft;
    doc = _doc;
    frame= _frame;
    frameSetFloating = false;
    frameSetProtectedSize = false;
    m_mainFrameSetIncluded = false;
    m_defaultFrameSetIncluded = false;
    if(frame==0) {
        kdDebug() << "ERROR: KWFrameDia::constructor no frame.."<<endl;
        return;
    }
    init();
}

KWFrameDia::KWFrameDia( QWidget *parent, QPtrList<KWFrame> listOfFrames) : KDialogBase( Tabbed, i18n("Frames Properties"), Ok | Apply | Cancel, Ok, parent, "framedialog", true) , allFrames() {
    noSignal=false;

    frame=0L;
    tab1 = tab2 = tab3 = tab4 = tab5 = 0;

    KWFrame *f=listOfFrames.first();
    if(f==0) {
        kdDebug() << "ERROR: KWFrameDia::constructor no frames.."<<endl;
        return;
    }
    if ( listOfFrames.count() == 1 )
        setCaption( i18n( "Frame Settings for %1" ).arg( f->frameSet()->getName() ) );

    KWFrameSet *fs = f->frameSet()->getGroupManager();
    if(fs==0L) fs=f->frameSet();
    frameType = fs->type();
    bool frameTypeUnset=true;
    doc = fs->kWordDocument();

    if( !fs->isMainFrameset() ) { // don't include the main fs.
        allFrames.append(f);
        frameTypeUnset=false;
    }
    f=listOfFrames.next();
    while(f) {
        fs = f->frameSet()->getGroupManager();
        if(fs==0L) fs=f->frameSet();
        if(doc->processingType() != KWDocument::WP || doc->frameSet(0) != fs) { // don't include the main fs.
            if(!frameTypeUnset && frameType != fs->type()) frameType= FT_TEXT;
            if(frameTypeUnset) {
                frameType = fs->type();
                frameTypeUnset = false;
            } else if(frameType != fs->type()) frameType= FT_TEXT;
            allFrames.append(f);
        }
        f=listOfFrames.next();
    }
    if(allFrames.count()==0)
        allFrames.append(listOfFrames.first());

    // Now that allFrames is set, calculate m_mainFrameSetIncluded and m_defaultFrameSetIncluded
    m_mainFrameSetIncluded = false;
    m_defaultFrameSetIncluded = false;
    for ( f = allFrames.first(); f; f = allFrames.next() ) {
        fs = f->frameSet();
        if ( !m_mainFrameSetIncluded )
            m_mainFrameSetIncluded = fs->isMainFrameset();
        if ( !m_defaultFrameSetIncluded )
            m_defaultFrameSetIncluded = fs->isMainFrameset() || fs->isHeaderOrFooter() || fs->isFootEndNote();
    }


    init();
}

void KWFrameDia::init() {

    tab1 = tab2 = tab3 = tab4 = tab5 = 0;
    sw = sh = 0L;
    cbAspectRatio=0L;
    KWFrameSet *fs=0;
    if(frame) {
        fs = frame->frameSet(); // 0 when creating a frame
        KoRect r = frame->normalize();
        frame->setRect( r.x(), r.y(), r.width(), r.height() );
    }
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
        setupTab5();
        setupTab4();
    }
    else if ( fs && (fs->isHeaderOrFooter() || fs->isFootEndNote()) )
    {
        setupTab1();
        if ( !fs->isMainFrameset() && !fs->isHeaderOrFooter() && !fs->isFootEndNote())
            setupTab2();
        setupTab4();
        setupTab5();
    }
    else if(frameType == FT_TEXT)
    {
        setupTab1();
        if ( fs && !fs->isMainFrameset() && !fs->isHeaderOrFooter() && !fs->isFootEndNote())
            setupTab2();
        else if ( !fs )
            setupTab2();
        setupTab3();
        setupTab4();
        setupTab5();
        if(! fs) // first creation
            showPage(2);
    }
    else if(frameType == FT_PICTURE
#if 0 // KWORD_HORIZONTAL_LINE
        || frameType == FT_HORZLINE
#endif
        )
    {
        setupTab1();
        if ( frameType == FT_PICTURE )
            setupTab2();
        if(frame)       // not for multiframe dia
            setupTab4();
        showPage(1); // while options are not implemented..
    }
    else if(frameType == FT_PART)
    {
        setupTab2();
        if(frame)       // not for multiframe dia
            setupTab4();
    }
    else if(frameType == FT_FORMULA)
    {
        setupTab1();
        setupTab2();
        if(frame)       // not for multiframe dia
            setupTab4();
        showPage(1); // while options are not implemented..
    }
    else if(frameType == FT_TABLE)
    {
        setupTab4();
        setupTab5();
    }
    setInitialSize( QSize(550, 400) );
}

void KWFrameDia::setupTab1(){ // TAB Frame Options
    //kdDebug() << "setup tab 1 Frame options"<<endl;
    tab1 = addPage( i18n("Options") );
    int columns = 0;
    if(frameType == FT_FORMULA || frameType == FT_PICTURE
#if 0 // KWORD_HORIZONTAL_LINE
        || frameType == FT_HORZLINE
#endif
        )
        columns = 1;
    else if(frameType == FT_TEXT)
        columns = 2;

    grid1 = new QGridLayout( tab1, 0 /*auto create*/, columns, KDialog::marginHint(), KDialog::spacingHint() );

    // Options for all types of frames
    cbCopy = new QCheckBox( i18n("Frame is a copy of the previous frame"),tab1 );
    grid1->addWidget(cbCopy,1,0);

    if(frame) {
        cbCopy->setChecked( frame->isCopy() );
        cbCopy->setEnabled( frame->frameSet() && frame->frameSet()->frame( 0 ) != frame ); // First one can't be a copy
    } else { // list of frames as input.
        KWFrame *f=allFrames.first();
        bool show=true;
        bool enabled=f->frameSet() && f->frameSet()->frame( 0 ) != f; // First one can't be a copy
        bool checked=f->isCopy();
        f=allFrames.next();
        while(f) {
            enabled=enabled || (f->frameSet() && f->frameSet()->frame( 0 ) != f);
            if(checked != f->isCopy()) show=false;
            f=allFrames.next();
        }
        if(! show) {
            cbCopy->setTristate();
            cbCopy->setNoChange();
        }
        else cbCopy->setChecked(checked);
        cbCopy->setEnabled( enabled );
    }

    // Well, for images, formulas etc. it doesn't make sense to activate 'is copy'. What else would it show ?
    if(frameType!=FT_TEXT)
        cbCopy->setEnabled( false );

    int row = 2;
    int column = 0;

    // Picture frame
    if(frameType==FT_PICTURE
#if 0 // KWORD_HORIZONTAL_LINE
        || frameType == FT_HORZLINE
#endif
        )
    {
        cbAspectRatio = new QCheckBox (i18n("Retain original aspect ratio"),tab1);
        connect( cbAspectRatio, SIGNAL(toggled(bool)),
                 this, SLOT(slotKeepRatioToggled(bool)));
        bool show=true;
        bool on=true;
        if(frame) {
            if ( frame->frameSet() )
                on= static_cast<KWPictureFrameSet *>( frame->frameSet() )->keepAspectRatio();
        } else {
            KWFrame *f=allFrames.first();
            KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
            if(fs)
                on=fs->keepAspectRatio();
            f=allFrames.next();
            while(f) {
                KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
                if(fs)
                    if(on != fs->keepAspectRatio()) {
                        show=false;
                        break;
                    }
                f=allFrames.next();
            }
        }
        cbAspectRatio->setChecked( on );
        if(! show) {
            cbAspectRatio->setTristate();
            cbAspectRatio->setNoChange();
        }
        grid1->addWidget(cbAspectRatio, row, 0);
        ++row;
    }
    else
        cbAspectRatio = 0L;

    // Text frame
    if(frameType==FT_TEXT)
    {
        // AutoCreateNewFrame policy.
        endOfFrame = new QGroupBox(i18n("If Text is Too Long for Frame"), tab1 );
        grid1->addWidget( endOfFrame, row, 0 );

        eofGrid= new QGridLayout (endOfFrame, 4, 1, KDialog::marginHint(), KDialog::spacingHint());
        rAppendFrame = new QRadioButton( i18n( "Create a new page" ), endOfFrame );
        QWhatsThis::add( rAppendFrame, "<b>Create a new page:</b><br/> if there is too "
            "much text for this text frame, a new page will be created and, "
            "since \"Reconnect frame to current flow\" is the only possible "
            "option together with this one, "
            "the new page will have a frame for showing the additional text." );
        eofGrid->addWidget( rAppendFrame, 1, 0 );

        rResizeFrame = new QRadioButton( i18n( "Resize last frame" ), endOfFrame );
        QWhatsThis::add( rResizeFrame, "<b>Resize last frame:</b><br/> "
            "if there is too much text for this text frame, "
            "the frame will resize itself (increasing its height) as much as it needs, "
            "to be able to contain all the text. More precisely, when the frameset has "
            "multiple chained frames, it's always the last one which will be resized." );
        eofGrid->addWidget( rResizeFrame, 2, 0 );

        rNoShow = new QRadioButton( i18n( "Don't show the extra text" ), endOfFrame );
        QWhatsThis::add( rNoShow, "<b>Don't show the extra text:</b><br/> "
            "if there is too much text for this text frame, nothing happens "
            "automatically. Initially the extra text won't appear. One possibility "
            "then is to resize the frame manually. The other possibility is, with the option "
            "\"Reconnect frame to current flow\" selected, to create a new page "
            "which will then have a followup frame with the extra text." );
        eofGrid->addWidget( rNoShow, 3, 0 );
        QButtonGroup *grp = new QButtonGroup( endOfFrame );
        grp->hide();
        grp->setExclusive( true );
        grp->insert( rAppendFrame );
        grp->insert( rResizeFrame );
        grp->insert( rNoShow );

        eofGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );
        KWFrame::FrameBehavior fb;
        bool show=true;
        if(frame) {
            fb = frame->frameBehavior();
        } else {
            KWFrame *f=allFrames.first();
            fb = f->frameBehavior();
            f=allFrames.next();
            while(f) {
                if(fb != f->frameBehavior()) {
                    show=false;
                    break;
                }
                f=allFrames.next();
            }
        }
        if(show) {
            if(fb == KWFrame::AutoExtendFrame) {
                rResizeFrame->setChecked(true);
            } else if (fb == KWFrame::AutoCreateNewFrame) {
                rAppendFrame->setChecked(true);
            } else {
                rNoShow->setChecked(true);
            }
        }
        column++;
    } else {
        rResizeFrame = 0L;
        rAppendFrame = 0L;
        rNoShow = 0L;
    }

    // NewFrameBehavior - now for all type of frames
    onNewPage = new QGroupBox(i18n("On New Page Creation"),tab1);
    grid1->addWidget( onNewPage, row, column );

    onpGrid = new QGridLayout( onNewPage, 4, 1, KDialog::marginHint(), KDialog::spacingHint() );
    reconnect = new QRadioButton (i18n ("Reconnect frame to current flow"), onNewPage);
    QWhatsThis::add(reconnect, i18n("<b>Reconnect frame to current flow:</b><br/>"
        "When a new page is created, a new frame will be created for this "
        "frameset, so that the text can flow from one page to the next if necessary. "
        "This is what happens for the \"main text frameset\", but this option makes it possible "
        "to choose the same behavior for other framesets, for instance in magazine layouts."));
    if ( rResizeFrame )
        connect( reconnect, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOn() ) );
    onpGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );
    onpGrid->addWidget( reconnect, 1, 0 );

    noFollowup = new QRadioButton (i18n ("Do not create a followup frame"), onNewPage);
    QWhatsThis::add(noFollowup, i18n("<b>Do not create a followup frame:</b><br/>"
        "When a new page is created, no frame will be created for this frameset."));
    if ( rResizeFrame )
        connect( noFollowup, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOn() ) );
    onpGrid->addWidget( noFollowup, 2, 0 );

    copyRadio= new QRadioButton (i18n ("Place a copy of this frame"), onNewPage);
    QWhatsThis::add(copyRadio, i18n("<b>Place a copy of this frame:</b><br/>"
        "When a new page is created, a frame will be created for this frameset, "
        "which will always show the exact same thing as the frame on the previous "
        "page. This is what happens for headers and footers, but this option "
        "makes it possible to choose the same behavior for other framesets, for "
        "instance a company logo and/or title that should appear exactly the same on every page."));
    if ( rResizeFrame )
        connect( copyRadio, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOff() ) );
    onpGrid->addWidget( copyRadio, 3, 0);

    enableOnNewPageOptions();

    QButtonGroup *grp2 = new QButtonGroup( onNewPage );
    grp2->hide();
    grp2->setExclusive( true );
    grp2->insert( reconnect );
    grp2->insert( noFollowup );
    grp2->insert( copyRadio );
    grid1->addRowSpacing( row, onNewPage->height());
    KWFrame::NewFrameBehavior nfb;
    bool show=true;
    if(frame) {
        nfb = frame->newFrameBehavior();
    } else {
        KWFrame *f=allFrames.first();
        nfb = f->newFrameBehavior();
        f=allFrames.next();
        while(f) {
            if(nfb != f->newFrameBehavior()) {
                show=false;
                break;
            }
            f=allFrames.next();
        }
    }
    if(show) {
        if(nfb == KWFrame::Reconnect) {
            reconnect->setChecked(true);
        } else if(nfb == KWFrame::NoFollowup) {
            noFollowup->setChecked(true);
        } else {
            copyRadio->setChecked(true);
            setFrameBehaviorInputOff();
        }
    }


    // SideHeads definition - for text frames only
    if( false && frameType == FT_TEXT ) // disabled in the GUI for now! (TZ June 2002)
    {
        row++;
        sideHeads = new QGroupBox(i18n("SideHead Definition"),tab1);
        sideHeads->setEnabled(false); //###
        grid1->addWidget(sideHeads, row, 0);

        sideGrid = new QGridLayout( sideHeads, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );
        sideTitle1 = new QLabel ( i18n("Size (%1):").arg(doc->unitName()),sideHeads);
        sideTitle1->resize(sideTitle1->sizeHint());
        sideGrid->addWidget(sideTitle1,1,0);
        sideWidth= new QLineEdit(sideHeads,"");
        sideWidth->setMaxLength(6);
        sideGrid->addWidget(sideWidth,1,1);
        sideTitle2 = new QLabel( i18n("Gap size (%1):").arg(doc->unitName()),sideHeads);
        sideTitle2->resize(sideTitle2->sizeHint());
        sideGrid->addWidget(sideTitle2,2,0);
        sideGap = new QLineEdit(sideHeads,"");
        sideGap->setMaxLength(6);
        sideGrid->addWidget(sideGap,2,1);
        sideAlign = new QComboBox (false,sideHeads);
        sideAlign->setAutoResize(false);
        sideAlign->insertItem ( i18n("Left"));
        sideAlign->insertItem ( i18n("Right"));
        sideAlign->insertItem ( i18n("Closest to Binding"));
        sideAlign->insertItem ( i18n("Closest to Page Edge"));
        sideAlign->resize(sideAlign->sizeHint());
        sideGrid->addMultiCellWidget(sideAlign,3,3,0,1);
        sideGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );

        // init for sideheads.
        sideWidth->setText("0");
        sideWidth->setValidator( new KFloatValidator(0,9999,true, sideWidth) );

        sideGap->setText("0");
        sideGap->setValidator( new KFloatValidator(0,9999,true, sideGap) );
        // add rest of sidehead init..
    }

    cbAllFrames = new QCheckBox (i18n("Changes will be applied to all frames in frameset"),tab1);
    cbAllFrames->setChecked(frame!=0L);
    row++;
    grid1->addMultiCellWidget(cbAllFrames,row,row, 0, 1);
    cbProtectContent = new QCheckBox( i18n("Protect content"), tab1);
    QWhatsThis::add(cbProtectContent, i18n("<b>Protect content:</b><br/>"
        "Disallow changes to be made to the contents of the frame(s)."));
    connect( cbProtectContent, SIGNAL(toggled ( bool ) ), this, SLOT(slotProtectContentChanged( bool )));
    row++;
    grid1->addMultiCellWidget(cbProtectContent,row,row, 0, 1);
    if( frameType != FT_TEXT || frame!=0 && frame->frameSet()==0) {
        cbAllFrames->setChecked(false);
        cbAllFrames->hide();
        cbProtectContent->setChecked( false );
        cbProtectContent->hide();
    }
    else if ( frameType == FT_TEXT /*&& frame!=0 && frame->frameSet()*/ )
    {
        bool show=true;
        bool on=true;
        if(frame)
        {
            if ( frame->frameSet() )
                on= static_cast<KWTextFrameSet *>(frame->frameSet() )->textObject()->protectContent();
        }
        else
        {
            KWFrame *f=allFrames.first();
            KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *> (f->frameSet());
            if(fs)
                on=fs->textObject()->protectContent();
            f=allFrames.next();
            while(f) {
                KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *> (f->frameSet());
                if(fs)
                {
                    if(on != fs->textObject()->protectContent())
                    {
                        show=false;
                        break;
                    }
                }
                f=allFrames.next();
            }
        }
        cbProtectContent->setChecked( on );
        if(! show) {
            cbProtectContent->setTristate();
            cbProtectContent->setNoChange();
        }
    }

    for(int i=0;i < row;i++)
        grid1->setRowStretch( i, 0 );
    grid1->setRowStretch( row + 1, 1 );
}

void KWFrameDia::setupTab2() { // TAB Text Runaround
    tab2 = addPage( i18n( "Text Run Around" ) );

    QVBoxLayout *tabLayout = new QVBoxLayout( tab2, 11, 6, "tabLayout");

    // First groupbox
    runGroup = new QButtonGroup(  i18n( "Layout of Text in Other Frames" ), tab2);
    runGroup->setColumnLayout( 0, Qt::Vertical );
    runGroup->layout()->setSpacing( KDialog::spacingHint() );
    runGroup->layout()->setMargin( KDialog::marginHint() );
    QGridLayout *groupBox1Layout = new QGridLayout( runGroup->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    rRunNo = new QRadioButton( i18n( "Text will run &through this frame" ), runGroup );
    groupBox1Layout->addWidget( rRunNo, 0, 1 );

    rRunBounding = new QRadioButton( i18n( "Text will run &around the frame" ), runGroup );
    groupBox1Layout->addWidget( rRunBounding, 1, 1 );

    rRunSkip = new QRadioButton( i18n( "Text will &not run around this frame" ), runGroup );
    groupBox1Layout->addWidget( rRunSkip, 2, 1 );

    QPixmap pixmap = KWBarIcon( "run_not" );
    QLabel *lRunNo = new QLabel( runGroup );
    lRunNo->setBackgroundPixmap( pixmap );
    lRunNo->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunNo, 0, 0 );

    pixmap = KWBarIcon( "run_bounding" );
    QLabel *lRunBounding = new QLabel( runGroup );
    lRunBounding->setBackgroundPixmap( pixmap );
    lRunBounding->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunBounding, 1, 0 );

    pixmap = KWBarIcon( "run_skip" );
    QLabel *lRunSkip = new QLabel( runGroup );
    lRunSkip->setBackgroundPixmap( pixmap );
    lRunSkip->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunSkip, 2, 0 );

    tabLayout->addWidget( runGroup );

    // Second groupbox
    runSideGroup = new QButtonGroup(  i18n( "Run Around Side" ), tab2);
    runSideGroup->setColumnLayout( 0, Qt::Vertical );
    runSideGroup->layout()->setSpacing( KDialog::spacingHint() );
    runSideGroup->layout()->setMargin( KDialog::marginHint() );
    QGridLayout *runSideLayout = new QGridLayout( runSideGroup->layout() );
    runSideLayout->setAlignment( Qt::AlignTop );

    rRunLeft = new QRadioButton( i18n( "Run Around", "&Left" ), runSideGroup );
    runSideLayout->addWidget( rRunLeft, 0, 0 /*1*/ );

    rRunRight = new QRadioButton( i18n( "Run Around", "&Right" ), runSideGroup );
    runSideLayout->addWidget( rRunRight, 1, 0 /*1*/ );

    rRunBiggest = new QRadioButton( i18n( "Run Around", "Lon&gest side" ), runSideGroup );
    runSideLayout->addWidget( rRunBiggest, 2, 0 /*1*/ );

#if 0 // TODO icons!
    QPixmap pixmap = KWBarIcon( "run_left" );
    QLabel *label = new QLabel( runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 0, 0 );

    pixmap = KWBarIcon( "run_right" );
    label = new QLabel( runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 1, 0 );

    pixmap = KWBarIcon( "run_biggest" );
    label = new QLabel( runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 2, 0 );
#endif

    tabLayout->addWidget( runSideGroup );

    m_raDistConfigWidget = new KWFourSideConfigWidget( doc, i18n("Distance Between Frame && Text (%1)").arg(doc->unitName()), tab2 );
    if ( frame )
        m_raDistConfigWidget->setValues( QMAX(0.00, frame->runAroundLeft()),
                                         QMAX(0.00, frame->runAroundRight()),
                                         QMAX(0.00, frame->runAroundTop()),
                                         QMAX(0.00, frame->runAroundBottom()) );
    tabLayout->addWidget( m_raDistConfigWidget );



    // Show current settings

    // Runaround
    bool show=true;
    KWFrame::RunAround ra = KWFrame::RA_NO;
    if ( frame )
        ra = frame->runAround();
    else {
        KWFrame *f=allFrames.first();
        ra = f->runAround();
        f=allFrames.next();
        while(f) {
            if(ra != f->runAround()) show=false;
            f=allFrames.next();
        }
    }

    if(show) {
        switch ( ra ) {
            case KWFrame::RA_NO: rRunNo->setChecked( true ); break;
            case KWFrame::RA_BOUNDINGRECT: rRunBounding->setChecked( true ); break;
            case KWFrame::RA_SKIP: rRunSkip->setChecked( true ); break;
        }
    }

    // Runaround side
    show = true;
    KWFrame::RunAroundSide rs = KWFrame::RA_BIGGEST;
    if ( frame )
        rs = frame->runAroundSide();
    else {
        KWFrame *f=allFrames.first();
        rs = f->runAroundSide();
        f = allFrames.next();
        while(f) {
            if(rs != f->runAroundSide()) show=false;
            f=allFrames.next();
        }
    }

    if(show) {
        switch ( rs ) {
            case KWFrame::RA_LEFT: rRunLeft->setChecked( true ); break;
            case KWFrame::RA_RIGHT: rRunRight->setChecked( true ); break;
            case KWFrame::RA_BIGGEST: rRunBiggest->setChecked( true ); break;
        }
    }

    // Runaround gap
    show=true;
    double ragapLeft = 0;
    double ragapRight = 0;
    double ragapTop = 0;
    double ragapBottom = 0;
    if ( frame ) {
        ragapLeft = frame->runAroundLeft();
        ragapRight = frame->runAroundRight();
        ragapTop = frame->runAroundTop();
        ragapBottom = frame->runAroundBottom();
    }
    else {
        KWFrame *f = allFrames.first();
        ragapLeft = f->runAroundLeft();
        ragapRight = f->runAroundRight();
        ragapTop = f->runAroundTop();
        ragapBottom = f->runAroundBottom();
        for( f = allFrames.next() ; f ; f = allFrames.next() ) {
            if( ragapLeft != f->runAroundLeft() ||
                ragapRight != f->runAroundRight() ||
                ragapTop != f->runAroundTop() ||
                ragapBottom != f->runAroundBottom() )
                show = false; // TODO special value in the spinbox
            f = allFrames.next();
        }
    }

    if(show)
        m_raDistConfigWidget->setValues( ragapLeft, ragapRight, ragapTop, ragapBottom );

    enableRunAround();

    // Changing the type of runaround needs to enable/disable the runaround-side options
    connect( runGroup, SIGNAL( clicked(int) ), this, SLOT( enableRunAround() ) );
}

void KWFrameDia::setupTab3(){ // TAB Frameset
    /*
     * here the user can select from the current TEXT framesets, a new one is
     * included in the list.
     * Afterwards (on ok) the frame should be checked if it is already owned by a
     * frameset, if so that connection must be disconnected (if different) and
     * framebehaviour will be copied from the frameset
     * then the new connection should be made.
     */
    //kdDebug() << "setup tab 3 frameSet"<<endl;
    tab3 = addPage( i18n( "Connect Text Frames" ) );

    QVBoxLayout *tabLayout = new QVBoxLayout( tab3, 11, 6);

    QButtonGroup *myGroup = new QButtonGroup(this);
    myGroup->hide();

    rExistingFrameset = new QRadioButton( tab3, "rExistingFrameset" );
    rExistingFrameset->setText( i18n("Select existing frameset to connect frame to:") );
    tabLayout->addWidget( rExistingFrameset );
    myGroup->insert(rExistingFrameset,1);
    connect (rExistingFrameset, SIGNAL( toggled(bool)), this, SLOT(ensureValidFramesetSelected()));

    QHBoxLayout *layout2 = new QHBoxLayout( 0, 0, 6);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout2->addItem( spacer );

    lFrameSList = new QListView( tab3, "lFrameSList" );
    lFrameSList->addColumn( i18n("No.") );
    lFrameSList->addColumn( i18n("Frameset Name") );
    lFrameSList->setAllColumnsShowFocus( true );
    lFrameSList->header()->setMovingEnabled( false );
    connect( lFrameSList, SIGNAL(selectionChanged ()),this,SLOT(selectExistingFrameset ()) );
    connect (lFrameSList, SIGNAL( selectionChanged()), this, SLOT(ensureValidFramesetSelected()));

    layout2->addWidget( lFrameSList );
    tabLayout->addLayout( layout2 );

    rNewFrameset = new QRadioButton( tab3);
    rNewFrameset->setText( i18n( "Create a new frameset" ) );
    tabLayout->addWidget( rNewFrameset );
    myGroup->insert(rNewFrameset,2);

    QFrame *line1 = new QFrame( tab3 );
    line1->setProperty( "frameShape", (int)QFrame::HLine );
    line1->setFrameShadow( QFrame::Plain );
    line1->setFrameShape( QFrame::HLine );
    tabLayout->addWidget( line1 );

    QHBoxLayout *layout1 = new QHBoxLayout( 0, 0, 6 );
    QLabel *textLabel1 = new QLabel( tab3 );
    textLabel1->setText( i18n( "Name of frameset:" ) );
    layout1->addWidget( textLabel1 );

    eFrameSetName = new QLineEdit( tab3 );
    layout1->addWidget( eFrameSetName );
    tabLayout->addLayout( layout1 );

    int amount=0;
    // now fill the gui.
    for ( unsigned int i = 0; i < doc->numFrameSets(); i++ ) {
        KWFrameSet * fs = doc->frameSet( i );
        if ( i == 0 && doc->processingType() == KWDocument::WP )
            continue;
        if ( fs->type() != FT_TEXT || fs->isHeaderOrFooter() )
            continue;
        if ( fs->frameSetInfo() == KWFrameSet::FI_FOOTNOTE )
            continue;
        if ( fs->getGroupManager() )
            continue;
        if ( fs->isDeleted() )
            continue;
        QListViewItem *item = new QListViewItem( lFrameSList );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, fs->getName() );
        amount++;
        if( frame && frame->frameSet() == fs ) {
            lFrameSList->setSelected(item, TRUE );
            oldFrameSetName = fs->getName();
            rExistingFrameset->setChecked(true);
        }
    }
    if(amount==0) {
        rNewFrameset->setChecked(/*true*/false);
        rNewFrameset->setEnabled(false);
        rExistingFrameset->setEnabled(false);
        lFrameSList->setEnabled(false);
    }
    //we can't create a new frame when we select
    //multi frame!!!!
    if ( allFrames.count() > 1 ) {
        rNewFrameset->setChecked(false);
        rNewFrameset->setEnabled(false);
        myGroup->setRadioButtonExclusive( false );
    }
    if(frame && frame->frameSet() == 0) {
        oldFrameSetName = doc->generateFramesetName( i18n( "Text Frameset %1" ) );
        rNewFrameset->setChecked(true);
    }
    eFrameSetName->setText( oldFrameSetName );

    connect( lFrameSList, SIGNAL( currentChanged( QListViewItem * ) ),
             this, SLOT( connectListSelected( QListViewItem * ) ) );
    connect(eFrameSetName, SIGNAL(textChanged ( const QString & ) ),
             this,SLOT(textNameFrameChanged ( const QString & ) ) );
    connect(eFrameSetName, SIGNAL(textChanged ( const QString & )),
             this,SLOT(textNameFrameChanged ( const QString & ) ));
    connect( rNewFrameset, SIGNAL(toggled (bool)),
             this,SLOT(selectNewFrameset (bool)) );
}

void KWFrameDia::selectExistingFrameset() {
    rExistingFrameset->setChecked(true);
}

void KWFrameDia::selectNewFrameset(bool on) {
    if(!on) return;

    QListViewItem *frameSetItem  = lFrameSList->selectedItem();
    if ( !frameSetItem)
        return;
    QString str = frameSetItem->text( 0 );
    KWFrameSet *fs = doc->frameSet(str.toInt() - 1);

    frameSetItem->setText(1, fs->getName() );
}

void KWFrameDia::textNameFrameChanged ( const QString &text )
{
    if(rExistingFrameset->isChecked()) {
        QListViewItem *item = lFrameSList->selectedItem();
        if ( !item )
            return;
        item->setText(1, text );
    }
    if(rNewFrameset->isChecked() || rExistingFrameset->isChecked()) //when one of both is clicked.
        enableButtonOK( !text.isEmpty() );
    else
        enableButtonOK( true );
}

void KWFrameDia::setupTab4() { // TAB Geometry
    noSignal = true;

    tab4 = addPage( i18n( "Geometry" ) );
    QGridLayout* grid4 = new QGridLayout( tab4, 5, 1, KDialog::marginHint(), KDialog::spacingHint() );

    floating = new QCheckBox( i18n("Frame is inline"), tab4 );

    connect( floating, SIGNAL( toggled(bool) ), this, SLOT( slotFloatingToggled(bool) ) );
    int row = 0;
    grid4->addMultiCellWidget( floating, row, row, 0, 1 );

    row++;
    protectSize = new QCheckBox( i18n("Protect size and position"), tab4);
    grid4->addMultiCellWidget( protectSize, row, row, 0, 1 );
    connect( protectSize, SIGNAL( toggled(bool) ), this, SLOT( slotProtectSizeToggled(bool) ) );

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

    // TODO after 1.4: remove the unit from the label
    grp1 = new QGroupBox( i18n("Position (%1)").arg(doc->unitName()), tab4 );
    QGridLayout* pGrid = new QGridLayout( grp1, 3, 4, KDialog::marginHint(), KDialog::spacingHint() );

    lx = new QLabel( i18n( "Left:" ), grp1 );
    lx->resize( lx->sizeHint() );
    pGrid->addWidget( lx, 1, 0 );

    sx = new KoUnitDoubleSpinBox( grp1, 0, 9999, 1, 0.0, doc->unit() );

    sx->resize( sx->sizeHint() );
    pGrid->addWidget( sx, 1, 1 );

    ly = new QLabel( i18n( "Top:" ), grp1 );
    ly->resize( ly->sizeHint() );
    pGrid->addWidget( ly, 1, 2 );

    sy = new KoUnitDoubleSpinBox( grp1, 0, 9999, 1, 0.0, doc->unit() );
    sy->resize( sy->sizeHint() );
    pGrid->addWidget( sy, 1, 3 );

    lw = new QLabel( i18n( "Width:" ), grp1 );
    lw->resize( lw->sizeHint() );
    pGrid->addWidget( lw, 2, 0 );

    sw = new KoUnitDoubleSpinBox( grp1, 0, 9999, 1, 0.0, doc->unit() );

    sw->resize( sw->sizeHint() );
    connect( sw, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateHeightForWidth(double)) );

    pGrid->addWidget( sw, 2, 1 );

    lh = new QLabel( i18n( "Height:" ), grp1 );
    lh->resize( lh->sizeHint() );
    pGrid->addWidget( lh, 2, 2 );

    sh = new KoUnitDoubleSpinBox( grp1, 0, 9999, 1, 0.0, doc->unit() );
    connect( sh, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateWidthForHeight(double)) );

    sh->resize( sh->sizeHint() );

    pGrid->addWidget( sh, 2, 3 );

    pGrid->setRowSpacing( 0, KDialog::spacingHint() + 5 );

    row++;
    grid4->addMultiCellWidget( grp1, row, row, 0,1 );

    if(frame) {
        // TODO after 1.4: remove the unit from the label
        m_paddingConfigWidget = new KWFourSideConfigWidget( doc, i18n("Margins (%1)").arg(doc->unitName()), tab4 );
        m_paddingConfigWidget->setValues( QMAX(0.00, frame->paddingLeft()),
                                          QMAX(0.00, frame->paddingRight()),
                                          QMAX(0.00, frame->paddingTop()),
                                          QMAX(0.00, frame->paddingBottom()) );
        row++;
        grid4->addMultiCellWidget( m_paddingConfigWidget, row, row, 0, 1 );

        if ( tab1 && cbProtectContent )
        {
            m_paddingConfigWidget->setEnabled( !cbProtectContent->isChecked() );
        }
    }
    else
    {
        m_paddingConfigWidget = 0;
    }


    if ( frame ) {
        // is single frame dia. Fill position strings and checkboxes now.

        // Can't use frame->pageNum() here since frameset might be 0
        int pageNum = QMIN( static_cast<int>(frame->y() / doc->ptPaperHeight()), doc->numPages()-1 );

        sx->setValue( KoUnit::toUserValue( frame->x(), doc->unit() ) );
        sy->setValue( KoUnit::toUserValue( frame->y() - (pageNum * doc->ptPaperHeight()), doc->unit() ) );
        sw->setValue( KoUnit::toUserValue( frame->width(), doc->unit() ) );
        sh->setValue( KoUnit::toUserValue( frame->height(), doc->unit() ) );

        calcRatio();

        // userValue leads to some rounding -> store old values from the ones
        // displayed, so that the "change detection" in apply() works.
        oldX = sx->value();
        oldY = sy->value();
        oldW = sw->value();
        oldH = sh->value();

#ifdef ALLOW_NON_INLINE_TABLES
        KWFrameSet * fs = frame->frameSet();
        if ( fs && fs->getGroupManager() )
            floating->setText( i18n( "Table is inline" ) );
#else
        floating->hide();
#endif

        floating->setChecked( frameSetFloating );
        protectSize->setChecked( frameSetProtectedSize);
    } else {
        // multi frame. Fill inline and protect checkbox, leave away the position strings.
        KWFrame *f=allFrames.first();
        KWFrameSet *fs=f->frameSet();
        bool ps=fs->isProtectSize();
        protectSize->setChecked( ps );

        bool table=fs->getGroupManager();
        if(table)
            fs=fs->getGroupManager();
        bool inlineframe =fs->isFloating();
        floating->setChecked( inlineframe );

        double commonWidth = f->width();
        double commonHeight = f->height();
        sw->setEnabled( true );
        sh->setEnabled( true );

        f=allFrames.next();
        while(f) {
            KWFrameSet *fs=f->frameSet();
            if(ps != fs->isProtectSize()) {
                protectSize->setTristate();
                protectSize->setNoChange();
            }
            if(fs->getGroupManager()) //table
                fs=fs->getGroupManager();
            else
                table=false;

            if(inlineframe != fs->isFloating()) {
                floating->setTristate();
                floating->setNoChange();
            }

            if ( kAbs( f->width() - commonWidth ) > 1E-6 ) {
                kdDebug() << k_funcinfo << "width differs:" << f->width() << " " << commonWidth << endl;
                sw->setEnabled( false );
            }
            if ( kAbs( f->height() - commonHeight ) > 1E-6 ) {
                kdDebug() << k_funcinfo << "height differs:" << f->height() << " " << commonHeight << endl;
                sh->setEnabled( false );
            }

            f=allFrames.next();
        }
        // TODO port to KoUnitDoubleSpinBox
        // and TODO show a special value when frames have a different width/height
        if ( sw->isEnabled() )
            sw->setValue( KoUnit::toUserValue( commonWidth, doc->unit() ) );
        if ( sh->isEnabled() )
            sh->setValue( KoUnit::toUserValue( commonHeight, doc->unit() ) );
        if(table)
        {
#ifdef ALLOW_NON_INLINE_TABLES
            floating->setText( i18n( "Table is inline" ) );
#else
            floating->hide();
#endif
        }
    }

    if ( !frame || m_defaultFrameSetIncluded ) {
        // is multi frame, positions don't work for that..
        // also not for default frames.
        sx->setEnabled( false );
        sy->setEnabled( false );
        lx->setEnabled( false );
        ly->setEnabled( false );
        lw->setEnabled( false );
        lh->setEnabled( false );
        floating->setEnabled( false );
    }
    if ( m_defaultFrameSetIncluded ) { // the multiframe case is handled before
        sw->setEnabled( false );
        sh->setEnabled( false );
        grp1->setEnabled( false );
    }


    if ( m_mainFrameSetIncluded )
    {
        grp1->hide();
        floating->hide( );
        protectSize->hide();
    }
    noSignal=false;

    ++row;
    grid4->setRowStretch( row, 1 );
}

void KWFrameDia::setupTab5() { // Tab Background fill/color
    tab5 = addPage( i18n("Background") );
    QGridLayout* grid5 = new QGridLayout( tab5, 0 /*auto*/, 2, KDialog::marginHint(), KDialog::spacingHint() );

    int row = 0;
    if (!frame ) {
        overwriteColor = new QCheckBox (i18n("Set new color on all selected frames"), tab5);
        grid5->addMultiCellWidget(overwriteColor,row,row,0,1);
        row++;
    }
    //brushPreview=new KWBrushStylePreview(tab5);
    //grid5->addMultiCellWidget(brushPreview,row,5,1,1);

    transparentCB = new QCheckBox( i18n( "Transparent background" ), tab5 );
    grid5->addWidget(transparentCB,row++,0);

    QLabel *labelBgColor = new QLabel( i18n( "Background color:" ), tab5 );

    grid5->addWidget(labelBgColor,row++,0);

    brushColor = new KColorButton( Qt::white, tab5 );
    grid5->addWidget(brushColor,row++,0);

//    connect( brushColor, SIGNAL( changed( const QColor & ) ),
//        this, SLOT( updateBrushPreview() ) );


    // ###########################
    // Fill styles are ugly and not WYSIWYG (due to being pixel-based)
    // Feature not in OOo either (they have a configurable level of transparency instead, much nicer)
#if 0
    l = new QLabel( i18n( "Background style:" ), tab5 );
    grid5->addWidget(l,row++,0);

    brushStyle = new QComboBox( false,tab5, "BStyle" );
    grid5->addWidget(brushStyle,row++,0);

    brushStyle->insertItem( i18n( "No Background Fill" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "100% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "94% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "88% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "63% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "50% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "37% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "12% Fill Pattern" ) );
    // xgettext:no-c-format
    brushStyle->insertItem( i18n( "6% Fill Pattern" ) );
    brushStyle->insertItem( i18n( "Horizontal Lines" ) );
    brushStyle->insertItem( i18n( "Vertical Lines" ) );
    brushStyle->insertItem( i18n( "Crossing Lines" ) );
    brushStyle->insertItem( i18n( "Diagonal Lines ( / )" ) );
    brushStyle->insertItem( i18n( "Diagonal Lines ( \\ )" ) );
    brushStyle->insertItem( i18n( "Diagonal Crossing Lines" ) );
    connect(  brushStyle, SIGNAL( activated( int ) ),
        this, SLOT( updateBrushPreview() ) );

    updateBrushPreview();
#endif

    connect( transparentCB, SIGNAL( toggled( bool ) ), labelBgColor, SLOT( setDisabled( bool ) ) );
    connect( transparentCB, SIGNAL( toggled( bool ) ), brushColor, SLOT( setDisabled( bool ) ) );
    initBrush();

    QSpacerItem* spacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid5->addItem( spacer,row,0 );
}

void KWFrameDia::slotProtectContentChanged( bool b )
{
    if (tab4 && !noSignal && m_paddingConfigWidget) {
        m_paddingConfigWidget->setEnabled( !b );
    }
}

void KWFrameDia::slotUpdateWidthForHeight(double height)
{
    if ( !cbAspectRatio || cbAspectRatio->state() != QButton::NoChange)
        return;
    if ( heightByWidthRatio == 0 )
        return; // avoid DBZ
    sw->setValue( height / heightByWidthRatio );

}

void KWFrameDia::slotUpdateHeightForWidth( double width )
{
    if ( !cbAspectRatio || cbAspectRatio->state() != QButton::NoChange)
        return;
    sh->setValue( width * heightByWidthRatio );
}

void KWFrameDia::slotKeepRatioToggled(bool on)
{
    if ( !on || !sw || !sh) return;
    calcRatio();
}
void KWFrameDia::ensureValidFramesetSelected()
{
    enableButtonOK( rNewFrameset->isChecked() || rExistingFrameset->isChecked() && lFrameSList->selectedItem() != NULL);
}

void KWFrameDia::calcRatio()
{
    if ( sw->value() == 0 )
        heightByWidthRatio = 1.0; // arbitrary
    else
        heightByWidthRatio = sh->value() / sw->value();
}

void KWFrameDia::initBrush()
{
    bool allFramesSame=true;
    if ( frame )
        newBrushStyle = frame->backgroundColor();
    else {
        KWFrame *f=allFrames.first();
        newBrushStyle = f->backgroundColor();
        f=allFrames.next();
        while(f) {
            if(newBrushStyle != f->backgroundColor()) {
                allFramesSame=false;
                break;
            }
            f=allFrames.next();
        }
        overwriteColor->setChecked(allFramesSame);
    }

    transparentCB->setChecked( newBrushStyle.style() == NoBrush );

#if 0
    switch ( newBrushStyle.style() )
    {
        case NoBrush:
            brushStyle->setCurrentItem( 0 );
            break;
        case SolidPattern:
            brushStyle->setCurrentItem( 1 );
            break;
        case Dense1Pattern:
            brushStyle->setCurrentItem( 2 );
            break;
        case Dense2Pattern:
            brushStyle->setCurrentItem( 3 );
            break;
        case Dense3Pattern:
            brushStyle->setCurrentItem( 4 );
            break;
        case Dense4Pattern:
            brushStyle->setCurrentItem( 5 );
            break;
        case Dense5Pattern:
            brushStyle->setCurrentItem( 6 );
            break;
        case Dense6Pattern:
            brushStyle->setCurrentItem( 7 );
            break;
        case Dense7Pattern:
            brushStyle->setCurrentItem( 8 );
            break;
        case HorPattern:
            brushStyle->setCurrentItem( 9 );
            break;
        case VerPattern:
            brushStyle->setCurrentItem( 10 );
            break;
        case CrossPattern:
            brushStyle->setCurrentItem( 11 );
            break;
        case BDiagPattern:
            brushStyle->setCurrentItem( 12 );
            break;
        case FDiagPattern:
            brushStyle->setCurrentItem( 13 );
            break;
        case DiagCrossPattern:
            brushStyle->setCurrentItem( 14 );
            break;
        case CustomPattern:
            break;
    }
#endif

    QColor col = newBrushStyle.color();
    col = col.isValid() ? col : QApplication::palette().color( QPalette::Active, QColorGroup::Base );

    brushColor->setColor( col );
}

QBrush KWFrameDia::frameBrushStyle() const
{
    QBrush brush;

    brush.setStyle( transparentCB->isChecked() ? NoBrush : SolidPattern );

#if 0
    switch ( brushStyle->currentItem() )
    {
        case 0:
            brush.setStyle( NoBrush );
            break;
        case 1:
            brush.setStyle( SolidPattern );
            break;
        case 2:
            brush.setStyle( Dense1Pattern );
            break;
        case 3:
            brush.setStyle( Dense2Pattern );
            break;
        case 4:
            brush.setStyle( Dense3Pattern );
            break;
        case 5:
            brush.setStyle( Dense4Pattern );
            break;
        case 6:
            brush.setStyle( Dense5Pattern );
            break;
        case 7:
            brush.setStyle( Dense6Pattern );
            break;
        case 8:
            brush.setStyle( Dense7Pattern );
            break;
        case 9:
            brush.setStyle( HorPattern );
            break;
        case 10:
            brush.setStyle( VerPattern );
            break;
        case 11:
            brush.setStyle( CrossPattern );
            break;
        case 12:
            brush.setStyle( BDiagPattern );
            break;
        case 13:
            brush.setStyle( FDiagPattern );
            break;
        case 14:
            brush.setStyle( DiagCrossPattern );
            break;
    }
#endif

    brush.setColor( newBrushStyle.color() );

    return brush;
}

#if 0
void KWFrameDia::updateBrushPreview()
{
    if(brushStyle->currentItem()==0) {
        brushPreview->hide();
    } else {
        brushPreview->show();
        brushPreview->setBrush(frameBrushStyle());
        brushPreview->repaint(true);
    }
}
#endif

// Called when "reconnect" or "no followup" is checked
void KWFrameDia::setFrameBehaviorInputOn() {
    if ( tab4 && floating->isChecked() )
        return;
    if( rAppendFrame && rResizeFrame && rNoShow /*&& !rAppendFrame->isEnabled()*/ ) {
        if(frameBehavior== KWFrame::AutoExtendFrame) {
            rResizeFrame->setChecked(true);
        } else if (frameBehavior== KWFrame::AutoCreateNewFrame) {
            rAppendFrame->setChecked(true);
        } else {
            rNoShow->setChecked(true);
        }
        rResizeFrame->setEnabled(true);
        // Can't have "create a new page" if "no followup", that wouldn't work
        kdDebug() << "setFrameBehaviorInputOn: reconnect->isChecked()==" << reconnect->isChecked() << endl;
        rAppendFrame->setEnabled( reconnect->isChecked() );
        rNoShow->setEnabled(true);
    }
}

// Called when "place a copy" is checked
void KWFrameDia::setFrameBehaviorInputOff() {
    if ( tab4 && floating->isChecked() )
        return;
    if( rAppendFrame && rResizeFrame && rNoShow && rAppendFrame->isEnabled() ) {
        if(rResizeFrame->isChecked()) {
            frameBehavior=KWFrame::AutoExtendFrame;
        } else if ( rAppendFrame->isChecked()) {
            frameBehavior=KWFrame::AutoCreateNewFrame;
        } else {
            frameBehavior=KWFrame::Ignore;
        }
        // In "Place a copy" mode, we can't have "create new page if text too long"
        if ( rAppendFrame->isChecked() )
            rNoShow->setChecked(true);
        rAppendFrame->setEnabled(false);
        rResizeFrame->setEnabled(true);
        rNoShow->setEnabled(true);
    }
}

void KWFrameDia::enableSizeAndPosition()
{
    bool canMove = ( floating->state() == QButton::Off ) // can move if no frame is floating
                   && ( protectSize->state() == QButton::Off ) // protects size too
                   && !m_defaultFrameSetIncluded // those can't be moved
                   && frame; // can't move if multiple frames selected
    sx->setEnabled( canMove );
    sy->setEnabled( canMove );
    bool canResize = ( protectSize->state() == QButton::Off ) // can resize if no frame is protect-size'd
                     && !m_defaultFrameSetIncluded; // those can't be resized
    sw->setEnabled( canResize );
    sh->setEnabled( canResize );
}

void KWFrameDia::slotProtectSizeToggled(bool)
{
    enableSizeAndPosition();
}

void KWFrameDia::slotFloatingToggled(bool b)
{
    enableSizeAndPosition();
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
        else if(frame) {
            KWFrameSet *fs = frame->frameSet(); // 0 when creating a frame
            if ( fs && (fs->isHeaderOrFooter() || fs->isFootEndNote() ))
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
        if ( tab4 && floating->isChecked() ) {
            runGroup->setEnabled( false ); // Runaround options don't make sense for floating frames
        } else
        {
            if ( frame && frame->frameSet() )
                runGroup->setEnabled( !frameSetFloating && !frame->frameSet()->isMainFrameset() && !frame->frameSet()->isHeaderOrFooter() && !frame->frameSet()->isFootEndNote() );
            else
                runGroup->setEnabled( true );
        }
        runSideGroup->setEnabled( runGroup->isEnabled() && rRunBounding->isChecked() );
        m_raDistConfigWidget->setEnabled( runGroup->isEnabled() &&
            ( rRunBounding->isChecked() || rRunSkip->isChecked() ) );
    }
}

bool KWFrameDia::applyChanges()
{
    kdDebug() << "KWFrameDia::applyChanges"<<endl;
    KWFrame *frameCopy = 0L;
    bool isNewFrame=false;
    if(frame) { // only do undo/redo when we have 1 frame to change for now..
        frameCopy = frame->getCopy(); // keep a copy of the original (for undo/redo)
        isNewFrame = frame->frameSet() == 0L; // true if we are creating a newframe
    }
    QString name=QString::null;

    KMacroCommand * macroCmd=0L;
    if ( tab3 ) { // TAB Frameset
        // Frame/Frameset belonging, and frameset naming
        // We basically have three cases:
        // * Creating a new frame (fs==0), and creating a frameset (rNewFrameset selected)
        // * Creating a frame (fs==0), and attaching to an existing frameset (other)
        // * Editing a frame (fs!=0), possibly changing the frameset attachment (maybe creating a new one)

        name = eFrameSetName->text();
        if ( name.isEmpty() ) // Don't allow empty names
            name = doc->generateFramesetName( i18n( "Text Frameset %1" ) );
        KWFrameSet *fs = 0L;
        QListViewItem *frameSetItem  = lFrameSList->selectedItem();
        if(frameSetItem) {
            QString str = frameSetItem->text( 0 );
            fs = doc->frameSet(str.toInt() - 1);
        }
        if(rNewFrameset->isChecked()) { // create a new FS.
            if(frame && frame->frameSet()) {
                // disconnect.
                if(! mayDeleteFrameSet( static_cast<KWTextFrameSet*>(frame->frameSet())))
                    return false;
                frame->frameSet()->delFrame( frame, false );
            } else {
                // first check all frames and ask the user if its ok to disconnect.
                for(KWFrame *f=allFrames.first();f; f=allFrames.next()) {
                    if(! mayDeleteFrameSet( static_cast<KWTextFrameSet*>(f->frameSet())))
                        return false;
                }
                for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                    f->frameSet()->delFrame( f, false );
            }
        } else if(rExistingFrameset->isChecked()) { // rename and/or reconnect a new frameset for this frame.
            if(frameSetItem && (fs->getName() != frameSetItem->text( 1 ))) { // rename FS.
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Rename Frameset") );
                // Rename frameset
                typedef KoSetPropCommand<QString, KWFrameSet, &KWFrameSet::setName> FramesetNameCommand;
                FramesetNameCommand* cmd = new FramesetNameCommand( fs, i18n( "Rename Frameset" ), fs->getName(), frameSetItem->text( 1 ) );
                macroCmd->addCommand(cmd);
                cmd->execute();
            }
            if(frame && fs ) {
                if(frame->frameSet() != fs)  {
                    if(frame->frameSet()!=0) {
                        // reconnect.
                        if(! mayDeleteFrameSet( dynamic_cast<KWTextFrameSet*>(frame->frameSet())))
                            return false;
                        frame->frameSet()->delFrame( frame, false );
                    }
                    fs->addFrame(frame);
                }
            } else if ( fs ){
                // first check all frames and ask the user if its ok to reconnect.
                for(KWFrame *f=allFrames.first();f; f=allFrames.next()) {
                    if(f->frameSet() != fs) {  // reconnect.
                        if(! mayDeleteFrameSet( dynamic_cast<KWTextFrameSet*>(f->frameSet())))
                            return false;
                    }
                }
                if ( fs )
                {
                    // then do the reconnects.
                    for(KWFrame *f=allFrames.first();f; f=allFrames.next()) {
                        KWFrameSet *fs2=f->frameSet();
                        if(! (fs2->isHeaderOrFooter() || fs2->isMainFrameset()) ) {
                            if(fs2 != fs) {  // reconnect.
                                f->frameSet()->delFrame( f, false );
                                fs->addFrame(f);
                            }
                        }
                    }
                }
            }
        }
        if(rNewFrameset->isChecked() || rExistingFrameset->isChecked()) {
            // check if new name is unique
            for (QPtrListIterator<KWFrameSet> fit = doc->framesetsIterator(); fit.current() ; ++fit ) {
                if ( !fit.current()->isDeleted() &&  // Allow to reuse a deleted frameset's name
                     fs != fit.current() && fit.current()->getName() == name) {
                    if ( rNewFrameset->isChecked() )
                        KMessageBox::sorry( this,
                                            i18n( "A new frameset with the name '%1' "
                                                  "can not be made because a frameset with that name "
                                                  "already exists. Please enter another name or select "
                                                  "an existing frameset from the list.").arg(name));
                    else
                        KMessageBox::sorry( this,
                                            i18n( "A frameset with the name '%1' "
                                                  "already exists. Please enter another name." ).arg(name) );
                    eFrameSetName->setText(oldFrameSetName);
                    return false;
                }
            }
        }
    }
    if ( tab1 ) { // TAB Frame Options
        // Copy
        if(frame)
            frame->setCopy( cbCopy->isChecked() );
        else if(cbCopy->state() != QButton::NoChange) {
            for(KWFrame *f=allFrames.first();f; f=allFrames.next()) {
                if(f == f->frameSet()->frame(0))  continue; // skip first frame of any frameset.
                f->setCopy( cbCopy->isChecked() );
            }
        }

        // FrameBehavior
        if ( frameType == FT_TEXT )
        {
            bool update=true;
            KWFrame::FrameBehavior fb=KWFrame::AutoCreateNewFrame;
            if(rResizeFrame->isChecked())
                fb = KWFrame::AutoExtendFrame;
            else if ( rAppendFrame->isChecked())
                fb = KWFrame::AutoCreateNewFrame;
            else if ( rNoShow->isChecked())
                fb = KWFrame::Ignore;
            else
                update=false;

            if(frame)
                if(cbAllFrames->isChecked() && frame->frameSet())
                    frame->frameSet()->setFrameBehavior(fb);
                else
                    frame->setFrameBehavior(fb);
            else if(update) {
                for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                    if(cbAllFrames->isChecked())
                        f->frameSet()->setFrameBehavior(fb);
                    else
                        f->setFrameBehavior(fb);
            }
            if ( frame && frame->frameSet())
            {
                KWTextFrameSet * frm=static_cast<KWTextFrameSet *>( frame->frameSet() );
                if(frm->textObject()->protectContent()!=cbProtectContent->isChecked())
                {
                    if(!macroCmd)
                        macroCmd = new KMacroCommand( i18n("Protect Content") );
                    KWProtectContentCommand * cmd = new KWProtectContentCommand( i18n("Protect Content"), frm,cbProtectContent->isChecked() );
                    cmd->execute();
                    macroCmd->addCommand(cmd);
                }
            }
            else
            {
                if ( cbProtectContent->state() != QButton::NoChange)
                {
                    for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                    {
                        KWTextFrameSet * frm=dynamic_cast<KWTextFrameSet *>( f->frameSet() );
                        if ( frm )
                        {
                            if(frm->textObject()->protectContent()!=cbProtectContent->isChecked())
                            {
                                if(!macroCmd)
                                    macroCmd = new KMacroCommand( i18n("Protect Content") );
                                KWProtectContentCommand * cmd = new KWProtectContentCommand( i18n("Protect Content"), frm,cbProtectContent->isChecked() );
                                cmd->execute();
                                macroCmd->addCommand(cmd);
                            }
                        }

                    }
                }
            }

        }
        // NewFrameBehavior
        bool update=true;
        KWFrame::NewFrameBehavior nfb=KWFrame::Reconnect;
        if( reconnect && reconnect->isChecked() )
            nfb = KWFrame::Reconnect;
        else if ( noFollowup->isChecked() )
            nfb = KWFrame::NoFollowup;
        else if ( copyRadio->isChecked() )
            nfb = KWFrame::Copy;
        else
            update=false;

        if(frame)
            if(cbAllFrames->isChecked() && frame->frameSet())
                frame->frameSet()->setNewFrameBehavior(nfb);
            else
                frame->setNewFrameBehavior(nfb);
        else if(update)
            for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                if(cbAllFrames->isChecked())
                    f->frameSet()->setNewFrameBehavior(nfb);
                else
                    f->setNewFrameBehavior(nfb);

        // aspect ratio
        if ( cbAspectRatio && (frameType==FT_PICTURE
#if 0 // KWORD_HORIZONTAL_LINE
            || frameType == FT_HORZLINE
#endif
            ))
        {
            typedef KoSetBasicPropCommand<bool, KWPictureFrameSet, &KWPictureFrameSet::setKeepAspectRatio> FramesetSetKeepAspectRatioCommand;
            if(frame) {
                KWPictureFrameSet * frm=static_cast<KWPictureFrameSet *>( frame->frameSet() );
                if ( frm->keepAspectRatio() != cbAspectRatio->isChecked() )
                {
                    if(!macroCmd)
                        macroCmd = new KMacroCommand( i18n("Frame Properties") );
                    FramesetSetKeepAspectRatioCommand* cmd = new FramesetSetKeepAspectRatioCommand( frm, QString::null, frm->keepAspectRatio(), cbAspectRatio->isChecked() );

                    cmd->execute();

                    macroCmd->addCommand(cmd);
                }
            } else if(cbAspectRatio->state() != QButton::NoChange) {
                for(KWFrame *f=allFrames.first();f; f=allFrames.next()) {
                    KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
                    if(fs) {
                        if(fs->keepAspectRatio()!=cbAspectRatio->isChecked())
                        {
                            if(!macroCmd)
                                macroCmd = new KMacroCommand( i18n("Frame Properties") );
                            FramesetSetKeepAspectRatioCommand* cmd = new FramesetSetKeepAspectRatioCommand( fs, QString::null, fs->keepAspectRatio(), cbAspectRatio->isChecked() );

                            cmd->execute();

                            macroCmd->addCommand(cmd);
                        }
                    }
                }
            }
        }
    }
    if ( tab2 ) { // TAB Text Runaround
        // Run around
        KWFrame::RunAround ra=KWFrame::RA_BOUNDINGRECT;
        bool update=true;
        if ( rRunNo->isChecked() )
            ra = KWFrame::RA_NO;
        else if ( rRunBounding->isChecked() )
            ra = KWFrame::RA_BOUNDINGRECT;
        else if ( rRunSkip->isChecked() )
            ra = KWFrame::RA_SKIP;
        else
            update=false;
        if(frame)
            frame->setRunAround(ra);
        else if (update) {
            for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                f->setRunAround(ra);
        }

        // Run around side.
        KWFrame::RunAroundSide rs=KWFrame::RA_BIGGEST;
        update=true;
        if ( rRunLeft->isChecked() )
            rs = KWFrame::RA_LEFT;
        else if ( rRunRight->isChecked() )
            rs = KWFrame::RA_RIGHT;
        else if ( rRunBiggest->isChecked() )
            rs = KWFrame::RA_BIGGEST;
        else
            update=false;
        if(frame)
            frame->setRunAroundSide(rs);
        else if (update) {
            for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                f->setRunAroundSide(rs);
        }

        // Run around gap.
        double runAroundLeft = m_raDistConfigWidget->leftValue();
        double runAroundRight = m_raDistConfigWidget->rightValue();
        double runAroundTop = m_raDistConfigWidget->topValue();
        double runAroundBottom = m_raDistConfigWidget->bottomValue();

        if(frame) {
            frame->setRunAroundGap( runAroundLeft, runAroundRight, runAroundTop, runAroundBottom );
        }
        else
            for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                f->setRunAroundGap( runAroundLeft, runAroundRight, runAroundTop, runAroundBottom );
    }
    if(tab5) { // Tab Background fill/color
        QBrush tmpBrush=frameBrushStyle();
        if(frame) {
            if(tmpBrush!=frame->backgroundColor()) {
                frame->setBackgroundColor(tmpBrush);
            }
        } else if(overwriteColor->isChecked()) {
            for(KWFrame *f=allFrames.first();f; f=allFrames.next())
                f->setBackgroundColor(tmpBrush);
        }
    }


    double px=0.0;
    double py=0.0;
    double pw=0.0;
    double ph=0.0;
    double uLeft = 0.0;
    double uTop = 0.0;
    double uBottom = 0.0;
    double uRight = 0.0;
    if(tab4) { // TAB Geometry
        if ( frame ) {
            px = QMAX( 0, sx->value() );
            int pageNum = QMIN( static_cast<int>( frame->y() / doc->ptPaperHeight() ), doc->numPages() - 1 );
            py = QMAX( 0, sy->value() ) + pageNum * doc->ptPaperHeight();
        }
        pw = QMAX( sw->value(), 0 );
        ph = QMAX( sh->value(), 0 );
        if ( m_paddingConfigWidget )
        {
            uLeft = m_paddingConfigWidget->leftValue();
            uRight = m_paddingConfigWidget->rightValue();
            uTop = m_paddingConfigWidget->topValue();
            uBottom = m_paddingConfigWidget->bottomValue();
        }
    }
    KoRect rect( px, py, pw, ph );

    //kdDebug() << "New geom: " << sx->text().toDouble() << ", " << sy->text().toDouble()
    //<< " " << sw->text().toDouble() << "x" << sh->text().toDouble() << endl;
    //kdDebug()<<" rect :"<<px <<" py :"<<py<<" pw :"<<pw <<" ph "<<ph<<endl;
    // Undo/redo for frame properties
    if(frame) { // only do undo/redo when we edit 1 frame for now..

        if(frame->frameSet() == 0L ) { // if there is no frameset (anymore)
            KWTextFrameSet *_frameSet = new KWTextFrameSet( doc, name );
            _frameSet->addFrame( frame );

            if( !doc->isOutOfPage( rect , frame->pageNum() ) ) {
                frame->setRect( px, py, pw, ph );
                //don't change margins when frame is protected.
                if ( m_paddingConfigWidget && ( !tab1 || (tab1 && cbProtectContent && !cbProtectContent->isChecked())) )
                    frame->setFramePadding( uLeft, uTop, uRight, uBottom );
                doc->frameChanged( frame );
            } else {
                KMessageBox::sorry( this,i18n("The frame will not be resized because the new size would be greater than the size of the page."));
            }

            doc->addFrameSet( _frameSet );
            if(!macroCmd)
                macroCmd = new KMacroCommand( i18n("Create Text Frame") );
            KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Text Frame"), frame) ;
            macroCmd->addCommand(cmd);
        }
        if(!isNewFrame && (frameCopy->isCopy()!=frame->isCopy()
                           || frameCopy->frameBehavior()!=frame->frameBehavior()
                           || frameCopy->newFrameBehavior()!=frame->newFrameBehavior()
                           || frameCopy->runAround()!=frame->runAround()
                           || frameCopy->runAroundSide()!=frame->runAroundSide()
                           || frameCopy->runAroundLeft()!=frame->runAroundLeft()
                           || frameCopy->runAroundRight()!=frame->runAroundRight()
                           || frameCopy->runAroundTop()!=frame->runAroundTop()
                           || frameCopy->runAroundBottom()!=frame->runAroundBottom()
                           || (tab5 && frameCopy->backgroundColor()!=frameBrushStyle())))
        {
            if(!macroCmd)
                macroCmd = new KMacroCommand( i18n("Frame Properties") );

            KWFramePropertiesCommand*cmd = new KWFramePropertiesCommand( QString::null, frameCopy, frame );
            macroCmd->addCommand(cmd);
            frameCopy = 0L;
        } else
            delete frameCopy;
    }
    if ( tab4 ) { // TAB Geometry

        KWFrame *f=allFrames.first();
        if(f==0L) f=frame;
        while(f) {
            // The floating attribute applies to the whole frameset...
            KWFrameSet * fs = f->frameSet();
            KWFrameSet * parentFs = fs->getGroupManager() ? fs->getGroupManager() : fs;

            // Floating
            if ( floating->isChecked() &&
                 floating->state() != QButton::NoChange &&
                 !parentFs->isFloating() )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Make Frameset Inline") );

                QValueList<FrameIndex> frameindexList;
                QValueList<FrameMoveStruct> frameindexMove;

                KoPoint oldPos = f->topLeft();

                // turn non-floating frame into floating frame
                KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( QString::null, parentFs, true );
                cmd->execute();

                frameindexList.append( FrameIndex( f ) );
                frameindexMove.append( FrameMoveStruct( oldPos, f->topLeft() ) );

                KWFrameMoveCommand *cmdMoveFrame = new KWFrameMoveCommand( QString::null, frameindexList, frameindexMove );

                macroCmd->addCommand(cmdMoveFrame);
                macroCmd->addCommand(cmd);
            }
            else if ( !floating->isChecked() &&
                      floating->state() != QButton::NoChange &&
                      parentFs->isFloating() )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Make Frameset Non-Inline") );
                // turn floating-frame into non-floating frame
                KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( QString::null, parentFs, false );
                macroCmd->addCommand(cmd);
                cmd->execute();
            }
            if ( fs->isProtectSize() != protectSize->isChecked()
                 && protectSize->state() != QButton::NoChange )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Protect Size") );
                typedef KoSetBasicPropCommand<bool, KWFrameSet, &KWFrameSet::setProtectSize> FramesetSetProtectSizeCommand;
                FramesetSetProtectSizeCommand* cmd = new FramesetSetProtectSizeCommand( fs, QString::null, fs->isProtectSize(), protectSize->isChecked() );
                macroCmd->addCommand(cmd);
                cmd->execute();

            }
            if ( !fs->isMainFrameset() &&
                ( oldX != sx->value() || oldY != sy->value() || oldW != sw->value() || oldH != sh->value() ) )
            {
                //kdDebug() << "Old geom: " << oldX << ", " << oldY<< " " << oldW << "x" << oldH << endl;
                //kdDebug() << "New geom: " << sx->text().toDouble() << ", " << sy->text().toDouble()
                //          << " " << sw->text().toDouble() << "x" << sh->text().toDouble() << endl;

                if( !doc->isOutOfPage( rect, f->pageNum() ) )
                {
                    FrameIndex index( f );
                    KoRect initialRect = f->normalize();
                    double initialMinFrameHeight = f->minFrameHeight();
                    if ( frame ) // single frame: can be moved and resized
                        f->setRect( px, py, pw, ph );
                    else { // multiple frames: can only be resized
                        f->setWidth( pw );
                        f->setHeight( ph );
                    }
                    FrameResizeStruct tmpResize( initialRect, initialMinFrameHeight, f->normalize() );
                    if(!macroCmd)
                        macroCmd = new KMacroCommand( i18n("Resize Frame") );

                    KWFrameResizeCommand *cmd = new KWFrameResizeCommand( i18n("Resize Frame"), index, tmpResize ) ;
                    macroCmd->addCommand(cmd);
                    doc->frameChanged( f );
                }
                else
                {
                    KMessageBox::sorry( this,i18n("The frame will not be resized because the new size would be greater than the size of the page."));
                }
            }
            if ( m_paddingConfigWidget &&
                 (!tab1 || (tab1 && cbProtectContent && !cbProtectContent->isChecked())) &&
                 m_paddingConfigWidget->changed() )
            {
                FrameIndex index( f );
                FramePaddingStruct tmpMargBegin(f);
                FramePaddingStruct tmpMargEnd(uLeft, uTop, uRight, uBottom);
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Change Margin Frame") );
                KWFrameChangeFramePaddingCommand *cmd = new KWFrameChangeFramePaddingCommand( i18n("Change Margin Frame"), index, tmpMargBegin, tmpMargEnd) ;
                cmd->execute();
                macroCmd->addCommand(cmd);
            }
            f=allFrames.next();
        }
    }
    if(macroCmd)
        doc->addCommand(macroCmd);

    updateFrames();
    return true;
}

void KWFrameDia::updateFrames()
{
    QPtrList<KWFrame> frames=doc->getSelectedFrames();

    doc->updateAllFrames();
    doc->layout();

    for(KWFrame *f=frames.first();f;f=frames.next())
        f->updateResizeHandles();

    doc->repaintAllViews();
}

void KWFrameDia::slotApply()
{
    applyChanges();
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
/* belongs to TAB3, is activated when the user selects another frameset from the list */
    if ( !item )
        item = lFrameSList->selectedItem();

    if ( !item ) return; // assertion

    rExistingFrameset->setChecked(true);
    eFrameSetName->setText( item->text(1) );
}

bool KWFrameDia::mayDeleteFrameSet(KWTextFrameSet *fs) {
    if(fs==0) return true;
    if(fs->getNumFrames() > 1) return true;
    KoTextParag * parag = fs->textDocument()->firstParag();
    if(parag==0) return true;
    bool isEmpty = parag->next() == 0L && parag->length() == 1;
    if ( !isEmpty ) {
        int result = KMessageBox::warningContinueCancel(this,
           i18n( "You are about to reconnect the last frame of the "
           "frameset '%1'. "
           "The contents of this frameset will be deleted.\n"
           "Are you sure you want to do that?").arg(fs->getName()),
           i18n("Reconnect Frame"), i18n("&Reconnect"));
        if (result != KMessageBox::Continue)
            return false;
    }
    return true;
}

KWFourSideConfigWidget::KWFourSideConfigWidget( KWDocument* _doc, const QString& title,
                                                QWidget* parent, const char* name )
    : QGroupBox( title, parent, name ),
      doc( _doc ),
      m_changed( false ), noSignal( false )
{
    QGroupBox *grp2 = this;
    QGridLayout* mGrid = new QGridLayout( grp2, 4, 4, KDialog::marginHint(), KDialog::spacingHint() );

    m_synchronize=new QCheckBox( i18n("Synchronize changes"), grp2 );
    QWhatsThis::add(m_synchronize, i18n("<b>Synchronize changes:</b><br/>"
        "When this is checked any change in margins will be used for all directions."));
    mGrid->addMultiCellWidget( m_synchronize, 1, 1, 0, 1 );

    QLabel* lml = new QLabel( i18n( "Left:" ), grp2 );
    //lml->resize( lml->sizeHint() );
    mGrid->addWidget( lml, 2, 0 );

    m_inputLeft = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, doc->unit() );

    mGrid->addWidget( m_inputLeft, 2, 1 );

    QLabel* lmt = new QLabel( i18n( "Top:" ), grp2 );
    //lmt->resize( lmt->sizeHint() );
    mGrid->addWidget( lmt, 2, 2 );

    m_inputTop = new /*KDoubleNumInput*/KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, doc->unit() );

    //m_inputTop->resize( m_inputTop->sizeHint() );

    mGrid->addWidget( m_inputTop, 2, 3 );

    QLabel* lmr = new QLabel( i18n( "Right:" ), grp2 );
    //lmr->resize( lmr->sizeHint() );
    mGrid->addWidget( lmr, 3, 0 );

    m_inputRight = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, doc->unit() );

    //m_inputRight->resize( m_inputRight->sizeHint() );
    mGrid->addWidget( m_inputRight, 3, 1 );

    QLabel* lmb = new QLabel( i18n( "Bottom:" ), grp2 );
    //lmb->resize( lmb->sizeHint() );
    mGrid->addWidget( lmb, 3, 2 );

    m_inputBottom = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, doc->unit() );

    //m_inputBottom->resize( m_inputBottom->sizeHint() );
    mGrid->addWidget( m_inputBottom, 3, 3 );
    mGrid->setRowSpacing( 0, KDialog::spacingHint() + 5 );

    connect( m_inputLeft, SIGNAL( valueChanged(double)), this, SLOT( slotValueChanged( double )));
    connect( m_inputRight, SIGNAL( valueChanged(double)), this, SLOT( slotValueChanged( double )));
    connect( m_inputTop, SIGNAL( valueChanged(double)), this, SLOT( slotValueChanged( double )));
    connect( m_inputBottom, SIGNAL( valueChanged(double)), this, SLOT( slotValueChanged( double )));
}

// Called right after the ctor, so m_synchronize can't be checked
void KWFourSideConfigWidget::setValues( double left, double right, double top, double bottom )
{
    m_inputLeft->setValue( KoUnit::toUserValue( left, doc->unit() ) );
    m_inputRight->setValue( KoUnit::toUserValue( right, doc->unit() ) );
    m_inputTop->setValue( KoUnit::toUserValue( top, doc->unit() ) );
    m_inputBottom->setValue( KoUnit::toUserValue( bottom, doc->unit() ) );
}

void KWFourSideConfigWidget::slotValueChanged( double val )
{
    m_changed = true;
    if ( m_synchronize->isChecked() && !noSignal )
    {
        noSignal = true;
        m_inputLeft->setValue( val );
        m_inputBottom->setValue( val );
        m_inputRight->setValue( val );
        m_inputTop->setValue( val );
        noSignal = false;
    }
}

double KWFourSideConfigWidget::leftValue() const
{
    return KoUnit::fromUserValue( m_inputLeft->value(), doc->unit() );
}

double KWFourSideConfigWidget::rightValue() const
{
    return KoUnit::fromUserValue( m_inputRight->value(), doc->unit() );
}

double KWFourSideConfigWidget::topValue() const
{
    return KoUnit::fromUserValue( m_inputTop->value(), doc->unit() );
}

double KWFourSideConfigWidget::bottomValue() const
{
    return KoUnit::fromUserValue( m_inputBottom->value(), doc->unit() );
}

// not needed, setEnabled does it
// m_inputBottom->setEnabled( state );
// m_inputRight->setEnabled( state );
// m_inputTop->setEnabled( state );
// m_inputLeft->setEnabled( state );
// m_synchronize->setEnabled( state );
