// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
/*
    This file is part of KDE.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2003 Waldo Bastian <bastian@kde.org>
    Copyright (c) 2003 Zack Rusin <zack@kde.org>
    Copyright (c) 2006 Michaël Larouche <michael.larouche@kdemail.net>
    Copyright (c) 2008 Allen Winter <winter@kde.org>

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Compiling this file with this flag is just crazy
#undef QT_NO_CAST_FROM_ASCII

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtXml/QDomAttr>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#include <ostream>
#include <iostream>
#include <stdlib.h>

namespace
{
  QTextStream cout(stdout);
  QTextStream cerr(stderr);
}

static void parseArgs(const QStringList &args, QString &directory, QString &file1, QString &file2)
{
    int fileCount = 0;
    directory = QChar::fromLatin1('.');

    for (int i = 1; i < args.count(); ++i) {
        if (args.at(i) == QLatin1String("-d") ||  args.at(i) == QLatin1String("--directory")) {
            if (i + 1 > args.count()) {
                cerr << args.at(i) << " needs an argument" << endl;
                exit(1);
            }
            directory = args.at(++i);
        } else if (args.at(i).startsWith(QLatin1String("-d"))) {
            directory = args.at(i).mid(2);
        } else if (args.at(i) == QLatin1String("--help") || args.at(i) == QLatin1String("-h")) {
            cout << "Options:" << endl;
            cout << "  -L --license              Display software license" << endl;
            cout << "  -d, --directory <dir>     Directory to generate files in [.]" << endl;
            cout << "  -h, --help                Display this help" << endl;
            cout << endl;
            cout << "Arguments:" << endl;
            cout << "      file.kcfg                 Input kcfg XML file" << endl;
            cout << "      file.kcfgc                Code generation options file" << endl;
            exit(0);
        } else if (args.at(i) == QLatin1String("--license") || args.at(i) == QLatin1String("-L")) {
            cout << "Copyright 2003 Cornelius Schumacher, Waldo Bastian, Zack Rusin," << endl;
            cout << "    Reinhold Kainhofer, Duncan Mac-Vicar P., Harald Fernengel" << endl;
            cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
            cout << "You may redistribute copies of this program" << endl;
            cout << "under the terms of the GNU Library Public License." << endl;
            cout << "For more information about these matters, see the file named COPYING." << endl;
            exit(0);
        } else if (args.at(i).startsWith(QLatin1Char('-'))) {
            cerr << "Unknown option: " << args.at(i) << endl;
            exit(1);
        } else if (fileCount == 0) {
            file1 = args.at(i);
            ++fileCount;
        } else if (fileCount == 1) {
            file2 = args.at(i);
            ++fileCount;
        } else {
            cerr << "Too many arguments" << endl;
            exit(1);
        }
    }
    if (fileCount < 2) {
        cerr << "Too few arguments" << endl;
        exit(1);
    }
}

QStringList allNames;
QRegExp *validNameRegexp;
QString This;
QString Const;

/**
   Configuration Compiler Configuration
*/
class CfgConfig
{
public:
  CfgConfig( const QString &codegenFilename )
  {
    // Configure the compiler with some settings
    QSettings codegenConfig(codegenFilename, QSettings::IniFormat);

    nameSpace = codegenConfig.value("NameSpace").toString();
    className = codegenConfig.value("ClassName").toString();
    if ( className.isEmpty() ) {
      cerr << "Class name missing" << endl;
      exit(1);
    }
    inherits = codegenConfig.value("Inherits").toString();
    if ( inherits.isEmpty() ) inherits = "KConfigSkeleton";
    visibility = codegenConfig.value("Visibility").toString();
    if ( !visibility.isEmpty() ) visibility += ' ';
    forceStringFilename = codegenConfig.value("ForceStringFilename", false).toBool();
    singleton = codegenConfig.value("Singleton", false).toBool();
    staticAccessors = singleton;
    customAddons = codegenConfig.value("CustomAdditions", false).toBool();
    memberVariables = codegenConfig.value("MemberVariables").toString();
    dpointer = (memberVariables == "dpointer");
    headerIncludes = codegenConfig.value("IncludeFiles", QStringList()).toStringList();
    sourceIncludes = codegenConfig.value("SourceIncludeFiles", QStringList()).toStringList();
    mutators = codegenConfig.value("Mutators", QStringList()).toStringList();
    allMutators = ((mutators.count() == 1) && (mutators.at(0).toLower() == "true"));
    itemAccessors = codegenConfig.value("ItemAccessors", false).toBool();
    setUserTexts = codegenConfig.value("SetUserTexts", false).toBool();
    defaultGetters = codegenConfig.value("DefaultValueGetters", QStringList()).toStringList();
    allDefaultGetters = (defaultGetters.count() == 1) && (defaultGetters.at(0).toLower() == "true");
    globalEnums = codegenConfig.value("GlobalEnums", false).toBool();
    useEnumTypes = codegenConfig.value("UseEnumTypes", false).toBool();

    const QString trString = codegenConfig.value("TranslationSystem").toString().toLower();
    if ( trString == "kde" ) {
        translationSystem = KdeTranslation;
    } else {
        if ( !trString.isEmpty() && trString != "qt" ) {
            cerr << "Unknown translation system, falling back to Qt tr()" << endl;
        }
        translationSystem = QtTranslation;
    }
  }

public:
  enum TranslationSystem {
      QtTranslation,
      KdeTranslation
  };

  // These are read from the .kcfgc configuration file
  QString nameSpace;     // The namespace for the class to be generated
  QString className;     // The class name to be generated
  QString inherits;      // The class the generated class inherits (if empty, from KConfigSkeleton)
  QString visibility;
  bool forceStringFilename;
  bool singleton;        // The class will be a singleton
  bool staticAccessors;  // provide or not static accessors
  bool customAddons;
  QString memberVariables;
  QStringList headerIncludes;
  QStringList sourceIncludes;
  QStringList mutators;
  QStringList defaultGetters;
  bool allMutators;
  bool setUserTexts;
  bool allDefaultGetters;
  bool dpointer;
  bool globalEnums;
  bool useEnumTypes;
  bool itemAccessors;
  TranslationSystem translationSystem;
};


struct SignalArguments
{
      QString type;
      QString variableName;
};

class Signal {
public:
  QString name;
  QString label;
  QList<SignalArguments> arguments;
};




class CfgEntry
{
  public:
    struct Choice
    {
      QString name;
      QString context;
      QString label;
      QString toolTip;
      QString whatsThis;
    };
    class Choices
    {
      public:
        Choices() {}
        Choices( const QList<Choice> &d, const QString &n, const QString &p )
             : prefix(p), choices(d), mName(n)
        {
          int i = n.indexOf(QLatin1String("::"));
          if (i >= 0)
            mExternalQual = n.left(i + 2);
        }
        QString prefix;
        QList<Choice> choices;
        const QString& name() const  { return mName; }
        const QString& externalQualifier() const  { return mExternalQual; }
        bool external() const  { return !mExternalQual.isEmpty(); }
      private:
        QString mName;
        QString mExternalQual;
    };

    CfgEntry( const QString &group, const QString &type, const QString &key,
              const QString &name, const QString &labelContext, const QString &label,
              const QString &toolTipContext, const QString &toolTip, const QString &whatsThisContext, const QString &whatsThis, const QString &code,
              const QString &defaultValue, const Choices &choices, const QList<Signal> signalList,
              bool hidden )
      : mGroup( group ), mType( type ), mKey( key ), mName( name ),
        mLabelContext( labelContext ), mLabel( label ), mToolTipContext( toolTipContext ), mToolTip( toolTip ),
        mWhatsThisContext( whatsThisContext ), mWhatsThis( whatsThis ),
        mCode( code ), mDefaultValue( defaultValue ), mChoices( choices ),
        mSignalList(signalList), mHidden( hidden )
    {
    }

    void setGroup( const QString &group ) { mGroup = group; }
    QString group() const { return mGroup; }

    void setType( const QString &type ) { mType = type; }
    QString type() const { return mType; }

    void setKey( const QString &key ) { mKey = key; }
    QString key() const { return mKey; }

    void setName( const QString &name ) { mName = name; }
    QString name() const { return mName; }

    void setLabelContext( const QString &labelContext ) { mLabelContext = labelContext; }
    QString labelContext() const { return mLabelContext; }

    void setLabel( const QString &label ) { mLabel = label; }
    QString label() const { return mLabel; }

    void setToolTipContext( const QString &toolTipContext ) { mToolTipContext = toolTipContext; }
    QString toolTipContext() const { return mToolTipContext; }

    void setToolTip( const QString &toolTip ) { mToolTip = toolTip; }
    QString toolTip() const { return mToolTip; }

    void setWhatsThisContext( const QString &whatsThisContext ) { mWhatsThisContext = whatsThisContext; }
    QString whatsThisContext() const { return mWhatsThisContext; }

    void setWhatsThis( const QString &whatsThis ) { mWhatsThis = whatsThis; }
    QString whatsThis() const { return mWhatsThis; }

    void setDefaultValue( const QString &d ) { mDefaultValue = d; }
    QString defaultValue() const { return mDefaultValue; }

    void setCode( const QString &d ) { mCode = d; }
    QString code() const { return mCode; }

    void setMinValue( const QString &d ) { mMin = d; }
    QString minValue() const { return mMin; }

    void setMaxValue( const QString &d ) { mMax = d; }
    QString maxValue() const { return mMax; }

    void setParam( const QString &d ) { mParam = d; }
    QString param() const { return mParam; }

    void setParamName( const QString &d ) { mParamName = d; }
    QString paramName() const { return mParamName; }

    void setParamType( const QString &d ) { mParamType = d; }
    QString paramType() const { return mParamType; }

    void setChoices( const QList<Choice> &d, const QString &n, const QString &p ) { mChoices = Choices( d, n, p ); }
    Choices choices() const { return mChoices; }

    void setParamValues( const QStringList &d ) { mParamValues = d; }
    QStringList paramValues() const { return mParamValues; }

    void setParamDefaultValues( const QStringList &d ) { mParamDefaultValues = d; }
    QString paramDefaultValue(int i) const { return mParamDefaultValues[i]; }

    void setParamMax( int d ) { mParamMax = d; }
    int paramMax() const { return mParamMax; }

    void setSignalList( const QList<Signal> &value ) { mSignalList = value; }
    QList<Signal> signalList() const { return mSignalList; }

    bool hidden() const { return mHidden; }

    void dump() const
    {
      cerr << "<entry>" << endl;
      cerr << "  group: " << mGroup << endl;
      cerr << "  type: " << mType << endl;
      cerr << "  key: " << mKey << endl;
      cerr << "  name: " << mName << endl;
      cerr << "  label context: " << mLabelContext << endl;
      cerr << "  label: " << mLabel << endl;
// whatsthis
      cerr << "  code: " << mCode << endl;
//      cerr << "  values: " << mValues.join(":") << endl;

      if (!param().isEmpty())
      {
        cerr << "  param name: "<< mParamName << endl;
        cerr << "  param type: "<< mParamType << endl;
        cerr << "  paramvalues: " << mParamValues.join(QChar::fromLatin1(':')) << endl;
      }
      cerr << "  default: " << mDefaultValue << endl;
      cerr << "  hidden: " << mHidden << endl;
      cerr << "  min: " << mMin << endl;
      cerr << "  max: " << mMax << endl;
      cerr << "</entry>" << endl;
    }

  private:
    QString mGroup;
    QString mType;
    QString mKey;
    QString mName;
    QString mLabelContext;
    QString mLabel;
    QString mToolTipContext;
    QString mToolTip;
    QString mWhatsThisContext;
    QString mWhatsThis;
    QString mCode;
    QString mDefaultValue;
    QString mParam;
    QString mParamName;
    QString mParamType;
    Choices mChoices;
    QList<Signal> mSignalList;
    QStringList mParamValues;
    QStringList mParamDefaultValues;
    int mParamMax;
    bool mHidden;
    QString mMin;
    QString mMax;
};

class Param {
public:
  QString name;
  QString type;
};

// returns the name of an member variable
// use itemPath to know the full path
// like using d-> in case of dpointer
static QString varName(const QString &n, const CfgConfig &cfg)
{
  QString result;
  if ( !cfg.dpointer ) {
    result = QChar::fromLatin1('m') + n;
    result[1] = result[1].toUpper();
  }
  else {
    result = n;
    result[0] = result[0].toLower();
  }
  return result;
}

static QString varPath(const QString &n, const CfgConfig &cfg)
{
  QString result;
  if ( cfg.dpointer ) {
    result = "d->"+varName(n, cfg);
  }
  else {
    result = varName(n, cfg);
  }
  return result;
}

static QString enumName(const QString &n)
{
  QString result = QString::fromLatin1("Enum") + n;
  result[4] = result[4].toUpper();
  return result;
}

static QString enumName(const QString &n, const CfgEntry::Choices &c)
{
  QString result = c.name();
  if ( result.isEmpty() )
  {
    result = QString::fromLatin1("Enum") + n;
    result[4] = result[4].toUpper();
  }
  return result;
}

static QString enumType(const CfgEntry *e, bool globalEnums)
{
  QString result = e->choices().name();
  if ( result.isEmpty() )
  {
    result = QString::fromLatin1("Enum") + e->name();
    if( !globalEnums )
        result += QString::fromLatin1("::type");
    result[4] = result[4].toUpper();
  }
  return result;
}

static QString enumTypeQualifier(const QString &n, const CfgEntry::Choices &c)
{
  QString result = c.name();
  if ( result.isEmpty() )
  {
    result = QString::fromLatin1("Enum") + n + QString::fromLatin1("::");
    result[4] = result[4].toUpper();
  }
  else if ( c.external() )
    result = c.externalQualifier();
  else
    result.clear();
  return result;
}

static QString setFunction(const QString &n, const QString &className = QString())
{
  QString result = QString::fromLatin1("set") + n;
  result[3] = result[3].toUpper();

  if ( !className.isEmpty() )
    result = className + QString::fromLatin1("::") + result;
  return result;
}

static QString getDefaultFunction(const QString &n, const QString &className = QString())
{
  QString result = QString::fromLatin1("default") +  n + QString::fromLatin1("Value");
  result[7] = result[7].toUpper();

  if ( !className.isEmpty() )
    result = className + QString::fromLatin1("::") + result;
  return result;
}

static QString getFunction(const QString &n, const QString &className = QString())
{
  QString result = n;
  result[0] = result[0].toLower();

  if ( !className.isEmpty() )
    result = className + QString::fromLatin1("::") + result;
  return result;
}


static void addQuotes( QString &s )
{
  if ( !s.startsWith( QLatin1Char('"') ) )
    s.prepend( QLatin1Char('"') );
  if ( !s.endsWith( QLatin1Char('"') ) )
    s.append( QLatin1Char('"') );
}

static QString quoteString( const QString &s )
{
  QString r = s;
  r.replace( QLatin1Char('\\'), QLatin1String("\\\\") );
  r.replace( QLatin1Char('\"'), QLatin1String("\\\"") );
  r.remove( QLatin1Char('\r') );
  r.replace( QLatin1Char('\n'), QLatin1String("\\n\"\n\"") );
  return QLatin1Char('\"') + r + QLatin1Char('\"');
}

static QString literalString( const QString &s )
{
  bool isAscii = true;
  for(int i = s.length(); i--;)
     if (s[i].unicode() > 127) isAscii = false;

  if (isAscii)
     return QString::fromLatin1("QLatin1String( ") + quoteString(s) + QString::fromLatin1(" )");
  else
     return QString::fromLatin1("QString::fromUtf8( ") + quoteString(s) + QString::fromLatin1(" )");
}

static QString dumpNode(const QDomNode &node)
{
  QString msg;
  QTextStream s(&msg, QIODevice::WriteOnly );
  node.save(s, 0);

  msg = msg.simplified();
  if (msg.length() > 40)
    return msg.left(37) + QString::fromLatin1("...");
  return msg;
}

static QString filenameOnly(const QString& path)
{
   int i = path.lastIndexOf(QRegExp(QLatin1String("[/\\]")));
   if (i >= 0)
      return path.mid(i+1);
   return path;
}

static QString signalEnumName(const QString &signalName)
{
  QString result;
  result = QString::fromLatin1("signal") + signalName;
  result[6] = result[6].toUpper();

  return result;
}

static void preProcessDefault( QString &defaultValue, const QString &name,
                               const QString &type,
                               const CfgEntry::Choices &choices,
                               QString &code, const CfgConfig &cfg )
{
    if ( type == QLatin1String("String") && !defaultValue.isEmpty() ) {
      defaultValue = literalString(defaultValue);

    } else if ( type == QLatin1String("Path") && !defaultValue.isEmpty() ) {
      defaultValue = literalString( defaultValue );
    } else if ( type == QLatin1String("Url") && !defaultValue.isEmpty() ) {
      // Use fromUserInput in order to support absolute paths and absolute urls, like KDE4's KUrl(QString) did.
      defaultValue = QString::fromLatin1("QUrl::fromUserInput( ") + literalString(defaultValue) + QLatin1Char(')');
    } else if ( ( type == QLatin1String("UrlList") || type == QLatin1String("StringList") || type == QLatin1String("PathList")) && !defaultValue.isEmpty() ) {
      QTextStream cpp( &code, QIODevice::WriteOnly | QIODevice::Append );
      if (!code.isEmpty())
         cpp << endl;

      if( type == "UrlList" ) {
        cpp << "  QList<QUrl> default" << name << ";" << endl;
      } else {
        cpp << "  QStringList default" << name << ";" << endl;
      }
      const QStringList defaults = defaultValue.split(QLatin1Char(','));
      QStringList::ConstIterator it;
      for( it = defaults.constBegin(); it != defaults.constEnd(); ++it ) {
        cpp << "  default" << name << ".append( ";
        if( type == QLatin1String("UrlList") ) {
          cpp << "QUrl::fromUserInput(";
        }
        cpp << "QString::fromUtf8( \"" << *it << "\" ) ";
        if( type == QLatin1String("UrlList") ) {
          cpp << ") ";
        }
        cpp << ");" << endl;
      }
      defaultValue = QString::fromLatin1("default") + name;

    } else if ( type == QLatin1String("Color") && !defaultValue.isEmpty() ) {
      QRegExp colorRe(QLatin1String("\\d+,\\s*\\d+,\\s*\\d+(,\\s*\\d+)?"));
      if (colorRe.exactMatch(defaultValue))
      {
        defaultValue = QLatin1String("QColor( ") + defaultValue + QLatin1String(" )");
      }
      else
      {
        defaultValue = QLatin1String("QColor( \"") + defaultValue + QLatin1String("\" )");
      }

    } else if ( type == QLatin1String("Enum") ) {
      QList<CfgEntry::Choice>::ConstIterator it;
      for( it = choices.choices.constBegin(); it != choices.choices.constEnd(); ++it ) {
        if ( (*it).name == defaultValue ) {
          if ( cfg.globalEnums && choices.name().isEmpty() )
            defaultValue.prepend( choices.prefix );
          else
            defaultValue.prepend( enumTypeQualifier(name, choices) + choices.prefix );
          break;
        }
      }

    } else if ( type == QLatin1String("IntList") ) {
      QTextStream cpp( &code, QIODevice::WriteOnly | QIODevice::Append );
      if (!code.isEmpty())
         cpp << endl;

      cpp << "  QList<int> default" << name << ";" << endl;
      if (!defaultValue.isEmpty())
      {
        const QStringList defaults = defaultValue.split( QLatin1Char(',') );
        QStringList::ConstIterator it;
        for( it = defaults.constBegin(); it != defaults.constEnd(); ++it ) {
          cpp << "  default" << name << ".append( " << *it << " );"
              << endl;
        }
      }
      defaultValue = QString::fromLatin1("default") + name;
    }
}


