#ifndef KEXIPROJECT_H
#define KEXIPROJECT_H

#include <qobject.h>
#include <qvaluelist.h>

class KexiProjectConnectionData;

namespace KexiDB
{
	class DriverManager;
	class Driver;
	class Connection;
}

namespace KexiPart
{
	class Manager;
	class Item;
	class Info;
}

typedef QValueList<KexiPart::Item> ItemList;

/**
 * this class represents a project it contains data about connections, current file state etc..
 */
class KexiProject : public QObject
{
	Q_OBJECT

	public:
		KexiProject();
		~KexiProject();

		/**
		 * opens a project/xml-connection
		 * @returns true on success
		 */
		bool		open(const QString &doc);

		/**
		 * @returns a error wich may have occured at actions like open/openConnection or QString::null if none
		 */
		const QString		error() { return m_error; }

		/**
		 * @returns the part manager
		 */
		KexiPart::Manager	*partManager() { return m_partManager; }

		/**
		 * @returns true if a we are connected to a database
		 */
		bool			isConnected();

		/**
		 * @returns all items of a type in this project
		 */
		ItemList		items(KexiPart::Info *i);

		/**
		 * @returns the database connection assosiated with this project
		 */
		KexiDB::Connection	*dbConnection() { return m_connection; }


	protected:
		bool			openConnection(KexiProjectConnectionData *connection);

	signals:
		/**
		 * this signal gets emmited after succesfully connected to a db
		 */
		void			dbAvailable();


	private:
		KexiDB::DriverManager		*m_drvManager;
		KexiDB::Connection		*m_connection;
		KexiProjectConnectionData	*m_connData;
		KexiPart::Manager		*m_partManager;
		QString				m_error;
};

#endif

