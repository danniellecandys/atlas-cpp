/*
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com

changes:

23 Jan 2000 - fex

    Just simplifying for clarity and checking things over

*/

#include <cstdlib>

#include "../Debug/Debug.h"
#include "../Object/Object.h"

#include "Protocol.h"
#include "XMLDecoder.h"

//constants
const char OPEN_TAG=        '<';
const char CLOSE_TAG=       '>';
const char END_TAG=         '/';
const char QUOTES=          '\"';
const char* MSG_TOP_NAME=   "obj";
const char* NAME_OPEN=      "name=";
const char* DEFAULT_NAME=   "";

namespace Atlas
{

void XMLDecoder::newStream()
{
	// reset decoder
	state = 1;
	nestd = 0;
	token = 0;
}

void XMLDecoder::feedStream(const string& data)
{
	buffer.append(data);
}

int XMLDecoder::getToken()
{
	int tmp = token;
	token = 0;
	return tmp;
}


int XMLDecoder::hasTokens()
{
    //check for buffer overflow
    if (token == -1)
        return -1;
	
    bool stateOK;
    size_t	pos;
    string	typ;
    string	tag;
    string	buf;

    do {
        stateOK = false;

        Debug::Msg(1, "xmldecoder :: tokenise loop in state  -> %i", state);
        Debug::Msg(6, "xmldecoder :: current buffer contents -> [%s%s", buffer.c_str()), "]";

        // this is where we are gonna put the state machine
        switch (state) {

        case 1:
            //start of message, find first tag
            if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
                break;

            buffer = buffer.substr(pos);

            if ( (pos = buffer.find( CLOSE_TAG )) == string::npos )
                break;

            {
            tag = buffer.substr( 1, 3 );
            buffer = buffer.substr( pos );
            stateOK = true;

            if ( tag == MSG_TOP_NAME ) {
                token = Protocol::atlasMSGBEG;
                state = 2;
            } else
                token = Protocol::atlasERRTOK;

            break;
            }
		
		case 2: {
		    //check for complete tag
			if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
			    break;
			buffer = buffer.substr(pos);
			if ( (pos = buffer.find( CLOSE_TAG )) == string::npos )
			    break;
			
			// got a complete tag.. lets tear it up !!
			buf = buffer.substr( 0, pos+1 );
			Debug::Msg(1,"xmldecoder :: PARSING TAG = %s", buf.c_str());
			buffer = buffer.substr( pos+1 );
			
			// grab the tag name
			pos = buf.find_first_of( " >" );
			tag = buf.substr( 1, pos-1 );
			buf = buf.substr( pos+1 );
			
			Debug::Msg(1,"xmldecoder :: PARSING TAG='%s' BUF='%s'", tag.c_str(), buf.c_str());
			
			//if not anonymous, get the name
			if ( buf.substr(0,5) == NAME_OPEN ) {
				name = buf.substr(5,buf.length()-6);
				Debug::Msg(1,"xmldecoder :: PARSING NAME=%s", name.c_str());
				
				//strip quotes if quoted
				if ( name[0] == QUOTES ) {
					name = name.substr(1,name.length()-2);
				}
				Debug::Msg(1,"xmldecoder :: PARSING NAME=%s", name.c_str());
			}
			else
			    name = DEFAULT_NAME;
			
			// got name and type... interpret type
			bool endTag = false;
			if ( tag[0] == END_TAG) {
				endTag = true;
				tag = tag.substr(1);
			}
						
			if ( tag == MSG_TOP_NAME ) {
				if ( endTag ) {
					token = Protocol::atlasMSGEND;
					state = 1;
				} else {
					// bad tag !! push token back
					buffer.insert(0,buf);
					token = Protocol::atlasERRTOK;
					Debug::Msg(1,"xmldecoder :: BAD TAG=%s", tag.c_str());
				}
				break;
			}
			else if (tag == "int")		type = Protocol::atlasINT;
			else if (tag == "str")		type = Protocol::atlasSTR;
			else if (tag == "string")	type = Protocol::atlasSTR;
			else if (tag == "float")	type = Protocol::atlasFLT;
			else if (tag == "map")		type = Protocol::atlasMAP;
			else if (tag == "list")		type = Protocol::atlasLST;
			else {
				// bad tag type !!
				Debug::Msg(1,"xmldecoder :: BAD TYPE=%s", tag.c_str());
				token = Protocol::atlasERRTOK;
				break;
			}
			if ( !endTag ) {
				token = Protocol::atlasATRBEG;
				state = 3;
				if (type == Protocol::atlasMAP) state = 2;
				if (type == Protocol::atlasLST) state = 2;
			} else {
				token = Protocol::atlasATREND;
				state = 2;
			}
			stateOK = true;
			break;
        }
		case 3:
			if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
			    break;
			
			sval = buffer.substr( 0, pos );
			buffer = buffer.substr(pos);
			
			if (type == Protocol::atlasINT)
			    ival = atoi(sval.c_str());
			
			if (type == Protocol::atlasFLT)
			    fval = atof(sval.c_str());
			
			token = Protocol::atlasATRVAL;
			state = 2;
			stateOK = true;
			break;

		default:
			// invalid state
			token = -1;
			stateOK = 1;
			break;
		}

	} while ( stateOK && token == 0);

	// see if we have a token to return
	Debug::Msg(1, "xmldecoder :: END TOKEN=%i", token);
	Debug::Msg(1, "xmldecoder :: finished in state -> %i", state);
    Debug::Msg(6, "xmldecoder :: current buffer contents -> [%s%s", buffer.c_str()), "]";
	
	if (token != 0)
	    return 1;
	return 0;
}


} // namespace Atlas
