// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_ENCODER_H
#define ATLAS_STREAM_ENCODER_H

#include "../Object/Object.h"

namespace Atlas { namespace Stream {

/** Atlas stream encoder

This class presents an interface for sending and receiving Atlas messages.
Each outgoing message is converted to a byte stream and piped through an
optional chain of filters for compression or other transformations, then
passed to a socket for transmission. Incoming messages are read from the
socket, piped through the filters in the opposite direction and passed to
user specified callback functions FIXME this doesn't happen yet FIXME

Codecs should declare an instance of Codec::Factory in the module they are
defined in. This will allow them to be automatically included in the
negotiation process that chooses which codecs and filters an Atlas connection
will use. FIXME talk about codec metrics FIXME

@see Filter
@see Socket
@see Factory
@see Negotiate

*/

class Encoder
{
    public:

    virtual ~Encoder();

    // Interface for top level context

    virtual void MessageBegin() = 0;
    virtual void MessageEnd() = 0;
    
    // Interface for map context
    
    virtual void ListBegin(const std::string& name) = 0;
    virtual void MapBegin(const std::string& name) = 0;
    virtual void Item(const std::string& name, int) = 0;
    virtual void Item(const std::string& name, float) = 0;
    virtual void Item(const std::string& name, const std::string&) = 0;
    virtual void Item(const std::string& name, const Atlas::Object&) = 0;
    virtual void ListEnd() = 0;
    
    // Interface for list context
    
    virtual void ListBegin() = 0;
    virtual void MapBegin() = 0;
    virtual void Item(int) = 0;
    virtual void Item(float) = 0;
    virtual void Item(const std::string&) = 0;
    virtual void Item(const Atlas::Object&) = 0;
    virtual void MapEnd() = 0;
};

} } // Atlas::Stream

#endif