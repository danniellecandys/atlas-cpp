// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Perceive.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Perceive::Perceive()
     : Get()
{
    SetAttr("id", string("perceive"));
    Object::ListType parents;
    parents.push_back(string("get"));
    SetAttr("parents", parents);
    SetAttr("specification", string("atlas-game"));
}

Perceive Perceive::Instantiate()
{
    Perceive value;

    Object::ListType parents;
    parents.push_back(string("perceive"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation