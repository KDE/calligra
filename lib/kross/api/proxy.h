/***************************************************************************
 * proxy.h
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_API_PROXY_H
#define KROSS_API_PROXY_H

#include "../main/krossconfig.h"
#include "object.h"
#include "list.h"
#include "class.h"
#include "variant.h"

#include <qstring.h>

namespace Kross { namespace Api {

    template< class OBJECT, typename TYPE >
    class ProxyValue
    {
        public:
            typedef TYPE type;
            typedef OBJECT object;
    };

    template< class INSTANCE,
              class RET  = ProxyValue<Kross::Api::Object,void>,
              class ARG1 = ProxyValue<Kross::Api::Object,void>,
              class ARG2 = ProxyValue<Kross::Api::Object,void>,
              class ARG3 = ProxyValue<Kross::Api::Object,void>,
              class ARG4 = ProxyValue<Kross::Api::Object,void> >
    class ProxyFunction : public Function
    {
        private:
            typedef typename RET::type (INSTANCE::*Method)( typename ARG1::type, typename ARG2::type, typename ARG3::type, typename ARG4::type );
            INSTANCE* m_instance;
            Method m_method;

            template<class PROXYFUNC, typename RETURNRYPE>
            struct ProxyFunctionCaller {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2, typename ARG3::type arg3, typename ARG4::type arg4) {
                    return new class RET::object( ( (self->m_instance)->*(self->m_method) )(arg1,arg2,arg3,arg4) );
                }
            };

            template<class PROXYFUNC>
            struct ProxyFunctionCaller<PROXYFUNC, void> {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2, typename ARG3::type arg3, typename ARG4::type arg4) {
                    ( (self->m_instance)->*(self->m_method) )(arg1,arg2,arg3,arg4);
                    return 0;
                }
            };

        public:
            ProxyFunction(INSTANCE* instance, Method method)
                : m_instance(instance), m_method(method) {}
            Object::Ptr call(List::Ptr args) {
                return ProxyFunctionCaller<ProxyFunction, typename RET::type>::exec(this,
                    Kross::Api::Object::fromObject<typename ARG1::object>(args->item(0))->operator typename ARG1::type(),
                    Kross::Api::Object::fromObject<typename ARG2::object>(args->item(1))->operator typename ARG2::type(),
                    Kross::Api::Object::fromObject<typename ARG3::object>(args->item(2))->operator typename ARG3::type(),
                    Kross::Api::Object::fromObject<typename ARG4::object>(args->item(3))->operator typename ARG4::type()
                );
            }
    };

    template<class INSTANCE, class RET, class ARG1, class ARG2, class ARG3>
    class ProxyFunction<INSTANCE, RET, ARG1, ARG2, ARG3 > : public Function
    {
        private:
            typedef typename RET::type (INSTANCE::*Method)( typename ARG1::type, typename ARG2::type, typename ARG3::type );
            INSTANCE* m_instance;
            Method m_method;

            template<class PROXYFUNC, typename RETURNRYPE>
            struct ProxyFunctionCaller {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2, typename ARG3::type arg3) {
                    return new class RET::object( ( (self->m_instance)->*(self->m_method) )(arg1,arg2,arg3) );
                }
            };

            template<class PROXYFUNC>
            struct ProxyFunctionCaller<PROXYFUNC, void> {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2, typename ARG3::type arg3) {
                    ( (self->m_instance)->*(self->m_method) )(arg1,arg2,arg3);
                    return 0;
                }
            };

        public:
            ProxyFunction(INSTANCE* instance, Method method)
                : m_instance(instance), m_method(method) {}
            Object::Ptr call(List::Ptr args) {
                return ProxyFunctionCaller<ProxyFunction, typename RET::type>::exec(this,
                    Kross::Api::Object::fromObject<typename ARG1::object>(args->item(0))->operator typename ARG1::type(),
                    Kross::Api::Object::fromObject<typename ARG2::object>(args->item(1))->operator typename ARG2::type(),
                    Kross::Api::Object::fromObject<typename ARG3::object>(args->item(2))->operator typename ARG3::type()
                );
            }
    };

    template<class INSTANCE, class RET, class ARG1, class ARG2>
    class ProxyFunction<INSTANCE, RET, ARG1, ARG2 > : public Function
    {
        private:
            typedef typename RET::type (INSTANCE::*Method)( typename ARG1::type, typename ARG2::type );
            INSTANCE* m_instance;
            Method m_method;

            template<class PROXYFUNC, typename RETURNRYPE>
            struct ProxyFunctionCaller {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2) {
                    return new class RET::object( ( (self->m_instance)->*(self->m_method) )(arg1,arg2) );
                }
            };

            template<class PROXYFUNC>
            struct ProxyFunctionCaller<PROXYFUNC, void> {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1, typename ARG2::type arg2) {
                    ( (self->m_instance)->*(self->m_method) )(arg1,arg2);
                    return 0;
                }
            };

        public:
            ProxyFunction(INSTANCE* instance, Method method)
                : m_instance(instance), m_method(method) {}
            Object::Ptr call(List::Ptr args) {
                return ProxyFunctionCaller<ProxyFunction, typename RET::type>::exec(this,
                    Kross::Api::Object::fromObject<typename ARG1::object>(args->item(0))->operator typename ARG1::type(),
                    Kross::Api::Object::fromObject<typename ARG2::object>(args->item(1))->operator typename ARG2::type()
                );
            }
    };

    template<class INSTANCE, class RET, class ARG1>
    class ProxyFunction<INSTANCE, RET, ARG1 > : public Function
    {
        private:
            typedef typename RET::type (INSTANCE::*Method)( typename ARG1::type );
            INSTANCE* m_instance;
            Method m_method;

            template<class PROXYFUNC, typename RETURNRYPE>
            struct ProxyFunctionCaller {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1) {
                    return new class RET::object( ( (self->m_instance)->*(self->m_method) )(arg1) );
                }
            };

            template<class PROXYFUNC>
            struct ProxyFunctionCaller<PROXYFUNC, void> {
                inline static Object::Ptr exec(PROXYFUNC* self, typename ARG1::type arg1) {
                    ( (self->m_instance)->*(self->m_method) )(arg1);
                    return 0;
                }
            };

        public:
            ProxyFunction(INSTANCE* instance, Method method)
                : m_instance(instance), m_method(method) {}
            Object::Ptr call(List::Ptr args) {
                return ProxyFunctionCaller<ProxyFunction, typename RET::type>::exec(this,
                    Kross::Api::Object::fromObject<typename ARG1::object>(args->item(0))->operator typename ARG1::type()
                );
            }
    };

    template<class INSTANCE, class RET>
    class ProxyFunction<INSTANCE, RET > : public Function
    {
        private:
            typedef typename RET::type (INSTANCE::*Method)();
            INSTANCE* m_instance;
            Method m_method;

            template<class PROXYFUNC, typename RETURNRYPE>
            struct ProxyFunctionCaller {
                inline static Object::Ptr exec(PROXYFUNC* self) {
                    return new class RET::object( ( (self->m_instance)->*(self->m_method) )() );
                }
            };

            template<class PROXYFUNC>
            struct ProxyFunctionCaller<PROXYFUNC, void> {
                inline static Object::Ptr exec(PROXYFUNC* self) {
                    ( (self->m_instance)->*(self->m_method) )();
                    return 0;
                }
            };

        public:
            ProxyFunction(INSTANCE* instance, Method method)
                : m_instance(instance), m_method(method) {}
            Object::Ptr call(List::Ptr args) {
                return ProxyFunctionCaller<ProxyFunction, typename RET::type>::exec(this);
            }
    };


    template<class INSTANCE>
    class ProxyClass : public Class< ProxyClass<INSTANCE> >
    {
        protected:
            INSTANCE* m_instance;
        public:
            typedef INSTANCE type;
            ProxyClass(const QString& classname, INSTANCE* instance)
                : Class< ProxyClass<INSTANCE> >(classname)
                , m_instance(instance) {}
    };

}}

#endif

