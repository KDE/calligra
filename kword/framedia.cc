/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Frame Dialog                                           */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"
#include "frame.h"
#include "framedia.h"
#include "framedia.moc"
#include "defs.h"

#include <klocale.h>
#include <kapp.h>
#include <krestrictedline.h>
#include <kiconloader.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qevent.h>
#include <qlistbox.h>
#include <qbuttongroup.h>

#include <stdlib.h>
#include <limits.h>

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

/*================================================================*/
KWFrameDia::KWFrameDia(QWidget* parent,const char* name,KWFrame *_frame,KWordDocument *_doc,KWPage *_page,int _flags)
	: QTabDialog(parent,name,true)
{
	frame = _frame;
	if (frame)
    {
		KRect r = frame->normalize();
		frame->setRect(r.x(),r.y(),r.width(),r.height());
    }

	flags = _flags;
	doc = _doc;
	page = _page;

	if ((flags & FD_FRAME_CONNECT) && doc)
		setupTab3ConnectTextFrames();

	if ((flags & FD_FRAME_SET) && doc)
		setupTab1TextFrameSet();

	if ((flags & FD_FRAME) && doc)
		setupTab2TextFrame();

	if ((flags & FD_GEOMETRY) && doc)
		setupTab4Geometry();

	setCancelButton(i18n("Cancel"));
	setOkButton(i18n("OK"));

	connect(this,SIGNAL(applyButtonPressed()),this,SLOT(applyChanges()));

	resize(550,400);
}

/*================================================================*/
void KWFrameDia::setupTab1TextFrameSet()
{
	tab1 = new QWidget(this);

	grid1 = new QGridLayout(tab1,4,1,15,7);

	lNewFrame = new QLabel(i18n("If the text of the frameset doesn't fit into the the frames of the frameset anymore:"),tab1);
	lNewFrame->resize(lNewFrame->sizeHint());
	grid1->addWidget(lNewFrame,0,0);

	rAppendFrame = new QRadioButton(i18n("Create automatically a new frame"),tab1);
	rAppendFrame->resize(rAppendFrame->sizeHint());
	grid1->addWidget(rAppendFrame,1,0);

	rResizeFrame = new QRadioButton(i18n("Resize automatically last frame"),tab1);
	rResizeFrame->resize(rResizeFrame->sizeHint());
	grid1->addWidget(rResizeFrame,2,0);

	QButtonGroup *grp = new QButtonGroup(tab1);
	grp->hide();
	grp->setExclusive(true);
	grp->insert(rAppendFrame);
	grp->insert(rResizeFrame);

	grid1->addRowSpacing(0,lNewFrame->height());
	grid1->addRowSpacing(1,rAppendFrame->height());
	grid1->addRowSpacing(2,rResizeFrame->height());
	grid1->setRowStretch(0,0);
	grid1->setRowStretch(1,0);
	grid1->setRowStretch(2,0);
	grid1->setRowStretch(3,1);

	grid1->addColSpacing(0,lNewFrame->width());
	grid1->addColSpacing(0,rAppendFrame->width());
	grid1->addColSpacing(0,rResizeFrame->width());
	grid1->setColStretch(0,1);

	grid1->activate();

	addTab(tab1,i18n("Frameset"));

	rAppendFrame->setChecked(doc->getAutoCreateNewFrame());
	rResizeFrame->setChecked(!doc->getAutoCreateNewFrame());
}

