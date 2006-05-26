#include "keximacroerror.h"

#include <qtimer.h>

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroError::Private
{
	public:
		KoMacro::Context::Ptr context;

		Private(KoMacro::Context* const c)
			: context(c)
		{
		}
};
KexiMacroError::KexiMacroError(QWidget* parent, const char* name, WFlags fl,KoMacro::Context::Ptr context)
	: KexiMacroErrorBase(parent,name,fl)
	, d ( new Private(context))
{
	//errortext, errorlist, continuebtn,cancelbtn, designerbtn
	
	KoMacro::Exception* exception = context->exception();

	/*KMessageBox::detailedError(
			mainWin(), //parent
			i18n("<qt>Failed to execute the Macro \"%1\".<br>%2</qt>").arg( d->macro->name() ).arg( exception->errorMessage() ), //text
			exception->traceMessages(), //details
			i18n("Execution failed") //caption
	);*/

	iconlbl->setPixmap(KGlobal::instance()->iconLoader()->loadIcon( "messagebox_critical", KIcon::Small, 48));
	errorlbl->setText(i18n("<qt>Failed to execute the Macro \"%1\".<br>%2</qt>").arg( context->macro()->name() ).arg( exception->errorMessage() ));

	int i = 1;
	KoMacro::MacroItem::List items = context->macro()->items();
	for (KoMacro::MacroItem::List::ConstIterator mit = items.begin(); mit != items.end(); mit++)
	{
		QListViewItem* qlistviewitem = new QListViewItem (errorlist);
		qlistviewitem->setText(0,QString("%1").arg(i++));
		qlistviewitem->setText(1,"Action");
		KoMacro::MacroItem::Ptr macroitem = *mit;

		if (macroitem != 0)
		{
			if (macroitem->action() != 0)
			{
			qlistviewitem->setText(2,macroitem->action()->name());
			}
			else {
				qlistviewitem->setText(2,i18n("No such action."));
			}
	
		}

		if(macroitem == context->macroItem())
		{
			qlistviewitem->setOpen(true);
			qlistviewitem->setSelected(true);
		}
		
		KoMacro::Variable::Map variables = macroitem->variables();
		KoMacro::Variable::Map::ConstIterator vit;
		//for ( it = variables.constBegin(); it != variables.constEnd(); ++it ) {
		for ( vit = variables.begin(); vit != variables.end(); ++vit ) {
			QListViewItem* child = new QListViewItem (qlistviewitem);
			child->setText(1,vit.key());		
			child->setText(2,vit.data()->toString());
		}

	}
	
	connect(designerbtn, SIGNAL(clicked()), this, SLOT(designbtnClicked()));
	connect(continuebtn, SIGNAL(clicked()), this, SLOT(continuebtnClicked()));
}

KexiMacroError::~KexiMacroError()
{
	delete d;
}

void KexiMacroError::designbtnClicked()
{
	
}

void KexiMacroError::continuebtnClicked()
{	
	QTimer::singleShot(200, d->context, SLOT(activateNext()));
	close();
}
