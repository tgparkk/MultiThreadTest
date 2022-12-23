#include "stdafx.h"
#include "worker_creator.h"
#include "worker_common.h"
#include "worker_selectnodes.h"
#include "worker_passinfos.h"

IWorker* createWorker(const IMessage* requset)
{
	//추후 외부함수로 빼기.
	IWorker* worker = NULL;
	switch (requset->msg_id())
	{
	case eNetID::Req_Welcome_V1:				worker = new WorkerWelcome(requset); break;
	case eNetID::Req_LoadPolylines_Area_V1:		worker = new WorkerSelectPolylineArea(requset); break;
	case eNetID::Req_LoadPolylines_Mapid_V1:	worker = new WorkerSelectPolylineMapid(requset); break;
	case eNetID::Req_LoadPolygons_Area_V1:		worker = new WorkerSelectPolygonArea(requset); break;	
	case eNetID::Req_LoadPolygons_Mapid_V1:		worker = new WorkerSelectPolygonMapid(requset); break;
	case eNetID::Req_LoadLinks_Area_V1:			worker = new WorkerSelectLinkArea(requset); break;
	case eNetID::Req_LoadLinks_Mapid_V1:		worker = new WorkerSelectLinkMapid(requset); break;
	case eNetID::Req_LoadNodes_Area_V1:			worker = new WorkerSelectNodeArea(requset); break;
	case eNetID::Req_LoadNodes_Mapid_V1:		worker = new WorkerSelectNodeMapid(requset); break;
	case eNetID::Req_LoadPassInfos_Area_V1:		worker = new WorkerPassInfoArea(requset); break;
	case eNetID::Req_LoadPassInfos_Mapid_V1:	worker = new WorkerPassInfoMapid(requset); break;
	case eNetID::Req_LoadPolygon3DInfos_V1:		worker = new WorkerPGInfoFor3D(requset); break;
	case eNetID::Req_LoadHeight3DInfos_V1:		worker = new WorkerHeightInfoFor3D(requset); break;	
	}

	return worker;
	
}
