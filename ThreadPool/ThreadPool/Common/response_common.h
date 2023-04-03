#pragma once


#include <sstream>
#include <vector>
#include <boost/array.hpp>
#include "ae_binary_iarchive.h"
#include "ae_binary_oarchive.h"

// class ResPolygon;
// class ResPolyline;

//typedef std::vector<BYTE, boost::pool_allocator<BYTE>> vector_byte;
//typedef std::vector<ResPolyline, boost::pool_allocator<ResPolyline>> vector_ResPolyline;
//typedef std::vector<ResPolygon, boost::pool_allocator<ResPolygon>> vector_ResPolygon;

// typedef std::vector<BYTE, boost::fast_pool_allocator<BYTE>> vector_byte;
// typedef std::vector<ResPolyline, boost::fast_pool_allocator<ResPolyline>> vector_ResPolyline;
// typedef std::vector<ResPolygon, boost::fast_pool_allocator<ResPolygon>> vector_ResPolygon;

typedef std::vector<BYTE> vector_byte;
//typedef std::vector<ResPolyline> vector_ResPolyline;
//typedef std::vector<ResPolygon> vector_ResPolygon;

namespace out
{
class Polyline
{
public:
	Polyline() : m_id(0), m_mapid(0), m_kind(0), m_bdgid_bes(), m_bdgid_grs(), m_xy_bes(), m_xy_grs(), m_syncflag(0) {}
	
	enum {ID_LEN = 12,}; //4의 배수로 맞춤.
	
	long m_id;
	long m_mapid;
	long m_kind;				// short 가능
	boost::array<char, ID_LEN> m_bdgid_bes;	// 3d빌딩id 베셀
	boost::array<char, ID_LEN> m_bdgid_grs;	// 3d빌딩id GRS
	vector_byte m_xy_bes;	
	vector_byte m_xy_grs;
	long m_syncflag;			// byte 가능
	
	void on_save_to(ae_binary_oarchive & ar) const
	{
		ar.write(m_id);
		ar.write(m_mapid);
		ar.write(m_kind);
		ar.write_buf(&m_bdgid_bes[0], sizeof(m_bdgid_bes));
		ar.write_buf(&m_bdgid_grs[0], sizeof(m_bdgid_grs));
		ar.write_iteratable_dump(m_xy_bes);
		ar.write_iteratable_dump(m_xy_grs);
		ar.write(m_syncflag);
	}

	void on_load_from(ae_binary_iarchive & ar)
	{
		ar.read(m_id);
		ar.read(m_mapid);
		ar.read(m_kind);
		ar.read_buf(&m_bdgid_bes[0], sizeof(m_bdgid_bes));
		ar.read_buf(&m_bdgid_grs[0], sizeof(m_bdgid_grs));
		ar.read_iteratable_dump(m_xy_bes);
		ar.read_iteratable_dump(m_xy_grs);
		ar.read(m_syncflag);
	}
};

class Polygon
{
public:
	Polygon() : m_id(0), m_mapid(0), m_kind(0), m_upper_level(0), m_lane_code(), m_arrow_code(), m_bd_floor(0), m_xy_bes(), m_xy_grs(), m_syncflag(0) {}

	enum {CODE_LEN = 20,}; //4의 배수로 맞춤.

	long m_id;
	long m_mapid;
	long m_kind;				// short 가능
	long m_upper_level;			// byte 가능
	boost::array<char, CODE_LEN> m_lane_code;	// DB spec : 16
	boost::array<char, CODE_LEN> m_arrow_code;	// DB spec : 16
	long m_bd_floor;			// byte 가능
	vector_byte m_xy_bes;	
	vector_byte m_xy_grs;
	long m_syncflag;			// byte 가능

	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(m_id);
		archive.write(m_mapid);
		archive.write(m_kind);
		archive.write(m_upper_level);
		archive.write_buf(&m_lane_code[0], sizeof(m_lane_code));
		archive.write_buf(&m_arrow_code[0], sizeof(m_arrow_code));
		archive.write(m_bd_floor);
		archive.write_iteratable_dump(m_xy_bes);
		archive.write_iteratable_dump(m_xy_grs);
		archive.write(m_syncflag);
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(m_id);
		archive.read(m_mapid);
		archive.read(m_kind);
		archive.read(m_upper_level);
		archive.read_buf(&m_lane_code[0], sizeof(m_lane_code));
		archive.read_buf(&m_arrow_code[0], sizeof(m_arrow_code));
		archive.read(m_bd_floor);
		archive.read_iteratable_dump(m_xy_bes);
		archive.read_iteratable_dump(m_xy_grs);
		archive.read(m_syncflag);
	}

};