CfgEntry *parseEntry( const QString &group, const QDomElement &element, const CfgConfig &cfg )
{
  bool defaultCode = false;
  QString type = element.attribute( "type" );
  QString name = element.attribute( "name" );
  QString key = element.attribute( "key" );
  QString hidden = element.attribute( "hidden" );
  QString labelContext;
  QString label;
  QString toolTipContext;
  QString toolTip;
  QString whatsThisContext;
  QString whatsThis;
  QString defaultValue;
  QString code;
  QString param;
  QString paramName;
  QString paramType;
  CfgEntry::Choices choices;
  QList<Signal> signalList;
  QStringList paramValues;
  QStringList paramDefaultValues;
  QString minValue;
  QString maxValue;
  int paramMax = 0;

  for ( QDomElement e = element.firstChildElement(); !e.isNull(); e = e.nextSiblingElement() ) {
    QString tag = e.tagName();
    if ( tag == "label" ) {
      label = e.text();
      labelContext = e.attribute( "context" );
    }
    else if ( tag == "tooltip" ) {
      toolTip = e.text();
      toolTipContext = e.attribute( "context" );
    }
    else if ( tag == "whatsthis" ) {
      whatsThis = e.text();
      whatsThisContext = e.attribute( "context" );
    }
    else if ( tag == "min" ) minValue = e.text();
    else if ( tag == "max" ) maxValue = e.text();
    else if ( tag == "code" ) code = e.text();
    else if ( tag == "parameter" )
    {
      param = e.attribute( "name" );
      paramType = e.attribute( "type" );
      if ( param.isEmpty() ) {
        cerr << "Parameter must have a name: " << dumpNode(e) << endl;
        return 0;
      }
      if ( paramType.isEmpty() ) {
        cerr << "Parameter must have a type: " << dumpNode(e) << endl;
        return 0;
      }
      if ((paramType == "Int") || (paramType == "UInt"))
      {
         bool ok;
         paramMax = e.attribute("max").toInt(&ok);
         if (!ok)
         {
           cerr << "Integer parameter must have a maximum (e.g. max=\"0\"): "
                       << dumpNode(e) << endl;
           return 0;
         }
      }
      else if (paramType == "Enum")
      {
         for ( QDomElement e2 = e.firstChildElement(); !e2.isNull(); e2 = e2.nextSiblingElement() ) {
           if (e2.tagName() == "values")
           {
             for ( QDomElement e3 = e2.firstChildElement(); !e3.isNull(); e3 = e3.nextSiblingElement() ) {
               if (e3.tagName() == "value")
               {
                  paramValues.append( e3.text() );
               }
             }
             break;
           }
         }
         if (paramValues.isEmpty())
         {
           cerr << "No values specified for parameter '" << param
                       << "'." << endl;
           return 0;
         }
         paramMax = paramValues.count()-1;
      }
      else
      {
        cerr << "Parameter '" << param << "' has type " << paramType
                    << " but must be of type int, uint or Enum." << endl;
        return 0;
      }
    }
    else if ( tag == "default" )
    {
      if (e.attribute("param").isEmpty())
      {
        defaultValue = e.text();
        if (e.attribute( "code" ) == "true")
          defaultCode = true;
      }
    }
    else if ( tag == "choices" ) {
      QString name = e.attribute( "name" );
      QString prefix = e.attribute( "prefix" );
      QList<CfgEntry::Choice> chlist;
      for( QDomElement e2 = e.firstChildElement(); !e2.isNull(); e2 = e2.nextSiblingElement() ) {
        if ( e2.tagName() == "choice" ) {
          CfgEntry::Choice choice;
          choice.name = e2.attribute( "name" );
          if ( choice.name.isEmpty() ) {
            cerr << "Tag <choice> requires attribute 'name'." << endl;
          }
          for( QDomElement e3 = e2.firstChildElement(); !e3.isNull(); e3 = e3.nextSiblingElement() ) {
            if ( e3.tagName() == "label" ) {
              choice.label = e3.text();
              choice.context = e3.attribute( "context" );
            }
            if ( e3.tagName() == "tooltip" ) {
              choice.toolTip = e3.text();
              choice.context = e3.attribute( "context" );
            }
            if ( e3.tagName() == "whatsthis" ) {
              choice.whatsThis = e3.text();
              choice.context = e3.attribute( "context" );
            }
          }
          chlist.append( choice );
        }
      }
      choices = CfgEntry::Choices( chlist, name, prefix );
    }
   else if ( tag == "emit" ) {
    QDomNode signalNode;
    Signal signal;
    signal.name = e.attribute( "signal" );
    signalList.append( signal);
   }
  }


  bool nameIsEmpty = name.isEmpty();
  if ( nameIsEmpty && key.isEmpty() ) {
    cerr << "Entry must have a name or a key: " << dumpNode(element) << endl;
    return 0;
  }

  if ( key.isEmpty() ) {
    key = name;
  }

  if ( nameIsEmpty ) {
    name = key;
    name.remove( ' ' );
  } else if ( name.contains( ' ' ) ) {
    cout<<"Entry '"<<name<<"' contains spaces! <name> elements can not contain spaces!"<<endl;
    name.remove( ' ' );
  }

  if (name.contains("$("))
  {
    if (param.isEmpty())
    {
      cerr << "Name may not be parameterized: " << name << endl;
      return 0;
    }
  }
  else
  {
    if (!param.isEmpty())
    {
      cerr << "Name must contain '$(" << param << ")': " << name << endl;
      return 0;
    }
  }

  if ( label.isEmpty() ) {
    label = key;
  }

  if ( type.isEmpty() ) type = "String"; // XXX : implicit type might be bad

  if (!param.isEmpty())
  {
    // Adjust name
    paramName = name;
    name.remove("$("+param+')');
    // Lookup defaults for indexed entries
    for(int i = 0; i <= paramMax; i++)
    {
      paramDefaultValues.append(QString());
    }

    for ( QDomElement e = element.firstChildElement(); !e.isNull(); e = e.nextSiblingElement() ) {
      QString tag = e.tagName();
      if ( tag == "default" )
      {
        QString index = e.attribute("param");
        if (index.isEmpty())
           continue;

        bool ok;
        int i = index.toInt(&ok);
        if (!ok)
        {
          i = paramValues.indexOf(index);
          if (i == -1)
          {
            cerr << "Index '" << index << "' for default value is unknown." << endl;
            return 0;
          }
        }

        if ((i < 0) || (i > paramMax))
        {
          cerr << "Index '" << i << "' for default value is out of range [0, "<< paramMax<<"]." << endl;
          return 0;
        }

        QString tmpDefaultValue = e.text();

        if (e.attribute( "code" ) != "true")
           preProcessDefault(tmpDefaultValue, name, type, choices, code, cfg);

        paramDefaultValues[i] = tmpDefaultValue;
      }
    }
  }

  if (!validNameRegexp->exactMatch(name))
  {
    if (nameIsEmpty)
      cerr << "The key '" << key << "' can not be used as name for the entry because "
                   "it is not a valid name. You need to specify a valid name for this entry." << endl;
    else
      cerr << "The name '" << name << "' is not a valid name for an entry." << endl;
    return 0;
  }

  if (allNames.contains(name))
  {
    if (nameIsEmpty)
      cerr << "The key '" << key << "' can not be used as name for the entry because "
                   "it does not result in a unique name. You need to specify a unique name for this entry." << endl;
    else
      cerr << "The name '" << name << "' is not unique." << endl;
    return 0;
  }
  allNames.append(name);

  if (!defaultCode)
  {
    preProcessDefault(defaultValue, name, type, choices, code, cfg);
  }

  CfgEntry *result = new CfgEntry( group, type, key, name, labelContext, label, toolTipContext, toolTip, whatsThisContext, whatsThis,
                                   code, defaultValue, choices, signalList,
                                   hidden == "true" );
  if (!param.isEmpty())
  {
    result->setParam(param);
    result->setParamName(paramName);
    result->setParamType(paramType);
    result->setParamValues(paramValues);
    result->setParamDefaultValues(paramDefaultValues);
    result->setParamMax(paramMax);
  }
  result->setMinValue(minValue);
  result->setMaxValue(maxValue);

  return result;
}

static bool isUnsigned(const QString& type)
{
    if ( type == "UInt" )        return true;
    if ( type == "ULongLong" )   return true;
    return false;
}

/**
  Return parameter declaration for given type.
*/
QString param( const QString &t )
{
    const QString type = t.toLower();
    if ( type == "string" )           return "const QString &";
    else if ( type == "stringlist" )  return "const QStringList &";
    else if ( type == "font" )        return "const QFont &";
    else if ( type == "rect" )        return "const QRect &";
    else if ( type == "size" )        return "const QSize &";
    else if ( type == "color" )       return "const QColor &";
    else if ( type == "point" )       return "const QPoint &";
    else if ( type == "int" )         return "int";
    else if ( type == "uint" )        return "uint";
    else if ( type == "bool" )        return "bool";
    else if ( type == "double" )      return "double";
    else if ( type == "datetime" )    return "const QDateTime &";
    else if ( type == "longlong" )    return "qint64";
    else if ( type == "ulonglong" )   return "quint64";
    else if ( type == "intlist" )     return "const QList<int> &";
    else if ( type == "enum" )        return "int";
    else if ( type == "path" )        return "const QString &";
    else if ( type == "pathlist" )    return "const QStringList &";
    else if ( type == "password" )    return "const QString &";
    else if ( type == "url" )         return "const QUrl &";
    else if ( type == "urllist" )     return "const QList<QUrl> &";
    else {
        cerr <<"kconfig_compiler does not support type \""<< type <<"\""<<endl;
        return "QString"; //For now, but an assert would be better
    }
}

/**
  Actual C++ storage type for given type.
*/
QString cppType( const QString &t )
{
    const QString type = t.toLower();
    if ( type == "string" )           return "QString";
    else if ( type == "stringlist" )  return "QStringList";
    else if ( type == "font" )        return "QFont";
    else if ( type == "rect" )        return "QRect";
    else if ( type == "size" )        return "QSize";
    else if ( type == "color" )       return "QColor";
    else if ( type == "point" )       return "QPoint";
    else if ( type == "int" )         return "int";
    else if ( type == "uint" )        return "uint";
    else if ( type == "bool" )        return "bool";
    else if ( type == "double" )      return "double";
    else if ( type == "datetime" )    return "QDateTime";
    else if ( type == "longlong" )    return "qint64";
    else if ( type == "ulonglong" )   return "quint64";
    else if ( type == "intlist" )     return "QList<int>";
    else if ( type == "enum" )        return "int";
    else if ( type == "path" )        return "QString";
    else if ( type == "pathlist" )    return "QStringList";
    else if ( type == "password" )    return "QString";
    else if ( type == "url" )         return "QUrl";
    else if ( type == "urllist" )     return "QList<QUrl>";
    else {
        cerr<<"kconfig_compiler does not support type \""<< type <<"\""<<endl;
        return "QString"; //For now, but an assert would be better
    }
}

QString defaultValue( const QString &t )
{
    const QString type = t.toLower();
    if ( type == "string" )           return "\"\""; // Use empty string, not null string!
    else if ( type == "stringlist" )  return "QStringList()";
    else if ( type == "font" )        return "QFont()";
    else if ( type == "rect" )        return "QRect()";
    else if ( type == "size" )        return "QSize()";
    else if ( type == "color" )       return "QColor(128, 128, 128)";
    else if ( type == "point" )       return "QPoint()";
    else if ( type == "int" )         return "0";
    else if ( type == "uint" )        return "0";
    else if ( type == "bool" )        return "false";
    else if ( type == "double" )      return "0.0";
    else if ( type == "datedime" )    return "QDateTime()";
    else if ( type == "longlong" )    return "0";
    else if ( type == "ulonglong" )   return "0";
    else if ( type == "intlist" )     return "QList<int>()";
    else if ( type == "enum" )        return "0";
    else if ( type == "path" )        return "\"\""; // Use empty string, not null string!
    else if ( type == "pathlist" )    return "QStringList()";
    else if ( type == "password" )    return "\"\""; // Use empty string, not null string!
    else if ( type == "url" )         return "QUrl()";
    else if ( type == "urllist" )     return "QList<QUrl>()";
    else {
        cerr<<"Error, kconfig_compiler does not support the \""<< type <<"\" type!"<<endl;
        return "QString"; //For now, but an assert would be better
    }
}

QString itemType( const QString &type )
{
  QString t;

  t = type;
  t.replace( 0, 1, t.left( 1 ).toUpper() );

  return t;
}

static QString itemDeclaration(const CfgEntry *e, const CfgConfig &cfg)
{
  if (cfg.itemAccessors)
     return QString();

  QString fCap = e->name();
  fCap[0] = fCap[0].toUpper();
  return "  "+cfg.inherits+"::Item"+itemType( e->type() ) +
         "  *item" + fCap +
         ( (!e->param().isEmpty())?(QString("[%1]").arg(e->paramMax()+1)) : QString()) +
         ";\n";
}

// returns the name of an item variable
// use itemPath to know the full path
// like using d-> in case of dpointer
static QString itemVar(const CfgEntry *e, const CfgConfig &cfg)
{
  QString result;
  if (cfg.itemAccessors)
  {
    if ( !cfg.dpointer )
    {
      result = 'm' + e->name() + "Item";
      result[1] = result[1].toUpper();
    }
    else
    {
      result = e->name() + "Item";
      result[0] = result[0].toLower();
    }
  }
  else
  {
    result = "item" + e->name();
    result[4] = result[4].toUpper();
  }
  return result;
}

static QString itemPath(const CfgEntry *e, const CfgConfig &cfg)
{
  QString result;
  if ( cfg.dpointer ) {
    result = "d->"+itemVar(e, cfg);
  }
  else {
    result = itemVar(e, cfg);
  }
  return result;
}

QString newItem( const QString &type, const QString &name, const QString &key,
                 const QString &defaultValue, const CfgConfig &cfg, const QString &param = QString())
{
  QString t = "new "+cfg.inherits+"::Item" + itemType( type ) +
              "( currentGroup(), " + key + ", " + varPath( name, cfg ) + param;
  if ( type == "Enum" ) t += ", values" + name;
  if ( !defaultValue.isEmpty() ) {
    t += ", ";
    if ( type == "String" ) t += defaultValue;
    else t+= defaultValue;
  }
  t += " );";

  return t;
}

QString paramString(const QString &s, const CfgEntry *e, int i)
{
  QString result = s;
  QString needle = "$("+e->param()+')';
  if (result.contains(needle))
  {
    QString tmp;
    if (e->paramType() == "Enum")
    {
      tmp = e->paramValues()[i];
    }
    else
    {
      tmp = QString::number(i);
    }

    result.replace(needle, tmp);
  }
  return result;
}

QString paramString(const QString &group, const QList<Param> &parameters)
{
  QString paramString = group;
  QString arguments;
  int i = 1;
  for (QList<Param>::ConstIterator it = parameters.constBegin();
       it != parameters.constEnd(); ++it)
  {
     if (paramString.contains("$("+(*it).name+')'))
     {
       QString tmp;
       tmp.sprintf("%%%d", i++);
       paramString.replace("$("+(*it).name+')', tmp);
       arguments += ".arg( mParam"+(*it).name+" )";
     }
  }
  if (arguments.isEmpty())
    return "QLatin1String( \""+group+"\" )";

  return "QString( QLatin1String( \""+paramString+"\" ) )"+arguments;
}

QString translatedString(const CfgConfig &cfg, const QString &string, const QString &context = QString(), const QString &param = QString(), const QString &paramValue = QString())
{
    QString result;

    switch (cfg.translationSystem) {
    case CfgConfig::QtTranslation:
        if (!context.isEmpty()) {
            result+= "/*: " + context + " */ QObject::tr(";
        } else {
            result+= "QObject::tr(";
        }
        break;

    case CfgConfig::KdeTranslation:
        if (!context.isEmpty()) {
            result+= "i18nc(" + quoteString(context) + ", ";
        } else {
            result+= "i18n(";
        }
        break;
    }

    if (!param.isEmpty()) {
        QString resolvedString = string;
        resolvedString.replace("$("+param+')', paramValue);
        result+= quoteString(resolvedString);
    } else {
        result+= quoteString(string);
    }

    result+= ')';

    return result;
}

/* int i is the value of the parameter */
QString userTextsFunctions( CfgEntry *e, const CfgConfig &cfg, QString itemVarStr=QString(), QString i=QString() )
{
  QString txt;
  if (itemVarStr.isNull()) itemVarStr=itemPath(e, cfg);
  if ( !e->label().isEmpty() ) {
    txt += "  " + itemVarStr + "->setLabel( ";
    txt += translatedString(cfg, e->label(), e->labelContext(), e->param(), i);
    txt += " );\n";
  }
  if ( !e->toolTip().isEmpty() ) {
    txt += "  " + itemVarStr + "->setToolTip( ";
    txt += translatedString(cfg, e->toolTip(), e->toolTipContext(), e->param(), i);
    txt += " );\n";
  }
  if ( !e->whatsThis().isEmpty() ) {
    txt += "  " + itemVarStr + "->setWhatsThis( ";
    txt += translatedString(cfg, e->whatsThis(), e->whatsThisContext(), e->param(), i);
    txt += " );\n";
  }
  return txt;
}

// returns the member accesor implementation
// which should go in the h file if inline
// or the cpp file if not inline
QString memberAccessorBody( CfgEntry *e, bool globalEnums, const CfgConfig &cfg )
{
    QString result;
    QTextStream out(&result, QIODevice::WriteOnly);
    QString n = e->name();
    QString t = e->type();
    bool useEnumType = cfg.useEnumTypes && t == "Enum";

    out << "return ";
    if (useEnumType)
      out << "static_cast<" << enumType(e, globalEnums) << ">(";
    out << This << varPath(n, cfg);
    if (!e->param().isEmpty())
      out << "[i]";
    if (useEnumType)
      out << ")";
    out << ";" << endl;

    return result;
}

// returns the member mutator implementation
// which should go in the h file if inline
// or the cpp file if not inline
QString memberMutatorBody( CfgEntry *e, const CfgConfig &cfg )
{
  QString result;
  QTextStream out(&result, QIODevice::WriteOnly);
  QString n = e->name();
  QString t = e->type();

  if (!e->minValue().isEmpty())
  {
    if (e->minValue() != "0" || !isUnsigned(t)) { // skip writing "if uint<0" (#187579)
      out << "if (v < " << e->minValue() << ")" << endl;
      out << "{" << endl;
      out << "  qDebug() << \"" << setFunction(n);
      out << ": value \" << v << \" is less than the minimum value of ";
      out << e->minValue()<< "\";" << endl;
      out << "  v = " << e->minValue() << ";" << endl;
      out << "}" << endl;
    }
  }

  if (!e->maxValue().isEmpty())
  {
    out << endl << "if (v > " << e->maxValue() << ")" << endl;
    out << "{" << endl;
    out << "  qDebug() << \"" << setFunction(n);
    out << ": value \" << v << \" is greater than the maximum value of ";
    out << e->maxValue()<< "\";" << endl;
    out << "  v = " << e->maxValue() << ";" << endl;
    out << "}" << endl << endl;
  }

  out << "if (!" << This << "isImmutable( QString::fromLatin1( \"";
  if (!e->param().isEmpty())
  {
    out << e->paramName().replace("$("+e->param()+")", "%1") << "\" ).arg( ";
    if ( e->paramType() == "Enum" ) {
      out << "QLatin1String( ";

      if (cfg.globalEnums)
        out << enumName(e->param()) << "ToString[i]";
      else
        out << enumName(e->param()) << "::enumToString[i]";

        out << " )";
    }
    else
    {
      out << "i";
    }
    out << " )";
  }
  else
  {
    out << n << "\" )";
  }
  out << " ))" << (!e->signalList().empty() ? " {" : "") << endl;
  out << "  " << This << varPath(n, cfg);
  if (!e->param().isEmpty())
    out << "[i]";
  out << " = v;" << endl;

  if ( !e->signalList().empty() ) {
    Q_FOREACH(const Signal &signal, e->signalList()) {
      out << "  " << This << varPath("settingsChanged", cfg) << " |= " << signalEnumName(signal.name) << ";" << endl;
    }
    out << "}" << endl;
  }

  return result;
}

// returns the member get default implementation
// which should go in the h file if inline
// or the cpp file if not inline
QString memberGetDefaultBody( CfgEntry *e )
{
  QString result = e->code();
  QTextStream out(&result, QIODevice::WriteOnly);
  out << endl;

  if (!e->param().isEmpty()) {
    out << "  switch (i) {" << endl;
    for (int i = 0; i <= e->paramMax(); ++i) {
      if (!e->paramDefaultValue(i).isEmpty()) {
        out << "  case " << i << ": return " << e->paramDefaultValue(i) << ';' << endl;
      }
    }
    out << "  default:" << endl;
    out << "    return " << e->defaultValue().replace("$("+e->param()+')', "i") << ';' << endl;
    out << "  }" << endl;
  } else {
    out << "  return " << e->defaultValue() << ';';
  }

  return result;
}

// returns the item accesor implementation
// which should go in the h file if inline
// or the cpp file if not inline
QString itemAccessorBody( CfgEntry *e, const CfgConfig &cfg )
{
    QString result;
    QTextStream out(&result, QIODevice::WriteOnly);

    out << "return " << itemPath(e, cfg);
    if (!e->param().isEmpty()) out << "[i]";
    out << ";" << endl;

    return result;
}

//indents text adding X spaces per line
QString indent(QString text, int spaces)
{
    QString result;
    QTextStream out(&result, QIODevice::WriteOnly);
    QTextStream in(&text, QIODevice::ReadOnly);
    QString currLine;
    while ( !in.atEnd() )
    {
      currLine = in.readLine();
      if (!currLine.isEmpty())
        for (int i=0; i < spaces; i++)
          out << " ";
      out << currLine << endl;
    }
    return result;
}

// adds as many 'namespace foo {' lines to p_out as
// there are namespaces in p_ns
void beginNamespaces(const QString &p_ns, QTextStream &p_out)
{
  if ( !p_ns.isEmpty() ) {
    const QStringList nameSpaces = p_ns.split( "::" );
    foreach (const QString &ns, nameSpaces )
      p_out << "namespace " << ns << " {" << endl;
    p_out << endl;
  }
}

// adds as many '}' lines to p_out as
// there are namespaces in p_ns
void endNamespaces(const QString &p_ns, QTextStream &p_out)
{
  if ( !p_ns.isEmpty() ) {
    const int namespaceCount = p_ns.count( "::" ) + 1;
    for ( int i = 0; i < namespaceCount; ++i )
      p_out << "}" << endl;
    p_out << endl;
  }
}


