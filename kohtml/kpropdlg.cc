
// kpropdlg.cpp (C) 1998 by F. Zigterman

/***************************************************************

     Requires the Qt and KDE widget libraries, available at no cost at
     http://www.troll.no and http://www.kde.org respectively

     Copyright (C) 1997, 1998 Fester Zigterman ( fzr@dds.nl )

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


***************************************************************/


#include "kpropdlg.h"
#include "kpropdlg.moc"
#include "kapp.h"
#include "kpanner.h"

#define QT_14

KPropDlg::KPropDlg( int dlgtype, int buttons, const char *title, QWidget *parent, const char *name, bool modal )
	: QDialog( parent, name, modal )
{
	// Set window props
	setCaption( title );

	// Set some characteristics
	PageList = new QList<QWidget>;
	DlgType = dlgtype;
	Buttons = buttons;
	TreeWidth = 150;
	ActivePage = 0;

	QFontMetrics fm( font() );
	int bbsize = fm.height() + fm.ascent() + 32;

	if( DlgType == TREE )
	{
		KPanner *panner = new KPanner( this );
		panner->setSeparator( 30 );
		TreeList = new KTreeList( panner->child0() );
		(new QHBoxLayout( panner->child0() ) )->addWidget( TreeList );
		connect(TreeList, SIGNAL(highlighted(int)),this,SLOT(showPage(int)));

		rpane = panner->child1() ;
		VLayout = new QVBoxLayout( rpane );

		// Page title label
		Title = new QLabel( "Unnamed Dialog", rpane );
		Title->setFrameStyle( QFrame::Panel|QFrame::Raised );
		Title->setFixedHeight( 20 );
		Title->setText( title );
		VLayout->addWidget( Title );

		PageFrame = new QFrame( rpane );
		PageFrame->setFrameStyle( QFrame::Panel|QFrame::Raised);
		PageFrame->installEventFilter( this );
		VLayout->addWidget( PageFrame );

		ButtonWidget = new QLabel( this );
		ButtonLayout = new QVBoxLayout( this );
		ButtonLayout->addWidget( panner, 0, AlignRight );
		ButtonLayout->addWidget( ButtonWidget, 0, AlignRight );


	}
	else if( DlgType == TABS )
	{
		TabControl = new KTabCtl( this );
		ButtonWidget = new QLabel( this );
		ButtonLayout = new QVBoxLayout( this );
		ButtonLayout->addWidget( TabControl, 0, AlignRight );
		ButtonLayout->addWidget( ButtonWidget, 0, AlignRight );
	}
	else
	{
		VLayout = new QVBoxLayout( this );

		// Page title label
		Title = new QLabel( "Unnamed Dialog", this );
		Title->setFrameStyle( QFrame::Panel|QFrame::Raised );
		Title->setFixedHeight( 20 );
		Title->setText( title );
		VLayout->addWidget( Title );

		PageFrame = new QFrame( this );
		PageFrame->setFrameStyle( QFrame::Panel|QFrame::Raised);
		PageFrame->installEventFilter( this );
		VLayout->addWidget( PageFrame );

		ButtonWidget = new QLabel( this );
//		ButtonLayout = new QVBoxLayout( this );
//		ButtonLayout->addWidget( PageFrame, 0, AlignRight );
//		ButtonLayout->addWidget( ButtonWidget, 0, AlignRight );
		VLayout->addWidget( ButtonWidget, 0, AlignRight );
	}
	
	// fill the vlayout with widgets

	PrevButton = new QPushButton( i18n("Prev"), ButtonWidget );
	NextButton = new QPushButton( i18n("Next"), ButtonWidget  );
	ApplyButton = new QPushButton( i18n("Apply"), ButtonWidget );
	OKButton = new QPushButton( i18n("OK"), ButtonWidget );
	CancelButton = new QPushButton( i18n("Cancel"), ButtonWidget );
	
	resizeButtons();
	connectButtons();

	OKButton->setDefault(TRUE);
	DefaultButton = OKButton;

}

void KPropDlg::connectButtons( int buttons )
{
	// connect the button signals to their handlers
	
	connect (PrevButton, SIGNAL(clicked()), SLOT(receivePrev()));
	connect (NextButton, SIGNAL(clicked()), SLOT(receiveNext()));
	connect (ApplyButton, SIGNAL(clicked()), SLOT(receiveApply()));
	connect( OKButton, SIGNAL(clicked()), SLOT(receiveOK()));
	connect (CancelButton, SIGNAL(clicked()), SLOT(receiveCancel()));

	// connect the handler signals to their standard methods
	
	if( buttons&OK )	connect(this, SIGNAL(OKClicked()), SLOT(acceptConfig()));
	if( buttons&CANCEL )	connect(this, SIGNAL(CancelClicked()), SLOT(cancelConfig()));
	if( buttons&APPLY )	connect(this, SIGNAL(ApplyClicked()), SLOT(applyConfig()));

	if( buttons&PREV)	connect( this, SIGNAL(PrevClicked() ), SLOT( slotPrev() ) );
	if( buttons&NEXT )	connect( this, SIGNAL(NextClicked() ), SLOT( slotNext() ) );
	
}

KPropDlg::~KPropDlg()
{
}

QFrame *KPropDlg::addPage( int parentindex, const char *title, int position )
{
	QVBoxLayout *templayout;
	QFrame *page;


	if( DlgType == TABS )
		page = new QFrame( TabControl, title );
	else
		page = new QFrame( PageFrame, title );


	page->setFrameStyle( QFrame::NoFrame);
	page->setEnabled( FALSE );
	page->hide();
	
	//	printf("addPage\n");
	if( DlgType == TABS )
	{
		TabControl->addTab( page, title );
	}
	else
	{
		if( DlgType == TREE )
		{
			TreeList->insertItem( title, 0, parentindex );
		}
		templayout = new QVBoxLayout( PageFrame,2 );
		templayout->addWidget( page );
		templayout->activate();
		
		page->lower();
	}
	PageList->append( page );
	return page;
}

void KPropDlg::showPage(QWidget *w)
{
	int stat=0;
	

	// bring the new page up front
	w->raise();
	
	// deactivate old page
	if( ActivePage )
	{
		ActivePage->setEnabled( FALSE );
		ActivePage->hide();
	}
	// and select new one
	ActivePage=w;
	ActivePage->setEnabled( TRUE );
	ActivePage->show();
	
	// change the header for the dialog
	// highlight the item in the tree view, most important for
	// first instantiation

	int index = PageList->findRef(w);
	if( DlgType == TREE )
	{
		Title->setText(w->name());
		TreeList->setCurrentItem(index);
	}
	
	stat = pageListStatus();
	//	printf("stat %X, %d, %d\n",stat, (stat&LASTPAGE)==0, !(stat&FIRSTPAGE)==0 );

	if( Buttons&NEXT ) NextButton->setEnabled( (stat&LASTPAGE)==0  );
	if( Buttons&PREV ) PrevButton->setEnabled( (stat&FIRSTPAGE)==0 );


}

bool KPropDlg::showPage(int index)
{
	// find the page in our list
	QWidget *page = PageList->at( index );

	// printf("display page %d, %X\n", index, page);
	
	if( page == 0 )
	{	warning("error in page num %d", index);
		return FALSE;
	}

	// request that it be brought up front
	showPage(page);

	return TRUE;
}

bool KPropDlg::removePage( char *page, int index )
{
	warning("Removing pages is not yet implemented.");
	return FALSE;
}

void KPropDlg::setButtonsEnabled( int types )
{
	ActiveButtons = types;
	PrevButton->setEnabled(  types&PREV  != 0 );
	NextButton->setEnabled(  types&NEXT  != 0 );
	ApplyButton->setEnabled( types&APPLY != 0 );
	OKButton->setEnabled(    types&OK    != 0 );
	CancelButton->setEnabled(types&CANCEL!= 0 );
}

int  KPropDlg::buttonsEnabled()
{
	return ActiveButtons;
}

void KPropDlg::setButton( int type, const char *text )
{
}

void KPropDlg::setDefaultButton( int type )
{
	DefaultButton->setDefault( FALSE );
	switch( type )
	{
	case OK:
		DefaultButton = OKButton;
		break;
	case CANCEL:
		DefaultButton = CancelButton;
		break;
	case PREV:
		DefaultButton = PrevButton;
		break;
	case NEXT:
		DefaultButton = NextButton;
		break;
	case APPLY:
		DefaultButton = ApplyButton;
		break;
	default:	
		;
	}
	DefaultButton->setDefault( TRUE );
	resizeButtons();
}

