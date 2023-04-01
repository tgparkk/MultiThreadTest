#pragma once


#include "imessage.h"
#include "response_common.h"
#include "ae_binary_oarchive.h"
#include "ae_binary_iarchive.h"

#include <sstream>
#include <vector>
#include <boost/array.hpp>
#include <strsafe.h>
#include <string>

template <eNetID::T eid>
class Res_wstring : public IMessage
{
public:
	Res_wstring() : IMessage(eid), m_text() {}
	Res_wstring(const std::wstring& text) : IMessage(eid), m_text(text) {}
	virtual ~Res_wstring() {}
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"error:%s", m_text.c_str());
		return ret_text;
	}
	
	const std::wstring& getdatas() const {return m_text;}

private:
	//template method
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write(m_text);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read(m_text);
	}

private:
	std::wstring m_text;
};



template <eNetID::T eid, typename Item>
class Res_vector : public IMessage
{
public:
	Res_vector() : IMessage(eid), m_objs() {}
	Res_vector(const std::vector<Item>& objs) : IMessage(eid), m_objs(objs) {}
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"count:%d", (int)m_objs.size());
		return ret_text;
	}

	const std::vector<Item>& getdatas() const {return m_objs;}
private:
	//template method
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write_iteratable(m_objs);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read_iteratable(m_objs);
	}

private:
	std::vector<Item> m_objs;

};



// read/write 시에 vector item 단위로 저장하는게 아니라 갯수만큼의 메모리공간을 한번에 처리.
template <eNetID::T eid, typename Item>
class Res_vector_dump : public IMessage
{
public:
	Res_vector_dump() : IMessage(eid), m_objs() {}
	Res_vector_dump(const std::vector<Item>& objs) : IMessage(eid), m_objs(objs) {}
	virtual std::wstring to_string() const
	{
		wchar_t ret_text[256] = {0,};
		StringCbPrintfW(ret_text, sizeof(ret_text), L"count:%d", (int)m_objs.size());
		return ret_text;
	}

	const std::vector<Item>& getdatas() const {return m_objs;}
private:
	//template method
	void on_save_to(ae_binary_oarchive& archive) const
	{
		archive.write_iteratable_dump(m_objs);
	}
	void on_load_from(ae_binary_iarchive& archive)
	{
		archive.read_iteratable_dump(m_objs);
	}

private:
	std::vector<Item> m_objs;

};

typedef Res_wstring<eNetID::Res_Welcome_V1>									ResWelcome_V1;
typedef Res_wstring<eNetID::Res_Error_V1>									ResError_V1;

typedef Res_vector<eNetID::Res_LoadPolygons_V1, out::Polygon>				ResLoadPolygons_V1;
typedef Res_vector<eNetID::Res_LoadPolylines_V1, out::Polyline>				ResLoadPolylines_V1;

typedef Res_vector<eNetID::Res_LoadLinks_V1, out::Link>						ResLoadLinks_V1;
typedef Res_vector<eNetID::Res_LoadNodes_V1, out::Node>						ResLoadNodes_V1;
typedef Res_vector<eNetID::Res_LoadPassinfos_V1, out::PassInfo>				ResLoadPassInfos_V1;

typedef Res_vector<eNetID::Res_LoadPolygon3DInfos_V1, out::Polygon3DInfo>	ResLoadPolygon3DInfos_V1;
typedef Res_vector<eNetID::Res_LoadHeight3DInfos_V1, out::Height3DInfo>		ResLoadHeight3DInfos_V1;