class Link
{
public:
	Link() 
		: m_id(0), 
		m_mapid(0),
		m_st_node_id(0),
		m_ed_node_id(0),
		m_st_dir_bes(0),
		m_ed_dir_bes(0),
		m_st_dir_grs(0),
		m_ed_dir_grs(0),
		m_road_kind(0),
		m_link_kind_code(0),
		m_oneway(0),

		m_road_no(0),
		m_lane_num(0),
		m_level1(0),
		m_level2(0),
		m_max_speed_limit(0),
		m_level3(0),
		m_facility_code_p(0),
		m_facility_code_n(0),

		m_link_xy_bes(),
		m_link_xy_grs(),

		m_is_ctt(0),
		m_line_p_yn(0),
		m_line_p_xy_bes(),
		m_line_p_xy_grs(),
		m_line_n_yn(0),
		m_line_n_xy_bes(),
		m_line_n_xy_grs(),

		m_is_reallink(0),
		m_reallink_p_yn(0),
		m_reallink_p_xy_grs(),
		m_reallink_n_yn(0),
		m_reallink_n_xy_grs(),
		m_sync_flag(0)
	{
		ZeroMemory(m_road_name, sizeof(m_road_name));
		ZeroMemory(m_line_p_id, sizeof(m_line_p_id));
		ZeroMemory(m_line_n_id, sizeof(m_line_n_id));
	}


	enum {TEXT_LEN = 30,}; //4의 배수로 맞춤.

	long m_id;
	long m_mapid;
	long m_st_node_id;
	long m_ed_node_id;
	BYTE m_st_dir_bes;
	BYTE m_ed_dir_bes;
	BYTE m_st_dir_grs;
	BYTE m_ed_dir_grs;
	BYTE m_road_kind;
	short m_link_kind_code;
	BYTE m_oneway;	
	
	long m_road_no;
	BYTE m_lane_num;
	char m_road_name[TEXT_LEN];	
	BYTE m_level1;
	BYTE m_level2;
	BYTE m_max_speed_limit; 
	BYTE m_level3;
	BYTE m_facility_code_p;
	BYTE m_facility_code_n;

	vector_byte m_link_xy_bes;	
	vector_byte m_link_xy_grs;

	BYTE m_is_ctt;
	BYTE m_line_p_yn;
	char m_line_p_id[TEXT_LEN];
	vector_byte m_line_p_xy_bes;
	vector_byte m_line_p_xy_grs;
	BYTE m_line_n_yn;
	char m_line_n_id[TEXT_LEN];
	vector_byte m_line_n_xy_bes;
	vector_byte m_line_n_xy_grs;

	BYTE m_is_reallink;
	BYTE m_reallink_p_yn;
	vector_byte m_reallink_p_xy_grs;
	BYTE m_reallink_n_yn;
	vector_byte m_reallink_n_xy_grs;
	
	BYTE m_sync_flag;
	

	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(m_id);
		archive.write(m_mapid);
		archive.write(m_st_node_id);
		archive.write(m_ed_node_id);
		archive.write(m_st_dir_bes);
		archive.write(m_ed_dir_bes);
		archive.write(m_st_dir_grs);
		archive.write(m_ed_dir_grs);
		archive.write(m_road_kind);
		archive.write(m_link_kind_code);
		archive.write(m_oneway);
		