void KPropDlg::resizeButtons()
{
	QFontMetrics fm( font() );
	int bheight = fm.height() + fm.ascent();
	int bwid = 0;
	int bcount = 0;
	
	bwid = QMAX( PrevButton->sizeHint().width(), bwid );
	bwid = QMAX( NextButton->sizeHint().width(), bwid );
	bwid = QMAX( ApplyButton->sizeHint().width(), bwid );
	bwid = QMAX( OKButton->sizeHint().width(), bwid );
	bwid = QMAX( CancelButton->sizeHint().width(), bwid );

	if( Buttons&PREV )
	{
		PrevButton->move( (bcount++)*(bwid+4)+8, 8 );
	}
	else PrevButton->hide();
	
	if( Buttons&NEXT )
	{
		NextButton->move( (bcount++)*(bwid+4)+8, 8 );
	}
	else NextButton->hide();

	if( Buttons&APPLY )
	{
		ApplyButton->move( (bcount++)*(bwid+4)+12, 8 );
	}
	else ApplyButton->hide();

	if( Buttons&OK )
	{
		OKButton->move( (bcount++)*(bwid+4)+12, 8 );
	}
	else OKButton->hide();

	if( Buttons&CANCEL )
	{
		CancelButton->move( (bcount++)*(bwid+4)+16, 8 );
	}
	else CancelButton->hide();
	
	ButtonWidget->setFixedSize( bcount*(bwid+4) + 20, bheight + 16 );

	OKButton->setFixedSize( bwid, bheight );
	CancelButton->setFixedSize( bwid, bheight );
	ApplyButton->setFixedSize( bwid, bheight );
	PrevButton->setFixedSize( bwid, bheight );
	NextButton->setFixedSize( bwid, bheight );
}	

void KPropDlg::receiveOK()
{
	emit OKClicked();
}

void KPropDlg::receiveCancel() {
	emit CancelClicked();
}

void KPropDlg::receiveNext() {
	emit NextClicked();
}

void KPropDlg::receivePrev() {
	emit PrevClicked();
}

void KPropDlg::receiveApply() {
	emit ApplyClicked();
}

void KPropDlg::slotNext()
{
	int stat = pageListStatus();
	// printf("statnext %X, %d, %d\n",stat, (stat&LASTPAGE)==0, !(stat&FIRSTPAGE)==0 );

	if( stat < NOCURRENT &&stat !=0 )
	{
		printf("NOCURRENT\n");
		return;
	}
	PageList->next();
	// printf("count:%d at:%d\n", PageList->count(), PageList->at() );
	showPage( PageList->at() );
}

void KPropDlg::slotPrev()
{
	int stat = pageListStatus();
//	printf("statprev %X, %d, %d\n",stat, (stat&LASTPAGE)==0, !(stat&FIRSTPAGE)==0 );

	if( stat < NOCURRENT && stat !=0 ) return;
	PageList->prev();

	showPage( PageList->at() );
}


int KPropDlg::pageListStatus()
{
	int status = 0;
	if( ! PageList ) return NOLIST;
	if( ! PageList->count() ) return NOPAGES;
	if( ! PageList->current() ) return NOCURRENT;
	if( PageList->current() == PageList->getLast() ) status = LASTPAGE;
	if( PageList->current() == PageList->getFirst() ) status = FIRSTPAGE;
	return status;
}

void KPropDlg::acceptConfig()
{
	// Write configuration and exit dialog cleanly.

	emit setConfig();
	kapp->getConfig()->sync();
	emit configChanged();
	accept();
}

void KPropDlg::cancelConfig()
{
	// Don't write configuration and cancel dialog.
	reject();
}

void KPropDlg::applyConfig()
{
	// Write configuration and return to dialog.
	//printf("applyConfig\n");
	emit setConfig();
	emit configChanged();
}

void KPropDlg::connectConfig( QObject * propconf )
{
	connect( this,
			SIGNAL( setConfig() ),
			propconf,
			SLOT( setConfig() )
			);
	connect( this,
			SIGNAL( getConfig() ),
			propconf,
			SLOT( getConfig() )
			);
}

bool KPropDlg::eventFilter( QObject *obj, QEvent *ev )
{
	if( ev->type() == QEvent::Resize )
	{
		adjustPageHeight();
		if( ( (QResizeEvent*)ev )->size().height() < minimumPageHeight )
			PageFrame->setMinimumHeight( minimumPageHeight );
	}
}

void KPropDlg::adjustPageHeight()
{
	// calculate the minimum size
	minimumPageHeight = 0;
	int index = PageList->at();
	for( PageList->first(); PageList->current(); PageList->next() )
	{
		minimumPageHeight = QMAX( minimumPageHeight, PageList->current()->minimumSize().height() );
		//	printf("minimumHeight:%d, %d\n",minimumPageHeight, PageList->current()->minimumSize().height()  );
	}
	PageFrame->setMinimumHeight( (minimumPageHeight += 10) );
	PageList->at(index);
}
