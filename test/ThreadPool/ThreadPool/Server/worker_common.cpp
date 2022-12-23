#include "stdafx.h"
#include "worker_common.h"
#include "app_option.h"

#include "request.h"
#include "response.h"

#include "spmanager/rbpolyline.h"
#include "spmanager/rbpolygon.h"
#include "spmanager/rblink.h"
#include "spmanager/rbobject.h"


IMessage* WorkerWelcome::processing()
{
	const ReqWelcomeMsg_V1* request = dynamic_cast<const ReqWelcomeMsg_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	wchar_t text[256] = {0,};
	StringCbPrintfW(text, sizeof(text), L"welcome:%d", request->m_number);
	return new ResWelcome_V1(text);
}


IMessage* WorkerSelectPolylineArea::processing()
{
	const ReqLoadPolylinesArea_V1* request = dynamic_cast<const ReqLoadPolylinesArea_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//coconutre tree 의 로직 그대로 사용.(grs80만 소숫점)
	CString area_text;
	const CMapDoubleArea& area = request->m_area;
	if (request->m_ellipsoid == eEllipsoid::vGRS_80)
	{
		area_text.Format(_T("%f, %f, %f, %f"), area.LLong, area.LLat, area.RLong, area.ULat);
	}
	else 
	{
		area_text.Format(_T("%u, %u, %u, %u"), (DWORD)area.LLong, (DWORD)area.LLat, (DWORD)area.RLong, (DWORD)area.ULat);
	}
	
	CString strQuery;
	switch (request->m_load_type)
	{
	case 5: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_LEVEL(5, 3, %s)", area_text); break;
	case 4: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_LEVEL(4, 3, %s)", area_text); break;
	case 3: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_LEVEL(3, 3, %s)", area_text); break;
	case 2: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_LEVEL(2, 3, %s)", area_text); break;
	case 1: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_LEVEL(1, 3, %s)", area_text); break;
	default: strQuery.Format(L"call GP_BTM_POLYLINE_SEL_INTERSECTS(%s)", area_text); break;
	}

	CRBPolyline rbpolyline;
	rbpolyline.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Polyline> polylines;
	//polylines.reserve(60000);
	if (!rbpolyline.GetResPolyline(strQuery, polylines))
	{
		return NULL;
	}
	
	return new ResLoadPolylines_V1(polylines);

}

IMessage* WorkerSelectPolylineMapid::processing()
{
	const ReqLoadPolylinesMapid_V1* request = dynamic_cast<const ReqLoadPolylinesMapid_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	CString strQuery;
	strQuery.Format(L"call GP_BTM_POLYLINE_SEL_REGION(%s);", request->m_mapid.c_str());
	
	CRBPolyline rbpolyline;
	rbpolyline.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());
	
	std::vector<out::Polyline> polylines;
	//polylines.reserve(60000);
	if (!rbpolyline.GetResPolyline(strQuery, polylines))
	{
		return NULL;
	}

	return new ResLoadPolylines_V1(polylines);
}

IMessage* WorkerSelectPolygonArea::processing()
{
	const ReqLoadPolygonsArea_V1* request = dynamic_cast<const ReqLoadPolygonsArea_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//coconutre tree 의 로직 그대로 사용.(grs80만 소숫점)
	CString area_text;
	const CMapDoubleArea& area = request-> m_area;
	if (request->m_ellipsoid == eEllipsoid::vGRS_80)
	{
		area_text.Format(_T("%f, %f, %f, %f"), area.LLong, area.LLat, area.RLong, area.ULat);
	}
	else 
	{
		area_text.Format(_T("%u, %u, %u, %u"), (DWORD)area.LLong, (DWORD)area.LLat, (DWORD)area.RLong, (DWORD)area.ULat);
	}

	CString strQuery;
	switch (request->m_load_type)
	{
	case ePolygonLoad::Level_5:		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(5, 3, %s)"), area_text ); break;
	case ePolygonLoad::Level_4:		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(4, 3, %s)"), area_text ); break;		
	case ePolygonLoad::Level_3:		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(3, 3, %s)"), area_text ); break;
	case ePolygonLoad::Level_2:		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(2, 3, %s)"), area_text ); break;
	case ePolygonLoad::Level_1:		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(1, 3, %s)"), area_text ); break;
	case ePolygonLoad::UP_6LEVEL:	strQuery.Format( _T("call GP_BTM_POLYGON_SEL_LEVEL(6, 3, %s)"), area_text ); break;
	default:						strQuery.Format( _T("call GP_BTM_POLYGON_SEL_INTERSECTS(%s)"), area_text ); break;
	}
	
	CRBPolygon rbpolygon;
	rbpolygon.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Polygon> polygons;
	if (!rbpolygon.GetBinary(strQuery, polygons)) 
	{
		return NULL;
	}

	return new ResLoadPolygons_V1(polygons);

	
}

IMessage* WorkerSelectPolygonMapid::processing()
{
	const ReqLoadPolygonsMapid_V1* request = dynamic_cast<const ReqLoadPolygonsMapid_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}
	
	if (request->m_load_type == ePGMapIdLoad::TABLE_MAPID)
	{
		CRBPolygon rbpolygon;
		rbpolygon.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());
		
		CString strQuery;
		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_REGION(%s)"), request->m_mapid.c_str());
		
		std::vector<out::Polygon> polygons;
		if (!rbpolygon.GetBinary(strQuery, polygons)) 
		{
			return NULL;
		}

		return new ResLoadPolygons_V1(polygons);
	}
	else if (request->m_load_type == ePGMapIdLoad::HOUSE_MAPID)
	{
		CRBPolygon rbpolygon;
		rbpolygon.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

		CString strQuery;
		strQuery.Format( _T("call GP_BTM_POLYGON_SEL_REGION_HOUSE(%s)"), request->m_mapid.c_str());

		std::vector<out::Polygon> polygons;
		if (!rbpolygon.GetBinary(strQuery, polygons)) 
		{
			return NULL;
		}

		return new ResLoadPolygons_V1(polygons);
	}
	else if (request->m_load_type == ePGMapIdLoad::AUTOFIX_MAPID)
	{
		CRBPolygon rbpolygon;
		rbpolygon.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());
		std::vector<out::Polygon> polygons;

		CString strQuery;
		strQuery.Format(_T("EXEC UP_BTM_POLYGON_AUTO_SEL %s"), request->m_mapid.c_str());
		if (!rbpolygon.GetPABinary(strQuery, polygons)) 
		{
			return NULL;
		}

		return new ResLoadPolygons_V1(polygons);
	}
	else
	{
		ASSERT(false);
	}

	return NULL;
	
}

IMessage* WorkerSelectLinkArea::processing()
{
	const ReqLoadLinksArea_V1* request = dynamic_cast<const ReqLoadLinksArea_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//coconutre tree 의 로직 그대로 사용.(grs80만 소숫점)
	CString area_text;
	const CMapDoubleArea& area = request->m_area;
	if (request->m_ellipsoid == eEllipsoid::vGRS_80)
	{
		area_text.Format(_T("%f, %f, %f, %f"), area.LLong, area.LLat, area.RLong, area.ULat);
	}
	else 
	{
		area_text.Format(_T("%u, %u, %u, %u"), (DWORD)area.LLong, (DWORD)area.LLat, (DWORD)area.RLong, (DWORD)area.ULat);
	}

	CString strQuery;
	switch (request->m_load_type)
	{
	case eLinkAreaLoad::Level_5:	strQuery.Format( _T("call GP_RTM_LINK_SEL_LEVEL_CTT_REALLINK(5, 3, %s)"), area_text ); break;
	case eLinkAreaLoad::Level_4:	strQuery.Format( _T("call GP_RTM_LINK_SEL_LEVEL_CTT_REALLINK(4, 3, %s)"), area_text ); break;
	case eLinkAreaLoad::Level_3:	strQuery.Format( _T("call GP_RTM_LINK_SEL_LEVEL_CTT_REALLINK(3, 3, %s)"), area_text ); break;
	default:						strQuery.Format( _T("call GP_RTM_LINK_SEL_INTERSECTS_CTT_REALLINK(%s)"), area_text); break;
	}

	CRBLink rblink;
	rblink.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Link> links;
	if (!rblink.GetBinary(strQuery, links)) 
	{
		return NULL;
	}

	return new ResLoadLinks_V1(links);

}

IMessage* WorkerSelectLinkMapid::processing()
{
	const ReqLoadLinksMapid_V1* request = dynamic_cast<const ReqLoadLinksMapid_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//case LK_TABLE_MAPID

	CString strQuery;
	strQuery.Format(_T("call GP_RTM_LINK_SEL_REGION_CTT_REALLINK(%s)"), request->m_mapid.c_str());

	CRBLink rblink;
	rblink.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Link> links;
	if (!rblink.GetBinary(strQuery, links)) 
	{
		return NULL;
	}

	return new ResLoadLinks_V1(links);
}

IMessage* WorkerPGInfoFor3D::processing()
{
	const ReqLoadPolygon3DInfos_V1* request = dynamic_cast<const ReqLoadPolygon3DInfos_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//coconutre tree 의 로직 그대로 사용.(grs80만 소숫점)
	CString area_text;
	const CMapDoubleArea& area = request->m_area;
	if (request->m_ellipsoid == eEllipsoid::vGRS_80)
	{
		area_text.Format(_T("%f, %f, %f, %f"), area.LLong, area.LLat, area.RLong, area.ULat);
	}
	else 
	{
		area_text.Format(_T("%u, %u, %u, %u"), (DWORD)area.LLong, (DWORD)area.LLat, (DWORD)area.RLong, (DWORD)area.ULat);
	}

	CString strQuery;
	strQuery.Format(_T("call GP_BTD_B3D_PG_LANE_SEL_INTERSECTS(%s)"), area_text);		

	CRBPolygon rbpolygon;
	rbpolygon.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Polygon3DInfo> infos;
	if (!rbpolygon.SelectPolyInfoForConv3D(strQuery, infos))
	{
		return NULL;
	}


	return new ResLoadPolygon3DInfos_V1(infos);


}

IMessage* WorkerHeightInfoFor3D::processing()
{
	const ReqLoadHeight3DInfos_V1* request = dynamic_cast<const ReqLoadHeight3DInfos_V1*>(m_request);
	if (!request) {
		return NULL;	// new ResponseError("요청변환실패");
	}

	//coconutre tree 의 로직 그대로 사용.(grs80만 소숫점)
	CString area_text;
	const CMapDoubleArea& area = request->m_area;
	if (request->m_ellipsoid == eEllipsoid::vGRS_80)
	{
		area_text.Format(_T("%f, %f, %f, %f"), area.LLong, area.LLat, area.RLong, area.ULat);
	}
	else 
	{
		area_text.Format(_T("%u, %u, %u, %u"), (DWORD)area.LLong, (DWORD)area.LLat, (DWORD)area.RLong, (DWORD)area.ULat);
	}

	// eHeightInfoLoad
	// LINK_OBJECT = 2,
	// POLYLINE_OBJECT = 8,
	
	CString strQuery;
	strQuery.Format(_T("call GP_HIGH_SEL_INTERSECTS(%d, %s)"), (int)request->m_load_type, area_text);
	
	CRBObject rbobject;
	rbobject.SetKairosServerName(GetAppOption().GetMainDBNameW().c_str());

	std::vector<out::Height3DInfo> infos;
	if (!rbobject.SelectObjHeightInfo(strQuery, infos))
	{
		return NULL;
	}
	
	return new ResLoadHeight3DInfos_V1(infos);
}
