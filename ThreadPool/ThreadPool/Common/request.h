#pragma once


#include "imessage.h"
#include "request_common.h"
#include "library/include_new/common/area.h"
#include <strsafe.h>
#include "ae_binary_oarchive.h"
#include "ae_binary_iarchive.h"

template <eNetID::T eid>
class Req_Int : public IMessage
{
public:	
	Req_Int() : IMessage(eid), m_number(0) {}
	Req_Int(int number) : IMessage(eid), m_number(number) {}

	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"number:%d", m_number);
		return ret_text;
	}

private:
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_number);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read(m_number);
	}

public:
	int m_number;
};

template <eNetID::T eid>
class Req_Ellipse_Area : public IMessage
{
public:	
	Req_Ellipse_Area() : IMessage(eid), m_ellipsoid(eEllipsoid::vGRS_80), m_area() {}
	Req_Ellipse_Area(eEllipsoid::T ellipsoid, const CMapDoubleArea& area)
		: IMessage(eid), m_ellipsoid(ellipsoid), m_area(area) {}
	
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"ellipsoid:%d, area(L:%.1f,B:%.1f,R:%.1f,T:%.1f)", 
			m_ellipsoid, m_area.LLong, m_area.LLat, m_area.RLong, m_area.ULat);
		return ret_text;
	}

private:
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_ellipsoid);
		archive.write_struct(m_area);
	}

	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read((int&)m_ellipsoid);
		archive.read_struct(m_area);
	}

public:
	eEllipsoid::T m_ellipsoid;
	CMapDoubleArea m_area;

};


template <eNetID::T eid, typename LoadType>
class Req_Ellipse_Loadtype_Area : public IMessage
{
public:	
	Req_Ellipse_Loadtype_Area() : IMessage(eid), m_ellipsoid(), m_load_type(), m_area() {}
	Req_Ellipse_Loadtype_Area(eEllipsoid::T ellipsoid, LoadType load_type, const CMapDoubleArea& area)
		: IMessage(eid), m_ellipsoid(ellipsoid), m_load_type(load_type), m_area(area) {}
	
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"ellipse:%d, load_type:%d, area(L:%.1f,B:%.1f,R:%.1f,T:%.1f)", 
			m_ellipsoid, m_load_type, m_area.LLong, m_area.LLat, m_area.RLong, m_area.ULat);
		return ret_text;
	}

private:
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_ellipsoid);
		archive.write(m_load_type);
		archive.write_struct(m_area);
	}

	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read((int&)m_ellipsoid);
		archive.read((int&)m_load_type);
		archive.read_struct(m_area);
	}

public:
	eEllipsoid::T m_ellipsoid;
	LoadType m_load_type;
	CMapDoubleArea m_area;

};

template <eNetID::T eid>
class Req_Mapid : public IMessage
{
public:	
	Req_Mapid() : IMessage(eid), m_mapid() {}
	Req_Mapid(const std::wstring& mapid)
		: IMessage(eid), m_mapid(mapid) {}
	
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"mapid:%s", m_mapid.c_str());
		return ret_text;
	}

private:
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_mapid);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read(m_mapid);
	}

public:
	std::wstring m_mapid;
};


template <eNetID::T eid, typename LoadType>
class Req_Loadtype_Mapid : public IMessage
{
public:	
	Req_Loadtype_Mapid() : IMessage(eid), m_load_type(), m_mapid() {}
	Req_Loadtype_Mapid(LoadType load_type, const std::wstring& mapid)
		: IMessage(eid), m_load_type(load_type), m_mapid(mapid) {}
	
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"load_type:%d, mapid:%s", m_load_type, m_mapid.c_str());
		return ret_text;
	}

private:
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_load_type);
		archive.write(m_mapid);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read((int&)m_load_type);
		archive.read(m_mapid);
	}

public:
	LoadType m_load_type;
	std::wstring m_mapid;
};

typedef Req_Int<eNetID::Req_Welcome_V1>														ReqWelcomeMsg_V1;
typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadPolylines_Area_V1, int>					ReqLoadPolylinesArea_V1;
typedef Req_Mapid<eNetID::Req_LoadPolylines_Mapid_V1>										ReqLoadPolylinesMapid_V1;

typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadPolygons_Area_V1, ePolygonLoad::T>		ReqLoadPolygonsArea_V1;
typedef Req_Loadtype_Mapid<eNetID::Req_LoadPolygons_Mapid_V1, ePGMapIdLoad::T>				ReqLoadPolygonsMapid_V1;


typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadNodes_Area_V1, eNodeAreaLoad::T>			ReqLoadNodesArea_V1;
typedef Req_Loadtype_Mapid<eNetID::Req_LoadNodes_Mapid_V1, eNodeMapidLoad::T>				ReqLoadNodesMapid_V1;

typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadLinks_Area_V1, eLinkAreaLoad::T>			ReqLoadLinksArea_V1;
typedef Req_Mapid<eNetID::Req_LoadLinks_Mapid_V1>											ReqLoadLinksMapid_V1;

typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadPassInfos_Area_V1, ePassInfoLoad::T>		ReqLoadPassInfosArea_V1;
typedef Req_Loadtype_Mapid<eNetID::Req_LoadPassInfos_Mapid_V1, ePassInfoLoad::T>			ReqLoadPassInfosMapid_V1;

typedef Req_Ellipse_Area<eNetID::Req_LoadPolygon3DInfos_V1>									ReqLoadPolygon3DInfos_V1;
typedef Req_Ellipse_Loadtype_Area<eNetID::Req_LoadHeight3DInfos_V1, eHeightInfoLoad::T>		ReqLoadHeight3DInfos_V1;
