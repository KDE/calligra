
#ifndef KFMVIEW_H
#define KFMVIEW_H


#include <kmainwindow.h>

/**
 *
 * Lucijan Busch
 **/

namespace KFormDesigner
{
	class Form;
}

class KFMView : public KMainWindow
{
	Q_OBJECT

	public:
		KFMView();
		~KFMView();

	public slots:
		void	debugTree();

	public slots:
		void	slotWidget();
		KFormDesigner::Form *m_form;
};

#endif
