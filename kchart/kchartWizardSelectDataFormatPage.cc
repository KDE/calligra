
#include "kchartWizardSelectDataFormatPage.h"
#include "kchart_view.h"
#include "kchart_part.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qpainter.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>

#include "kchart_params.h"

namespace KChart
{

KChartWizardSelectDataFormatPage::KChartWizardSelectDataFormatPage( QWidget* parent,
                                                                        KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
    QGridLayout *grid1 = new QGridLayout(this, 2, 1, KDialog::marginHint(),
                     KDialog::spacingHint());

    QButtonGroup* gb = new QVButtonGroup( i18n( "Data Format" ), this );
    QGridLayout *grid2 = new QGridLayout(gb, 2, 1, KDialog::marginHint(),
                     KDialog::spacingHint());

    m_rowMajor = new QRadioButton( i18n( "Data in rows" ), gb );
    m_rowMajor->resize( m_rowMajor->sizeHint() );
    grid2->addWidget( m_rowMajor, 0, 0);

    m_colMajor = new QRadioButton( i18n( "Data in columns" ), gb );
    m_colMajor->resize( m_colMajor->sizeHint() );
    grid2->addWidget( m_colMajor, 1, 0);
    grid2->setColStretch(1, 0);

    grid2->activate();
    grid1->addWidget(gb, 0, 0);
    grid1->setColStretch(1, 0);

    grid1->activate();

    if( _chart->auxdata()->m_dataDirection == KChartAuxiliary::DataColumns)
      m_colMajor->setChecked(true);
    else
      m_rowMajor->setChecked(true);
}



void KChartWizardSelectDataFormatPage::apply()
{
    if (m_rowMajor->isChecked())
      _chart->auxdata()->m_dataDirection = KChartAuxiliary::DataRows;
    else
      _chart->auxdata()->m_dataDirection = KChartAuxiliary::DataColumns;
}


}  //KChart namespace

#include "kchartWizardSelectDataFormatPage.moc"
