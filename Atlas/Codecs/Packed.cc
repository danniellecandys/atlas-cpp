// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

#include "../Stream/Codec.h"
#include "Utility.h"

#include <stack>

using namespace std;
using namespace Atlas::Stream;

/*

[type][name=][data][|endtype]
  
{} for message
() for lists
[] for maps
$ for string
@ for int
# for float

*/

class PackedAscii : public Codec
{
public:
    
    PackedAscii(const Codec::Parameters&);

    virtual void Poll();

    virtual void MessageBegin();
    virtual void MessageItem(const Map&);
    virtual void MessageEnd();
    
    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, double);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(double);
    virtual void ListItem(const std::string&);
    virtual void ListEnd();

protected:
    
    iostream& socket;
    Filter* filter;
    Bridge* bridge;

    enum State
    {
	PARSE_STREAM,
        PARSE_MESSAGE,
        PARSE_MAP,
        PARSE_LIST,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING,
        PARSE_NAME,
        PARSE_VALUE,
	PARSE_ENCODED,
    };
    
    stack<State> state;

    string nameFragment;
    string stringFragment;
    int intFragment;
    float floatFragment;

    inline void ParseStream(char);
    inline void ParseMessage(char);
    inline void ParseMap(char);
    inline void ParseList(char);
    inline void ParseInt(char);
    inline void ParseFloat(char);
    inline void ParseString(char);
    inline void ParseName(char);
    inline void ParseEncoded(char);
};

namespace
{
    Codec::Factory<PackedAscii> factory("PackedAscii", Codec::Metrics(1, 2));
}

PackedAscii::PackedAscii(const Codec::Parameters& p) :
    socket(p.stream), filter(p.filter), bridge(p.bridge)
{
    state.push(PARSE_STREAM);

    stringFragment = "";
    intFragment = 0;
    floatFragment = 0.0;
}

void PackedAscii::ParseStream(char next)
{
    switch (next)
    {
	case '{':
	    bridge->MessageBegin();
	    state.push(PARSE_MESSAGE);
	break;
    
	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void PackedAscii::ParseMessage(char next)
{
    switch (next)
    {
	case '[':
	    bridge->MessageItem(MapBegin);
	    state.push(PARSE_MAP);
	break;
    
	case '}':
	    bridge->MessageEnd();
	    state.pop();
	break;
	
	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void PackedAscii::ParseMap(char next)
{
    switch (next)
    {
	case ']':
	    bridge->MapEnd();
	    state.pop();
	break;

	case '[':
	    state.push(PARSE_MAP);
	    state.push(PARSE_NAME);
	break;

	case '(':
	    state.push(PARSE_LIST);
	    state.push(PARSE_NAME);
	break;

	case '$':
	    state.push(PARSE_STRING);
	    state.push(PARSE_NAME);
	break;

	case '@':
	    state.push(PARSE_INT);
	    state.push(PARSE_NAME);
	break;

	case '#':
	    state.push(PARSE_FLOAT);
	    state.push(PARSE_NAME);
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void PackedAscii::ParseList(char next)
{
    switch (next)
    {
	case ')':
	    bridge->ListEnd();
	    state.pop();
	break;

	case '[':
	    bridge->ListItem(MapBegin);
	    state.push(PARSE_MAP);
	break;

	case '(':
	    bridge->ListItem(ListBegin);
	    state.push(PARSE_LIST);
	break;

	case '$':
	    state.push(PARSE_STRING);
	break;

	case '@':
	    state.push(PARSE_INT);
	break;

	case '#':
	    state.push(PARSE_FLOAT);
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void PackedAscii::ParseInt(char next)
{
    switch (next)
    {
	case '[':
	case '(':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(nameFragment, intFragment);
		nameFragment = "";
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(intFragment);
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    intFragment = 0;
	break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    intFragment *= 10;
	    intFragment += next - '0';
	break;
    }
}

void PackedAscii::ParseFloat(char next)
{
    switch (next)
    {
	case '[':
	case '(':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(nameFragment, floatFragment);
		nameFragment = "";
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(floatFragment);
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    floatFragment = 0.0;
	break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    floatFragment *= 10;
	    floatFragment += next - '0';
	break;
    }
}

void PackedAscii::ParseString(char next)
{
    switch (next)
    {
	case '[':
	case '(':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(nameFragment, stringFragment);
		nameFragment = "";
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(stringFragment);
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    stringFragment = "";
	break;

	case '+':
	   state.push(PARSE_ENCODED);
	break;

	case '=':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    stringFragment += next;
	break;
    }
}

void PackedAscii::ParseName(char next)
{
    switch (next)
    {
	case '=':
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(nameFragment, MapBegin);
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->MapItem(nameFragment, ListBegin);
	    }
	break;

	case '+':
	    state.push(PARSE_ENCODED);
	break;
	
	case '[':
	case '(':
	case '$':
	case '@':
	case '#':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    nameFragment += next;
	break;
    }
}

void PackedAscii::ParseEncoded(char next)
{
    switch (next)
    {
    }
}

void PackedAscii::Poll()
{
    while (socket.rdbuf()->in_avail())
    {
	char next = socket.get(); // get character

	switch (state.top())
	{
	    case PARSE_STREAM:	ParseStream(next); break;
	    case PARSE_MESSAGE:	ParseMessage(next); break;
	    case PARSE_MAP:	ParseMap(next); break;
	    case PARSE_LIST:	ParseList(next); break;
	    case PARSE_INT:	ParseInt(next); break;
	    case PARSE_FLOAT:	ParseFloat(next); break;
	    case PARSE_STRING:	ParseString(next); break;
	    case PARSE_NAME:	ParseName(next); break;
	    case PARSE_ENCODED:	ParseEncoded(next); break;
	}
    }
}

void PackedAscii::MessageBegin()
{
    socket << "{";
}

void PackedAscii::MessageItem(const Map&)
{
    socket << "[";
}

void PackedAscii::MessageEnd()
{
    socket << "}";
}

void PackedAscii::MapItem(const std::string& name, const Map&)
{
    socket << "[" << hexEncode("+", "+{}[]()@#$=", name) << "=";
}

void PackedAscii::MapItem(const std::string& name, const List&)
{
    socket << "(" << hexEncode("+", "+{}[]()@#$=", name) << "=";
}

void PackedAscii::MapItem(const std::string& name, int data)
{
    socket << "@" << hexEncode("+", "+{}[]()@#$=", name) << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, double data)
{
    socket << "#" << hexEncode("+", "+{}[]()@#$=", name) << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, const std::string& data)
{
    socket << "$" << hexEncode("+", "+{}[]()@#$=", name) << "=" <<
            hexEncode("+", "+{}[]()@#$=", data);
}

void PackedAscii::MapEnd()
{
    socket << "]";
}

void PackedAscii::ListItem(const Map&)
{
    socket << "[";
}

void PackedAscii::ListItem(const List&)
{
    socket << "(";
}

void PackedAscii::ListItem(int data)
{
    socket << "@" << data;
}

void PackedAscii::ListItem(double data)
{
    socket << "#" << data;
}

void PackedAscii::ListItem(const std::string& data)
{
    socket << "$" << hexEncode("+", "+{}[]()@#$=", data);
}

void PackedAscii::ListEnd()
{
    socket << ")";
}