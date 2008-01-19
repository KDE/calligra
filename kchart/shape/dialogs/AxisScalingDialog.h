#ifndef KCHART_AXIS_SCALING_DIALOG_H
#define KCHART_AXIS_SCALING_DIALOG_H

#include "ui_AxisScalingDialog.h"

namespace KChart {

class AxisScalingDialog : public QDialog, public Ui::AxisScalingDialog
{
public:
    AxisScalingDialog( QWidget *parent = 0 );
    ~AxisScalingDialog();
};

} // Namespace KChart

#endif // KCHART_AXIS_SCALING_DIALOG_H

