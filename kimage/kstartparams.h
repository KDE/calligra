/* This file is part of the KDE project
   Copyright (C) 1999 Michael Koch <mkoch@bigfoot.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KSTARTPARAMS_H_
#define _KSTARTPARAMS_H_

#include <qstring.h>
#include <qstringlist.h>

/**
 *  This is a class for simple access to the command-line arguments.
 *
 *  You have to give two arguments to the constructor, a number and an
 *  array of char[]. The first represents the number of strings in the
 *  array and the second is the array itself.
 *
 *  @short A class for simple command-line argument access.
 *  @author Michael Koch
 *  @version 0.0.1
 */
class KStartParams
{
public:
  /**
   *  Konstructor.
   *
   *  The given arguments are only copied, nor deleted.
   */
  KStartParams( int& argc, char** argv );
  /**
   *  Destrcutor.
   */
  ~KStartParams();
  /**
   *  Retrieves the current number of arguments saved in the object.
   *  
   *  @return Returns the number of parameters stored in the object.
   */
  uint countParams() const;
  /**
   *  Retrieves a argument that is saved in the object. The index begins
   *  with zero und ends with the number of arguments saved in the object
   *  minus one.
   *
   *  @param _index Index of the argument that will be retrieved.
   *  
   *  @return Returns the string that is stored at the position _index.
   *  If there is no entry with that index an empty string will be returned.
   */
  QString getParam( const uint _index ) const;
  /**
   *  Deletes an argument that is saved in the object. The index begins
   *  with zero und ends with the number of arguments saved in the object
   *  minus one.
   *
   *  @param _index Index of the argument that will be deleted.
   */
  void deleteParam( const uint _index );
  /**
   *  Deletes an argument that is saved in the object. The iterator
   *  represents the param that will be deleted.
   *
   *  @param _index Iterator that represents the argument that will be deleted.
   */
  void deleteParam( const QStringList::Iterator _it );
  /**
   *  Deletes the first appearance of an argument that is saved in the object.
   *  If it is not present in the list, nothing will be deleted
   *
   *  @param _param Beginning of the argument that will be deleted.
   */
  //void deleteParam( const QString& _param );
  void deleteParam( const QString& _param, bool _check );
  void deleteParam( const QString& _shortparam, const QString& _longparam, bool _check );
  /**
   *  Test if an argument is present in the list of arguments.
   *
   *  @param _param The string the will be compared with all stored
   *  arguments stored in the obejct.
   *  
   *  @return Returns true if the paramter is present or false if it is not.
   */
  //bool paramIsPresent( const QString& _param );
  bool paramIsPresent( const QString& _param, bool _check, QStringList::Iterator& _it );
  /**
   *  Test if an argument is present in the list of arguments, either in the 
   *  short or the long version of the option.
   *
   *  @param _longparam This string represents the long version that will be
   *  compared with all stored arguments stored in the object.
   *  @param _shortparam This string represents the short version that will be
   *  compared with all stored arguments stored in the object.
   *  
   *  @return Returns true if the paramter is present or false if it is not.
   */
  //bool paramIsPresent( const QString& _longparam, const QString& _shortparam );
  bool paramIsPresent( const QString& _longparam, const QString& _shortparam, bool _check, QStringList::Iterator& _it );
  /**
   *  Retrieves the index of the first appearance of _param in the
   *  argument list.
   *  
   *  @param _param String of the param that will be searched for.
   *  @param _checkWholeString If this parameter is true the whole command-line argument will 
   *  be compared with _param. If it is false the beginning of the command-line arguments will
   *  be checked.
   *  @param _it In this variable a iterator for later access to the parameter will be stored
   *  
   *  @return Returns either true if a parmater of a command-line begins with
   *  _param or false no argument meets this.
   */
  bool getIndex( const QString& _param, bool _check, QStringList::Iterator& _it );
  /**
   *  Retrieves the index of the first appearance of one of the parameters in the
   *  argument list.
   *  
   *  @param _longparam This string represents the long version that will be
   *  compared with all stored arguments stored in the object.
   *  @param _shortparam This string represents the short version that will be
   *  compared with all stored arguments stored in the object.
   *  @param _check If this parameter is true the whole command-line argument will 
   *  be compared with _param. If it is false the beginning of the command-line arguments will
   *  be checked.
   *  @param _it In this variable a iterator for later access to the parameter will be stored
   *  
   *  @return Returns either true if a parmater of a command-line begins with
   *  _longparam or _shortparam or false no argument meets this.
   */
  bool getIndex( const QString& _longparam, const QString& _shortparam, bool _check, QStringList::Iterator& _it );

protected:
  bool compareParam( const QString& _arg, const QString& _param, bool _check ) const;

private:
  QStringList m_paramList;
};

#endif
