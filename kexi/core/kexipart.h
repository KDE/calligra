#ifndef KEXIPART_H
#define KEXIPART_H

#include <qobject.h>

class KexiMainWindow;

namespace KexiPart
{

	class Info;
/**
 * the main class for kexi frontend parts like tables, queries, relations
 */
class Part : public QObject
{
	Q_OBJECT

	public:
		
		Part(QObject *parent, const char *name, const QStringList &);
		~Part();


		virtual void	execute(KexiMainWindow *win, const QString &oname)=0;

	protected:
		friend class Manager;


		void		setInfo(Info *info) { m_info = info; }
		Info		*info() { return m_info; }

	private:
		Info	*m_info;
};

}

#endif

