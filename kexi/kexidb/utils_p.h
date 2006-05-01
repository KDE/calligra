
#ifndef KEXIDB_UTILS_P_H
#define KEXIDB_UTILS_P_H

#include <qtimer.h>
#include <qwaitcondition.h>

#include <kprogressdialog.h>

#include "msghandler.h"

class ConnectionTestThread;

class ConnectionTestDialog : protected KProgressDialog
{
		Q_OBJECT
	public:
		ConnectionTestDialog(QWidget* parent, 
			const KexiDB::ConnectionData& data, KexiDB::MessageHandler& msgHandler);
		virtual ~ConnectionTestDialog();

		int exec();

		void error(KexiDB::Object *obj);

	protected slots:
		void slotTimeout();
		virtual void reject();

	protected:
		ConnectionTestThread* m_thread;
		KexiDB::ConnectionData m_connData;
		QTimer m_timer;
		KexiDB::MessageHandler* m_msgHandler;
		uint m_elapsedTime;
		KexiDB::Object *m_errorObj;
		QWaitCondition m_wait;
		bool m_stopWaiting : 1;
};

#endif
