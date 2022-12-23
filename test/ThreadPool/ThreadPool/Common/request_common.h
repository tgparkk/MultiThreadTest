#pragma once

#include "ae_binary_iarchive.h"
#include "ae_binary_oarchive.h"

namespace eEllipsoid
{
	enum T
	{
		vWGS_84 = 0,
		vBESSEL = 1,
		vGRS_80 = 2,
	};
}

namespace ePolygonLoad
{
	enum T
	{
		Level_1 = 1,
		Level_2 = 2,
		Level_3 = 3,
		Level_4 = 4,
		Level_5 = 5,
		Level_6 = 6,
		Level_7 = 7,
		Level_8 = 8,
		UP_6LEVEL = 217, // PG_UP_6LEVEL_AREA
	};
}

namespace ePGMapIdLoad
{
	enum T
	{
		TABLE_MAPID = 211,		// PG_TABLE_MAPID,
		HOUSE_MAPID = 214,		// PG_HOUSE_MAPID
		AUTOFIX_MAPID = 232,	// PG_AUTOFIX_MAPID
	};
}

namespace eLinkAreaLoad
{
	enum T
	{
		None    = 0,
// 		Level_1 = 1,
// 		Level_2 = 2,
		Level_3 = 3,
		Level_4 = 4,
		Level_5 = 5,
		Level_6 = 6,
		Level_7 = 7,
		Level_8 = 8,
	};
}

namespace eNodeAreaLoad
{
	enum T
	{
		node_KAIROS_ST_INTERSECTS,		// const int KAIROS_ST_INTERSECTS = 1;	// 포함 + 교차 객체
		node_COMMECIAL_VEHICLE_DT_DRAW,	//COMMECIAL_VEHICLE_DT_DRAW = 149,		//! 20190118 P0235(ysjang@): 상용차
		pednode_KAIROS_ST_INTERSECTS,

	};
}

namespace eNodeMapidLoad
{
	enum T
	{
		node_ND_TABLE_MAPID,			// const int ND_TABLE_MAPID	=	SP_ND + 11;(300 + 11)			//(node_map_id)선택 도엽 노드 
		pednode_ND_TABLE_MAPID,			
	};
}

namespace ePassInfoLoad
{
	enum T
	{
		node,
		ped_node,
	};
}

namespace eHeightInfoLoad
{
	enum T
	{
		link		= 2, // LINK_OBJECT,
		polyline	= 8, // POLYLINE_OBJECT,
	};

}
