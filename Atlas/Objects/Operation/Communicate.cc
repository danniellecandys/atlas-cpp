// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Communicate.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Communicate::Communicate()
     : Create()
{
    SetAttr("id", string("communicate"));
    Object::ListType parents;
    parents.push_back(string("create"));
    SetAttr("parents", parents);
    SetAttr("specification", string("atlas-game"));
}

Communicate Communicate::Instantiate()
{
    Communicate value;

    Object::ListType parents;
    parents.push_back(string("communicate"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation