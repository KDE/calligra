#include "kspread_dlg_paper.h"

KSpreadPaperLayout::KSpreadPaperLayout() : QTabDialog( 0L, 0L )
{
    setGeometry( x(), y(), 400, 400 );
    setCancelButton();
    setOKButton();
    setApplyButton();

    borderPage = new KSpreadPaperBorder( this );
    headingsPage = new KSpreadPaperHeadings( this );

    addTab( borderPage, "Sizes" );
    addTab( headingsPage, "Headings" );

    show();
}

KSpreadPaperLayout::~KSpreadPaperLayout()
{
}

#include "kspread_dlg_paper.moc"