/*================================================================*/
void KWFrameDia::setupTab2TextFrame()
{
	tab2 = new QWidget(this);

	grid2 = new QGridLayout(tab2,3,2,15,7);

	runGroup = new QGroupBox(i18n("Text Run Around"),tab2);
	grid2->addWidget(runGroup,0,0);

	runGrid = new QGridLayout(runGroup,5,2,15,7);

	QPixmap pixmap = Icon("run_not.xpm");
	lRunNo = new QLabel(runGroup);
	lRunNo->setBackgroundPixmap(pixmap);
	lRunNo->resize(pixmap.size());
	runGrid->addWidget(lRunNo,1,0);

	pixmap = Icon("run_bounding.xpm");
	lRunBounding = new QLabel(runGroup);
	lRunBounding->setBackgroundPixmap(pixmap);
	lRunBounding->resize(pixmap.size());
	runGrid->addWidget(lRunBounding,2,0);

	pixmap = Icon("run_contur.xpm");
	lRunContur = new QLabel(runGroup);
	lRunContur->setBackgroundPixmap(pixmap);
	lRunContur->resize(pixmap.size());
	runGrid->addWidget(lRunContur,3,0);

	rRunNo = new QRadioButton(i18n("&Don't run around other frames"),runGroup);
	rRunNo->resize(rRunNo->sizeHint());
	runGrid->addWidget(rRunNo,1,1);
	connect(rRunNo,SIGNAL(clicked()),this,SLOT(runNoClicked()));

	rRunBounding = new QRadioButton(i18n("Run around the &Bounding Rectangle of other frames"),runGroup);
	rRunBounding->resize(rRunBounding->sizeHint());
	runGrid->addWidget(rRunBounding,2,1);
	connect(rRunBounding,SIGNAL(clicked()),this,SLOT(runBoundingClicked()));

	rRunContur = new QRadioButton(i18n("Run around the &Contur of other frames"),runGroup);
	rRunContur->resize(rRunContur->sizeHint());
	runGrid->addWidget(rRunContur,3,1);
	connect(rRunContur,SIGNAL(clicked()),this,SLOT(runConturClicked()));

	runGrid->addColSpacing(0,lRunNo->width());
	runGrid->addColSpacing(1,rRunNo->width());
	runGrid->addColSpacing(1,rRunBounding->width());
	runGrid->addColSpacing(1,rRunContur->width());
	runGrid->setColStretch(1,1);

	runGrid->addRowSpacing(0,10);
	runGrid->addRowSpacing(1,lRunNo->height());
	runGrid->addRowSpacing(1,rRunNo->height());
	runGrid->addRowSpacing(2,lRunBounding->height());
	runGrid->addRowSpacing(2,rRunBounding->height());
	runGrid->addRowSpacing(3,lRunContur->height());
	runGrid->addRowSpacing(3,rRunContur->height());
	runGrid->setRowStretch(4,1);

	runGrid->activate();

	grid2->addMultiCellWidget(runGroup,0,0,0,1);

	lRGap = new QLabel(i18n(QString("Runaround Gap (" + doc->getUnit() + "):")),tab2);
	lRGap->resize(lRGap->sizeHint());
	lRGap->setAlignment(AlignRight | AlignVCenter);
	grid2->addWidget(lRGap,1,0);

	eRGap = new KRestrictedLine(tab2,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	eRGap->setText("0.00");
	eRGap->setMaxLength(5);
	eRGap->setEchoMode(QLineEdit::Normal);
	eRGap->setFrame(true);
	eRGap->resize(eRGap->sizeHint());
	grid2->addWidget(eRGap,1,1);

	grid2->addColSpacing(0,lRGap->width());
	grid2->addColSpacing(1,eRGap->width());
	grid2->setColStretch(1,1);

	grid2->addRowSpacing(0,runGroup->height());
	grid2->addRowSpacing(1,lRGap->height());
	grid2->addRowSpacing(1,eRGap->height());
	grid2->setRowStretch(2,1);

	grid2->activate();

	addTab(tab2,i18n("Text Frame"));

	uncheckAllRuns();
	switch (frame ? frame->getRunAround() : doc->getRunAround())
    {
    case RA_NO: rRunNo->setChecked(true);
		break;
    case RA_BOUNDINGRECT: rRunBounding->setChecked(true);
		break;
    case RA_CONTUR: rRunContur->setChecked(true);
		break;
    }

	QString str;
	switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM: str.sprintf("%g",frame ? frame->getRunAroundGap().mm() : doc->getRunAroundGap().mm());
		break;
    case U_INCH: str.sprintf("%g",frame ? frame->getRunAroundGap().inch() : doc->getRunAroundGap().inch());
		break;
    case U_PT: str.sprintf("%d",frame ? frame->getRunAroundGap().pt() : doc->getRunAroundGap().pt());
		break;
    }

	eRGap->setText(str);
}

