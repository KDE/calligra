#ifndef KEXIUSERACTION_H
#define KEXIUSERACTION_H

#include <kaction.h>

#include "kexiuseractionmethod.h"

namespace KexiDB
{
	class Cursor;
}
class KActionCollection;

/*! action that can be defined by a user for a special scope e.g. main, form ...
    the actions can have some predefined \ref Methods which are described in \ref KexiUserActionMethod
    e.g. OpenObject, ExecuteScript ... those methods take different arguments also described in \ref KexiUserActionMethod
*/ 
class KEXICORE_EXPORT KexiUserAction : public KAction
{
	Q_OBJECT

	public:
		typedef QVector<QVariant> Arguments;

		/*! bytecode of available methods */
		enum Methods
		{
			MethodNone = 0,
			OpenObject = 1,
			CloseObject = 2,
			DeleteObject = 3,
			ExecuteScript = 4,
			ExitKexi = 5,

			LastMethod = 6 //use the last integer here... so we can stop iteration
		};

		/*! argument types */
		enum ArgTypes
		{
			String = 0,
			Integer = 1,
			Bool = 2,
			KexiPart = 3,
			KexiItem = 4
		};

		/*! constructs an action
		    \note methods are associated using setMethod() 
		    */
		KexiUserAction(KActionCollection *parent, const QString &name, const QString &text,
			const QString &pixmap);
		~KexiUserAction();

		/*! sets execution information associated with this action this will mostly look like
		    \code
		    KexiUserAction *action = new KexiUserAction(...);
		    Arguments arg;
		    arg.append(QVariant("kexi/form"));
		    arg.append(QVariant("main"));
		    action->setMethod(KexiUserAction::OpenAction, arg);
		    \endcode
		    */
		void setMethod(int method, Arguments args);

		/*! creates a KexiUserAction from current record in \a c
		    mostly needed for creation from kexi__useractions table */
		static KexiUserAction *fromCurrentRecord(
			KActionCollection *parent, KexiDB::Cursor *c);

	protected slots:
		/*! actually executes the associated method
		    \note KexiUserAction automatically connects KAction::activated() to KexiUserAction::execute()
		    */
		void execute();

	private:
		int m_method;
		Arguments m_args;
};

#endif

