#ifndef KSCRIPT_EXT_QSTRUCTS_H
#define KSCRIPT_EXT_QSTRUCTS_H

#include "kscript_struct.h"

#include <qrect.h>
#include <qpoint.h>

namespace KSQt
{
    class Rect : public KSBuiltinStructClass
    {
    public:
	Rect( KSModule* module, const QString& name );

	bool constructor( KSContext& c );
	bool destructor( void* object );
	KSStruct* clone( KSBuiltinStruct* );

	static QRect convert( KSContext& context, const KSValue::Ptr& value );
	static KSValue::Ptr convert( KSContext& context, const QRect& rect );
	
    protected:
	KSValue::Ptr property( KSContext& context, void* object, const QString& name );
	bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

    class Point : public KSBuiltinStructClass
    {
    public:
        Point( KSModule* module, const QString& name );

        bool constructor( KSContext& c );
        bool destructor( void* object );
        KSStruct* clone( KSBuiltinStruct* );

        static QPoint convert( KSContext& context, const KSValue::Ptr& value );
        static KSValue::Ptr convert( KSContext& context, const QPoint& value );

    protected:
        KSValue::Ptr property( KSContext& context, void* object, const QString& name );
        bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

    class Size : public KSBuiltinStructClass
    {
    public:
        Size( KSModule* module, const QString& name );

        bool constructor( KSContext& c );
        bool destructor( void* object );
        KSStruct* clone( KSBuiltinStruct* );

        static QSize convert( KSContext& context, const KSValue::Ptr& value );
        static KSValue::Ptr convert( KSContext& context, const QSize& value );

    protected:
        KSValue::Ptr property( KSContext& context, void* object, const QString& name );
        bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

};

#endif