/*================================================================*/
void KWFrameDia::setupTab3ConnectTextFrames()
{
	tab3 = new QWidget(this);

	grid3 = new QGridLayout(tab3,2,1,15,7);

	lFrameSet = new QLabel(i18n("Choose a frameset to which the current frame should be connected:"),tab3);
	lFrameSet->resize(lFrameSet->sizeHint());
	grid3->addWidget(lFrameSet,0,0);

	lFrameSList = new QListBox(tab3);

	for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
		if (i == 0 && doc->getProcessingType() == KWordDocument::WP) continue;
		QString str;
		str.sprintf("Frameset Nr. %d",i + 1);
		lFrameSList->insertItem(str,-1);
    }

	if (flags & FD_PLUS_NEW_FRAME)
		lFrameSList->insertItem(i18n("Create a new Frameset with this frame"),-1);

	connect(lFrameSList,SIGNAL(highlighted(int)),this,SLOT(connectListSelected(int)));
	grid3->addWidget(lFrameSList,1,0);

	grid3->addColSpacing(0,lFrameSet->width());
	grid3->setColStretch(0,1);

	grid3->addRowSpacing(0,lFrameSet->height());
	grid3->addRowSpacing(1,lFrameSet->height());
	grid3->setRowStretch(1,1);

	grid3->activate();

	addTab(tab3,i18n("Connect Text Frames"));

	lFrameSList->setSelected(0,true);
}

/*================================================================*/
void KWFrameDia::setupTab4Geometry()
{
	tab4 = new QWidget(this);
	grid4 = new QGridLayout(tab4,3,1,15,7);

	grp1 = new QGroupBox(i18n(QString("Position in " + doc->getUnit())),tab4);
	pGrid = new QGridLayout(grp1,5,2,7,7);

	lx = new QLabel(i18n("Left:"),grp1);
	lx->resize(lx->sizeHint());
	pGrid->addWidget(lx,1,0);

	sx = new KRestrictedLine(grp1,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	sx->setText("0.00");
	sx->setMaxLength(16);
	sx->setEchoMode(QLineEdit::Normal);
	sx->setFrame(true);
	sx->resize(sx->sizeHint());
	pGrid->addWidget(sx,2,0);

	ly = new QLabel(i18n("Top:"),grp1);
	ly->resize(ly->sizeHint());
	pGrid->addWidget(ly,1,1);

	sy = new KRestrictedLine(grp1,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	sy->setText("0.00");
	sy->setMaxLength(16);
	sy->setEchoMode(QLineEdit::Normal);
	sy->setFrame(true);
	sy->resize(sy->sizeHint());
	pGrid->addWidget(sy,2,1);

	lw = new QLabel(i18n("Width:"),grp1);
	lw->resize(lw->sizeHint());
	pGrid->addWidget(lw,3,0);

	sw = new KRestrictedLine(grp1,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	sw->setText("0.00");
	sw->setMaxLength(16);
	sw->setEchoMode(QLineEdit::Normal);
	sw->setFrame(true);
	sw->resize(sw->sizeHint());
	pGrid->addWidget(sw,4,0);

	lh = new QLabel(i18n("Height:"),grp1);
	lh->resize(lh->sizeHint());
	pGrid->addWidget(lh,3,1);

	sh = new KRestrictedLine(grp1,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	sh->setText("0.00");
	sh->setMaxLength(16);
	sh->setEchoMode(QLineEdit::Normal);
	sh->setFrame(true);
	sh->resize(sh->sizeHint());
	pGrid->addWidget(sh,4,1);


	pGrid->addRowSpacing(0,7);
	pGrid->addRowSpacing(1,lx->height());
	pGrid->addRowSpacing(1,ly->height());
	pGrid->addRowSpacing(2,sx->height());
	pGrid->addRowSpacing(2,sy->height());
	pGrid->addRowSpacing(3,lw->height());
	pGrid->addRowSpacing(3,lh->height());
	pGrid->addRowSpacing(4,sw->height());
	pGrid->addRowSpacing(4,sh->height());
	pGrid->setRowStretch(0,0);
	pGrid->setRowStretch(1,0);
	pGrid->setRowStretch(2,0);
	pGrid->setRowStretch(3,0);
	pGrid->setRowStretch(4,0);

	pGrid->addColSpacing(0,lx->width());
	pGrid->addColSpacing(0,sx->width());
	pGrid->addColSpacing(0,lw->width());
	pGrid->addColSpacing(0,sw->width());
	pGrid->addColSpacing(1,ly->width());
	pGrid->addColSpacing(1,sy->width());
	pGrid->addColSpacing(1,lh->width());
	pGrid->addColSpacing(1,sh->width());
	pGrid->setColStretch(0,1);
	pGrid->setColStretch(1,1);

	pGrid->activate();
	grid4->addWidget(grp1,0,0);

	grp2 = new QGroupBox(i18n(QString("Margins in " + doc->getUnit())),tab4);
	mGrid = new QGridLayout(grp2,5,2,7,7);

	lml = new QLabel(i18n("Left:"),grp2);
	lml->resize(lml->sizeHint());
	mGrid->addWidget(lml,1,0);

	sml = new KRestrictedLine(grp2,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	sml->setText("0.00");
	sml->setMaxLength(5);
	sml->setEchoMode(QLineEdit::Normal);
	sml->setFrame(true);
	sml->resize(sml->sizeHint());
	mGrid->addWidget(sml,2,0);

	lmr = new QLabel(i18n("Right:"),grp2);
	lmr->resize(lmr->sizeHint());
	mGrid->addWidget(lmr,1,1);

	smr = new KRestrictedLine(grp2,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	smr->setText("0.00");
	smr->setMaxLength(5);
	smr->setEchoMode(QLineEdit::Normal);
	smr->setFrame(true);
	smr->resize(smr->sizeHint());
	mGrid->addWidget(smr,2,1);

	lmt = new QLabel(i18n("Top:"),grp2);
	lmt->resize(lmt->sizeHint());
	mGrid->addWidget(lmt,3,0);

	smt = new KRestrictedLine(grp2,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	smt->setText("0.00");
	smt->setMaxLength(5);
	smt->setEchoMode(QLineEdit::Normal);
	smt->setFrame(true);
	smt->resize(smt->sizeHint());
	mGrid->addWidget(smt,4,0);

	lmb = new QLabel(i18n("Bottom:"),grp2);
	lmb->resize(lmb->sizeHint());
	mGrid->addWidget(lmb,3,1);

	smb = new KRestrictedLine(grp2,"",KWUnit::unitType(doc->getUnit()) == U_PT ? "1234567890" : "1234567890.");
	smb->setText("0.00");
	smb->setMaxLength(5);
	smb->setEchoMode(QLineEdit::Normal);
	smb->setFrame(true);
	smb->resize(smb->sizeHint());
	mGrid->addWidget(smb,4,1);

	mGrid->addRowSpacing(0,7);
	mGrid->addRowSpacing(1,lml->height());
	mGrid->addRowSpacing(1,lmr->height());
	mGrid->addRowSpacing(2,sml->height());
	mGrid->addRowSpacing(2,smr->height());
	mGrid->addRowSpacing(3,lmt->height());
	mGrid->addRowSpacing(3,lmb->height());
	mGrid->addRowSpacing(4,smt->height());
	mGrid->addRowSpacing(4,smb->height());
	mGrid->setRowStretch(0,0);
	mGrid->setRowStretch(1,0);
	mGrid->setRowStretch(2,0);
	mGrid->setRowStretch(3,0);
	mGrid->setRowStretch(4,0);

	mGrid->addColSpacing(0,lml->width());
	mGrid->addColSpacing(0,sml->width());
	mGrid->addColSpacing(0,lmt->width());
	mGrid->addColSpacing(0,smt->width());
	mGrid->addColSpacing(1,lmr->width());
	mGrid->addColSpacing(1,smr->width());
	mGrid->addColSpacing(1,lmb->width());
	mGrid->addColSpacing(1,smb->width());
	mGrid->setColStretch(0,1);
	mGrid->setColStretch(1,1);

	mGrid->activate();
	grid4->addWidget(grp2,1,0);

	grid4->addRowSpacing(0,grp1->height());
	grid4->addRowSpacing(1,grp2->height());
	grid4->setRowStretch(0,0);
	grid4->setRowStretch(1,0);
	grid4->setRowStretch(2,1);

	grid4->addColSpacing(0,grp1->width());
	grid4->addColSpacing(0,grp2->width());
	grid4->setColStretch(0,1);

	grid4->activate();

	addTab(tab4,i18n("Geometry"));

	KWUnit l,r,t,b;
	doc->getFrameMargins(l,r,t,b);
	switch (KWUnit::unitType(doc->getUnit()))
    {
    case U_MM:
	{
		sml->setText(QString().setNum(l.mm()));
		smr->setText(QString().setNum(r.mm()));
		smt->setText(QString().setNum(t.mm()));
		smb->setText(QString().setNum(b.mm()));
	} break;
    case U_INCH:
	{
		sml->setText(QString().setNum(l.inch()));
		smr->setText(QString().setNum(r.inch()));
		smt->setText(QString().setNum(t.inch()));
		smb->setText(QString().setNum(b.inch()));
	} break;
    case U_PT:
	{
		sml->setText(QString().setNum(l.pt()));
		smr->setText(QString().setNum(r.pt()));
		smt->setText(QString().setNum(t.pt()));
		smb->setText(QString().setNum(b.pt()));
	} break;
    }

	if (doc->isOnlyOneFrameSelected() && (doc->getProcessingType() == KWordDocument::DTP ||
										  (doc->getProcessingType() == KWordDocument::WP &&
										   doc->getFrameSetNum(doc->getFirstSelectedFrameSet()) > 0)))
    {
		unsigned int x,y,w,h,_num;

		KWFrameSet *frms = doc->getFrameCoords(x,y,w,h,_num);
		QString _x,_y,_w,_h;
		y -= frms->getPageOfFrame(_num) * doc->getPTPaperHeight();

		switch (KWUnit::unitType(doc->getUnit()))
		{
		case U_MM:
		{
			_x.sprintf("%g",POINT_TO_MM(x));
			_y.sprintf("%g",POINT_TO_MM(y));
			_w.sprintf("%g",POINT_TO_MM(w));
			_h.sprintf("%g",POINT_TO_MM(h));
		} break;
		case U_INCH:
		{
			_x.sprintf("%g",POINT_TO_INCH(x));
			_y.sprintf("%g",POINT_TO_INCH(y));
			_w.sprintf("%g",POINT_TO_INCH(w));
			_h.sprintf("%g",POINT_TO_INCH(h));
		} break;
		case U_PT:
		{
			_x.sprintf("%d",x);
			_y.sprintf("%d",y);
			_w.sprintf("%d",w);
			_h.sprintf("%d",h);
		} break;
		}

		oldX = atof(_x);
		oldY = atof(_y);
		oldW = atof(_w);
		oldH = atof(_h);

		sx->setText(_x);
		sy->setText(_y);
		sw->setText(_w);
		sh->setText(_h);
    }
	else
    {
		sx->setEnabled(false);
		sy->setEnabled(false);
		sw->setEnabled(false);
		sh->setEnabled(false);
    }
}

/*================================================================*/
void KWFrameDia::uncheckAllRuns()
{
	rRunNo->setChecked(false);
	rRunBounding->setChecked(false);
	rRunContur->setChecked(false);
}

/*================================================================*/
void KWFrameDia::runNoClicked()
{
	uncheckAllRuns();
	rRunNo->setChecked(true);
}

/*================================================================*/
void KWFrameDia::runBoundingClicked()
{
	uncheckAllRuns();
	rRunBounding->setChecked(true);
}

/*================================================================*/
void KWFrameDia::runConturClicked()
{
	uncheckAllRuns();
	rRunContur->setChecked(true);
}

/*================================================================*/
void KWFrameDia::applyChanges()
{
	if ((flags & FD_FRAME_SET) && doc)
		doc->setAutoCreateNewFrame(rAppendFrame->isChecked());

	if ((flags & FD_FRAME) && doc)
    {
		if (frame)
		{
			if (rRunNo->isChecked())
				frame->setRunAround(RA_NO);
			else if (rRunBounding->isChecked())
				frame->setRunAround(RA_BOUNDINGRECT);
			else if (rRunContur->isChecked())
				frame->setRunAround(RA_CONTUR);

			KWUnit u;
			switch (KWUnit::unitType(doc->getUnit()))
			{
			case U_MM: u.setMM(atof(eRGap->text()));
				break;
			case U_INCH: u.setINCH(atof(eRGap->text()));
				break;
			case U_PT: u.setPT(atoi(eRGap->text()));
				break;
			}
			frame->setRunAroundGap(u);
		}
		else
		{
			if (rRunNo->isChecked())
				doc->setRunAround(RA_NO);
			else if (rRunBounding->isChecked())
				doc->setRunAround(RA_BOUNDINGRECT);
			else if (rRunContur->isChecked())
				doc->setRunAround(RA_CONTUR);

			KWUnit u;
			switch (KWUnit::unitType(doc->getUnit()))
			{
			case U_MM: u.setMM(atof(eRGap->text()));
				break;
			case U_INCH: u.setINCH(atof(eRGap->text()));
				break;
			case U_PT: u.setPT(atoi(eRGap->text()));
				break;
			}
			doc->setRunAroundGap(u);
		}
    }

	if (flags & FD_FRAME_CONNECT && doc)
    {
		unsigned int _num = static_cast<unsigned int>(lFrameSList->currentItem());
		if (doc->getProcessingType() == KWordDocument::WP) _num++;

		if (static_cast<unsigned int>(_num) < doc->getNumFrameSets())
			doc->getFrameSet(_num)->addFrame(frame);
		else
		{
			KWTextFrameSet *_frameSet = new KWTextFrameSet(doc);
			_frameSet->addFrame(frame);
			_frameSet->setAutoCreateNewFrame(false);
			doc->addFrameSet(_frameSet);
			page->repaint(false);
			_frameSet->setAutoCreateNewFrame(true);
			return;
		}
    }

	if (flags & FD_GEOMETRY && doc)
    {
		if (doc->isOnlyOneFrameSelected() && (doc->getProcessingType() == KWordDocument::DTP ||
											  (doc->getProcessingType() == KWordDocument::WP &&
											   doc->getFrameSetNum(doc->getFirstSelectedFrameSet()) > 0)))
		{
			if (oldX != atof(sx->text()) || oldY != atof(sy->text()) || oldW != atof(sw->text()) || oldH != atof(sh->text()))
			{
				unsigned int px,py,pw,ph;
				switch (KWUnit::unitType(doc->getUnit()))
				{
				case U_MM:
				{
					px = MM_TO_POINT(atof(sx->text()));
					py = MM_TO_POINT(atof(sy->text()));
					pw = MM_TO_POINT(atof(sw->text()));
					ph = MM_TO_POINT(atof(sh->text()));
				} break;
				case U_INCH:
				{
					px = INCH_TO_POINT(atof(sx->text()));
					py = INCH_TO_POINT(atof(sy->text()));
					pw = INCH_TO_POINT(atof(sw->text()));
					ph = INCH_TO_POINT(atof(sh->text()));
				} break;
				case U_PT:
				{
					px = atoi(sx->text());
					py = atoi(sy->text());
					pw = atoi(sw->text());
					ph = atoi(sh->text());
				} break;
				}
				doc->setFrameCoords(px,py,pw,ph);
			}
		}

		KWUnit u1,u2,u3,u4;
		switch (KWUnit::unitType(doc->getUnit()))
		{
		case U_MM:
		{
			u1.setMM(atof(sml->text()));
			u2.setMM(atof(smr->text()));
			u3.setMM(atof(smt->text()));
			u4.setMM(atof(smb->text()));
		} break;
		case U_INCH:
		{
			u1.setINCH(atof(sml->text()));
			u2.setINCH(atof(smr->text()));
			u3.setINCH(atof(smt->text()));
			u4.setINCH(atof(smb->text()));
		} break;
		case U_PT:
		{
			u1.setPT(atoi(sml->text()));
			u2.setPT(atoi(smr->text()));
			u3.setPT(atoi(smt->text()));
			u4.setPT(atoi(smb->text()));
		} break;
		}
		doc->setFrameMargins(u1,u2,u3,u4);
    }

	page->repaint(false);
}

/*================================================================*/
void KWFrameDia::connectListSelected(int _num)
{
	if (doc->getProcessingType() == KWordDocument::WP) _num++;

	if (static_cast<unsigned int>(_num) < doc->getNumFrameSets())
    {
		if (doc->getFrameSet(_num)->getFrameType() != FT_TEXT || doc->getFrameSet(_num)->getFrameInfo() != FI_BODY)
		{
			lFrameSList->setCurrentItem(lFrameSList->count() - 1);
			page->setHilitFrameSet(-1);
		}
		else
			page->setHilitFrameSet(_num);
    }
	else
		page->setHilitFrameSet(-1);
}
