#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include <kexipart.h>

class KexiMainWin;

class KexiTablePart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiTablePart(QObject *parent, const char *name, const QStringList &);
		~KexiTablePart();

		virtual void	execute(KexiMainWindow *win, const QString &table);
		virtual void    createGUIClient(KexiMainWindow *win);
};

#endif

