#include "stdafx.h"
#include "request_creator.h"
#include "msg_implements.h"

IMessage* RequestCreator::create(eMsgID::T msgid)
{
	IMessage* msg = NULL;
	switch (msgid)
	{
	case eMsgID::Req_Welcome_V1:					msg = new ReqWelcomeMsg_V1(); break;
	case eMsgID::Req_LoadPolylines_Area_V1:			msg = new ReqLoadPolylinesArea_V1(); break;
	case eMsgID::Req_LoadPolylines_Mapid_V1:		msg = new ReqLoadPolylinesMapid_V1(); break;
	case eMsgID::Req_LoadPolygons_Area_V1:			msg = new ReqLoadPolygonsArea_V1(); break;
	case eMsgID::Req_LoadPolygons_Mapid_V1:			msg = new ReqLoadPolygonsMapid_V1(); break;
	case eMsgID::Req_LoadLinks_Area_V1:				msg = new ReqLoadLinksArea_V1(); break;
	case eMsgID::Req_LoadLinks_Mapid_V1:			msg = new ReqLoadLinksMapid_V1(); break;
	case eMsgID::Req_LoadNodes_Area_V1:				msg = new ReqLoadNodesArea_V1(); break;
	case eMsgID::Req_LoadNodes_Mapid_V1:			msg = new ReqLoadNodesMapid_V1(); break;
	case eMsgID::Req_LoadPassInfos_Area_V1:			msg = new ReqLoadPassInfosArea_V1(); break;
	case eMsgID::Req_LoadPassInfos_Mapid_V1:		msg = new ReqLoadPassInfosMapid_V1(); break;
	case eMsgID::Req_LoadPolygon3DInfos_V1:			msg = new ReqLoadPolygon3DInfos_V1(); break;
	case eMsgID::Req_LoadHeight3DInfos_V1:			msg = new ReqLoadHeight3DInfos_V1(); break;
	case eMsgID::Req_LoadModelinglnfos_Area_V1:		msg = new ReqLoadModelingInfosArea_V1(); break;
	case eMsgID::Req_LoadModelingInfos_Mapid_V1:	msg = new ReqLoadModelingInfosMapid_V1(); break;
	case eMsgID::Req_LoadModelingInfo_V1:			msg = new ReqLoadModelingInfo_V1(); break;
	case eMsgID::Req_LoadGEMs_GemKeys_V1:			msg = new ReqLoadGEMsGemKeys_V1(); break;
	case eMsgID::Req_LoadGEM_GemKey_V1:				msg = new ReqLoadGEMGemKey_V1(); break;
	
	default:
		_ASSERT(false);
		AELOG("msg id 에 해당하는 메시지를 생성할 수 없습니다.");
	}

	return msg;
}
