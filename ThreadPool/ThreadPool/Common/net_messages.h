#pragma once

#include "EnumStringW.h"

// PG : polygon
// PL : polyline
// LK : link
// ND : node


namespace eNetID
{
	//1,16,256, 4096, 65536
	enum T
	{
		None,

		//Request
		START_Request				= 100000,
		Req_Welcome_V1				= 100101,
		Req_LoadPolylines_Area_V1	= 100201,
		Req_LoadPolylines_Mapid_V1	= 100301,
		Req_LoadPolygons_Area_V1	= 100401,
		Req_LoadPolygons_Mapid_V1	= 100501,
		Req_LoadLinks_Area_V1		= 100601,
		Req_LoadLinks_Mapid_V1		= 100701,
		Req_LoadNodes_Area_V1		= 100801,
		Req_LoadNodes_Mapid_V1		= 100901,
		Req_LoadPassInfos_Area_V1	= 101001,
		Req_LoadPassInfos_Mapid_V1	= 101101,
		Req_LoadPolygon3DInfos_V1	= 101201,
		Req_LoadHeight3DInfos_V1	= 101301,

		//Response
		START_Response				= 500000,
		Res_Welcome_V1				= 500101,
		Res_Error_V1				= 500201,
		Res_LoadPolylines_V1		= 500301,
		Res_LoadPolygons_V1			= 500401,
		Res_LoadLinks_V1			= 500501,
		Res_LoadNodes_V1			= 500601,
		Res_LoadPassinfos_V1		= 500701,
		Res_LoadPolygon3DInfos_V1	= 500801,
		Res_LoadHeight3DInfos_V1	= 500901,
		//END_Response,
	
		Max
	};
}

namespace eNetID
{
	Begin_Enum_StringW(T)
	{
		Enum_StringW(None				);

		Enum_StringW(START_Request				);
		Enum_StringW(Req_Welcome_V1				);
		Enum_StringW(Req_LoadPolylines_Area_V1	);
		Enum_StringW(Req_LoadPolylines_Mapid_V1	);
		Enum_StringW(Req_LoadPolygons_Area_V1	);
		Enum_StringW(Req_LoadPolygons_Mapid_V1	);
		Enum_StringW(Req_LoadLinks_Area_V1		);
		Enum_StringW(Req_LoadLinks_Mapid_V1		);
		Enum_StringW(Req_LoadNodes_Area_V1		);
		Enum_StringW(Req_LoadNodes_Mapid_V1		);
		Enum_StringW(Req_LoadPassInfos_Area_V1	);
		Enum_StringW(Req_LoadPassInfos_Mapid_V1	);
		Enum_StringW(Req_LoadPolygon3DInfos_V1	);	
		Enum_StringW(Req_LoadHeight3DInfos_V1	);	

		Enum_StringW(START_Response				);
		Enum_StringW(Res_Welcome_V1				);
		Enum_StringW(Res_Error_V1				);
		Enum_StringW(Res_LoadPolylines_V1		);
		Enum_StringW(Res_LoadPolygons_V1		);
		Enum_StringW(Res_LoadLinks_V1			);
		Enum_StringW(Res_LoadNodes_V1			);
		Enum_StringW(Res_LoadPassinfos_V1		);
		Enum_StringW(Res_LoadPolygon3DInfos_V1		);
		Enum_StringW(Res_LoadHeight3DInfos_V1	);

	}
	End_Enum_StringW;
}

