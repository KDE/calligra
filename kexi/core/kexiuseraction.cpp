#include <kmessagebox.h>
#include <kdebug.h>
#include <kshortcut.h>

#include <kexidb/cursor.h>

#include "kexipart.h"
#include "kexipartmanager.h"

#include "kexiproject.h"
#include "keximainwindow.h"
#include "kexiuseraction.h"

KexiUserAction::KexiUserAction(KexiMainWindow *win, KActionCollection *parent, const QString &name, const QString &text, const QString &pixmap)
 : KAction(text, pixmap, KShortcut(), this, SLOT(execute()), parent, name.toLatin1())
{
	m_win = win;
	m_method = 0;
	connect(this, SIGNAL(activated()), this, SLOT(execute()));
}

void
KexiUserAction::setMethod(int method, Arguments args)
{
	m_method = method;
	m_args = args;
}

void
KexiUserAction::execute()
{
	kDebug() << "KexiUserAction::execute(): " << KexiUserActionMethod::methodName(m_method) << endl;

	switch(m_method)
	{
		case OpenObject: //open a project object
		{
			//get partinfo
			KexiPart::Info *i = Kexi::partManager().infoForMimeType(m_args[0].toString().toLatin1());
			if (!i) {
				KMessageBox::error(m_win, i18n("Specified part does not exist"));
				return;
			}

			Kexi::partManager().part(i); //load part if doesn't exists
			KexiPart::Item *item = m_win->project()->item(i, m_args[1].toString());
			bool openingCancelled;
			if(!m_win->openObject(item, Kexi::DataViewMode, openingCancelled) && !openingCancelled) {
				KMessageBox::error(m_win, i18n("Specified document could not be opened."));
				return;
			}
			if (openingCancelled)
				return;
			break;
		}	
		default:
			break;
	}
}

KexiUserAction *
KexiUserAction::fromCurrentRecord(KexiMainWindow *context, KActionCollection *parent, KexiDB::Cursor *c)
{
	if(!c || c->bof() || c->eof())
		return 0;

	KexiUserAction *a = new KexiUserAction(context, parent, c->value(1).toString(), c->value(2).toString(), c->value(3).toString());
	QString args = c->value(5).toString();
	bool quote = false;

	Arguments arg;
	QString tmp;
	const int len = args.length();
	for(int i=0; i < len; i++)
	{
		if(args[i] == '"') // if current char is quoted unqote or other way round
		{
			quote = !quote;
		}
		else if(args[i] == ',' && !quote) //if item end add tmp to argumentstack and strip quotes if nessesery
		{
			if(tmp.left(1)=="\"" && tmp.right(1)=="\"")
				tmp = tmp.mid(1, tmp.length()-2);

			arg.append(QVariant(tmp));
			tmp = "";
		}
		else //else simply add char to tmp
		{
			tmp += args[i];
		}
	}

	if(tmp.left(1)=="\"" && tmp.right(1)=="\"")
		tmp = tmp.mid(1, tmp.length()-2);

	arg.append(QVariant(tmp));

	a->setMethod(c->value(4).toInt(), arg);
	return a;
}

KexiUserAction::~KexiUserAction()
{
}

#include "kexiuseraction.moc"

