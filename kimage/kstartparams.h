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
   */
  int countParams();
  /**
   *  Retrieves a argument that is saved in the object. The index begins
   *  with zero und ends with the number of arguments saved in the object
   *  minus one.
   *
   *  @param _index Index of the argument that will be retrieved.
   */
  QString getParam( uint _index );
  /**
   *  Deletes an argument that is saved in the object. The index begins
   *  with zero und ends with the number of arguments saved in the object
   *  minus one.
   *
   *  @param _index Index of the argument that will be deleted.
   */
  void deleteParam( uint _index );
  /**
   *  Test if an argument is present in the list of arguments.
   *
   *  @param _param The string the will be compared with all stored
   *  arguments stored in the obejct.
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
    */
   bool paramIsPresent( const QString& _longparam, const QString& _shortparam );
   /**
    *  Retrieves the index of the first appearance of _param in the
    *  argument list.
    *  
    *  @param _param String of the param that will be searched for.
    */
   uint getIndex( const QString& _param );
   /**
    *  Retrieves the index of the first appearance of one of the parameters in the
    *  argument list.
    *  
    *  @param _longparam This string represents the long version that will be
    *  compared with all stored arguments stored in the object.
    *  @param _shortparam This string represents the short version that will be
    *  compared with all stored arguments stored in the object.
    */
   uint getIndex( const QString& _longparam, const QString& _shortparam );

protected:
  bool compareParam( const QString& _arg, const QString& _param );

private:
  QStringList m_paramList;
};

#endif