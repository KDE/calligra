/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "messageaction.h"

#include "../lib/action.h"
#include "../lib/context.h"

#include <kexi_export.h>
#include <core/keximainwindow.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

using namespace KexiMacro;

MessageAction::MessageAction()
	: KexiAction("message", i18n("Message"))
{
	setVariable("caption", i18n("Caption"), QString(""));
	setVariable("message", i18n("Message"), QString(""));
}

MessageAction::~MessageAction() 
{
}

KoMacro::Variable::List MessageAction::notifyUpdated(const QString& variablename, KoMacro::Variable::Map variablemap)
{
	Q_UNUSED(variablename);
	Q_UNUSED(variablemap);
	return KoMacro::Variable::List();
}

void MessageAction::activate(KoMacro::Context::Ptr context)
{
	kdDebug() << "MessageAction::activate(Context::Ptr)" << endl;
	const QString caption = context->variable("caption")->variant().toString();
	const QString message = context->variable("message")->variant().toString();
	KMessageBox::information(mainWin(), message, caption);
}

#include "messageaction.moc"
