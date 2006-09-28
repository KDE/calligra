/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#include "KWFrameDia.h"
#include "KWTextFrameSet.h"
#include "KWDocument.h"
#include "KWFrameDia.moc"
#include "KWCommand.h"
#include "KWTableFrameSet.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWPictureFrameSet.h"

#include <KoSetPropCommand.h>
#include <KoBorderPreview.h>
#include <KoTextParag.h>

#include <knuminput.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knumvalidator.h>
#include <kcolorbutton.h>
#include <kdebug.h>


#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QRadioButton>
#include <q3listview.h>
#include <q3buttongroup.h>
#include <q3hbox.h>
#include <q3header.h>
#include <QLayout>
//Added by qt3to4:
#include <QPixmap>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <Q3Frame>
#include <QList>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QMouseEvent>

#include <stdlib.h>
#include <limits.h>

/******************************************************************/
/* Class KWBrushStylePreview - only used by kwframestylemanager   */
/******************************************************************/
KWBrushStylePreview::KWBrushStylePreview( QWidget* parent )
    : Q3Frame(parent)
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

KWFrameDia::KWFrameDia( QWidget* parent, KWFrame *frame)
    : KPageDialog( parent)
{
    setButtons( Ok | Apply| Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    m_noSignal=false;
    m_frame = frame;
    m_mainFrame = 0;
    if(m_frame==0) {
        kDebug() << "ERROR: KWFrameDia::constructor no frame.."<<endl;
        return;
    }
    setCaption( i18n( "Frame Properties for %1" , m_frame->frameSet()->name() ) );
    KWFrameSet *fs = m_frame->frameSet()->groupmanager();
    if(fs==0L) fs=m_frame->frameSet();
    m_frameType = fs->type();
    m_frameSetFloating = fs->isFloating();
    m_frameSetProtectedSize = fs->isProtectSize();
    m_mainFrameSetIncluded = fs->isMainFrameset();
    m_defaultFrameSetIncluded = fs->isMainFrameset() || fs->isHeaderOrFooter() || fs->isFootEndNote();
    if ( m_mainFrameSetIncluded )
        m_mainFrame = m_frame;

    m_doc = 0;
    init();
}

/* Contructor when the dialog is used on creation of frame */
KWFrameDia::KWFrameDia( QWidget* parent, KWFrame *frame, KWDocument *doc, FrameSetType ft )
    : KPageDialog(parent)
{
    setCaption( i18n("Frame Properties for New Frame") );
    setButtons(  Ok | Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    m_noSignal=false;
    m_frameType=ft;
    m_doc = doc;
    m_frame= frame;
    m_frameSetFloating = false;
    m_frameSetProtectedSize = false;
    m_mainFrameSetIncluded = false;
    m_defaultFrameSetIncluded = false;
    if(m_frame==0) {
        kDebug() << "ERROR: KWFrameDia::constructor no m_frame.."<<endl;
        return;
    }
    m_mainFrame = 0;
    init();
}

KWFrameDia::KWFrameDia( QWidget *parent, Q3PtrList<KWFrame> listOfFrames)
    : KPageDialog( parent) , m_allFrames() {
    setCaption( i18n("Frames Properties") );
    setButtons( Ok | Apply | Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    m_noSignal=false;

    m_mainFrame = m_frame = 0L;
    m_tab1 = m_tab2 = m_tab3 = m_tab4 = m_tab5 = m_tab6 = 0;

    KWFrame *f=listOfFrames.first();
    if(f==0) {
        kDebug() << "ERROR: KWFrameDia::constructor no frames.."<<endl;
        return;
    }
    if ( listOfFrames.count() == 1 )
        setCaption( i18n( "Frame Settings for %1" , f->frameSet()->name() ) );

    KWFrameSet *fs = f->frameSet()->groupmanager();
    if(fs==0L) fs=f->frameSet();
    m_frameType = fs->type();
    bool frameTypeUnset=true;
    m_doc = fs->kWordDocument();

    if( !fs->isMainFrameset() ) { // don't include the main fs.
        m_allFrames.append(f);
        frameTypeUnset=false;
    }
    else
        m_mainFrame = f;

    f=listOfFrames.next();
    while(f) {
        fs = f->frameSet()->groupmanager();
        if(fs==0L) fs=f->frameSet();
        if(m_doc->processingType() != KWDocument::WP || m_doc->frameSet(0) != fs) { // don't include the main fs.
            if(!frameTypeUnset && m_frameType != fs->type()) m_frameType= FT_TEXT;
            if(frameTypeUnset) {
                m_frameType = fs->type();
                frameTypeUnset = false;
            } else if(m_frameType != fs->type()) m_frameType= FT_TEXT;
            m_allFrames.append(f);
        }
        f=listOfFrames.next();
    }
    if(m_allFrames.count()==0)
        m_allFrames.append(listOfFrames.first());

    // Now that m_allFrames is set, calculate m_mainFrameSetIncluded and m_defaultFrameSetIncluded
    m_mainFrameSetIncluded = false;
    m_defaultFrameSetIncluded = false;
    for ( f = m_allFrames.first(); f; f = m_allFrames.next() ) {
        fs = f->frameSet();
        if ( !m_mainFrameSetIncluded )
        {
            m_mainFrameSetIncluded = fs->isMainFrameset();
            if ( m_mainFrameSetIncluded )
                m_mainFrame = f;
        }
        if ( !m_defaultFrameSetIncluded )
            m_defaultFrameSetIncluded = fs->isMainFrameset() || fs->isHeaderOrFooter() || fs->isFootEndNote();
    }


    init();
}

void KWFrameDia::init() {
#if 0

    m_tab1 = m_tab2 = m_tab3 = m_tab4 = m_tab5 = m_tab6 = 0;
    m_sw = m_sh = 0L;
    m_cbAspectRatio=0L;
    KWFrameSet *fs=0;
    if(m_frame) {
        fs = m_frame->frameSet(); // 0 when creating a frame
        KoRect r = m_frame->normalize();
        m_frame->setRect( r.x(), r.y(), r.width(), r.height() );
    }
    if(!m_doc && fs)
    {
        m_doc = fs->kWordDocument();
    }
    if(!m_doc)
    {
        kDebug() << "ERROR: KWFrameDia::init frame has no reference to m_doc.."<<endl;
        return;
    }
    if( fs && fs->isMainFrameset() )
    {
        setupTab6();
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
        setupTab6();
    }
    else if(m_frameType == FT_TEXT)
    {
        setupTab1();
        if ( fs && !fs->isMainFrameset() && !fs->isHeaderOrFooter() && !fs->isFootEndNote())
            setupTab2();
        else if ( !fs )
            setupTab2();
        setupTab3();
        setupTab4();
        setupTab5();
        setupTab6();
        if(! fs) // first creation
            showPage(2);
    }
    else if(m_frameType == FT_PICTURE)
    {
        setupTab1();
        if ( m_frameType == FT_PICTURE )
            setupTab2();
        if(m_frame)       // not for multiframe dia
            setupTab4();
        setupTab6();
        showPage(1); // while options are not implemented..
    }
    else if(m_frameType == FT_PART)
    {
        setupTab2();
        if(m_frame)       // not for multiframe dia
            setupTab4();
        setupTab6();
    }
    else if(m_frameType == FT_FORMULA)
    {
        setupTab1();
        setupTab2();
        if(m_frame)       // not for multiframe dia
            setupTab4();
        setupTab6();
        showPage(1); // while options are not implemented..
    }
    else if(m_frameType == FT_TABLE)
    {
        setupTab4();
        setupTab5();
        setupTab6();
    }
    setInitialSize( QSize(550, 400) );
#endif
}

void KWFrameDia::setupTab1(){ // TAB Frame Options
#if 0
    //kDebug() << "setup tab 1 Frame options"<<endl;
    m_tab1 = addPage( i18n("Options") );
    int columns = 0;
    if(m_frameType == FT_FORMULA || m_frameType == FT_PICTURE)
        columns = 1;
    else if(m_frameType == FT_TEXT)
        columns = 2;

    m_grid1 = new Q3GridLayout( m_tab1, 0 /*auto create*/, columns, 0, KDialog::spacingHint() );

    // Options for all types of frames
    m_cbCopy = new QCheckBox( i18n("Frame is a copy of the previous frame"),m_tab1 );
    m_grid1->addWidget(m_cbCopy,1,0);

    if(m_frame) {
        m_cbCopy->setChecked( m_frame->isCopy() );
        m_cbCopy->setEnabled( m_frame->frameSet() && m_frame->frameSet()->frame( 0 ) != m_frame ); // First one can't be a copy
    } else { // list of frames as input.
        KWFrame *f=m_allFrames.first();
        bool show=true;
        bool enabled=f->frameSet() && f->frameSet()->frame( 0 ) != f; // First one can't be a copy
        bool checked=f->isCopy();
        f=m_allFrames.next();
        while(f) {
            enabled=enabled || (f->frameSet() && f->frameSet()->frame( 0 ) != f);
            if(checked != f->isCopy()) show=false;
            f=m_allFrames.next();
        }
        if(! show) {
            m_cbCopy->setTristate();
            m_cbCopy->setNoChange();
        }
        else m_cbCopy->setChecked(checked);
        m_cbCopy->setEnabled( enabled );
    }

    // Well, for images, formulas etc. it doesn't make sense to activate 'is copy'. What else would it show ?
    if(m_frameType!=FT_TEXT)
        m_cbCopy->setEnabled( false );

    int row = 2;
    int column = 0;

    // Picture m_frame
    if(m_frameType==FT_PICTURE)
    {
        m_cbAspectRatio = new QCheckBox (i18n("Retain original aspect ratio"),m_tab1);
        connect( m_cbAspectRatio, SIGNAL(toggled(bool)),
                 this, SLOT(slotKeepRatioToggled(bool)));
        bool show=true;
        bool on=true;
        if(m_frame) {
            if ( m_frame->frameSet() )
                on= static_cast<KWPictureFrameSet *>( m_frame->frameSet() )->keepAspectRatio();
        } else {
            KWFrame *f=m_allFrames.first();
            KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
            if(fs)
                on=fs->keepAspectRatio();
            f=m_allFrames.next();
            while(f) {
                KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
                if(fs)
                    if(on != fs->keepAspectRatio()) {
                        show=false;
                        break;
                    }
                f=m_allFrames.next();
            }
        }
        m_cbAspectRatio->setChecked( on );
        if(! show) {
            m_cbAspectRatio->setTristate();
            m_cbAspectRatio->setNoChange();
        }
        m_grid1->addWidget(m_cbAspectRatio, row, 0);
        ++row;
    }
    else
        m_cbAspectRatio = 0L;

    // Text m_frame
    if(m_frameType==FT_TEXT)
    {
        // AutoCreateNewFrame policy.
        m_endOfFrame = new Q3GroupBox(i18n("If Text is Too Long for Frame"), m_tab1 );
        m_grid1->addWidget( m_endOfFrame, row, 0 );

        m_eofGrid= new Q3GridLayout (m_endOfFrame, 4, 1, KDialog::marginHint(), KDialog::spacingHint());
        m_rAppendFrame = new QRadioButton( i18n( "Create a new page" ), m_endOfFrame );
        m_rAppendFrame->setWhatsThis( "<b>Create a new page:</b><br/> if there is too "
            "much text for this text frame, a new page will be created and, "
            "since \"Reconnect frame to current flow\" is the only possible "
            "option together with this one, "
            "the new page will have a frame for showing the additional text." );
        m_eofGrid->addWidget( m_rAppendFrame, 1, 0 );

        m_rResizeFrame = new QRadioButton( i18n( "Resize last frame" ), m_endOfFrame );
        m_rResizeFrame->setWhatsThis( "<b>Resize last frame:</b><br/> "
            "if there is too much text for this text frame, "
            "the frame will resize itself (increasing its height) as much as it needs, "
            "to be able to contain all the text. More precisely, when the frameset has "
            "multiple chained frames, it's always the last one which will be resized." );
        m_eofGrid->addWidget( m_rResizeFrame, 2, 0 );

        m_rNoShow = new QRadioButton( i18n( "Don't show the extra text" ), m_endOfFrame );
        m_rNoShow->setWhatsThis( "<b>Don't show the extra text:</b><br/> "
            "if there is too much text for this text frame, nothing happens "
            "automatically. Initially the extra text won't appear. One possibility "
            "then is to resize the frame manually. The other possibility is, with the option "
            "\"Reconnect frame to current flow\" selected, to create a new page "
            "which will then have a followup frame with the extra text." );
        m_eofGrid->addWidget( m_rNoShow, 3, 0 );
        Q3ButtonGroup *grp = new Q3ButtonGroup( m_endOfFrame );
        grp->hide();
        grp->setExclusive( true );
        grp->insert( m_rAppendFrame );
        grp->insert( m_rResizeFrame );
        grp->insert( m_rNoShow );

        m_eofGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );
        KWFrame::FrameBehavior fb;
        bool show=true;
        if(m_frame) {
            fb = m_frame->frameBehavior();
        } else {
            KWFrame *f=m_allFrames.first();
            fb = f->frameBehavior();
            f=m_allFrames.next();
            while(f) {
                if(fb != f->frameBehavior()) {
                    show=false;
                    break;
                }
                f=m_allFrames.next();
            }
        }
        if(show) {
            if(fb == KWFrame::AutoExtendFrame) {
                m_rResizeFrame->setChecked(true);
            } else if (fb == KWFrame::AutoCreateNewFrame) {
                m_rAppendFrame->setChecked(true);
            } else {
                m_rNoShow->setChecked(true);
            }
        }
        column++;
    } else {
        m_rResizeFrame = 0L;
        m_rAppendFrame = 0L;
        m_rNoShow = 0L;
    }

    // NewFrameBehavior - now for all type of frames
    m_onNewPage = new Q3GroupBox(i18n("On New Page Creation"),m_tab1);
    m_grid1->addWidget( m_onNewPage, row, column );

    m_onpGrid = new Q3GridLayout( m_onNewPage, 4, 1, KDialog::marginHint(), KDialog::spacingHint() );
    m_reconnect = new QRadioButton (i18n ("Reconnect frame to current flow"), m_onNewPage);
    m_reconnect->setWhatsThis( i18n("<b>Reconnect frame to current flow:</b><br/>"
        "When a new page is created, a new frame will be created for this "
        "frameset, so that the text can flow from one page to the next if necessary. "
        "This is what happens for the \"main text frameset\", but this option makes it possible "
        "to choose the same behavior for other framesets, for instance in magazine layouts."));
    if ( m_rResizeFrame )
        connect( m_reconnect, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOn() ) );
    m_onpGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );
    m_onpGrid->addWidget( m_reconnect, 1, 0 );

    m_noFollowup = new QRadioButton (i18n ("Do not create a followup frame"), m_onNewPage);
    m_noFollowup->setWhatsThis( i18n("<b>Do not create a followup frame:</b><br/>"
        "When a new page is created, no frame will be created for this frameset."));
    if ( m_rResizeFrame )
        connect( m_noFollowup, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOn() ) );
    m_onpGrid->addWidget( m_noFollowup, 2, 0 );

    m_copyRadio= new QRadioButton (i18n ("Place a copy of this frame"), m_onNewPage);
    m_copyRadio->setWhatsThis( i18n("<b>Place a copy of this frame:</b><br/>"
        "When a new page is created, a frame will be created for this frameset, "
        "which will always show the exact same thing as the frame on the previous "
        "page. This is what happens for headers and footers, but this option "
        "makes it possible to choose the same behavior for other framesets, for "
        "instance a company logo and/or title that should appear exactly the same on every page."));
    if ( m_rResizeFrame )
        connect( m_copyRadio, SIGNAL( clicked() ), this, SLOT( setFrameBehaviorInputOff() ) );
    m_onpGrid->addWidget( m_copyRadio, 3, 0);

    enableOnNewPageOptions();

    Q3ButtonGroup *grp2 = new Q3ButtonGroup( m_onNewPage );
    grp2->hide();
    grp2->setExclusive( true );
    grp2->insert( m_reconnect );
    grp2->insert( m_noFollowup );
    grp2->insert( m_copyRadio );
    m_grid1->addRowSpacing( row, m_onNewPage->height());
    KWFrame::NewFrameBehavior nfb;
    bool show=true;
    if(m_frame) {
        nfb = m_frame->newFrameBehavior();
    } else {
        KWFrame *f=m_allFrames.first();
        nfb = f->newFrameBehavior();
        f=m_allFrames.next();
        while(f) {
            if(nfb != f->newFrameBehavior()) {
                show=false;
                break;
            }
            f=m_allFrames.next();
        }
    }
    if(show) {
        if(nfb == KWFrame::Reconnect) {
            m_reconnect->setChecked(true);
        } else if(nfb == KWFrame::NoFollowup) {
            m_noFollowup->setChecked(true);
        } else {
            m_copyRadio->setChecked(true);
            setFrameBehaviorInputOff();
        }
    }


    // SideHeads definition - for text frames only
    if( false && m_frameType == FT_TEXT ) // disabled in the GUI for now! (TZ June 2002)
    {
        row++;
        m_sideHeads = new Q3GroupBox(i18n("SideHead Definition"),m_tab1);
        m_sideHeads->setEnabled(false); //###
        m_grid1->addWidget(m_sideHeads, row, 0);

        m_sideGrid = new Q3GridLayout( m_sideHeads, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );
        sideTitle1 = new QLabel ( i18n("Size (%1):",m_doc->unitName()),m_sideHeads);
        sideTitle1->resize(sideTitle1->sizeHint());
        m_sideGrid->addWidget(sideTitle1,1,0);
        m_sideWidth= new QLineEdit(m_sideHeads,"");
        m_sideWidth->setMaxLength(6);
        m_sideGrid->addWidget(m_sideWidth,1,1);
        sideTitle2 = new QLabel( i18n("Gap size (%1):",m_doc->unitName()),m_sideHeads);
        sideTitle2->resize(sideTitle2->sizeHint());
        m_sideGrid->addWidget(sideTitle2,2,0);
        m_sideGap = new QLineEdit(m_sideHeads,"");
        m_sideGap->setMaxLength(6);
        m_sideGrid->addWidget(m_sideGap,2,1);
        m_sideAlign = new QComboBox (false,m_sideHeads);
#warning "kde4: port it		"
        //m_sideAlign->setAutoResize(false);
        m_sideAlign->insertItem ( i18n("Left"));
        m_sideAlign->insertItem ( i18n("Right"));
        m_sideAlign->insertItem ( i18n("Closest to Binding"));
        m_sideAlign->insertItem ( i18n("Closest to Page Edge"));
        m_sideAlign->resize(m_sideAlign->sizeHint());
        m_sideGrid->addMultiCellWidget(m_sideAlign,3,3,0,1);
        m_sideGrid->addRowSpacing( 0, KDialog::marginHint() + 5 );

        // init for sideheads.
        m_sideWidth->setText("0");
        m_sideWidth->setValidator( new KFloatValidator(0,9999,true, m_sideWidth) );

        m_sideGap->setText("0");
        m_sideGap->setValidator( new KFloatValidator(0,9999,true, m_sideGap) );
        // add rest of sidehead init..
    }

    m_cbAllFrames = new QCheckBox (i18n("Changes will be applied to all frames in frameset"),m_tab1);
    m_cbAllFrames->setChecked(m_frame!=0L);
    row++;
    m_grid1->addMultiCellWidget(m_cbAllFrames,row,row, 0, 1);
    m_cbProtectContent = new QCheckBox( i18n("Protect content"), m_tab1);
    m_cbProtectContent->setWhatsThis( i18n("<b>Protect content:</b><br/>"
        "Disallow changes to be made to the contents of the frame(s)."));
    connect( m_cbProtectContent, SIGNAL(toggled ( bool ) ), this, SLOT(slotProtectContentChanged( bool )));
    row++;
    m_grid1->addMultiCellWidget(m_cbProtectContent,row,row, 0, 1);
    if( m_frameType != FT_TEXT || m_frame!=0 && m_frame->frameSet()==0) {
        m_cbAllFrames->setChecked(false);
        m_cbAllFrames->hide();
        m_cbProtectContent->setChecked( false );
        m_cbProtectContent->hide();
    }
    else if ( m_frameType == FT_TEXT /*&& m_frame!=0 && m_frame->frameSet()*/ )
    {
        bool show=true;
        bool on=true;
        if(m_frame)
        {
            if ( m_frame->frameSet() )
                on= static_cast<KWTextFrameSet *>(m_frame->frameSet() )->textObject()->protectContent();
        }
        else
        {
            KWFrame *f=m_allFrames.first();
            KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *> (f->frameSet());
            if(fs)
                on=fs->textObject()->protectContent();
            f=m_allFrames.next();
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
                f=m_allFrames.next();
            }
        }
        m_cbProtectContent->setChecked( on );
        if(! show) {
            m_cbProtectContent->setTristate();
            m_cbProtectContent->setNoChange();
        }
    }

    for(int i=0;i < row;i++)
        m_grid1->setRowStretch( i, 0 );
    m_grid1->setRowStretch( row + 1, 1 );
