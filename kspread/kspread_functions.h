#ifndef KSPREAD_FUNCTIONS_H
#define KSPREAD_FUNCTIONS_H

class QDomElement;

#include <qstring.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qdict.h>

enum  KSpreadParameterType { KSpread_Int, KSpread_Float, KSpread_String, KSpread_Boolean, KSpread_Any };

class KSpreadFunctionParameter
{
public:
    KSpreadFunctionParameter();
    KSpreadFunctionParameter( const KSpreadFunctionParameter& param );
    KSpreadFunctionParameter( const QDomElement& element );
    
    QString helpText() const { return m_help; }
    KSpreadParameterType type() const { return m_type; }
    bool hasRange() const { return m_range; }
    
private:
    QString m_help;
    KSpreadParameterType m_type;
    bool m_range;
};

class KSpreadFunctionDescription
{
public:
    KSpreadFunctionDescription();
    KSpreadFunctionDescription( const QDomElement& element );
    KSpreadFunctionDescription( const KSpreadFunctionDescription& desc );
    
    const QStringList& examples() { return m_examples; }
    const QStringList& syntax() { return m_syntax; }
    QString helpText() const { return m_help; }
    QString name() const { return m_name; }
    KSpreadParameterType tyoe() const { return m_type; }
    
    int params() const { return m_params.count(); }
    KSpreadFunctionParameter& param( int i ) { return m_params[ i ]; }
    
    void setGroup( const QString& g ) { m_group = g; }
    QString group() const { return m_group; }
    
private:
    QString m_group;
    QStringList m_examples;
    QStringList m_syntax;
    QString m_help;
    QString m_name;
    KSpreadParameterType m_type;
    QValueList<KSpreadFunctionParameter> m_params;
};

class KSpreadFunctionRepository
{
public:
    KSpreadFunctionRepository();
    
    KSpreadFunctionDescription* function( const QString& name );
    
    QStringList functionNames( const QString& group );
    
    const QStringList& groups() const { return m_groups; }
    
private:
    QDict<KSpreadFunctionDescription> m_funcs;
    QStringList m_groups;
};

#endif
