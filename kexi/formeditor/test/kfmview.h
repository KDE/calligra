
#ifndef KFMVIEW_H
#define KFMVIEW_H


#include <kmainwindow.h>

/**
 *
 * Lucijan Busch
 **/
 
class QWorkspace;

namespace KFormDesigner
{
	class Form;
	class ObjectPropertyBuffer;
	class WidgetLibrary;
}

class KFMView : public KMainWindow
{
	Q_OBJECT

	public:
		KFMView();
		~KFMView();

	public slots:
		void	debugTree();
		void	loadForm();

	private:
		KFormDesigner::Form *m_form;
		QWorkspace  *w;
		KFormDesigner::ObjectPropertyBuffer *buff;
		KFormDesigner::WidgetLibrary *l;
};

#endif
