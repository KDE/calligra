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
/* Module: Frame Dialog (header)                                  */
/******************************************************************/

#ifndef framedia_h
#define framedia_h

#include <qtabdialog.h>

class KWFrame;
class KWordDocument;
class KWPage;
class QWidget;
class QGridLayout;
class QGroupBox;
class QRadioButton;
class QCheckBox;
class QLabel;
class KRestrictedLine;
class QCloseEvent;

static const int FD_FRAME_SET      = 1;
static const int FD_FRAME          = 2;
static const int FD_FRAME_CONNECT  = 4;
static const int FD_PLUS_NEW_FRAME = 8;
static const int FD_GEOMETRY       = 16;
static const int FD_BORDERS        = 32;

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

class KWFrameDia : public QTabDialog
{
    Q_OBJECT

public:
    KWFrameDia( QWidget *parent, const char *name, KWFrame *_frame, KWordDocument *_doc, KWPage *_page, int _flags );

protected:
    void setupTab1TextFrameSet();
    void setupTab2TextFrame();
    void setupTab3ConnectTextFrames();
    void setupTab4Geometry();
    void uncheckAllRuns();

    QWidget *tab1, *tab2, *tab3, *tab4;
    QGridLayout *grid1, *grid2, *grid3, *grid4, *runGrid, *pGrid, *mGrid;
    QGroupBox *runGroup, *grp1, *grp2;
    QRadioButton *rRunNo, *rRunBounding, *rRunContur, *rResizeFrame, *rAppendFrame;
    QCheckBox *cAutoCreateFrame;
    QLabel *lRunNo, *lRunBounding, *lRunContur, *lRGap, *lFrameSet, *lNewFrame;
    QLabel *lx, *ly, *lw, *lh, *lml, *lmr, *lmt, *lmb;
    QListBox *lFrameSList;
    KRestrictedLine *sx, *sy, *sw, *sh, *sml, *smr, *smt, *smb;

    KWFrame *frame;
    KRestrictedLine *eRGap;
    int flags;
    KWordDocument *doc;
    KWPage *page;

    float oldX, oldY, oldW, oldH;

    void closeEvent( QCloseEvent *e )
    { emit frameDiaClosed(); e->accept(); }

signals:
    void frameDiaClosed();

protected slots:
    void runNoClicked();
    void runBoundingClicked();
    void runConturClicked();
    void applyChanges();
    void connectListSelected( int );

};

#endif


