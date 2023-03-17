#pragma once

#include "imessage.h"

class RequestCreator
{
public:
	static IMessage* create(eMsgID::T msgid);
};




