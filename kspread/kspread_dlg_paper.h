#ifndef KSpreadPaperKSpreadLayout_included
#define KSpreadPaperKSpreadLayout_included

#include <stdlib.h>
#include <stdio.h>

#include <qtabdlg.h>

#include "kspread_dlg_paper_border.h"
#include "kspread_dlg_paper_headings.h"

/**
 */
class KSpreadPaperLayout : public QTabDialog
{
    Q_OBJECT

public:
    KSpreadPaperLayout();
    virtual ~KSpreadPaperLayout();

    void setA4() { borderPage->a4->setChecked( TRUE ); }
    void setA5() { borderPage->a5->setChecked( TRUE ); }
    void setA3() { borderPage->a3->setChecked( TRUE ); }    
    void setLetter() { borderPage->letter->setChecked( TRUE ); }
    void setLegal() { borderPage->legal->setChecked( TRUE ); }
    void setExecutive() { borderPage->executive->setChecked( TRUE ); }

    void setLeftBorder( float _b )
    { char buf[128]; sprintf( buf, "%.1f", _b / 10.0 ); borderPage->leftBorder->setText( buf ); }
    void setRightBorder( float _b )
    { char buf[128]; sprintf( buf, "%.1f", _b / 10.0 ); borderPage->rightBorder->setText( buf ); }
    void setTopBorder( float _b )
    { char buf[128]; sprintf( buf, "%.1f", _b / 10.0 ); borderPage->topBorder->setText( buf ); }    
    void setBottomBorder( float _b )
    { char buf[128]; sprintf( buf, "%.1f", _b / 10.0 ); borderPage->bottomBorder->setText( buf ); }	

    void setHeadLeft( const char *_s ) { headingsPage->m_headLeft->setText( _s ); }
    void setHeadMid( const char *_s ) { headingsPage->m_headMid->setText( _s ); }
    void setHeadRight( const char *_s ) { headingsPage->m_headRight->setText( _s ); }
    void setFootLeft( const char *_s ) { headingsPage->footLeft->setText( _s ); }
    void setFootMid( const char *_s ) { headingsPage->footMid->setText( _s ); }
    void setFootRight( const char *_s ) { headingsPage->footRight->setText( _s ); }

    const char* getHeadLeft() { return headingsPage->m_headLeft->text(); }
    const char* getHeadMid() { return headingsPage->m_headMid->text(); }
    const char* getHeadRight() { return headingsPage->m_headRight->text(); }
    const char* getFootLeft() { return headingsPage->footLeft->text(); }
    const char* getFootMid() { return headingsPage->footMid->text(); }
    const char* getFootRight() { return headingsPage->footRight->text(); }
    
    bool isA4() { return borderPage->a4->isChecked(); }
    bool isA5() { return borderPage->a5->isChecked(); }
    bool isA3() { return borderPage->a3->isChecked(); }    
    bool isLetter() { return borderPage->letter->isChecked(); }
    bool isExecutive() { return borderPage->executive->isChecked(); }

    float getLeftBorder() { return atoi( borderPage->leftBorder->text() ) * 10.0 ; }
    float getRightBorder() { return atoi( borderPage->rightBorder->text() ) * 10.0 ; }
    float getTopBorder() { return atoi( borderPage->topBorder->text() ) * 10.0 ; }
    float getBottomBorder() { return atoi( borderPage->bottomBorder->text() ) * 10.0 ; }

protected:
    KSpreadPaperBorder *borderPage;
    KSpreadPaperHeadings *headingsPage;
};

#endif // KSpreadPaperKSpreadLayout_included
