// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "AdminEntity.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

AdminEntity::AdminEntity()
     : RootEntity()
{
    SetAttr("id", string("admin_entity"));
    Object::ListType parents;
    parents.push_back(string("root_entity"));
    SetAttr("parents", parents);
}

AdminEntity AdminEntity::Instantiate()
{
    AdminEntity value;

    Object::ListType parents;
    parents.push_back(string("admin_entity"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity