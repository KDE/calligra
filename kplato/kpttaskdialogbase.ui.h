/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/




void KPTTaskDialogBase::scheduling_clicked( int schedulingType)
{
    switch (schedulingType)
    {
    case 0:
    case 1:
	schedulerDate->setEnabled(false);
	schedulerTime->setEnabled(false);
	specifyTime->setEnabled(false);
	break;
    default:
	schedulerDate->setEnabled(true);
	schedulerTime->setEnabled(true);
	specifyTime->setEnabled(true);
	break;
    }
}
