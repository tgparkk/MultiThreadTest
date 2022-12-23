#pragma once

#include "imessage.h"

class IWorker
{
public:
	IWorker(const IMessage* request) : m_request(request) {}
	virtual ~IWorker() {}
	virtual IMessage* processing() = 0; //응답을 리턴


	const IMessage* m_request;
};

