// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Perception.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Perception::Perception()
     : Info()
{
    SetAttr("id", string("perception"));
    Object::ListType parents;
    parents.push_back(string("info"));
    SetAttr("parents", parents);
    SetAttr("specification", string("atlas-game"));
}

Perception Perception::Instantiate()
{
    Perception value;

    Object::ListType parents;
    parents.push_back(string("perception"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation