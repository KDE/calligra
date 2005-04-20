/***************************************************************************
 * pythonscript.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "pythonscript.h"
#include "pythonmodule.h"
#include "pythoninterpreter.h"
//#include "../api/object.h"
//#include "../api/list.h"
//#include "../main/manager.h"
#include "../main/scriptcontainer.h"
//#include "../api/qtobject.h"
//#include "../api/interpreter.h"

#include <kdebug.h>

using namespace Kross::Python;

PythonScript::PythonScript(Kross::Api::Interpreter* interpreter, Kross::Api::ScriptContainer* scriptcontainer)
    : Kross::Api::Script(interpreter, scriptcontainer)
    , m_module(0)
{
}

PythonScript::~PythonScript()
{
    finalize();
}

void PythonScript::initialize()
{
    finalize();
    PyObject* pymod = PyModule_New((char*)m_scriptcontainer->getName().latin1());
    m_module = new Py::Module(pymod, true);
}

void PythonScript::finalize()
{
    delete m_module; m_module = 0;
    m_functions.clear();
    m_classes.clear();
}

const QStringList& PythonScript::getFunctionNames()
{
    if(! m_module) initialize();
    return m_functions;
    /*
    QStringList list;
    Py::List l = m_module->getDict().keys();
    int length = l.length();
    for(Py::List::size_type i = 0; i < length; ++i)
        list.append( l[i].str().as_string().c_str() );
    return list;
    */
}

Kross::Api::Object* PythonScript::execute()
{
    if(! m_module) initialize();

    try {
        Py::Dict mainmoduledict = ((PythonInterpreter*)m_interpreter)->m_mainmodule->getDict();

        PyObject* pyrun = PyRun_String(
            (char*)m_scriptcontainer->getCode().latin1(),
            Py_file_input,
            mainmoduledict.ptr(),
            m_module->getDict().ptr()
        );
        if(! pyrun) {
            Py::Object errobj = Py::value(Py::Exception()); // get last error
            throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
        }
        Py::Object run(pyrun, true); // the run-object takes care of freeing our pyrun pyobject.

        //kdDebug() << QString("PythonScript::execute --------------------------- 1") << endl;
        Py::Dict moduledict( m_module->getDict().ptr() );
        for(Py::Dict::iterator it = moduledict.begin(); it != moduledict.end(); ++it) {
            Py::Dict::value_type vt(*it);

            if(PyClass_Check( vt.second.ptr() )) {
                kdDebug() << QString("PythonScript::execute() class '%1' added.").arg(vt.first.as_string().c_str()) << endl;
                m_classes.append( vt.first.as_string().c_str() );
            }
            else if(vt.second.isCallable()) {
                kdDebug() << QString("PythonScript::execute() function '%1' added.").arg(vt.first.as_string().c_str()) << endl;
                m_functions.append( vt.first.as_string().c_str() );
            }

            /*
            QString s;
            if(vt.second.isCallable()) s += "isCallable ";
            if(vt.second.isDict()) s += "isDict ";
            if(vt.second.isList()) s += "isList ";
            if(vt.second.isMapping()) s += "isMapping ";
            if(vt.second.isNumeric()) s += "isNumeric ";
            if(vt.second.isSequence()) s += "isSequence ";
            if(vt.second.isTrue()) s += "isTrue ";
            if(vt.second.isInstance()) s += "isInstance ";
            if(PyClass_Check( vt.second.ptr() )) s += "vt.second.isClass ";

            Py::String rf = vt.first.repr();
            Py::String rs = vt.second.repr();
            kdDebug() << QString("PythonScript::execute m_module->getDict() rs='%1' vt.first='%2' rf='%3' test='%4' s='%5'")
                        .arg(rs.as_string().c_str())
                        .arg(vt.first.as_string().c_str())
                        .arg(rf.as_string().c_str())
                        .arg("")
                        .arg(s) << endl;
            //m_objects.replace(vt.first.repr().as_string().c_str(), vt.second);

            if(PyClass_Check( vt.second.ptr() )) {
                //PyObject *aclarg = Py_BuildValue("(s)", rs.as_string().c_str());
                PyObject *pyinst = PyInstance_New(vt.second.ptr(), 0, 0);//aclarg, 0);
                //Py_DECREF(aclarg);
                if (pyinst == 0)
                    kdDebug() << QString("PythonScript::execute PyInstance_New() returned NULL !!!") << endl;
                else {
                    Py::Object inst(pyinst, true);
                    kdDebug() << QString("====> PythonScript::execute inst='%1'").arg(inst.as_string().c_str()) << endl;
                }
            }
            */
        }
        //kdDebug() << QString("PythonScript::execute --------------------------- 2") << endl;

        //kdDebug() << QString("PythonScript::execute() PyRun_String='%1'").arg(run.str().as_string().c_str()) << endl;
        return PythonExtension::toObject(run);
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
}

Kross::Api::Object* PythonScript::callFunction(const QString& name, Kross::Api::List* args)
{
    if(! m_module)
        throw Kross::Api::RuntimeException(i18n("Script not initialized."));

    try {
        kdDebug() << QString("PythonScript::callFunction(%1, %2)")
                     .arg(name)
                     .arg(args ? QString::number(args->count()) : QString("NULL"))
                     << endl;

        Py::Dict moduledict = m_module->getDict();

        // Try to determinate the function we like to execute.
        PyObject* func = PyDict_GetItemString(moduledict.ptr(), name.latin1());
        if( (! m_functions.contains(name)) || (! func) )
            throw Kross::Api::AttributeException(i18n("No such function '%1'.").arg(name));

        Py::Callable funcobject(func, true); // the funcobject takes care of freeing our func pyobject.

        // Check if the object is really a function and therefore callable.
        if(! funcobject.isCallable())
            throw Kross::Api::AttributeException(i18n("Function is not callable."));

        // Call the function.
        Py::Object result = funcobject.apply(PythonExtension::toPyTuple(args));

/* TESTCASE
//PyObject* pDict = PyObject_GetAttrString(func,"__dict__");
//kdDebug()<<"INT => "<< PyObject_IsInstance(func,m_module->ptr()) <<endl;
//( PyCallable_Check(func)){//PyModule_CheckExact(func)){//PyModule_Check(func)){//PyMethod_Check(func)){//PyMethod_Function(func)){//PyInstance_Check(func) )
        kdDebug() << QString("PythonScript::callFunction result='%1' type='%2'").arg(result.as_string().c_str()).arg(result.type().as_string().c_str()) << endl;
        if(result.isInstance()) {
            kdDebug() << ">>> IS_INSTANCE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        }
        if(result.isCallable()) {
            kdDebug() << ">>> IS_CALLABLE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        }
*/

        return PythonExtension::toObject(result);
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
}

const QStringList& PythonScript::getClassNames()
{
    if(! m_module) initialize();
    return m_classes;
}

Kross::Api::Object* PythonScript::classInstance(const QString& name)
{
    if(! m_module)
        throw Kross::Api::RuntimeException(i18n("Script not initialized."));

    try {
        Py::Dict moduledict = m_module->getDict();

        // Try to determinate the class.
        PyObject* pyclass = PyDict_GetItemString(moduledict.ptr(), name.latin1());
        if( (! m_classes.contains(name)) || (! pyclass) )
            throw Kross::Api::AttributeException(i18n("No such class '%1'.").arg(name));

        //PyClass_Check( vt.second.ptr() ))
        //PyObject *aclarg = Py_BuildValue("(s)", rs.as_string().c_str());
        PyObject *pyobj = PyInstance_New(pyclass, 0, 0);//aclarg, 0);
        if(! pyobj)
            throw Kross::Api::AttributeException(i18n("Failed to create instance of class '%1'.").arg(name));

        Py::Object classobject(pyobj, true);
kdDebug() << QString("====> PythonScript::classInstance() inst='%1'").arg(classobject.as_string().c_str()) << endl;
        return PythonExtension::toObject(classobject);
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
}