#endif
}

void KWFrameDia::setupTab2() { // TAB Text Runaround
    m_tab2 = new QWidget();

    addPage( m_tab2, i18n( "Text Run Around" ) );

    Q3VBoxLayout *tabLayout = new Q3VBoxLayout( m_tab2, 0, KDialog::spacingHint(), "tabLayout");

    // First groupbox
    m_runGroup = new Q3ButtonGroup(  i18n( "Layout of Text in Other Frames" ), m_tab2);
    m_runGroup->setColumnLayout( 0, Qt::Vertical );
    m_runGroup->layout()->setSpacing( KDialog::spacingHint() );
    Q3GridLayout *groupBox1Layout = new Q3GridLayout( m_runGroup->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    m_rRunNo = new QRadioButton( i18n( "Text will run &through this frame" ), m_runGroup );
    groupBox1Layout->addWidget( m_rRunNo, 0, 1 );

    m_rRunBounding = new QRadioButton( i18n( "Text will run &around the frame" ), m_runGroup );
    groupBox1Layout->addWidget( m_rRunBounding, 1, 1 );

    m_rRunSkip = new QRadioButton( i18n( "Text will &not run around this frame" ), m_runGroup );
    groupBox1Layout->addWidget( m_rRunSkip, 2, 1 );

    QPixmap pixmap = KWBarIcon( "run_not" );
    QLabel *lRunNo = new QLabel( m_runGroup );
    lRunNo->setBackgroundPixmap( pixmap );
    lRunNo->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunNo, 0, 0 );

    pixmap = KWBarIcon( "run_bounding" );
    QLabel *lRunBounding = new QLabel( m_runGroup );
    lRunBounding->setBackgroundPixmap( pixmap );
    lRunBounding->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunBounding, 1, 0 );

    pixmap = KWBarIcon( "run_skip" );
    QLabel *lRunSkip = new QLabel( m_runGroup );
    lRunSkip->setBackgroundPixmap( pixmap );
    lRunSkip->setFixedSize( pixmap.size() );
    groupBox1Layout->addWidget( lRunSkip, 2, 0 );

    tabLayout->addWidget( m_runGroup );

    // Second groupbox
    m_runSideGroup = new Q3ButtonGroup(  i18n( "Run Around Side" ), m_tab2);
    m_runSideGroup->setColumnLayout( 0, Qt::Vertical );
    m_runSideGroup->layout()->setSpacing( KDialog::spacingHint() );
    m_runSideGroup->layout()->setMargin( KDialog::marginHint() );
    Q3GridLayout *runSideLayout = new Q3GridLayout( m_runSideGroup->layout() );
    runSideLayout->setAlignment( Qt::AlignTop );

    m_rRunLeft = new QRadioButton( i18nc( "Run Around", "&Left" ), m_runSideGroup );
    runSideLayout->addWidget( m_rRunLeft, 0, 0 /*1*/ );

    m_rRunRight = new QRadioButton( i18nc( "Run Around", "&Right" ), m_runSideGroup );
    runSideLayout->addWidget( m_rRunRight, 1, 0 /*1*/ );

    m_rRunBiggest = new QRadioButton( i18nc( "Run Around", "Lon&gest side" ), m_runSideGroup );
    runSideLayout->addWidget( m_rRunBiggest, 2, 0 /*1*/ );

