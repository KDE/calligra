#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <kmdichildview.h>
#include <kxmlguiclient.h>

class KexiMainWindow;
class KActionCollection;

class KexiDialogBase : public KMdiChildView, public KXMLGUIClient
{
	Q_OBJECT

	public:
		KexiDialogBase(KexiMainWindow *parent, const QString &title);
		~KexiDialogBase();
};

#endif

