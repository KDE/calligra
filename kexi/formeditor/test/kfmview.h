
#ifndef KFMVIEW_H
#define KFMVIEW_H


#include <kmainwindow.h>

/**
 *
 * Lucijan Busch
 **/

class QWorkspace;
namespace KFormDesigner {
class FormManager;
}

class KFMView : public KMainWindow
{
	Q_OBJECT

	public:
		KFMView();
		~KFMView();

	public slots:
		void slotPreviewForm();

	private:
		QWorkspace  *w;
		KFormDesigner::FormManager *manager;
};

#endif