#if 0 // TODO icons!
    QPixmap pixmap = KWBarIcon( "run_left" );
    QLabel *label = new QLabel( m_runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 0, 0 );

    pixmap = KWBarIcon( "run_right" );
    label = new QLabel( m_runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 1, 0 );

    pixmap = KWBarIcon( "run_biggest" );
    label = new QLabel( m_runSideGroup );
    label->setBackgroundPixmap( pixmap );
    label->setFixedSize( pixmap.size() );
    runSideLayout->addWidget( label, 2, 0 );
#endif

    tabLayout->addWidget( m_runSideGroup );

    m_raDistConfigWidget = new KWFourSideConfigWidget( m_doc, i18n("Distance Between Frame && Text"), m_tab2 );
    if ( m_frame )
        m_raDistConfigWidget->setValues( qMax(0.00, m_frame->runAroundLeft()),
                                         qMax(0.00, m_frame->runAroundRight()),
                                         qMax(0.00, m_frame->runAroundTop()),
                                         qMax(0.00, m_frame->runAroundBottom()) );
    tabLayout->addWidget( m_raDistConfigWidget );



    // Show current settings

    // Runaround
    bool show=true;
    KWFrame::RunAround ra = KWFrame::RA_NO;
    if ( m_frame )
        ra = m_frame->runAround();
    else {
        KWFrame *f=m_allFrames.first();
        ra = f->runAround();
        f=m_allFrames.next();
        while(f) {
            if(ra != f->runAround()) show=false;
            f=m_allFrames.next();
        }
    }

    if(show) {
        switch ( ra ) {
            case KWFrame::RA_NO: m_rRunNo->setChecked( true ); break;
            case KWFrame::RA_BOUNDINGRECT: m_rRunBounding->setChecked( true ); break;
            case KWFrame::RA_SKIP: m_rRunSkip->setChecked( true ); break;
        }
    }

    // Runaround side
    show = true;
    KWFrame::RunAroundSide rs = KWFrame::RA_BIGGEST;
    if ( m_frame )
        rs = m_frame->runAroundSide();
    else {
        KWFrame *f=m_allFrames.first();
        rs = f->runAroundSide();
        f = m_allFrames.next();
        while(f) {
            if(rs != f->runAroundSide()) show=false;
            f=m_allFrames.next();
        }
    }

    if(show) {
        switch ( rs ) {
            case KWFrame::RA_LEFT: m_rRunLeft->setChecked( true ); break;
            case KWFrame::RA_RIGHT: m_rRunRight->setChecked( true ); break;
            case KWFrame::RA_BIGGEST: m_rRunBiggest->setChecked( true ); break;
        }
    }

    // Runaround gap
    show=true;
    double ragapLeft = 0;
    double ragapRight = 0;
    double ragapTop = 0;
    double ragapBottom = 0;
    if ( m_frame ) {
        ragapLeft = m_frame->runAroundLeft();
        ragapRight = m_frame->runAroundRight();
        ragapTop = m_frame->runAroundTop();
        ragapBottom = m_frame->runAroundBottom();
    }
    else {
        KWFrame *f = m_allFrames.first();
        ragapLeft = f->runAroundLeft();
        ragapRight = f->runAroundRight();
        ragapTop = f->runAroundTop();
        ragapBottom = f->runAroundBottom();
        for( f = m_allFrames.next() ; f ; f = m_allFrames.next() ) {
            if( ragapLeft != f->runAroundLeft() ||
                ragapRight != f->runAroundRight() ||
                ragapTop != f->runAroundTop() ||
                ragapBottom != f->runAroundBottom() )
                show = false; // TODO special value in the spinbox
            f = m_allFrames.next();
        }
    }

    if(show)
        m_raDistConfigWidget->setValues( ragapLeft, ragapRight, ragapTop, ragapBottom );

    enableRunAround();

    // Changing the type of runaround needs to enable/disable the runaround-side options
    connect( m_runGroup, SIGNAL( clicked(int) ), this, SLOT( enableRunAround() ) );
}

void KWFrameDia::setupTab3(){ // TAB Frameset
    /*
     * here the user can select from the current TEXT framesets
     * Afterwards (on ok) the frame should be checked if it is already owned by a
     * frameset, if so that connection must be disconnected (if different) and
     * framebehaviour will be copied from the frameset
     * then the new connection should be made.
     */
    //kDebug() << "setup tab 3 frameSet"<<endl;
    m_tab3 = new QWidget();
    addPage(m_tab3, i18n( "Connect Text Frames" ) );

    Q3VBoxLayout *tabLayout = new Q3VBoxLayout( m_tab3, 0, KDialog::spacingHint());

    Q3ButtonGroup *myGroup = new Q3ButtonGroup(this);
    myGroup->hide();

    m_rExistingFrameset = new QRadioButton( m_tab3, "m_rExistingFrameset" );
    m_rExistingFrameset->setText( i18n("Select existing frameset to connect frame to:") );
    tabLayout->addWidget( m_rExistingFrameset );
    myGroup->insert(m_rExistingFrameset,1);
    connect (m_rExistingFrameset, SIGNAL( toggled(bool)), this, SLOT(ensureValidFramesetSelected()));

    Q3HBoxLayout *layout2 = new Q3HBoxLayout( 0, 0, 6);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout2->addItem( spacer );

    m_lFrameSList = new Q3ListView( m_tab3, "m_lFrameSList" );
    m_lFrameSList->addColumn( i18n("No.") );
    m_lFrameSList->addColumn( i18n("Frameset Name") );
    m_lFrameSList->setAllColumnsShowFocus( true );
    m_lFrameSList->header()->setMovingEnabled( false );
    connect( m_lFrameSList, SIGNAL(selectionChanged ()),this,SLOT(selectExistingFrameset ()) );
    connect (m_lFrameSList, SIGNAL( selectionChanged()), this, SLOT(ensureValidFramesetSelected()));

    layout2->addWidget( m_lFrameSList );
    tabLayout->addLayout( layout2 );

    m_rNewFrameset = new QRadioButton( m_tab3);
    m_rNewFrameset->setText( i18n( "Create a new frameset" ) );
    tabLayout->addWidget( m_rNewFrameset );
    myGroup->insert(m_rNewFrameset,2);

    Q3Frame *line1 = new Q3Frame( m_tab3 );
    line1->setProperty( "frameShape", (int)Q3Frame::HLine );
    line1->setFrameShadow( Q3Frame::Plain );
    line1->setFrameShape( Q3Frame::HLine );
    tabLayout->addWidget( line1 );

    Q3HBoxLayout *layout1 = new Q3HBoxLayout( 0, 0, 6 );
    QLabel *textLabel1 = new QLabel( m_tab3 );
    textLabel1->setText( i18n( "Name of frameset:" ) );
    layout1->addWidget( textLabel1 );

    m_eFrameSetName = new QLineEdit( m_tab3 );
    layout1->addWidget( m_eFrameSetName );
    tabLayout->addLayout( layout1 );

    int amount=0;
    // now fill the gui.
    for ( unsigned int i = 0; i < m_doc->frameSetCount(); i++ ) {
        KWFrameSet * fs = m_doc->frameSet( i );
        if ( i == 0 && m_doc->processingType() == KWDocument::WP )
            continue;
        if ( fs->type() != FT_TEXT || fs->isHeaderOrFooter() )
            continue;
        if ( fs->frameSetInfo() == KWFrameSet::FI_FOOTNOTE )
            continue;
        if ( fs->groupmanager() )
            continue;
        if ( fs->isDeleted() )
            continue;
        Q3ListViewItem *item = new Q3ListViewItem( m_lFrameSList );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, fs->name() );
        amount++;
        if( m_frame && m_frame->frameSet() == fs ) {
            m_lFrameSList->setSelected(item, true );
            m_oldFrameSetName = fs->name();
            m_rExistingFrameset->setChecked(true);
        }
    }
    if(amount==0) {
        m_rNewFrameset->setChecked(/*true*/false);
        m_rNewFrameset->setEnabled(false);
        m_rExistingFrameset->setEnabled(false);
        m_lFrameSList->setEnabled(false);
    }
    //we can't create a new frame when we select
    //multi frame!!!!
    if ( m_allFrames.count() > 1 ) {
        m_rNewFrameset->setChecked(false);
        m_rNewFrameset->setEnabled(false);
        myGroup->setRadioButtonExclusive( false );
    }
    if(m_frame && m_frame->frameSet() == 0) {
        m_oldFrameSetName = m_doc->generateFramesetName( i18n( "Text Frameset %1" ) );
        m_rNewFrameset->setChecked(true);
    }
    m_eFrameSetName->setText( m_oldFrameSetName );

    connect( m_lFrameSList, SIGNAL( currentChanged( Q3ListViewItem * ) ),
             this, SLOT( connectListSelected( Q3ListViewItem * ) ) );
    connect(m_eFrameSetName, SIGNAL(textChanged ( const QString & ) ),
             this,SLOT(textNameFrameChanged ( const QString & ) ) );
    connect(m_eFrameSetName, SIGNAL(textChanged ( const QString & )),
             this,SLOT(textNameFrameChanged ( const QString & ) ));
    connect( m_rNewFrameset, SIGNAL(toggled (bool)),
             this,SLOT(selectNewFrameset (bool)) );
}

void KWFrameDia::selectExistingFrameset() {
    m_rExistingFrameset->setChecked(true);
}

void KWFrameDia::selectNewFrameset(bool on) {
    if(!on) return;

    Q3ListViewItem *frameSetItem  = m_lFrameSList->selectedItem();
    if ( !frameSetItem)
        return;
    QString str = frameSetItem->text( 0 );
    KWFrameSet *fs = m_doc->frameSet(str.toInt() - 1);

    frameSetItem->setText(1, fs->name() );
}

void KWFrameDia::textNameFrameChanged ( const QString &text )
{
    if(m_rExistingFrameset->isChecked()) {
        Q3ListViewItem *item = m_lFrameSList->selectedItem();
        if ( !item )
            return;
        item->setText(1, text );
    }
    if(m_rNewFrameset->isChecked() || m_rExistingFrameset->isChecked()) //when one of both is clicked.
        enableButtonOk( !text.isEmpty() );
    else
        enableButtonOk( true );
}

void KWFrameDia::setupTab4() { // TAB Geometry
    m_noSignal = true;
    m_tab4 = new QWidget();

    addPage(m_tab4, i18n( "Geometry" ) );
    Q3GridLayout* grid4 = new Q3GridLayout( m_tab4, 5, 1, 0, KDialog::spacingHint() );

    m_floating = new QCheckBox( i18n("Frame is inline"), m_tab4 );

    connect( m_floating, SIGNAL( toggled(bool) ), this, SLOT( slotFloatingToggled(bool) ) );
    int row = 0;
    grid4->addMultiCellWidget( m_floating, row, row, 0, 1 );

    row++;
    m_protectSize = new QCheckBox( i18n("Protect size and position"), m_tab4);
    grid4->addMultiCellWidget( m_protectSize, row, row, 0, 1 );
    connect( m_protectSize, SIGNAL( toggled(bool) ), this, SLOT( slotProtectSizeToggled(bool) ) );

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

    m_grp1 = new Q3GroupBox( i18n("Position"), m_tab4 );
    Q3GridLayout* pGrid = new Q3GridLayout( m_grp1, 3, 4, KDialog::marginHint(), KDialog::spacingHint() );

    m_lx = new QLabel( i18n( "Left:" ), m_grp1 );
    m_lx->resize( m_lx->sizeHint() );
    pGrid->addWidget( m_lx, 1, 0 );

    m_sx = new KoUnitDoubleSpinBox( m_grp1, 0, 9999, 1, 0.0, m_doc->unit() );

    m_sx->resize( m_sx->sizeHint() );
    pGrid->addWidget( m_sx, 1, 1 );

    m_ly = new QLabel( i18n( "Top:" ), m_grp1 );
    m_ly->resize( m_ly->sizeHint() );
    pGrid->addWidget( m_ly, 1, 2 );

    m_sy = new KoUnitDoubleSpinBox( m_grp1, 0, 9999, 1, 0.0, m_doc->unit() );
    m_sy->resize( m_sy->sizeHint() );
    pGrid->addWidget( m_sy, 1, 3 );

    m_lw = new QLabel( i18n( "Width:" ), m_grp1 );
    m_lw->resize( m_lw->sizeHint() );
    pGrid->addWidget( m_lw, 2, 0 );

    m_sw = new KoUnitDoubleSpinBox( m_grp1, 0, 9999, 1, 0.0, m_doc->unit() );

    m_sw->resize( m_sw->sizeHint() );
    connect( m_sw, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateHeightForWidth(double)) );

    pGrid->addWidget( m_sw, 2, 1 );

    m_lh = new QLabel( i18n( "Height:" ), m_grp1 );
    m_lh->resize( m_lh->sizeHint() );
    pGrid->addWidget( m_lh, 2, 2 );

    m_sh = new KoUnitDoubleSpinBox( m_grp1, 0, 9999, 1, 0.0, m_doc->unit() );
    connect( m_sh, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateWidthForHeight(double)) );

    m_sh->resize( m_sh->sizeHint() );

    pGrid->addWidget( m_sh, 2, 3 );

    pGrid->setRowSpacing( 0, KDialog::spacingHint() + 5 );

    row++;
    grid4->addMultiCellWidget( m_grp1, row, row, 0,1 );

    if(m_frame) {
        m_paddingConfigWidget = new KWFourSideConfigWidget( m_doc, i18n("Margins"), m_tab4 );
        m_paddingConfigWidget->setValues( qMax(0.00, m_frame->paddingLeft()),
                                          qMax(0.00, m_frame->paddingRight()),
                                          qMax(0.00, m_frame->paddingTop()),
                                          qMax(0.00, m_frame->paddingBottom()) );
        row++;
        grid4->addMultiCellWidget( m_paddingConfigWidget, row, row, 0, 1 );

        if ( m_tab1 && m_cbProtectContent )
        {
            m_paddingConfigWidget->setEnabled( !m_cbProtectContent->isChecked() );
        }
    }
    else
    {
        m_paddingConfigWidget = 0;
    }

    initGeometrySettings();

    if ( !m_frame || m_defaultFrameSetIncluded ) {
        // is multi frame, positions don't work for that..
        // also not for default frames.
        m_sx->setEnabled( false );
        m_sy->setEnabled( false );
        m_lx->setEnabled( false );
        m_ly->setEnabled( false );
        m_lw->setEnabled( false );
        m_lh->setEnabled( false );
        m_floating->setEnabled( false );
    }

    if ( m_defaultFrameSetIncluded || m_mainFrameSetIncluded)
    {
        m_sw->setEnabled( false );
        m_sh->setEnabled( false );
        m_grp1->setEnabled( false );
    }

    if ( m_mainFrameSetIncluded )
    {
        m_grp1->hide();
        m_floating->hide( );
        m_protectSize->hide();
    }
    m_noSignal=false;

    ++row;
    grid4->setRowStretch( row, 1 );
}

void KWFrameDia::initGeometrySettings()
{
#if 0
    if ( m_frame ) {
        // is single frame dia. Fill position strings and checkboxes now.

        // Can't use frame->pageNum() here since frameset might be 0
        int pageNum = m_doc->pageManager()->pageNumber(m_frame);

        m_sx->setValue( KoUnit::toUserValue( m_frame->x(), m_doc->unit() ) );
        m_sy->setValue( KoUnit::toUserValue( m_frame->y() - m_doc->pageManager()->topOfPage(pageNum), m_doc->unit() ) );
        m_sw->setValue( KoUnit::toUserValue( m_frame->width(), m_doc->unit() ) );
        m_sh->setValue( KoUnit::toUserValue( m_frame->height(), m_doc->unit() ) );

        calcRatio();

        // userValue leads to some rounding -> store old values from the ones
        // displayed, so that the "change detection" in apply() works.
        m_oldX = m_sx->value();
        m_oldY = m_sy->value();
        m_oldW = m_sw->value();
        m_oldH = m_sh->value();

#ifdef ALLOW_NON_INLINE_TABLES
        KWFrameSet * fs = m_frame->frameSet();
        if ( fs && fs->groupmanager() )
            m_floating->setText( i18n( "Table is inline" ) );
#else
        m_floating->hide();
#endif

        m_floating->setChecked( m_frameSetFloating );
        m_protectSize->setChecked( m_frameSetProtectedSize);
    } else {
        // multi frame. Fill inline and protect checkbox, leave away the position strings.
        KWFrame *f=m_allFrames.first();
        KWFrameSet *fs=f->frameSet();
        bool ps=fs->isProtectSize();
        m_protectSize->setChecked( ps );

        bool table=fs->groupmanager();
        if(table)
            fs=fs->groupmanager();
        bool inlineframe =fs->isFloating();
        m_floating->setChecked( inlineframe );

        double commonWidth = f->width();
        double commonHeight = f->height();
        m_sw->setEnabled( true );
        m_sh->setEnabled( true );

        f=m_allFrames.next();
        while(f) {
            KWFrameSet *fs=f->frameSet();
            if(ps != fs->isProtectSize()) {
                m_protectSize->setTristate();
                m_protectSize->setNoChange();
            }
            if(fs->groupmanager()) //table
                fs=fs->groupmanager();
            else
                table=false;

            if(inlineframe != fs->isFloating()) {
                m_floating->setTristate();
                m_floating->setNoChange();
            }

            if ( qAbs( f->width() - commonWidth ) > 1E-6 ) {
                kDebug() << k_funcinfo << "width differs:" << f->width() << " " << commonWidth << endl;
                m_sw->setEnabled( false );
            }
            if ( qAbs( f->height() - commonHeight ) > 1E-6 ) {
                kDebug() << k_funcinfo << "height differs:" << f->height() << " " << commonHeight << endl;
                m_sh->setEnabled( false );
            }

            f=m_allFrames.next();
        }
        // TODO show a special value when frames have a different width/height
        if ( m_sw->isEnabled() )
            m_sw->setValue( KoUnit::toUserValue( commonWidth, m_doc->unit() ) );
        if ( m_sh->isEnabled() )
            m_sh->setValue( KoUnit::toUserValue( commonHeight, m_doc->unit() ) );
        if(table)
        {
#ifdef ALLOW_NON_INLINE_TABLES
            m_floating->setText( i18n( "Table is inline" ) );
#else
            m_floating->hide();
#endif
        }
    }
#endif
}

void KWFrameDia::setupTab5() { // Tab Background fill/color
    m_tab5 = new QWidget();

    addPage(m_tab5, i18n("Background") );
    Q3GridLayout* grid5 = new Q3GridLayout( m_tab5, 0 /*auto*/, 2, 0, KDialog::spacingHint() );

    int row = 0;
    if (!m_frame ) {
        m_overwriteColor = new QCheckBox (i18n("Set new color on all selected frames"), m_tab5);
        grid5->addMultiCellWidget(m_overwriteColor,row,row,0,1);
        row++;
    }
    //brushPreview=new KWBrushStylePreview(m_tab5);
    //grid5->addMultiCellWidget(brushPreview,row,5,1,1);

    m_transparentCB = new QCheckBox( i18n( "Transparent background" ), m_tab5 );
    grid5->addWidget(m_transparentCB,row++,0);
    m_transparentCB->setEnabled( !m_mainFrameSetIncluded );

    QLabel *labelBgColor = new QLabel( i18n( "Background color:" ), m_tab5 );

    grid5->addWidget(labelBgColor,row++,0);

    m_brushColor = new KColorButton( Qt::white, KWDocument::defaultBgColor(0), m_tab5 );
    grid5->addWidget(m_brushColor,row++,0);

//    connect( m_brushColor, SIGNAL( changed( const QColor & ) ),
//        this, SLOT( updateBrushPreview() ) );


    // ###########################
    // Fill styles are ugly and not WYSIWYG (due to being pixel-based)
    // Feature not in OOo either (they have a configurable level of transparency instead, much nicer)
#if 0
    l = new QLabel( i18n( "Background style:" ), m_tab5 );
    grid5->addWidget(l,row++,0);

    brushStyle = new QComboBox( false,m_tab5, "BStyle" );
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

    connect( m_transparentCB, SIGNAL( toggled( bool ) ), labelBgColor, SLOT( setDisabled( bool ) ) );
    connect( m_transparentCB, SIGNAL( toggled( bool ) ), m_brushColor, SLOT( setDisabled( bool ) ) );
    initBrush();

    QSpacerItem* spacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid5->addItem( spacer,row,0 );
}

void KWFrameDia::setupTab6() // Border style
{
    m_tab6 = new QWidget();

    addPage(m_tab6, i18n("&Borders") );

    Q3GridLayout *grid = new Q3GridLayout( m_tab6, 8, 2, 0, KDialog::spacingHint() );

    QLabel * lStyle = new QLabel( i18n( "St&yle:" ), m_tab6 );
    grid->addWidget( lStyle, 0, 0 );

    m_cStyle = new QComboBox( false, m_tab6 );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::SOLID ) );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::DASH ) );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::DOT ) );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::DASH_DOT ) );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::DASH_DOT_DOT ) );
    m_cStyle->insertItem( KoBorder::getStyle( KoBorder::DOUBLE_LINE  ) );
    lStyle->setBuddy( m_cStyle );
    grid->addWidget( m_cStyle, 1, 0 );

    QLabel * lWidth = new QLabel( i18n( "&Width:" ), m_tab6 );
    grid->addWidget( lWidth, 2, 0 );

    m_cWidth = new QComboBox( false, m_tab6 );
    for( unsigned int i = 1; i <= 10; i++ )
        m_cWidth->insertItem(QString::number(i));
    lWidth->setBuddy( m_cWidth );
    grid->addWidget( m_cWidth, 3, 0 );

    QLabel * lColor = new QLabel( i18n( "Co&lor:" ), m_tab6 );
    grid->addWidget( lColor, 4, 0 );

    m_bColor = new KColorButton( Qt::black,
                                Qt::black,
                                m_tab6 );

    lColor->setBuddy( m_bColor );
    grid->addWidget( m_bColor, 5, 0 );

    Q3ButtonGroup * bb = new Q3HButtonGroup( m_tab6 );
    //bb->setFrameStyle(Q3Frame::NoFrame);
    m_bLeft = new QPushButton(bb);
    m_bLeft->setPixmap( BarIcon( "borderleft" ) );
    m_bLeft->setToggleButton( true );
    m_bRight = new QPushButton(bb);
    m_bRight->setPixmap( BarIcon( "borderright" ) );
    m_bRight->setToggleButton( true );
    m_bTop = new QPushButton(bb);
    m_bTop->setPixmap( BarIcon( "bordertop" ) );
    m_bTop->setToggleButton( true );
    m_bBottom = new QPushButton(bb);
    m_bBottom->setPixmap( BarIcon( "borderbottom" ) );
    m_bBottom->setToggleButton( true );
    grid->addWidget( bb, 6, 0 );

    connect( m_bLeft, SIGNAL( toggled( bool ) ), this, SLOT( brdLeftToggled( bool ) ) );
    connect( m_bRight, SIGNAL( toggled( bool ) ), this, SLOT( brdRightToggled( bool ) ) );
    connect( m_bTop, SIGNAL( toggled( bool ) ), this, SLOT( brdTopToggled( bool ) ) );
    connect( m_bBottom, SIGNAL( toggled( bool ) ), this, SLOT( brdBottomToggled( bool ) ) );

    initBorderSettings();

    Q3GroupBox *grp=new Q3GroupBox( 0, Qt::Vertical, i18n( "Preview" ), m_tab6, "previewgrp" );
    grid->addMultiCellWidget( grp , 0, 7, 1, 1 );
    grp->layout()->setSpacing(KDialog::spacingHint());
    grp->layout()->setMargin(KDialog::marginHint());
    m_prev3 = new KoBorderPreview( grp );
    Q3VBoxLayout *lay1 = new Q3VBoxLayout( grp->layout() );
    lay1->addWidget(m_prev3);

    connect( m_prev3, SIGNAL( choosearea(QMouseEvent * ) ),
             this, SLOT( slotPressEvent(QMouseEvent *) ) );

    // update the bordet preview widget
    m_prev3->setLeftBorder( m_leftBorder );
    m_prev3->setRightBorder( m_rightBorder );
    m_prev3->setTopBorder( m_topBorder );
    m_prev3->setBottomBorder( m_bottomBorder );

    grid->setRowStretch( 7, 1 );
    grid->setColumnStretch( 1, 1 );
}

void KWFrameDia::initBorderSettings()
{
#if 0
    KWFrame *f = m_mainFrame ? m_mainFrame : ( m_frame ? m_frame : m_allFrames.first() );
    m_leftBorder = f->leftBorder();
    m_rightBorder = f->rightBorder();
    m_topBorder = f->topBorder();
    m_bottomBorder = f->bottomBorder();
#endif
}

#define OFFSETX 15
#define OFFSETY 7
#define Ko_SPACE 30
void KWFrameDia::slotPressEvent(QMouseEvent *ev)
{
    QRect r = m_prev3->contentsRect();
    QRect rect(r.x()+OFFSETX,r.y()+OFFSETY,r.width()-OFFSETX,r.y()+OFFSETY+Ko_SPACE);
    if(rect.contains(QPoint(ev->x(),ev->y())))
    {
        if( (  ((int)m_topBorder.penWidth() != m_cWidth->currentText().toInt()) ||(m_topBorder.color != m_bColor->color() )
               ||(m_topBorder.getStyle()!=KoBorder::getStyle(m_cStyle->currentText()) )) && m_bTop->isOn() )
        {
            m_topBorder.setPenWidth( m_cWidth->currentText().toInt() );
            m_topBorder.color = QColor( m_bColor->color() );
            m_topBorder.setStyle(KoBorder::getStyle(m_cStyle->currentText()));
            m_prev3->setTopBorder( m_topBorder );
        }
        else
            m_bTop->setOn(!m_bTop->isOn());
    }
    rect.setCoords(r.x()+OFFSETX,r.height()-OFFSETY-Ko_SPACE,r.width()-OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(ev->x(),ev->y())))
    {
        if( (  ((int)m_bottomBorder.penWidth() != m_cWidth->currentText().toInt()) ||(m_bottomBorder.color != m_bColor->color() )
               ||(m_bottomBorder.getStyle()!=KoBorder::getStyle(m_cStyle->currentText()) )) && m_bBottom->isOn() )
        {
            m_bottomBorder.setPenWidth(m_cWidth->currentText().toInt());
            m_bottomBorder.color = QColor( m_bColor->color() );
            m_bottomBorder.setStyle(KoBorder::getStyle(m_cStyle->currentText()));
            m_prev3->setBottomBorder( m_bottomBorder );
        }
        else
            m_bBottom->setOn(!m_bBottom->isOn());
    }

    rect.setCoords(r.x()+OFFSETX,r.y()+OFFSETY,r.x()+Ko_SPACE+OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(ev->x(),ev->y())))
    {

        if( (  ((int)m_leftBorder.penWidth() != m_cWidth->currentText().toInt()) ||(m_leftBorder.color != m_bColor->color() )
               ||(m_leftBorder.getStyle()!=KoBorder::getStyle(m_cStyle->currentText()) )) && m_bLeft->isOn() )
        {
            m_leftBorder.setPenWidth( m_cWidth->currentText().toInt());
            m_leftBorder.color = QColor( m_bColor->color() );
            m_leftBorder.setStyle(KoBorder::getStyle(m_cStyle->currentText()));
            m_prev3->setLeftBorder( m_leftBorder );
        }
        else
            m_bLeft->setOn(!m_bLeft->isOn());
    }
    rect.setCoords(r.width()-OFFSETX-Ko_SPACE,r.y()+OFFSETY,r.width()-OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(ev->x(),ev->y())))
    {

        if( (  ((int)m_rightBorder.penWidth() != m_cWidth->currentText().toInt()) ||(m_rightBorder.color != m_bColor->color() )
               ||(m_rightBorder.getStyle()!=KoBorder::getStyle(m_cStyle->currentText()) )) && m_bRight->isOn() )
        {
            m_rightBorder.setPenWidth( m_cWidth->currentText().toInt());
            m_rightBorder.color = m_bColor->color();
            m_rightBorder.setStyle(KoBorder::getStyle(m_cStyle->currentText()));
            m_prev3->setRightBorder( m_rightBorder );
        }
        else
            m_bRight->setOn(!m_bRight->isOn());
    }
}
#undef OFFSETX
#undef OFFSETY
#undef Ko_SPACE


void KWFrameDia::brdLeftToggled( bool on )
{
    if ( !on )
        m_leftBorder.setPenWidth(0);
    else {
        m_leftBorder.setPenWidth(m_cWidth->currentText().toInt());
        m_leftBorder.color = m_bColor->color();
        m_leftBorder.setStyle(KoBorder::getStyle( m_cStyle->currentText() ));
    }
    m_prev3->setLeftBorder( m_leftBorder );
}

void KWFrameDia::brdRightToggled( bool on )
{
    if ( !on )
        m_rightBorder.setPenWidth(0);
    else {
        m_rightBorder.setPenWidth(m_cWidth->currentText().toInt());
        m_rightBorder.color = m_bColor->color();
        m_rightBorder.setStyle( KoBorder::getStyle( m_cStyle->currentText() ));
    }
    m_prev3->setRightBorder( m_rightBorder );
}

void KWFrameDia::brdTopToggled( bool on )
{
    if ( !on )
        m_topBorder.setPenWidth(0);
    else {
        m_topBorder.setPenWidth(m_cWidth->currentText().toInt());
        m_topBorder.color = m_bColor->color();
        m_topBorder.setStyle(KoBorder::getStyle( m_cStyle->currentText() ));
    }
    m_prev3->setTopBorder( m_topBorder );
}

void KWFrameDia::brdBottomToggled( bool on )
{
    if ( !on )
        m_bottomBorder.setPenWidth ( 0 );
    else {
        m_bottomBorder.setPenWidth( m_cWidth->currentText().toInt());
        m_bottomBorder.color = m_bColor->color();
        m_bottomBorder.setStyle(KoBorder::getStyle(m_cStyle->currentText()));
    }
    m_prev3->setBottomBorder( m_bottomBorder );
}

void KWFrameDia::slotProtectContentChanged( bool b )
{
    if (m_tab4 && !m_noSignal && m_paddingConfigWidget) {
        m_paddingConfigWidget->setEnabled( !b );
    }
}

void KWFrameDia::slotUpdateWidthForHeight(double height)
{
    if ( !m_cbAspectRatio || m_cbAspectRatio->state() != QCheckBox::NoChange)
        return;
    if ( m_heightByWidthRatio == 0 )
        return; // avoid DBZ
    m_sw->setValue( height / m_heightByWidthRatio );

}

void KWFrameDia::slotUpdateHeightForWidth( double width )
{
    if ( !m_cbAspectRatio || m_cbAspectRatio->state() != QCheckBox::NoChange)
        return;
    m_sh->setValue( width * m_heightByWidthRatio );
}

void KWFrameDia::slotKeepRatioToggled(bool on)
{
    if ( !on || !m_sw || !m_sh) return;
    calcRatio();
}
void KWFrameDia::ensureValidFramesetSelected()
{
    enableButtonOk( m_rNewFrameset->isChecked() || m_rExistingFrameset->isChecked() && m_lFrameSList->selectedItem() != NULL);
}

void KWFrameDia::calcRatio()
{
    if ( m_sw->value() == 0 )
        m_heightByWidthRatio = 1.0; // arbitrary
    else
        m_heightByWidthRatio = m_sh->value() / m_sw->value();
}

void KWFrameDia::initBrush()
{
#if 0
    bool allFramesSame=true;
    if ( m_frame )
        m_newBrushStyle = m_frame->backgroundColor();
    else {
        KWFrame *f=m_allFrames.first();
        m_newBrushStyle = f->backgroundColor();
        f=m_allFrames.next();
        while(f) {
            if(m_newBrushStyle != f->backgroundColor()) {
                allFramesSame=false;
                break;
            }
            f=m_allFrames.next();
        }
        m_overwriteColor->setChecked(allFramesSame);
    }

    m_transparentCB->setChecked( m_newBrushStyle.style() == Qt::NoBrush );

#if 0
    switch ( m_newBrushStyle.style() )
    {
        case Qt::NoBrush:
            brushStyle->setCurrentItem( 0 );
            break;
        case Qt::SolidPattern:
            brushStyle->setCurrentItem( 1 );
            break;
        case Qt::Dense1Pattern:
            brushStyle->setCurrentItem( 2 );
            break;
        case Qt::Dense2Pattern:
            brushStyle->setCurrentItem( 3 );
            break;
        case Qt::Dense3Pattern:
            brushStyle->setCurrentItem( 4 );
            break;
        case Qt::Dense4Pattern:
            brushStyle->setCurrentItem( 5 );
            break;
        case Qt::Dense5Pattern:
            brushStyle->setCurrentItem( 6 );
            break;
        case Qt::Dense6Pattern:
            brushStyle->setCurrentItem( 7 );
            break;
        case Qt::Dense7Pattern:
            brushStyle->setCurrentItem( 8 );
            break;
        case Qt::HorPattern:
            brushStyle->setCurrentItem( 9 );
            break;
        case Qt::VerPattern:
            brushStyle->setCurrentItem( 10 );
            break;
        case Qt::CrossPattern:
            brushStyle->setCurrentItem( 11 );
            break;
        case Qt::BDiagPattern:
            brushStyle->setCurrentItem( 12 );
            break;
        case Qt::FDiagPattern:
            brushStyle->setCurrentItem( 13 );
            break;
        case Qt::DiagCrossPattern:
            brushStyle->setCurrentItem( 14 );
            break;
        case Qt::TexturePattern:
            break;
    }
#endif

    QColor col = KWDocument::resolveBgColor( m_newBrushStyle.color(), 0 );
    m_brushColor->setColor( col );
#endif
}

