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

		virtual QWidget* mainWidget();
		KexiDataTableView* tableView() const { return m_view; }

		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;

	protected:
		void init();
		void initActions();

	protected slots:
		void filter();


	private:
		KexiDataTableView	*m_view;
};

#endif

