/* This file is part of the KDE project
 *
 * Copyright (C) 2002-2004 George Staikos <staikos@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef FAKE_KWALLET_H
#define FAKE_KWALLET_H

#include <QtCore/QStringList>
#include <QtCore/QObject>
#include <qwindowdefs.h>
#include <kofake_export.h>

namespace KWallet {

/**
 * KDE Wallet
 *
 * This class implements a generic system-wide Wallet for KDE.  This is the
 * ONLY public interface.
 *
 * @author George Staikos <staikos@kde.org>
 * @short KDE Wallet Class
 */
class KOFAKE_EXPORT Wallet : public QObject
{
	Q_OBJECT
	protected:
		/**
		 *  Construct a KWallet object.
		 *  @internal
		 *  @param handle The handle for the wallet.
		 *  @param name The name of the wallet.
		 */
		Wallet(int handle, const QString& name) : QObject() {}
		/**
		 *  Copy a KWallet object.
		 *  @internal
		 */
		Wallet(const Wallet&) : QObject() {}

	public:
		enum OpenType { Synchronous=0, Asynchronous, Path, OpenTypeUnused=0xff };

		/**
		 *  Open the wallet @p name.  The user will be prompted to
		 *  allow your application to open the wallet, and may be
		 *  prompted for a password.  You are responsible for deleting
		 *  this object when you are done with it.
		 *  @param name The name of the wallet to open.
		 *  @param ot    If Asynchronous, the call will return
		 *               immediately with a non-null pointer to an
		 *               invalid wallet.  You must immediately connect
		 *               the walletOpened() signal to a slot so that
		 *               you will know when it is opened, or when it
		 *               fails.
		 *  @param w The window id to associate any dialogs with. You can pass
		 *           0 if you don't have a window the password dialog should
		 *           associate with.
		 *  @return Returns a pointer to the wallet if successful,
		 *          or a null pointer on error or if rejected.
		 */
		static Wallet* openWallet(const QString& name, WId w, OpenType ot = Synchronous) {return 0;}

		/**
		 *  The name of the wallet used to store local passwords.
		 */
		static const QString LocalWallet() {return QString();}

		/**
		 *  The standardized name of the password folder.
		 *  It is automatically created when a wallet is created, but
		 *  the user may still delete it so you should check for its
		 *  existence and recreate it if necessary and desired.
		 */
		static const QString PasswordFolder() {return QString();}
		/**
		 *  Determine if a folder does not exist in a wallet.  This
		 *  does not require decryption of the wallet.
		 *  This is a handy optimization to avoid prompting the user
		 *  if your data is certainly not in the wallet.
		 *  @param wallet The wallet to look in.
		 *  @param folder The folder to look up.
		 *  @return Returns true if the folder does NOT exist in the
		 *  wallet, or the wallet does not exist.
		 */
		static bool folderDoesNotExist(const QString& wallet, const QString& folder) {return true;}

		/**
		 *  Determine if an entry in a folder does not exist in a
		 *  wallet.  This does not require decryption of the wallet.
		 *  This is a handy optimization to avoid prompting the user
		 *  if your data is certainly not in the wallet.
		 *  @param wallet The wallet to look in.
		 *  @param folder The folder to look in.
		 *  @param key The key to look up.
		 *  @return Returns true if the key does NOT exist in the
		 *  wallet, or the folder or wallet does not exist.
		 */
		static bool keyDoesNotExist(const QString& wallet, const QString& folder,
					    const QString& key) {return true;}
					    
                /**
                 *  Determine if the current folder has they entry @p key.
                 *  @param key The key to search for.
                 *  @return Returns true if the folder contains @p key.
                 */
                virtual bool hasEntry(const QString& key) {return true;}

                /**
                 *  Remove the entry @p key from the current folder.
                 *  @param key The key to remove.
                 *  @return Returns 0 on success, non-zero on error.
                 */
                virtual int removeEntry(const QString& key) {return 0;}

                /**
                 *  Set the current working folder to @p f.  The folder must
                 *  exist, or this call will fail.  Create a folder with
                 *  createFolder().
                 *  @param f the name of the folder to make the working folder
                 *  @return Returns true if the folder was successfully set.
                 */
                virtual bool setFolder(const QString& f) {return true;}
                /**
                 *  Determine if the folder @p f exists in the wallet.
                 *  @param f the name of the folder to check for
                 *  @return Returns true if the folder exists in the wallet.
                 */
                virtual bool hasFolder(const QString& f) {return true;}
                /**
                 *  Created the folder @p f.
                 *  @param f the name of the folder to create
                 *  @return Returns true if the folder was successfully created.
                 */
                virtual bool createFolder(const QString& f) {return true;}

                /**
                 *  Read the password entry @p key from the current folder.
                 *  @param key The key of the entry to read.
                 *  @param value A password buffer to fill with the value.
                 *  @return Returns 0 on success, non-zero on error.  Will
                 *          return an error if the key was not originally
                 *          written as a password.
                 */
                virtual int readPassword(const QString& key, QString& value) {return 0;}

                /**
                 *  Write @p key = @p value as a password to the current folder.
                 *  @param key The key of the new entry.
                 *  @param value The value of the password.
                 *  @return Returns 0 on success, non-zero on error.
                 */
                virtual int writePassword(const QString& key, const QString& value) {return 0;}


};

}

#endif //FAKE_KWALLET_H