		archive.write(m_road_no);
		archive.write(m_lane_num);
		archive.write_buf(m_road_name, sizeof(m_road_name));
		archive.write(m_level1);
		archive.write(m_level2);
		archive.write(m_max_speed_limit);
		archive.write(m_level3);
		archive.write(m_facility_code_p);
		archive.write(m_facility_code_n);
		
		archive.write_iteratable_dump(m_link_xy_bes);
		archive.write_iteratable_dump(m_link_xy_grs);

		archive.write(m_is_ctt);
		archive.write(m_line_p_yn);
		archive.write_buf(m_line_p_id, sizeof(m_line_p_id));
		archive.write_iteratable_dump(m_line_p_xy_bes);
		archive.write_iteratable_dump(m_line_p_xy_grs);
		archive.write(m_line_n_yn);
		archive.write_buf(m_line_n_id, sizeof(m_line_n_id));
		archive.write_iteratable_dump(m_line_n_xy_bes);
		archive.write_iteratable_dump(m_line_n_xy_grs);

		archive.write(m_is_reallink);
		archive.write(m_reallink_p_yn);
		archive.write_iteratable_dump(m_reallink_p_xy_grs);
		archive.write(m_reallink_n_yn);
		archive.write_iteratable_dump(m_reallink_n_xy_grs);
		archive.write(m_sync_flag);
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(m_id);
		archive.read(m_mapid);
		archive.read(m_st_node_id);
		archive.read(m_ed_node_id);
		archive.read(m_st_dir_bes);
		archive.read(m_ed_dir_bes);
		archive.read(m_st_dir_grs);
		archive.read(m_ed_dir_grs);
		archive.read(m_road_kind);
		archive.read(m_link_kind_code);
		archive.read(m_oneway);

		archive.read(m_road_no);
		archive.read(m_lane_num);
		archive.read_buf(m_road_name, sizeof(m_road_name));
		archive.read(m_level1);
		archive.read(m_level2);
		archive.read(m_max_speed_limit);
		archive.read(m_level3);
		archive.read(m_facility_code_p);
		archive.read(m_facility_code_n);

		archive.read_iteratable_dump(m_link_xy_bes);
		archive.read_iteratable_dump(m_link_xy_grs);

		archive.read(m_is_ctt);
		archive.read(m_line_p_yn);
		archive.read_buf(m_line_p_id, sizeof(m_line_p_id));
		archive.read_iteratable_dump(m_line_p_xy_bes);
		archive.read_iteratable_dump(m_line_p_xy_grs);
		archive.read(m_line_n_yn);
		archive.read_buf(m_line_n_id, sizeof(m_line_n_id));
		archive.read_iteratable_dump(m_line_n_xy_bes);
		archive.read_iteratable_dump(m_line_n_xy_grs);

		archive.read(m_is_reallink);
		archive.read(m_reallink_p_yn);
		archive.read_iteratable_dump(m_reallink_p_xy_grs);
		archive.read(m_reallink_n_yn);
		archive.read_iteratable_dump(m_reallink_n_xy_grs);
		archive.read(m_sync_flag);
	}

};

class Node
{
public:
	Node() : m_id(0), m_mapid(0), m_rep_node_id(), m_kind(0), m_xy_bes(), m_xy_grs(), m_syncflag(0) 
	{
		ZeroMemory(m_name, sizeof(m_name));
		ZeroMemory(m_linkids, sizeof(m_linkids));
	}

	enum {CODE_LEN = 30,}; //4의 배수로 맞춤.

	long m_id;
	long m_mapid;
	long m_rep_node_id;	
	char m_name[CODE_LEN];
	BYTE m_kind;
	vector_byte m_xy_bes;	
	vector_byte m_xy_grs;
	long m_linkids[8]; 
	BYTE m_syncflag;
	
	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(m_id);
		archive.write(m_mapid);
		archive.write(m_rep_node_id);
		archive.write_buf(&m_name[0], sizeof(m_name));
		archive.write(m_kind);
		archive.write_iteratable_dump(m_xy_bes);
		archive.write_iteratable_dump(m_xy_grs);
		archive.write_buf((const char*)&m_linkids[0], sizeof(m_linkids)); //이전 소스에서 이전 id 와 같으면 0 으로 세팅하고 있엇음.
		archive.write(m_syncflag);
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(m_id);
		archive.read(m_mapid);
		archive.read(m_rep_node_id);
		archive.read_buf(&m_name[0], sizeof(m_name));
		archive.read(m_kind);
		archive.read_iteratable_dump(m_xy_bes);
		archive.read_iteratable_dump(m_xy_grs);
		archive.read_buf((char*)&m_linkids[0], sizeof(m_linkids)); //이전 소스에서 이전 id 와 같으면 0 으로 세팅하고 있엇음.
		archive.read(m_syncflag);
	}

};

