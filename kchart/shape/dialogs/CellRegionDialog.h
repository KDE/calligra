#ifndef KCHART_CELL_REGION_DIALOG_H
#define KCHART_CELL_REGION_DIALOG_H

#include "ui_CellRegionDialog.h"

namespace KChart {

class CellRegionDialog : public QDialog, public Ui::CellRegionDialog
{
public:
	CellRegionDialog( QWidget *parent = 0 );
    ~CellRegionDialog();
};

} // Namespace KChart

#endif // KCHART_CELL_REGION_DIALOG_H

