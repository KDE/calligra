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
   *  Deletes the first appearance of an argument that is saved in the object.
   *  If it is not present in the list, nothing will be deleted
   *
   *  @param _param Beginning of the argument that will be deleted.
   */
  void deleteParam( const QString& _param );
  /**
   *  Test if an argument is present in the list of arguments.
   *
   *  @param _param The string the will be compared with all stored
   *  arguments stored in the obejct.
   *  
   *  @return Returns true if the paramter is present or false if it is not.
   */
  bool paramIsPresent( const QString& _param );
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
  bool paramIsPresent( const QString& _longparam, const QString& _shortparam );
  /**
   *  Retrieves the index of the first appearance of _param in the
   *  argument list.
   *  
   *  @param _param String of the param that will be searched for.
   *  
   *  @return Returns the index of a command-line argument that begins with _param.
   *  If no argument meets this -1 will be returned.
   */
  int getIndex( const QString& _param );
  /**
   *  Retrieves the index of the first appearance of one of the parameters in the
   *  argument list.
   *  
   *  @param _longparam This string represents the long version that will be
   *  compared with all stored arguments stored in the object.
   *  @param _shortparam This string represents the short version that will be
   *  compared with all stored arguments stored in the object.
   *  
   *  @return Returns the index of a command-line argument that begins with
   *  _longparam or _shortparam. If no argument meets this -1 will be returned.
   */
  int getIndex( const QString& _longparam, const QString& _shortparam );

/*
  QString getParam( QStringList::Iterator _it );
  void deleteParam( QStringList::Iterator _it );
  QStringList::Iterator getIndex( const QString& _param );
  QStringList::Iterator getIndex( const QString& _longparam, const QString& _shortparam );
*/

protected:
  bool compareParam( const QString& _arg, const QString& _param ) const;

private:
  QStringList m_paramList;
};

#endif