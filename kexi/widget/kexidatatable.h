#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include <kexidialogbase.h>

class KexiMainWindow;
class KexiDataTableView;

namespace KexiDB
{
	class Cursor;
}

class KEXIEXTWIDGETS_EXPORT KexiDataTable : public KexiDialogBase
{
	Q_OBJECT

	public:
		/**
		 * crates a empty table dialog
		 */
		KexiDataTable(KexiMainWindow *win, const QString &caption);
		KexiDataTable(KexiMainWindow *win, const QString &caption, KexiDB::Cursor *cursor);
		~KexiDataTable();

		void			setData(KexiDB::Cursor *cursor);
	protected:
		void			initActions();

	protected slots:
		void filter();


	private:
		KexiDataTableView	*m_view;
};

#endif