class PassInfo
{
public:
	PassInfo() : m_id(0), m_mapid(0), m_dt_in_link_id(0), m_dt_out_link_id(0), m_dt_d_kind(0)
	{
		
	}

	//enum {CODE_LEN = 30,}; //4의 배수로 맞춤.

	long m_id;
	long m_mapid;
	long m_dt_in_link_id;
	long m_dt_out_link_id;
	BYTE m_dt_d_kind;
	BYTE m_dt_t_kind;
	short m_dt_ttti_id;

	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(m_id);
		archive.write(m_mapid);
		archive.write(m_dt_in_link_id);
		archive.write(m_dt_out_link_id);
		archive.write(m_dt_d_kind);
		archive.write(m_dt_t_kind);
		archive.write(m_dt_ttti_id);
		
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(m_id);
		archive.read(m_mapid);
		archive.read(m_dt_in_link_id);
		archive.read(m_dt_out_link_id);
		archive.read(m_dt_d_kind);
		archive.read(m_dt_t_kind);
		archive.read(m_dt_ttti_id);
		
	}

};

class Polygon3DInfo
{
public:
	Polygon3DInfo() : dwPGId(0), dwMapId(0), sz3DBdgID(), szPatternCode(), szArrowCode() {}

	long dwPGId;				// 폴리곤 ID
	long dwMapId;			// 도엽 ID
	boost::array<wchar_t, 10> sz3DBdgID;		// 3D 빌딩 ID
	boost::array<wchar_t, 30> szPatternCode;	// 패턴형 교차로 패턴 ID
	boost::array<wchar_t, 30> szArrowCode;		// 패턴형 교차로 화살표 ID

	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(dwPGId);
		archive.write(dwMapId);
		archive.write_buf((const char*)&sz3DBdgID[0], sizeof(sz3DBdgID));
		archive.write_buf((const char*)&szPatternCode[0], sizeof(szPatternCode));
		archive.write_buf((const char*)&szArrowCode[0], sizeof(szArrowCode));
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(dwPGId);
		archive.read(dwMapId);
		archive.read_buf((char*)&sz3DBdgID[0], sizeof(sz3DBdgID));
		archive.read_buf((char*)&szPatternCode[0], sizeof(szPatternCode));
		archive.read_buf((char*)&szArrowCode[0], sizeof(szArrowCode));
	}
};


struct GeoHeight
{
	GeoHeight() : rHeight(0.0), rDEMHeight(0.0) {}
	double rHeight;		// 0기준 상대 높이값
	double rDEMHeight;	// DEM기준 상대 높이값
};

class Height3DInfo
{
public:
	Height3DInfo() : dwObjId(0), dwMapId(0), vecHeight() 
	{
		ZeroMemory(szB3DId, sizeof(szB3DId));
	}

	long dwObjId;		
	long dwMapId;		
	char szB3DId[15];	
	std::vector<GeoHeight> vecHeight;

	void on_save_to(ae_binary_oarchive & archive) const
	{
		archive.write(dwObjId);
		archive.write(dwMapId);
		archive.write_buf((const char*)szB3DId, sizeof(szB3DId));
		archive.write_iteratable_dump(vecHeight);
	}

	void on_load_from(ae_binary_iarchive & archive)
	{
		archive.read(dwObjId);
		archive.read(dwMapId);
		archive.read_buf((char*)szB3DId, sizeof(szB3DId));
		archive.read_iteratable_dump(vecHeight);
	}
};


}