int main( int argc, char **argv )
{
  QCoreApplication app(argc, argv);

  validNameRegexp = new QRegExp("[a-zA-Z_][a-zA-Z0-9_]*");

  QString directoryName, inputFilename, codegenFilename;
  parseArgs(app.arguments(), directoryName, inputFilename, codegenFilename);

  QString baseDir = directoryName;
#ifdef Q_OS_WIN
  if (!baseDir.endsWith('/') && !baseDir.endsWith('\\'))
#else
  if (!baseDir.endsWith('/'))
#endif
    baseDir.append("/");

  if (!codegenFilename.endsWith(QLatin1String(".kcfgc")))
  {
    cerr << "Codegen options file must have extension .kcfgc" << endl;
    return 1;
  }
  QString baseName = QFileInfo(codegenFilename).fileName();
  baseName = baseName.left(baseName.length() - 6);

  CfgConfig cfg = CfgConfig( codegenFilename );

  QFile input( inputFilename );

  QDomDocument doc;
  QString errorMsg;
  int errorRow;
  int errorCol;
  if ( !doc.setContent( &input, &errorMsg, &errorRow, &errorCol ) ) {
    cerr << "Unable to load document." << endl;
    cerr << "Parse error in " << inputFilename << ", line " << errorRow << ", col " << errorCol << ": " << errorMsg << endl;
    return 1;
  }

  QDomElement cfgElement = doc.documentElement();

  if ( cfgElement.isNull() ) {
    cerr << "No document in kcfg file" << endl;
    return 1;
  }

  QString cfgFileName;
  bool cfgFileNameArg = false;
  QList<Param> parameters;
  QList<Signal> signalList;
  QStringList includes;
  bool hasSignals = false;

  QList<CfgEntry*> entries;

  for ( QDomElement e = cfgElement.firstChildElement(); !e.isNull(); e = e.nextSiblingElement() ) {
    QString tag = e.tagName();

    if ( tag == "include" ) {
      QString includeFile = e.text();
      if (!includeFile.isEmpty())
        includes.append(includeFile);

    } else if ( tag == "kcfgfile" ) {
      cfgFileName = e.attribute( "name" );
      cfgFileNameArg = e.attribute( "arg" ).toLower() == "true";
      for( QDomElement e2 = e.firstChildElement(); !e2.isNull(); e2 = e2.nextSiblingElement() ) {
        if ( e2.tagName() == "parameter" ) {
          Param p;
          p.name = e2.attribute( "name" );
          p.type = e2.attribute( "type" );
          if (p.type.isEmpty())
             p.type = "String";
          parameters.append( p );
        }
      }

    } else if ( tag == "group" ) {
      QString group = e.attribute( "name" );
      if ( group.isEmpty() ) {
        cerr << "Group without name" << endl;
        return 1;
      }
      for( QDomElement e2 = e.firstChildElement(); !e2.isNull(); e2 = e2.nextSiblingElement() ) {
        if ( e2.tagName() != "entry" ) continue;
        CfgEntry *entry = parseEntry( group, e2, cfg );
        if ( entry ) entries.append( entry );
        else {
          cerr << "Can not parse entry." << endl;
          return 1;
        }
      }
    }
    else if ( tag == "signal" ) {
      QString signalName = e.attribute( "name" );
      if ( signalName.isEmpty() ) {
        cerr << "Signal without name." << endl;
        return 1;
      }
      Signal theSignal;
      theSignal.name = signalName;

      for( QDomElement e2 = e.firstChildElement(); !e2.isNull(); e2 = e2.nextSiblingElement() ) {
        if ( e2.tagName() == "argument") {
          SignalArguments argument;
          argument.type = e2.attribute("type");
          if ( argument.type.isEmpty() ) {
            cerr << "Signal argument without type." << endl;
            return 1;
          }
          argument.variableName = e2.text();
          theSignal.arguments.append(argument);
        }
        else if( e2.tagName() == "label") {
          theSignal.label = e2.text();
        }
      }
      signalList.append(theSignal);
    }
  }

  if ( cfg.className.isEmpty() ) {
    cerr << "Class name missing" << endl;
    return 1;
  }

  if ( cfg.singleton && !parameters.isEmpty() ) {
    cerr << "Singleton class can not have parameters" << endl;
    return 1;
  }

  if ( !cfgFileName.isEmpty() && cfgFileNameArg)
  {
    cerr << "Having both a fixed filename and a filename as argument is not possible." << endl;
    return 1;
  }

  if ( entries.isEmpty() ) {
    cerr << "No entries." << endl;
  }

#if 0
  CfgEntry *cfg;
  for( cfg = entries.first(); cfg; cfg = entries.next() ) {
    cfg->dump();
  }
#endif

  hasSignals = !signalList.empty();
  QString headerFileName = baseName + ".h";
  QString implementationFileName = baseName + ".cpp";
  QString mocFileName = baseName + ".moc";
  QString cppPreamble; // code to be inserted at the beginnin of the cpp file, e.g. initialization of static values

  QFile header( baseDir + headerFileName );
  if ( !header.open( QIODevice::WriteOnly ) ) {
    cerr << "Can not open '" << baseDir  << headerFileName << "for writing." << endl;
    return 1;
  }

  QTextStream h( &header );

  h << "// This file is generated by kconfig_compiler from " << QFileInfo(inputFilename).fileName() << "." << endl;
  h << "// All changes you do to this file will be lost." << endl;

  h << "#ifndef " << ( !cfg.nameSpace.isEmpty() ? QString (QString(cfg.nameSpace).replace( "::", "_" ).toUpper() + '_') : "" )
    << cfg.className.toUpper() << "_H" << endl;
  h << "#define " << ( !cfg.nameSpace.isEmpty() ? QString (QString(cfg.nameSpace).replace( "::", "_" ).toUpper() + '_') : "" )
    << cfg.className.toUpper() << "_H" << endl << endl;

  // Includes
  QStringList::ConstIterator it;
  for( it = cfg.headerIncludes.constBegin(); it != cfg.headerIncludes.constEnd(); ++it ) {
    if ( (*it).startsWith('"') )
      h << "#include " << *it << endl;
    else
      h << "#include <" << *it << ">" << endl;
  }

  if ( cfg.headerIncludes.count() > 0 ) h << endl;

  if ( !cfg.singleton && parameters.isEmpty() )
    h << "#include <qglobal.h>" << endl;

  if ( cfg.inherits=="KCoreConfigSkeleton" ) {
    h << "#include <kcoreconfigskeleton.h>" << endl;
  } else {
    h << "#include <kconfigskeleton.h>" << endl;
  }

  h << "#include <QDebug>" << endl << endl;

  // Includes
  for( it = includes.constBegin(); it != includes.constEnd(); ++it ) {
    if ( (*it).startsWith('"') )
      h << "#include " << *it << endl;
    else
      h << "#include <" << *it << ">" << endl;
  }

  beginNamespaces(cfg.nameSpace, h);

  // Private class declaration
  if ( cfg.dpointer )
    h << "class " << cfg.className << "Private;" << endl << endl;

  // Class declaration header
  h << "class " << cfg.visibility << cfg.className << " : public " << cfg.inherits << endl;

  h << "{" << endl;
  // Add Q_OBJECT macro if the config need signals.
  if( hasSignals )
   h << "  Q_OBJECT" << endl;
  h << "  public:" << endl;

  // enums
  QList<CfgEntry*>::ConstIterator itEntry;
  for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
    const CfgEntry::Choices &choices = (*itEntry)->choices();
    const QList<CfgEntry::Choice> chlist = choices.choices;
    if ( !chlist.isEmpty() ) {
      QStringList values;
      QList<CfgEntry::Choice>::ConstIterator itChoice;
      for( itChoice = chlist.constBegin(); itChoice != chlist.constEnd(); ++itChoice ) {
        values.append( choices.prefix + (*itChoice).name );
      }
      if ( choices.name().isEmpty() ) {
        if ( cfg.globalEnums ) {
          h << "    enum " << enumName( (*itEntry)->name(), (*itEntry)->choices() ) << " { " << values.join( ", " ) << " };" << endl;
        } else {
          // Create an automatically named enum
          h << "    class " << enumName( (*itEntry)->name(), (*itEntry)->choices() ) << endl;
          h << "    {" << endl;
          h << "      public:" << endl;
          h << "      enum type { " << values.join( ", " ) << ", COUNT };" << endl;
          h << "    };" << endl;
        }
      } else if ( !choices.external() ) {
        // Create a named enum
        h << "    enum " << enumName( (*itEntry)->name(), (*itEntry)->choices() ) << " { " << values.join( ", " ) << " };" << endl;
      }
    }
    const QStringList values = (*itEntry)->paramValues();
    if ( !values.isEmpty() ) {
      if ( cfg.globalEnums ) {
        // ### FIXME!!
        // make the following string table an index-based string search!
        // ###
        h << "    enum " << enumName( (*itEntry)->param() ) << " { " << values.join( ", " ) << " };" << endl;
        h << "    static const char* const " << enumName( (*itEntry)->param() ) << "ToString[];" << endl;
        cppPreamble += "const char* const " + cfg.className + "::" + enumName( (*itEntry)->param() ) +
           "ToString[] = { \"" + values.join( "\", \"" ) + "\" };\n";
      } else {
        h << "    class " << enumName( (*itEntry)->param() ) << endl;
        h << "    {" << endl;
        h << "      public:" << endl;
        h << "      enum type { " << values.join( ", " ) << ", COUNT };" << endl;
        h << "      static const char* const enumToString[];" << endl;
        h << "    };" << endl;
        cppPreamble += "const char* const " + cfg.className + "::" + enumName( (*itEntry)->param() ) +
           "::enumToString[] = { \"" + values.join( "\", \"" ) + "\" };\n";
      }
    }
  }
  if ( hasSignals ) {
   h << "\n    enum {" << endl;
   unsigned val = 1;
   QList<Signal>::ConstIterator it, itEnd = signalList.constEnd();
   for ( it = signalList.constBegin(); it != itEnd; val <<= 1) {
     if ( !val ) {
       cerr << "Too many signals to create unique bit masks" << endl;
       exit(1);
     }
     Signal signal = *it;
     h << "      " << signalEnumName(signal.name) << " = 0x" << hex << val;
     if ( ++it != itEnd )
      h << ",";
     h << endl;
   }
   h << " };" << dec << endl;
  }
  h << endl;
  // Constructor or singleton accessor
  if ( !cfg.singleton ) {
    h << "    " << cfg.className << "(";
    if (cfgFileNameArg)
    {
        if(cfg.forceStringFilename)
            h << " const QString &cfgfilename"
                << (parameters.isEmpty() ? " = QString()" : ", ");
        else
            h << " KSharedConfig::Ptr config"
                << (parameters.isEmpty() ? " = KSharedConfig::openConfig()" : ", ");
    }
    for (QList<Param>::ConstIterator it = parameters.constBegin();
         it != parameters.constEnd(); ++it)
    {
       if (it != parameters.constBegin())
         h << ",";
       h << " " << param((*it).type) << " " << (*it).name;
    }
    h << " );" << endl;
  } else {
    h << "    static " << cfg.className << " *self();" << endl;
    if (cfgFileNameArg)
    {
      h << "    static void instance(const QString& cfgfilename);" << endl;
    }
  }

  // Destructor
  h << "    ~" << cfg.className << "();" << endl << endl;

  // global variables
  if (cfg.staticAccessors)
    This = "self()->";
  else
    Const = " const";

  for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
    QString n = (*itEntry)->name();
    QString t = (*itEntry)->type();

    // Manipulator
    if (cfg.allMutators || cfg.mutators.contains(n))
    {
      h << "    /**" << endl;
      h << "      Set " << (*itEntry)->label() << endl;
      h << "    */" << endl;
      if (cfg.staticAccessors)
        h << "    static" << endl;
      h << "    void " << setFunction(n) << "( ";
      if (!(*itEntry)->param().isEmpty())
        h << cppType((*itEntry)->paramType()) << " i, ";
      if (cfg.useEnumTypes && t == "Enum")
        h << enumType(*itEntry, cfg.globalEnums);
      else
        h << param( t );
      h << " v )";
      // function body inline only if not using dpointer
      // for BC mode
      if ( !cfg.dpointer )
      {
        h << endl << "    {" << endl;
        h << indent(memberMutatorBody(*itEntry, cfg), 6 );
        h << "    }" << endl;
      }
      else
      {
        h << ";" << endl;
      }
    }
    h << endl;
    // Accessor
    h << "    /**" << endl;
    h << "      Get " << (*itEntry)->label() << endl;
    h << "    */" << endl;
    if (cfg.staticAccessors)
      h << "    static" << endl;
    h << "    ";
    if (cfg.useEnumTypes && t == "Enum")
      h << enumType(*itEntry, cfg.globalEnums);
    else
      h << cppType(t);
    h << " " << getFunction(n) << "(";
    if (!(*itEntry)->param().isEmpty())
      h << " " << cppType((*itEntry)->paramType()) <<" i ";
    h << ")" << Const;
    // function body inline only if not using dpointer
    // for BC mode
    if ( !cfg.dpointer )
    {
       h << endl << "    {" << endl;
      h << indent(memberAccessorBody(*itEntry, cfg.globalEnums, cfg), 6 );
       h << "    }" << endl;
    }
    else
    {
      h << ";" << endl;
    }

    // Default value Accessor
    if ((cfg.allDefaultGetters || cfg.defaultGetters.contains(n)) && !(*itEntry)->defaultValue().isEmpty()) {
      h << endl;
      h << "    /**" << endl;
      h << "      Get " << (*itEntry)->label() << " default value" << endl;
      h << "    */" << endl;
      if (cfg.staticAccessors)
        h << "    static" << endl;
      h << "    ";
      if (cfg.useEnumTypes && t == "Enum")
        h << enumType(*itEntry, cfg.globalEnums);
      else
        h << cppType(t);
      h << " " << getDefaultFunction(n) << "(";
      if ( !(*itEntry)->param().isEmpty() )
          h << " " << cppType( (*itEntry)->paramType() ) <<" i ";
      h << ")" << Const << endl;
      h << "    {" << endl;
      h << "        return ";
      if (cfg.useEnumTypes && t == "Enum")
        h << "static_cast<" << enumType(*itEntry, cfg.globalEnums) << ">(";
      h << getDefaultFunction(n) << "_helper(";
      if ( !(*itEntry)->param().isEmpty() )
          h << " i ";
      h << ")";
      if (cfg.useEnumTypes && t == "Enum")
        h << ")";
      h << ";" << endl;
      h << "    }" << endl;
    }

    // Item accessor
    if ( cfg.itemAccessors ) {
      h << endl;
      h << "    /**" << endl;
      h << "      Get Item object corresponding to " << n << "()"
        << endl;
      h << "    */" << endl;
      h << "    Item" << itemType( (*itEntry)->type() ) << " *"
        << getFunction( n ) << "Item(";
      if (!(*itEntry)->param().isEmpty()) {
        h << " " << cppType((*itEntry)->paramType()) << " i ";
      }
      h << ")";
      if ( !cfg.dpointer )
      {
        h << endl << "    {" << endl;
        h << indent( itemAccessorBody((*itEntry), cfg), 6);
        h << "    }" << endl;
      }
      else
      {
        h << ";" << endl;
      }
    }

    h << endl;
  }


  // Signal definition.
  if( hasSignals ) {
    h << endl;
    h << "  Q_SIGNALS:";
    Q_FOREACH(const Signal &signal, signalList) {
      h << endl;
      if ( !signal.label.isEmpty() ) {
        h << "    /**" << endl;
        h << "      " << signal.label << endl;
        h << "    */" << endl;
      }
      h << "    void " << signal.name << "(";
      QList<SignalArguments>::ConstIterator it, itEnd = signal.arguments.constEnd();
      for ( it = signal.arguments.constBegin(); it != itEnd; ) {
        SignalArguments argument = *it;
        QString type = param(argument.type);
        if ( cfg.useEnumTypes && argument.type == "Enum" ) {
          for ( int i = 0, end = entries.count(); i < end; ++i ) {
            if ( entries[i]->name() == argument.variableName ) {
              type = enumType(entries[i], cfg.globalEnums);
              break;
            }
          }
        }
        h << type << " " << argument.variableName;
        if ( ++it != itEnd ) {
         h << ", ";
        }
      }
      h << ");" << endl;
    }
    h << endl;
  }

  h << "  protected:" << endl;

  // Private constructor for singleton
  if ( cfg.singleton ) {
    h << "    " << cfg.className << "(";
    if ( cfgFileNameArg )
      h << "const QString& arg";
    h << ");" << endl;
    h << "    friend class " << cfg.className << "Helper;" << endl << endl;
  }

  if ( hasSignals ) {
    h << "    virtual bool usrWriteConfig();" << endl;
  }

  // Member variables
  if ( !cfg.memberVariables.isEmpty() && cfg.memberVariables != "private" && cfg.memberVariables != "dpointer") {
    h << "  " << cfg.memberVariables << ":" << endl;
  }

  // Class Parameters
  for (QList<Param>::ConstIterator it = parameters.constBegin();
       it != parameters.constEnd(); ++it)
  {
     h << "    " << cppType((*it).type) << " mParam" << (*it).name << ";" << endl;
  }

  if ( cfg.memberVariables != "dpointer" )
  {
    QString group;
    for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
      if ( (*itEntry)->group() != group ) {
        group = (*itEntry)->group();
        h << endl;
        h << "    // " << group << endl;
      }
      h << "    " << cppType( (*itEntry)->type() ) << " " << varName( (*itEntry)->name(), cfg );
      if ( !(*itEntry)->param().isEmpty() )
      {
        h << QString("[%1]").arg( (*itEntry)->paramMax()+1 );
      }
      h << ";" << endl;

      if ( cfg.allDefaultGetters || cfg.defaultGetters.contains((*itEntry)->name()) )
      {
        h << "    ";
        if (cfg.staticAccessors)
          h << "static ";
        h << cppType((*itEntry)->type()) << " " << getDefaultFunction((*itEntry)->name()) << "_helper(";
        if ( !(*itEntry)->param().isEmpty() )
            h << " " << cppType( (*itEntry)->paramType() ) <<" i ";
        h << ")" << Const << ";" << endl;
      }
    }

    h << endl << "  private:" << endl;
    if ( cfg.itemAccessors ) {
       for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
        h << "    Item" << itemType( (*itEntry)->type() ) << " *" << itemVar( *itEntry, cfg );
        if ( !(*itEntry)->param().isEmpty() ) h << QString("[%1]").arg( (*itEntry)->paramMax()+1 );
        h << ";" << endl;
      }
    }
    if ( hasSignals )
     h << "    uint " << varName("settingsChanged", cfg) << ";" << endl;

  }
  else
  {
    // use a private class for both member variables and items
    h << "  private:" << endl;
    for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
      if ( cfg.allDefaultGetters || cfg.defaultGetters.contains((*itEntry)->name()) ) {
        h << "    ";
        if (cfg.staticAccessors)
          h << "static ";
        h << cppType((*itEntry)->type()) << " " << getDefaultFunction((*itEntry)->name()) << "_helper(";
        if ( !(*itEntry)->param().isEmpty() )
            h << " " << cppType( (*itEntry)->paramType() ) <<" i ";
        h << ")" << Const << ";" << endl;
      }
    }
    h << "    " + cfg.className + "Private *d;" << endl;
  }

  if (cfg.customAddons)
  {
     h << "    // Include custom additions" << endl;
     h << "    #include \"" << filenameOnly(baseName) << "_addons.h\"" <<endl;
  }

  h << "};" << endl << endl;

  endNamespaces(cfg.nameSpace, h);

  h << "#endif" << endl << endl;


  header.close();

  QFile implementation( baseDir + implementationFileName );
  if ( !implementation.open( QIODevice::WriteOnly ) ) {
    cerr << "Can not open '" << implementationFileName << "for writing."
              << endl;
    return 1;
  }

  QTextStream cpp( &implementation );


  cpp << "// This file is generated by kconfig_compiler from " << QFileInfo(inputFilename).fileName() << "." << endl;
  cpp << "// All changes you do to this file will be lost." << endl << endl;

  cpp << "#include \"" << headerFileName << "\"" << endl << endl;

  for( it = cfg.sourceIncludes.constBegin(); it != cfg.sourceIncludes.constEnd(); ++it ) {
    if ( (*it).startsWith('"') )
      cpp << "#include " << *it << endl;
    else
      cpp << "#include <" << *it << ">" << endl;
  }

  if ( cfg.sourceIncludes.count() > 0 ) cpp << endl;

  if ( cfg.setUserTexts && cfg.translationSystem==CfgConfig::KdeTranslation)
    cpp << "#include <klocalizedstring.h>" << endl << endl;

  // Header required by singleton implementation
  if ( cfg.singleton )
    cpp << "#include <qglobal.h>" << endl << "#include <QtCore/QFile>" << endl << endl;
  if ( cfg.singleton && cfgFileNameArg )
    cpp << "#include <QDebug>" << endl << endl;

  if ( !cfg.nameSpace.isEmpty() )
    cpp << "using namespace " << cfg.nameSpace << ";" << endl << endl;

  QString group;

  // private class implementation
  if ( cfg.dpointer )
  {
    beginNamespaces(cfg.nameSpace, cpp);
    cpp << "class " << cfg.className << "Private" << endl;
    cpp << "{" << endl;
    cpp << "  public:" << endl;
    for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
      if ( (*itEntry)->group() != group ) {
        group = (*itEntry)->group();
        cpp << endl;
        cpp << "    // " << group << endl;
      }
      cpp << "    " << cppType( (*itEntry)->type() ) << " " << varName( (*itEntry)->name(), cfg );
      if ( !(*itEntry)->param().isEmpty() )
      {
        cpp << QString("[%1]").arg( (*itEntry)->paramMax()+1 );
      }
      cpp << ";" << endl;
    }
    cpp << endl << "    // items" << endl;
    for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
      cpp << "    "+cfg.inherits+"::Item" << itemType( (*itEntry)->type() ) << " *" << itemVar( *itEntry, cfg );
      if ( !(*itEntry)->param().isEmpty() ) cpp << QString("[%1]").arg( (*itEntry)->paramMax()+1 );
        cpp << ";" << endl;
    }
    if ( hasSignals ) {
      cpp << "    uint " << varName("settingsChanged", cfg) << ";" << endl;
    }

    cpp << "};" << endl << endl;
    endNamespaces(cfg.nameSpace, cpp);
  }

  // Singleton implementation
  if ( cfg.singleton ) {
    beginNamespaces(cfg.nameSpace, cpp);
    cpp << "class " << cfg.className << "Helper" << endl;
    cpp << '{' << endl;
    cpp << "  public:" << endl;
    cpp << "    " << cfg.className << "Helper() : q(0) {}" << endl;
    cpp << "    ~" << cfg.className << "Helper() { delete q; }" << endl;
    cpp << "    " << cfg.className << " *q;" << endl;
    cpp << "};" << endl;
    endNamespaces(cfg.nameSpace, cpp);
    cpp << "Q_GLOBAL_STATIC(" << cfg.className << "Helper, s_global" << cfg.className << ")" << endl;

    cpp << cfg.className << " *" << cfg.className << "::self()" << endl;
    cpp << "{" << endl;
    if ( cfgFileNameArg ) {
      cpp << "  if (!s_global" << cfg.className << "()->q)" << endl;
      cpp << "     qFatal(\"you need to call " << cfg.className << "::instance before using\");" << endl;
    } else {
      cpp << "  if (!s_global" << cfg.className << "()->q) {" << endl;
      cpp << "    new " << cfg.className << ';' << endl;
      cpp << "    s_global" << cfg.className << "()->q->readConfig();" << endl;
      cpp << "  }" << endl << endl;
    }
    cpp << "  return s_global" << cfg.className << "()->q;" << endl;
    cpp << "}" << endl << endl;

    if ( cfgFileNameArg ) {
      cpp << "void " << cfg.className << "::instance(const QString& cfgfilename)" << endl;
      cpp << "{" << endl;
      cpp << "  if (s_global" << cfg.className << "()->q) {" << endl;
      cpp << "     qDebug() << \"" << cfg.className << "::instance called after the first use - ignoring\";" << endl;
      cpp << "     return;" << endl;
      cpp << "  }" << endl;
      cpp << "  new " << cfg.className << "(cfgfilename);" << endl;
      cpp << "  s_global" << cfg.className << "()->q->readConfig();" << endl;
      cpp << "}" << endl << endl;
    }
  }

  if ( !cppPreamble.isEmpty() )
    cpp << cppPreamble << endl;

  // Constructor
  cpp << cfg.className << "::" << cfg.className << "( ";
  if ( cfgFileNameArg ) {
    if ( !cfg.singleton && ! cfg.forceStringFilename)
      cpp << " KSharedConfig::Ptr config";
    else
      cpp << " const QString& config";
    cpp << (parameters.isEmpty() ? " " : ", ");
  }

  for (QList<Param>::ConstIterator it = parameters.constBegin();
       it != parameters.constEnd(); ++it)
  {
     if (it != parameters.constBegin())
       cpp << ",";
     cpp << " " << param((*it).type) << " " << (*it).name;
  }
  cpp << " )" << endl;

  cpp << "  : " << cfg.inherits << "(";
  if ( !cfgFileName.isEmpty() ) cpp << " QLatin1String( \"" << cfgFileName << "\" ";
  if ( cfgFileNameArg ) cpp << " config ";
  if ( !cfgFileName.isEmpty() ) cpp << ") ";
  cpp << ")" << endl;

  // Store parameters
  for (QList<Param>::ConstIterator it = parameters.constBegin();
       it != parameters.constEnd(); ++it)
  {
     cpp << "  , mParam" << (*it).name << "(" << (*it).name << ")" << endl;
  }

  if ( hasSignals && !cfg.dpointer )
    cpp << "  , " << varName("settingsChanged", cfg) << "(0)" << endl;

  cpp << "{" << endl;

  if (cfg.dpointer)
  {
    cpp << "  d = new " + cfg.className + "Private;" << endl;
    if (hasSignals)
      cpp << "  " << varPath("settingsChanged", cfg) << " = 0;" << endl;
  }
  // Needed in case the singleton class is used as baseclass for
  // another singleton.
  if (cfg.singleton) {
    cpp << "  Q_ASSERT(!s_global" << cfg.className << "()->q);" << endl;
    cpp << "  s_global" << cfg.className << "()->q = this;" << endl;
  }

  group.clear();

  for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
    if ( (*itEntry)->group() != group ) {
      if ( !group.isEmpty() ) cpp << endl;
      group = (*itEntry)->group();
      cpp << "  setCurrentGroup( " << paramString(group, parameters) << " );" << endl << endl;
    }

    QString key = paramString( (*itEntry)->key(), parameters );
    if ( !(*itEntry)->code().isEmpty() ) {
      cpp << (*itEntry)->code() << endl;
    }
    if ( (*itEntry)->type() == "Enum" ) {
      cpp << "  QList<"+cfg.inherits+"::ItemEnum::Choice> values"
          << (*itEntry)->name() << ";" << endl;
      const QList<CfgEntry::Choice> choices = (*itEntry)->choices().choices;
      QList<CfgEntry::Choice>::ConstIterator it;
      for( it = choices.constBegin(); it != choices.constEnd(); ++it ) {
        cpp << "  {" << endl;
        cpp << "    "+cfg.inherits+"::ItemEnum::Choice choice;" << endl;
        cpp << "    choice.name = QLatin1String(\"" << (*it).name << "\");" << endl;
        if ( cfg.setUserTexts ) {
          if ( !(*it).label.isEmpty() ) {
            cpp << "    choice.label = "
                << translatedString(cfg, (*it).label, (*it).context)
                << ";" << endl;
          }
          if ( !(*it).toolTip.isEmpty() ) {
            cpp << "    choice.toolTip = "
                << translatedString(cfg, (*it).toolTip, (*it).context)
                << ";" << endl;
          }
          if ( !(*it).whatsThis.isEmpty() ) {
            cpp << "    choice.whatsThis = "
                << translatedString(cfg, (*it).whatsThis, (*it).context)
                << ";" << endl;
          }
        }
        cpp << "    values" << (*itEntry)->name() << ".append( choice );" << endl;
        cpp << "  }" << endl;
      }
    }

    if (!cfg.dpointer)
      cpp << itemDeclaration( *itEntry, cfg );

    if ( (*itEntry)->param().isEmpty() )
    {
      // Normal case
      cpp << "  " << itemPath( *itEntry, cfg ) << " = "
          << newItem( (*itEntry)->type(), (*itEntry)->name(), key, (*itEntry)->defaultValue(), cfg ) << endl;

      if ( !(*itEntry)->minValue().isEmpty() )
        cpp << "  " << itemPath( *itEntry, cfg ) << "->setMinValue(" << (*itEntry)->minValue() << ");" << endl;
      if ( !(*itEntry)->maxValue().isEmpty() )
        cpp << "  " << itemPath( *itEntry, cfg ) << "->setMaxValue(" << (*itEntry)->maxValue() << ");" << endl;

      if ( cfg.setUserTexts )
        cpp << userTextsFunctions( (*itEntry), cfg );

      cpp << "  addItem( " << itemPath( *itEntry, cfg );
      QString quotedName = (*itEntry)->name();
      addQuotes( quotedName );
      if ( quotedName != key ) cpp << ", QLatin1String( \"" << (*itEntry)->name() << "\" )";
      cpp << " );" << endl;
    }
    else
    {
      // Indexed
      for(int i = 0; i <= (*itEntry)->paramMax(); i++)
      {
        QString defaultStr;
        QString itemVarStr(itemPath( *itEntry, cfg )+QString("[%1]").arg(i));

        if ( !(*itEntry)->paramDefaultValue(i).isEmpty() )
          defaultStr = (*itEntry)->paramDefaultValue(i);
        else if ( !(*itEntry)->defaultValue().isEmpty() )
          defaultStr = paramString( (*itEntry)->defaultValue(), (*itEntry), i );
        else
          defaultStr = defaultValue( (*itEntry)->type() );

        cpp << "  " << itemVarStr << " = "
            << newItem( (*itEntry)->type(), (*itEntry)->name(), paramString(key, *itEntry, i), defaultStr,cfg,  QString("[%1]").arg(i) )
            << endl;

        if ( cfg.setUserTexts )
          cpp << userTextsFunctions( *itEntry, cfg, itemVarStr, (*itEntry)->paramName() );

        // Make mutators for enum parameters work by adding them with $(..) replaced by the
        // param name. The check for isImmutable in the set* functions doesn't have the param
        // name available, just the corresponding enum value (int), so we need to store the
        // param names in a separate static list!.
        cpp << "  addItem( " << itemVarStr << ", QLatin1String( \"";
        if ( (*itEntry)->paramType()=="Enum" )
          cpp << (*itEntry)->paramName().replace( "$("+(*itEntry)->param()+')', "%1").arg((*itEntry)->paramValues()[i] );
        else
          cpp << (*itEntry)->paramName().replace( "$("+(*itEntry)->param()+')', "%1").arg(i);
        cpp << "\" ) );" << endl;
      }
    }
  }

  cpp << "}" << endl << endl;

  if (cfg.dpointer)
  {
    // setters and getters go in Cpp if in dpointer mode
    for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
      QString n = (*itEntry)->name();
      QString t = (*itEntry)->type();

      // Manipulator
      if (cfg.allMutators || cfg.mutators.contains(n))
      {
        cpp << "void " << setFunction(n, cfg.className) << "( ";
        if ( !(*itEntry)->param().isEmpty() )
          cpp << cppType( (*itEntry)->paramType() ) << " i, ";
        if (cfg.useEnumTypes && t == "Enum")
          cpp << enumType(*itEntry, cfg.globalEnums);
        else
          cpp << param( t );
        cpp << " v )" << endl;
        // function body inline only if not using dpointer
        // for BC mode
        cpp << "{" << endl;
        cpp << indent(memberMutatorBody( *itEntry, cfg ), 6);
        cpp << "}" << endl << endl;
      }

      // Accessor
      if (cfg.useEnumTypes && t == "Enum")
        cpp << enumType(*itEntry, cfg.globalEnums);
      else
        cpp << cppType(t);
      cpp << " " << getFunction(n, cfg.className) << "(";
      if ( !(*itEntry)->param().isEmpty() )
        cpp << " " << cppType( (*itEntry)->paramType() ) <<" i ";
      cpp << ")" << Const << endl;
      // function body inline only if not using dpointer
      // for BC mode
      cpp << "{" << endl;
      cpp << indent(memberAccessorBody( *itEntry, cfg.globalEnums, cfg ), 2);
      cpp << "}" << endl << endl;

      // Default value Accessor -- written by the loop below

      // Item accessor
      if ( cfg.itemAccessors )
      {
        cpp << endl;
        cpp << cfg.inherits+"::Item" << itemType( (*itEntry)->type() ) << " *"
          << getFunction( n, cfg.className ) << "Item(";
        if ( !(*itEntry)->param().isEmpty() ) {
          cpp << " " << cppType( (*itEntry)->paramType() ) << " i ";
        }
        cpp << ")" << endl;
        cpp << "{" << endl;
        cpp << indent(itemAccessorBody( *itEntry, cfg ), 2);
        cpp << "}" << endl;
      }

      cpp << endl;
    }
  }

  // default value getters always go in Cpp
  for( itEntry = entries.constBegin(); itEntry != entries.constEnd(); ++itEntry ) {
    QString n = (*itEntry)->name();
    QString t = (*itEntry)->type();

    // Default value Accessor, as "helper" function
    if (( cfg.allDefaultGetters || cfg.defaultGetters.contains(n) ) && !(*itEntry)->defaultValue().isEmpty() ) {
      cpp << cppType(t) << " " << getDefaultFunction(n, cfg.className) << "_helper(";
      if ( !(*itEntry)->param().isEmpty() )
        cpp << " " << cppType( (*itEntry)->paramType() ) <<" i ";
      cpp << ")" << Const << endl;
      cpp << "{" << endl;
      cpp << memberGetDefaultBody(*itEntry) << endl;
      cpp << "}" << endl << endl;
    }
  }

  // Destructor
  cpp << cfg.className << "::~" << cfg.className << "()" << endl;
  cpp << "{" << endl;
  if ( cfg.singleton ) {
    if ( cfg.dpointer )
      cpp << "  delete d;" << endl;
    cpp << "  s_global" << cfg.className << "()->q = 0;" << endl;
  }
  cpp << "}" << endl << endl;

  if ( hasSignals ) {
    cpp << "bool " << cfg.className << "::" << "usrWriteConfig()" << endl;
    cpp << "{" << endl;
    cpp << "  const bool res = " << cfg.inherits << "::usrWriteConfig();" << endl;
    cpp << "  if (!res) return false;" << endl << endl;
    Q_FOREACH(const Signal &signal, signalList) {
      cpp << "  if ( " << varPath("settingsChanged", cfg) << " & " << signalEnumName(signal.name) << " ) " << endl;
      cpp << "    emit " << signal.name << "(";
      QList<SignalArguments>::ConstIterator it, itEnd = signal.arguments.constEnd();
      for ( it = signal.arguments.constBegin(); it != itEnd; ) {
        SignalArguments argument = *it;
        bool cast = false;
        if ( cfg.useEnumTypes && argument.type == "Enum" ) {
          for ( int i = 0, end = entries.count(); i < end; ++i ) {
            if ( entries[i]->name() == argument.variableName ) {
              cpp << "static_cast<" << enumType(entries[i], cfg.globalEnums) << ">(";
              cast = true;
              break;
            }
          }
        }
        cpp << varPath(argument.variableName, cfg);
        if ( cast )
          cpp << ")";
        if ( ++it != itEnd )
          cpp << ", ";
      }
      cpp << ");" << endl << endl;
    }
    cpp << "  " << varPath("settingsChanged", cfg) << " = 0;" << endl;
    cpp << "  return true;" << endl;
    cpp << "}" << endl;
  }

  // Add includemoc if they are signals defined.
  if( hasSignals ) {
    cpp << endl;
    cpp << "#include \"" << mocFileName << "\"" << endl;
    cpp << endl;
  }

  // clear entries list
  qDeleteAll( entries );

  implementation.close();
}
