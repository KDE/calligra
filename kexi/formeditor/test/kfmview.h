
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

using KFormDesigner::Form;

class KFMView : public KMainWindow
{
	Q_OBJECT

	public:
		KFMView();
		~KFMView();

		void loadUIFile(const QString &filename);

	public slots:
		void slotPreviewForm();
		void slotCreateFormSlot(Form *form, const QString &widget, const QString &signal);

	protected slots:
		void slotWidgetSelected(Form *form, bool multiple);
		void slotFormWidgetSelected(Form *form);
		void slotNoFormSelected();

	protected:
		void disableWidgetActions();
		void enableFormActions();

	private:
		QWorkspace  *w;
		KFormDesigner::FormManager *manager;
};

#endif
