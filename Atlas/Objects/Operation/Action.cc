// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Action.h"

using Atlas::Message::Object;

namespace Atlas { namespace Objects { namespace Operation { 

Action::Action()
     : RootOperation()
{
    SetId(std::string("action"));
    Object::ListType parents(1,std::string("root_operation"));
    SetParents(parents);
}

Action Action::Instantiate()
{
    Action value;

    value.SetParents(Object::ListType(1,std::string("action")));
    value.SetObjtype(std::string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation