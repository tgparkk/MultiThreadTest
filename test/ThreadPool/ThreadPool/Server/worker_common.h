#pragma once


// #include "net_messages.h"
// #include "net_common.h"
#include "iworker.h"

//기존로직 : bessal 은 mapid 로 검색 , grs80 은 area 로 검색


class WorkerWelcome : public IWorker
{
public:
	WorkerWelcome(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerSelectPolylineArea : public IWorker
{
public:
	WorkerSelectPolylineArea(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerSelectPolylineMapid : public IWorker
{
public:
	WorkerSelectPolylineMapid(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerSelectPolygonArea : public IWorker
{
public:
	WorkerSelectPolygonArea(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerSelectPolygonMapid: public IWorker
{
public:
	WorkerSelectPolygonMapid(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};


class WorkerSelectLinkArea : public IWorker
{
public:
	WorkerSelectLinkArea(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerSelectLinkMapid: public IWorker
{
public:
	WorkerSelectLinkMapid(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerPGInfoFor3D: public IWorker
{
public:
	WorkerPGInfoFor3D(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};

class WorkerHeightInfoFor3D: public IWorker
{
public:
	WorkerHeightInfoFor3D(const IMessage* request) : IWorker(request) {}
	virtual IMessage* processing();
};



