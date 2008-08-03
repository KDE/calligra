#ifndef KEXIUSERACTIONMETHOD_H
#define KEXIUSERACTIONMETHOD_H

#include <q3valuevector.h>
#include <qstring.h>
#include <qvariant.h>

#include <kexi_export.h>

typedef Q3ValueVector<int> ArgTypes;
typedef Q3ValueVector<QString> ArgNames;

/*! describes a UserActionCommand */
class KEXICORE_EXPORT KexiUserActionMethod
{
public:
    /*! constructs a UserActionCommand describtion */
    KexiUserActionMethod(int method, ArgTypes types, ArgNames names);

    /*! \return method id of this method */
    int method() {
        return m_method;
    }

    /*! \return argument type information of this method */
    ArgTypes types() {
        return m_types;
    }

    /*! \return i18n argument names of this method */
    ArgNames names() {
        return m_names;
    }



    /*! \return i18n method name for \a method */
    static QString methodName(int method);

    /*! \return an i18n string for \a type */
    static QString typeName(int type);

private:
    int m_method;
    ArgTypes m_types;
    ArgNames m_names;
};

#endif