QBrush KWFrameDia::frameBrushStyle() const
{
    QBrush brush;

    brush.setStyle( m_transparentCB->isChecked() ? Qt::NoBrush : Qt::SolidPattern );

#if 0
    switch ( brushStyle->currentItem() )
    {
        case 0:
            brush.setStyle( Qt::NoBrush );
            break;
        case 1:
            brush.setStyle( Qt::SolidPattern );
            break;
        case 2:
            brush.setStyle( Qt::Dense1Pattern );
            break;
        case 3:
            brush.setStyle( Qt::Dense2Pattern );
            break;
        case 4:
            brush.setStyle( Qt::Dense3Pattern );
            break;
        case 5:
            brush.setStyle( Qt::Dense4Pattern );
            break;
        case 6:
            brush.setStyle( Qt::Dense5Pattern );
            break;
        case 7:
            brush.setStyle( Qt::Dense6Pattern );
            break;
        case 8:
            brush.setStyle( Qt::Dense7Pattern );
            break;
        case 9:
            brush.setStyle( Qt::HorPattern );
            break;
        case 10:
            brush.setStyle( Qt::VerPattern );
            break;
        case 11:
            brush.setStyle( Qt::CrossPattern );
            break;
        case 12:
            brush.setStyle( Qt::BDiagPattern );
            break;
        case 13:
            brush.setStyle( Qt::FDiagPattern );
            break;
        case 14:
            brush.setStyle( Qt::DiagCrossPattern );
            break;
    }
#endif

    brush.setColor( m_brushColor->color() );

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

// Called when "m_reconnect" or "no followup" is checked
void KWFrameDia::setFrameBehaviorInputOn() {
    if ( m_tab4 && m_floating->isChecked() )
        return;
    if( m_rAppendFrame && m_rResizeFrame && m_rNoShow /*&& !m_rAppendFrame->isEnabled()*/ ) {
        if(m_frameBehavior== KWFrame::AutoExtendFrame) {
            m_rResizeFrame->setChecked(true);
        } else if (m_frameBehavior== KWFrame::AutoCreateNewFrame) {
            m_rAppendFrame->setChecked(true);
        } else {
            m_rNoShow->setChecked(true);
        }
        m_rResizeFrame->setEnabled(true);
        // Can't have "create a new page" if "no followup", that wouldn't work
        kDebug() << "setFrameBehaviorInputOn: m_reconnect->isChecked()==" << m_reconnect->isChecked() << endl;
        m_rAppendFrame->setEnabled( m_reconnect->isChecked() );
        m_rNoShow->setEnabled(true);
    }
}

// Called when "place a copy" is checked
void KWFrameDia::setFrameBehaviorInputOff() {
    if ( m_tab4 && m_floating->isChecked() )
        return;
    if( m_rAppendFrame && m_rResizeFrame && m_rNoShow && m_rAppendFrame->isEnabled() ) {
        if(m_rResizeFrame->isChecked()) {
            m_frameBehavior=KWFrame::AutoExtendFrame;
        } else if ( m_rAppendFrame->isChecked()) {
            m_frameBehavior=KWFrame::AutoCreateNewFrame;
        } else {
            m_frameBehavior=KWFrame::Ignore;
        }
        // In "Place a copy" mode, we can't have "create new page if text too long"
        if ( m_rAppendFrame->isChecked() )
            m_rNoShow->setChecked(true);
        m_rAppendFrame->setEnabled(false);
        m_rResizeFrame->setEnabled(true);
        m_rNoShow->setEnabled(true);
    }
}

void KWFrameDia::enableSizeAndPosition()
{
    bool canMove = ( m_floating->state() == QCheckBox::Off ) // can move if no frame is floating
                   && ( m_protectSize->state() == QCheckBox::Off ) // protects size too
                   && !m_defaultFrameSetIncluded // those can't be moved
                   && m_frame; // can't move if multiple frames selected
    m_sx->setEnabled( canMove );
    m_sy->setEnabled( canMove );
    bool canResize = ( m_protectSize->state() == QCheckBox::Off ) // can resize if no frame is protect-size'd
                     && !m_defaultFrameSetIncluded; // those can't be resized
    m_sw->setEnabled( canResize );
    m_sh->setEnabled( canResize );
}

void KWFrameDia::slotProtectSizeToggled(bool)
{
    enableSizeAndPosition();
}

void KWFrameDia::slotFloatingToggled(bool b)
{
    enableSizeAndPosition();
    if (m_tab1 && m_rAppendFrame && m_rResizeFrame && m_rNoShow ) {
        m_cbCopy->setEnabled( !b ); // 'copy' irrelevant for floating frames.
        if ( m_rAppendFrame )
        {
            m_rAppendFrame->setEnabled( !b ); // 'create new page' irrelevant for floating frames.
            if ( b && m_rAppendFrame->isChecked() )
                m_rNoShow->setChecked( true );
        }
        enableOnNewPageOptions();
        if ( b ) {
            m_noFollowup->setChecked( true );
            m_cbCopy->setChecked( false );
        } else {
            // Revert to non-inline frame stuff
            m_rResizeFrame->setEnabled(true);
            m_rAppendFrame->setEnabled(true);
            m_rNoShow->setEnabled(true);
        }
    }

    enableRunAround();
}

// Enable or disable the "on new page" options
void KWFrameDia::enableOnNewPageOptions()
{
    if ( m_tab1 )
    {
        bool f = m_tab4 && m_floating->isChecked();
        // 'what happens on new page' is irrelevant for floating frames
        m_reconnect->setEnabled( !f );
        m_noFollowup->setEnabled( !f );
        m_copyRadio->setEnabled( !f );

        if( m_frameType != FT_TEXT )
            m_reconnect->setEnabled( false );
        else if(m_frame) {
            KWFrameSet *fs = m_frame->frameSet(); // 0 when creating a frame
            if ( fs && (fs->isHeaderOrFooter() || fs->isFootEndNote() ))
            {
                m_reconnect->setEnabled( false );
                m_noFollowup->setEnabled( false );
            }
        }
    }
}

void KWFrameDia::enableRunAround()
{
    if ( m_tab2 )
    {
        if ( m_tab4 && m_floating->isChecked() ) {
            m_runGroup->setEnabled( false ); // Runaround options don't make sense for floating frames
        } else
        {
            if ( m_frame && m_frame->frameSet() )
                m_runGroup->setEnabled( !m_frameSetFloating && !m_frame->frameSet()->isMainFrameset() && !m_frame->frameSet()->isHeaderOrFooter() && !m_frame->frameSet()->isFootEndNote() );
            else
                m_runGroup->setEnabled( true );
        }
        m_runSideGroup->setEnabled( m_runGroup->isEnabled() && m_rRunBounding->isChecked() );
        m_raDistConfigWidget->setEnabled( m_runGroup->isEnabled() &&
            ( m_rRunBounding->isChecked() || m_rRunSkip->isChecked() ) );
    }
}

bool KWFrameDia::applyChanges()
{
#if 0
    kDebug() << "KWFrameDia::applyChanges"<<endl;
    KWFrame *frameCopy = 0L;
    bool isNewFrame=false;
    if(m_frame) { // only do undo/redo when we have 1 frame to change for now..
        frameCopy = m_frame->getCopy(); // keep a copy of the original (for undo/redo)
        isNewFrame = m_frame->frameSet() == 0L; // true if we are creating a newframe
    }
    QString name;

    KMacroCommand * macroCmd=0L;
    if ( m_tab3 ) { // TAB Frameset
        // Frame/Frameset belonging, and frameset naming
        // We basically have three cases:
        // * Creating a new frame (fs==0), and creating a frameset (m_rNewFrameset selected)
        // * Creating a frame (fs==0), and attaching to an existing frameset (other)
        // * Editing a frame (fs!=0), possibly changing the frameset attachment (maybe creating a new one)

        name = m_eFrameSetName->text();
        if ( name.isEmpty() ) // Don't allow empty names
            name = m_doc->generateFramesetName( i18n( "Text Frameset %1" ) );
        KWFrameSet *fs = 0L;
        Q3ListViewItem *frameSetItem  = m_lFrameSList->selectedItem();
        if(frameSetItem) {
            QString str = frameSetItem->text( 0 );
            fs = m_doc->frameSet(str.toInt() - 1);
        }
        if(m_rNewFrameset->isChecked()) { // create a new FS.
            if(m_frame && m_frame->frameSet()) {
                // disconnect.
                if(! mayDeleteFrameSet( static_cast<KWTextFrameSet*>(m_frame->frameSet())))
                    return false;
                m_frame->frameSet()->deleteFrame( m_frame, false );
            } else {
                // first check all frames and ask the user if its ok to disconnect.
                for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next()) {
                    if(! mayDeleteFrameSet( static_cast<KWTextFrameSet*>(f->frameSet())))
                        return false;
                }
                for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                    f->frameSet()->deleteFrame( f, false );
            }
        } else if(m_rExistingFrameset->isChecked()) { // rename and/or m_reconnect a new frameset for this frame.
            if(frameSetItem && (fs->name() != frameSetItem->text( 1 ))) { // rename FS.
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Rename Frameset") );
                // Rename frameset
                typedef KoSetPropCommand<QString, KWFrameSet, &KWFrameSet::setName> FramesetNameCommand;
                FramesetNameCommand* cmd = new FramesetNameCommand( fs, i18n( "Rename Frameset" ), fs->name(), frameSetItem->text( 1 ) );
                macroCmd->addCommand(cmd);
                cmd->execute();
            }
            if(m_frame && fs ) {
                if(m_frame->frameSet() != fs)  {
                    if(m_frame->frameSet()!=0) {
                        // m_reconnect.
                        if(! mayDeleteFrameSet( dynamic_cast<KWTextFrameSet*>(m_frame->frameSet())))
                            return false;
                        m_frame->frameSet()->deleteFrame( m_frame, false );
                    }
                    fs->addFrame(m_frame);
                }
            } else if ( fs ){
                // first check all frames and ask the user if its ok to m_reconnect.
                for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next()) {
                    if(f->frameSet() != fs) {  // m_reconnect.
                        if(! mayDeleteFrameSet( dynamic_cast<KWTextFrameSet*>(f->frameSet())))
                            return false;
                    }
                }
                if ( fs )
                {
                    // then do the reconnects.
                    for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next()) {
                        KWFrameSet *fs2=f->frameSet();
                        if(! (fs2->isHeaderOrFooter() || fs2->isMainFrameset()) ) {
                            if(fs2 != fs) {  // m_reconnect.
                                f->frameSet()->deleteFrame( f, false );
                                fs->addFrame(f);
                            }
                        }
                    }
                }
            }
        }
        if(m_rNewFrameset->isChecked() || m_rExistingFrameset->isChecked()) {
            // check if new name is unique
            for (Q3PtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator(); fit.current() ; ++fit ) {
                if ( !fit.current()->isDeleted() &&  // Allow to reuse a deleted frameset's name
                     fs != fit.current() && fit.current()->name() == name) {
                    if ( m_rNewFrameset->isChecked() )
                        KMessageBox::sorry( this,
                                            i18n( "A new frameset with the name '%1' "
                                                  "can not be made because a frameset with that name "
                                                  "already exists. Please enter another name or select "
                                                  "an existing frameset from the list.",name));
                    else
                        KMessageBox::sorry( this,
                                            i18n( "A frameset with the name '%1' "
                                                  "already exists. Please enter another name.", name) );
                    m_eFrameSetName->setText(m_oldFrameSetName);
                    return false;
                }
            }
        }
    }
    if ( m_tab1 ) { // TAB Frame Options
        // Copy
        if(m_frame)
            m_frame->setCopy( m_cbCopy->isChecked() );
        else if(m_cbCopy->state() != QCheckBox::NoChange) {
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next()) {
                if(f == f->frameSet()->frame(0))  continue; // skip first frame of any frameset.
                f->setCopy( m_cbCopy->isChecked() );
            }
        }

        // FrameBehavior
        if ( m_frameType == FT_TEXT )
        {
            bool update=true;
            KWFrame::FrameBehavior fb=KWFrame::AutoCreateNewFrame;
            if(m_rResizeFrame->isChecked())
                fb = KWFrame::AutoExtendFrame;
            else if ( m_rAppendFrame->isChecked())
                fb = KWFrame::AutoCreateNewFrame;
            else if ( m_rNoShow->isChecked())
                fb = KWFrame::Ignore;
            else
                update=false;

            if(m_frame)
                if(m_cbAllFrames->isChecked() && m_frame->frameSet())
                    m_frame->frameSet()->setFrameBehavior(fb);
                else
                    m_frame->setFrameBehavior(fb);
            else if(update) {
                for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                    if(m_cbAllFrames->isChecked())
                        f->frameSet()->setFrameBehavior(fb);
                    else
                        f->setFrameBehavior(fb);
            }
            if ( m_frame && m_frame->frameSet())
            {
                KWTextFrameSet * frm=static_cast<KWTextFrameSet *>( m_frame->frameSet() );
                if(frm->textObject()->protectContent()!=m_cbProtectContent->isChecked())
                {
                    if(!macroCmd)
                        macroCmd = new KMacroCommand( i18n("Protect Content") );
                    KWProtectContentCommand * cmd = new KWProtectContentCommand( i18n("Protect Content"), frm,m_cbProtectContent->isChecked() );
                    cmd->execute();
                    macroCmd->addCommand(cmd);
                }
            }
            else
            {
                if ( m_cbProtectContent->state() != QCheckBox::NoChange)
                {
                    for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                    {
                        KWTextFrameSet * frm=dynamic_cast<KWTextFrameSet *>( f->frameSet() );
                        if ( frm )
                        {
                            if(frm->textObject()->protectContent()!=m_cbProtectContent->isChecked())
                            {
                                if(!macroCmd)
                                    macroCmd = new KMacroCommand( i18n("Protect Content") );
                                KWProtectContentCommand * cmd = new KWProtectContentCommand( i18n("Protect Content"), frm,m_cbProtectContent->isChecked() );
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
        if( m_reconnect && m_reconnect->isChecked() )
            nfb = KWFrame::Reconnect;
        else if ( m_noFollowup->isChecked() )
            nfb = KWFrame::NoFollowup;
        else if ( m_copyRadio->isChecked() )
            nfb = KWFrame::Copy;
        else
            update=false;

        if(m_frame)
            if(m_cbAllFrames->isChecked() && m_frame->frameSet())
                m_frame->frameSet()->setNewFrameBehavior(nfb);
            else
                m_frame->setNewFrameBehavior(nfb);
        else if(update)
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                if(m_cbAllFrames->isChecked())
                    f->frameSet()->setNewFrameBehavior(nfb);
                else
                    f->setNewFrameBehavior(nfb);

        // aspect ratio
        if ( m_cbAspectRatio && (m_frameType==FT_PICTURE))
        {
            typedef KoSetBasicPropCommand<bool, KWPictureFrameSet, &KWPictureFrameSet::setKeepAspectRatio> FramesetSetKeepAspectRatioCommand;
            if(m_frame) {
                KWPictureFrameSet * frm=static_cast<KWPictureFrameSet *>( m_frame->frameSet() );
                if ( frm->keepAspectRatio() != m_cbAspectRatio->isChecked() )
                {
                    if(!macroCmd)
                        macroCmd = new KMacroCommand( i18n("Frame Properties") );
                    FramesetSetKeepAspectRatioCommand* cmd = new FramesetSetKeepAspectRatioCommand( frm, QString::null, frm->keepAspectRatio(), m_cbAspectRatio->isChecked() );

                    cmd->execute();

                    macroCmd->addCommand(cmd);
                }
            } else if(m_cbAspectRatio->state() != QCheckBox::NoChange) {
                for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next()) {
                    KWPictureFrameSet *fs = dynamic_cast<KWPictureFrameSet *> (f->frameSet());
                    if(fs) {
                        if(fs->keepAspectRatio()!=m_cbAspectRatio->isChecked())
                        {
                            if(!macroCmd)
                                macroCmd = new KMacroCommand( i18n("Frame Properties") );
                            FramesetSetKeepAspectRatioCommand* cmd = new FramesetSetKeepAspectRatioCommand( fs, QString::null, fs->keepAspectRatio(), m_cbAspectRatio->isChecked() );

                            cmd->execute();

                            macroCmd->addCommand(cmd);
                        }
                    }
                }
            }
        }
    }
    if ( m_tab2 ) { // TAB Text Runaround
        // Run around
        KWFrame::RunAround ra=KWFrame::RA_BOUNDINGRECT;
        bool update=true;
        if ( m_rRunNo->isChecked() )
            ra = KWFrame::RA_NO;
        else if ( m_rRunBounding->isChecked() )
            ra = KWFrame::RA_BOUNDINGRECT;
        else if ( m_rRunSkip->isChecked() )
            ra = KWFrame::RA_SKIP;
        else
            update=false;
        if(m_frame)
            m_frame->setRunAround(ra);
        else if (update) {
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                f->setRunAround(ra);
        }

        // Run around side.
        KWFrame::RunAroundSide rs=KWFrame::RA_BIGGEST;
        update=true;
        if ( m_rRunLeft->isChecked() )
            rs = KWFrame::RA_LEFT;
        else if ( m_rRunRight->isChecked() )
            rs = KWFrame::RA_RIGHT;
        else if ( m_rRunBiggest->isChecked() )
            rs = KWFrame::RA_BIGGEST;
        else
            update=false;
        if(m_frame)
            m_frame->setRunAroundSide(rs);
        else if (update) {
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                f->setRunAroundSide(rs);
        }

        // Run around gap.
        double runAroundLeft = m_raDistConfigWidget->leftValue();
        double runAroundRight = m_raDistConfigWidget->rightValue();
        double runAroundTop = m_raDistConfigWidget->topValue();
        double runAroundBottom = m_raDistConfigWidget->bottomValue();

        if(m_frame) {
            m_frame->setRunAroundGap( runAroundLeft, runAroundRight, runAroundTop, runAroundBottom );
        }
        else
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
                f->setRunAroundGap( runAroundLeft, runAroundRight, runAroundTop, runAroundBottom );
    }
    if(m_tab5) { // Tab Background fill/color
        QBrush tmpBrush=frameBrushStyle();
        if(m_frame) {
            if ( tmpBrush != KWDocument::resolveBgBrush( m_frame->backgroundColor() ) ) {
                m_frame->setBackgroundColor(tmpBrush);
            }
        } else if(m_overwriteColor->isChecked()) {
            for(KWFrame *f=m_allFrames.first();f; f=m_allFrames.next())
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
    if(m_tab4) { // TAB Geometry
        if ( m_frame ) {
            px = qMax( 0.0, m_sx->value() );
            int pageNum = m_doc->pageManager()->pageNumber(m_frame);
            py = qMax( 0.0, m_sy->value() ) + m_doc->pageManager()->topOfPage(pageNum);
        }
        pw = qMax( m_sw->value(), 0.0 );
        ph = qMax( m_sh->value(), 0.0);
        if ( m_paddingConfigWidget )
        {
            uLeft = m_paddingConfigWidget->leftValue();
            uRight = m_paddingConfigWidget->rightValue();
            uTop = m_paddingConfigWidget->topValue();
            uBottom = m_paddingConfigWidget->bottomValue();
        }
    }
    KoRect rect( px, py, pw, ph );

    //kDebug() << "New geom: " << m_sx->text().toDouble() << ", " << m_sy->text().toDouble()
    //<< " " << m_sw->text().toDouble() << "x" << m_sh->text().toDouble() << endl;
    //kDebug()<<" rect :"<<px <<" py :"<<py<<" pw :"<<pw <<" ph "<<ph<<endl;
    // Undo/redo for frame properties
    if(m_frame) { // only do undo/redo when we edit 1 frame for now..

        if(m_frame->frameSet() == 0L ) { // if there is no frameset (anymore)
            KWTextFrameSet *frameSet = new KWTextFrameSet( m_doc, name );
            frameSet->addFrame( m_frame );
            KWPage *page = m_doc->pageManager()->page(m_frame);
            if( page->rect().contains(rect) ) {
                m_frame->setRect( px, py, pw, ph );
                //don't change margins when frame is protected.
                if ( m_paddingConfigWidget && ( !m_tab1 || (m_tab1 && m_cbProtectContent && !m_cbProtectContent->isChecked())) )
                    m_frame->setFramePadding( uLeft, uTop, uRight, uBottom );
                m_doc->frameChanged( m_frame );
            } else {
                KMessageBox::sorry( this,i18n("The frame will not be resized because the new size would be greater than the size of the page."));
            }

            m_doc->addFrameSet( frameSet );
            if(!macroCmd)
                macroCmd = new KMacroCommand( i18n("Create Text Frame") );
            KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create Text Frame"), m_frame) ;
            macroCmd->addCommand(cmd);
        }
        if(!isNewFrame && (frameCopy->isCopy()!=m_frame->isCopy()
                           || frameCopy->frameBehavior()!=m_frame->frameBehavior()
                           || frameCopy->newFrameBehavior()!=m_frame->newFrameBehavior()
                           || frameCopy->runAround()!=m_frame->runAround()
                           || frameCopy->runAroundSide()!=m_frame->runAroundSide()
                           || frameCopy->runAroundLeft()!=m_frame->runAroundLeft()
                           || frameCopy->runAroundRight()!=m_frame->runAroundRight()
                           || frameCopy->runAroundTop()!=m_frame->runAroundTop()
                           || frameCopy->runAroundBottom()!=m_frame->runAroundBottom()
                           || (m_tab5 && KWDocument::resolveBgBrush( frameCopy->backgroundColor() )!=frameBrushStyle())))
        {
            if(!macroCmd)
                macroCmd = new KMacroCommand( i18n("Frame Properties") );

            KWFramePropertiesCommand*cmd = new KWFramePropertiesCommand( QString::null, frameCopy, m_frame );
            macroCmd->addCommand(cmd);
            frameCopy = 0L;
        } else
            delete frameCopy;
    }
    if ( m_tab4 ) { // TAB Geometry

        KWFrame *f=m_allFrames.first();
        if(f==0L) f=m_frame;
        while(f) {
            // The floating attribute applies to the whole frameset...
            KWFrameSet * fs = f->frameSet();
            KWFrameSet * parentFs = fs->groupmanager() ? fs->groupmanager() : fs;

            // Floating
            if ( m_floating->isChecked() &&
                 m_floating->state() != QCheckBox::NoChange &&
                 !parentFs->isFloating() )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Make Frameset Inline") );

                QList<FrameIndex> frameindexList;
                QList<FrameMoveStruct> frameindexMove;

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
            else if ( !m_floating->isChecked() &&
                      m_floating->state() != QCheckBox::NoChange &&
                      parentFs->isFloating() )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Make Frameset Non-Inline") );
                // turn floating-frame into non-floating frame
                KWFrameSetInlineCommand *cmd = new KWFrameSetInlineCommand( QString::null, parentFs, false );
                macroCmd->addCommand(cmd);
                cmd->execute();
            }
            if ( fs->isProtectSize() != m_protectSize->isChecked()
                 && m_protectSize->state() != QCheckBox::NoChange )
            {
                if(!macroCmd)
                    macroCmd = new KMacroCommand( i18n("Protect Size") );
                typedef KoSetBasicPropCommand<bool, KWFrameSet, &KWFrameSet::setProtectSize> FramesetSetProtectSizeCommand;
                FramesetSetProtectSizeCommand* cmd = new FramesetSetProtectSizeCommand( fs, QString::null, fs->isProtectSize(), m_protectSize->isChecked() );
                macroCmd->addCommand(cmd);
                cmd->execute();
                m_frameSetProtectedSize = m_protectSize->isChecked();
            }
            if ( !fs->isMainFrameset() &&
                ( ( m_oldX != m_sx->value() && m_sx->isEnabled() )|| ( m_oldY != m_sy->value() && m_sy->isEnabled() ) || ( m_oldW != m_sw->value() && m_sw->isEnabled() ) || ( m_oldH != m_sh->value() && m_sh->isEnabled() ) ) )
            {
                //kDebug() << "Old geom: " << m_oldX << ", " << m_oldY<< " " << m_oldW << "x" << m_oldH << endl;
                //kDebug() << "New geom: " << m_sx->text().toDouble() << ", " << m_sy->text().toDouble()
                  //        << " " << m_sw->text().toDouble() << "x" << m_sh->text().toDouble() << endl;

                if( m_doc->pageManager()->page(f)->rect().contains(rect) )
                {
                    FrameIndex index( f );
                    KoRect initialRect = f->normalize();
                    double initialMinFrameHeight = f->minimumFrameHeight();
                    if ( m_frame ) // single frame: can be moved and resized
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
                    m_doc->frameChanged( f );
                }
                else
                {
                    KMessageBox::sorry( this,i18n("The frame will not be resized because the new size would be greater than the size of the page."));
                }
            }
            if ( m_paddingConfigWidget &&
                 (!m_tab1 || (m_tab1 && m_cbProtectContent && !m_cbProtectContent->isChecked())) &&
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
            f=m_allFrames.next();
        }
    }
    if ( m_tab6 ) // Border style
    {
        if ( m_mainFrame )
            m_allFrames.append( m_mainFrame );

        KWFrame *f = m_allFrames.first();
        if(f==0L) f=m_frame;
        for ( ; f ; f = m_allFrames.next() )
        {
            // TODO can't this become shorter??
            if ( f->leftBorder() != m_leftBorder )
            {
                kDebug() << "Changing borders!" << endl;
                Q3PtrList<KWFrameBorderCommand::FrameBorderTypeStruct> tmpBorderList;
                Q3PtrList<FrameIndex> frameindexList;
                f = KWFrameSet::settingsFrame( f );
                FrameIndex *index = new FrameIndex( f );
                KWFrameBorderCommand::FrameBorderTypeStruct *tmp =new KWFrameBorderCommand::FrameBorderTypeStruct;
                tmp->m_OldBorder = f->leftBorder();
                tmp->m_EFrameType = KoBorder::LeftBorder;
                tmpBorderList.append( tmp );
                frameindexList.append( index );
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Border"));
                KWFrameBorderCommand *cmd = new KWFrameBorderCommand(i18n("Change Left Border Frame"), frameindexList, tmpBorderList, m_leftBorder);
                cmd->execute();
                macroCmd->addCommand( cmd);
            }
            if ( f->rightBorder() != m_rightBorder )
            {
                Q3PtrList<KWFrameBorderCommand::FrameBorderTypeStruct> tmpBorderList;
                Q3PtrList<FrameIndex> frameindexList;
                f = KWFrameSet::settingsFrame( f );
                FrameIndex *index = new FrameIndex( f );
                KWFrameBorderCommand::FrameBorderTypeStruct *tmp =new KWFrameBorderCommand::FrameBorderTypeStruct;
                tmp->m_OldBorder = f->rightBorder();
                tmp->m_EFrameType = KoBorder::RightBorder;
                tmpBorderList.append( tmp );
                frameindexList.append( index );
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Border"));
                KWFrameBorderCommand *cmd = new KWFrameBorderCommand(i18n("Change Right Border Frame"), frameindexList, tmpBorderList, m_rightBorder);
                cmd->execute();
                macroCmd->addCommand( cmd);
            }
            if ( f->topBorder() != m_topBorder )
            {
                Q3PtrList<KWFrameBorderCommand::FrameBorderTypeStruct> tmpBorderList;
                Q3PtrList<FrameIndex> frameindexList;
                f = KWFrameSet::settingsFrame( f );
                FrameIndex *index = new FrameIndex( f );
                KWFrameBorderCommand::FrameBorderTypeStruct *tmp =new KWFrameBorderCommand::FrameBorderTypeStruct;
                tmp->m_OldBorder = f->topBorder();
                tmp->m_EFrameType = KoBorder::TopBorder;
                tmpBorderList.append( tmp );
                frameindexList.append( index );
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Border"));
                KWFrameBorderCommand *cmd = new KWFrameBorderCommand(i18n("Change Top Border Frame"), frameindexList, tmpBorderList, m_topBorder);
                cmd->execute();
                macroCmd->addCommand( cmd);
            }
            if ( f->bottomBorder() != m_bottomBorder )
            {
                Q3PtrList<KWFrameBorderCommand::FrameBorderTypeStruct> tmpBorderList;
                Q3PtrList<FrameIndex> frameindexList;
                f = KWFrameSet::settingsFrame( f );
                FrameIndex *index = new FrameIndex( f );
                KWFrameBorderCommand::FrameBorderTypeStruct *tmp =new KWFrameBorderCommand::FrameBorderTypeStruct;
                tmp->m_OldBorder = f->bottomBorder();
                tmp->m_EFrameType = KoBorder::BottomBorder;
                tmpBorderList.append( tmp );
                frameindexList.append( index );
                if ( !macroCmd )
                    macroCmd = new KMacroCommand( i18n("Change Border"));
                KWFrameBorderCommand *cmd = new KWFrameBorderCommand(i18n("Change Bottom Border Frame"), frameindexList, tmpBorderList, m_bottomBorder);
                cmd->execute();
                macroCmd->addCommand( cmd);
            }
        }
    }

    if(macroCmd)
        m_doc->addCommand(macroCmd);

    updateFrames();
    return true;
#endif
}

void KWFrameDia::updateFrames()
{
    m_doc->updateAllFrames();
    m_doc->layout();
    m_doc->repaintAllViews();
}

void KWFrameDia::slotApply()
{
    applyChanges();
    if ( m_tab4 )
        initGeometrySettings();
    if ( m_tab6 )
        initBorderSettings();
}

void KWFrameDia::slotOk()
{
    if (applyChanges())
    {
        KDialog::accept();
    }
}

void KWFrameDia::connectListSelected( Q3ListViewItem *item )
{
/* belongs to TAB3, is activated when the user selects another frameset from the list */
    if ( !item )
        item = m_lFrameSList->selectedItem();

    if ( !item ) return; // assertion

    m_rExistingFrameset->setChecked(true);
    m_eFrameSetName->setText( item->text(1) );
}

bool KWFrameDia::mayDeleteFrameSet(KWTextFrameSet *fs) {
#if 0
    if(fs==0) return true;
    if(fs->frameCount() > 1) return true;
    KoTextParag * parag = fs->textDocument()->firstParag();
    if(parag==0) return true;
    bool isEmpty = parag->next() == 0L && parag->length() == 1;
    if ( !isEmpty ) {
        int result = KMessageBox::warningContinueCancel(this,
           i18n( "You are about to reconnect the last frame of the "
           "frameset '%1'. "
           "The contents of this frameset will be deleted.\n"
           "Are you sure you want to do that?").arg(fs->name()),
           i18n("Reconnect Frame"), i18n("&Reconnect"));
        if (result != KMessageBox::Continue)
            return false;
    }
    return true;
#endif
}

KWFourSideConfigWidget::KWFourSideConfigWidget( KWDocument* doc, const QString& title,
                                                QWidget* parent, const char* name )
    : Q3GroupBox( title, parent, name ),
      m_doc( doc ),
      m_changed( false ), m_noSignal( false )
{
    Q3GroupBox *grp2 = this;
    Q3GridLayout* mGrid = new Q3GridLayout( grp2, 4, 4, KDialog::marginHint(), KDialog::spacingHint() );

    m_synchronize=new QCheckBox( i18n("Synchronize changes"), grp2 );
    m_synchronize->setWhatsThis( i18n("<b>Synchronize changes:</b><br/>"
        "When this is checked any change in margins will be used for all directions."));
    mGrid->addMultiCellWidget( m_synchronize, 1, 1, 0, 1 );

    QLabel* lml = new QLabel( i18n( "Left:" ), grp2 );
    //lml->resize( lml->sizeHint() );
    mGrid->addWidget( lml, 2, 0 );

    m_inputLeft = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, m_doc->unit() );

    mGrid->addWidget( m_inputLeft, 2, 1 );

    QLabel* lmt = new QLabel( i18n( "Top:" ), grp2 );
    //lmt->resize( lmt->sizeHint() );
    mGrid->addWidget( lmt, 2, 2 );

    m_inputTop = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, m_doc->unit() );

    //m_inputTop->resize( m_inputTop->sizeHint() );

    mGrid->addWidget( m_inputTop, 2, 3 );

    QLabel* lmr = new QLabel( i18n( "Right:" ), grp2 );
    //lmr->resize( lmr->sizeHint() );
    mGrid->addWidget( lmr, 3, 0 );

    m_inputRight = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, m_doc->unit() );

    //m_inputRight->resize( m_inputRight->sizeHint() );
    mGrid->addWidget( m_inputRight, 3, 1 );

    QLabel* lmb = new QLabel( i18n( "Bottom:" ), grp2 );
    //lmb->resize( lmb->sizeHint() );
    mGrid->addWidget( lmb, 3, 2 );

    m_inputBottom = new KoUnitDoubleSpinBox( grp2, 0, 9999, 1, 0.0, m_doc->unit() );

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
    m_inputLeft->setValue( KoUnit::toUserValue( left, m_doc->unit() ) );
    m_inputRight->setValue( KoUnit::toUserValue( right, m_doc->unit() ) );
    m_inputTop->setValue( KoUnit::toUserValue( top, m_doc->unit() ) );
    m_inputBottom->setValue( KoUnit::toUserValue( bottom, m_doc->unit() ) );
}

void KWFourSideConfigWidget::slotValueChanged( double val )
{
    m_changed = true;
    if ( m_synchronize->isChecked() && !m_noSignal )
    {
        m_noSignal = true;
        m_inputLeft->setValue( val );
        m_inputBottom->setValue( val );
        m_inputRight->setValue( val );
        m_inputTop->setValue( val );
        m_noSignal = false;
    }
}

double KWFourSideConfigWidget::leftValue() const
{
    return m_inputLeft->value();
}

double KWFourSideConfigWidget::rightValue() const
{
    return m_inputRight->value();
}

double KWFourSideConfigWidget::topValue() const
{
    return m_inputTop->value();
}

double KWFourSideConfigWidget::bottomValue() const
{
    return m_inputBottom->value();
}

/*
    TODO;
    features that are present in the frames but not shown (yet)
        - frames with 'copy' should have a radiogroup showing that its only going to be copied to
          the even/odd side or to both sides.
*/
