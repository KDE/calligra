
#ifndef KFMVIEW_H
#define KFMVIEW_H


#include <kmainwindow.h>

/**
 *
 * Lucijan Busch
 **/
class KFMView : public KMainWindow
{
	Q_OBJECT

	public:
		KFMView();
		~KFMView();

	public slots:
		void	slotWidget();
};

#endif
