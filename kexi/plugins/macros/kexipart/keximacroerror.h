#ifndef KEXIMACROERROR_H
#define KEXIMACROERROR_H
#include <qwidget.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kdebug.h>

#include "../lib/context.h"
#include "../lib/exception.h"
#include "../lib/macro.h"
#include "../lib/macroitem.h"

#include "keximacroerrorbase.h"

class KexiMacroError : public KexiMacroErrorBase {
	Q_OBJECT
	
	public:
		KexiMacroError(QWidget* parent, const char* name, WFlags fl, KoMacro::Context::Ptr context);
		
		virtual ~KexiMacroError();
	public slots:

		/**
		* Called if the "Open Macrodesigner"-Button is clicked.
		*/
		void designbtnClicked();

		/**
		* Called if the "continue"-Button is clicked.
		* @param context The @a Context this @a Macro should
		* be executed in.
		*/
		void continuebtnClicked();
	
	private:
		/// \internal d-pointer class.
		class Private;
		/// \internal d-pointer instance.
		Private* const d;
	
};
#endif
