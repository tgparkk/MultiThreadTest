void FirstDataLoader::load_from_cotree2()
{
	CKairosDBManager& kairosman = App::GetKairosDBManager();
	CString server_title = App::General::GetLoginInfo().GetTitle();
	
	std::string address;
	std::string cotree2_port;
	App::General::GetServerInfos().GetCotree2ConnInfo(server_title, address, cotree2_port);
	
	AsyncTcpClient client(1);
	client.SetEndpoints(address, cotree2_port); //개발은 7878, op는 7877로 세팅됨.
	//boost::this_thread::sleep_for(boost::chrono::seconds(100));

	kairosman.LoadObjStart(POLYLINE_OBJECT);
	kairosman.LoadObjStart(POLYGON_OBJECT);
	kairosman.LoadObjStart(LINK_OBJECT);
	kairosman.LoadObjStart(NODE_OBJECT);
	kairosman.LoadObjStart(MNS3D_POLYGON_ETCINFO);
	kairosman.LoadObjStart(MNS3D_HEIGHT_INFO);
	
	PERF_NAVI_START(ePerf_NAVI::LOAD_from_cotree2);
	PERF_NAVI_START(ePerf_NAVI::PolygonNewRequest);

	load_polyline(client);
	load_polygon(client);
	load_link(client);
	load_node(client);
	load_polygon_etcinfo(client);
	load_heights_link(client);
	load_heights_polyline(client);

	client.join_all();

	PERF_NAVI_END(ePerf_NAVI::LOAD_from_cotree2);

	this->merge_subinfos();

	kairosman.LoadObjComplete(POLYLINE_OBJECT);
	kairosman.LoadObjComplete(POLYGON_OBJECT);
	kairosman.LoadObjComplete(LINK_OBJECT);
	kairosman.LoadObjComplete(NODE_OBJECT);
	kairosman.LoadObjComplete(MNS3D_POLYGON_ETCINFO);
	kairosman.LoadObjComplete(MNS3D_HEIGHT_INFO);
}


void FirstDataLoader::load_polyline(AsyncTcpClient& client)
{
	CKairosDBManager& kairosman = App::GetKairosDBManager();

	
	//level 이 6이상일때, or  상위배경 access 권한이 있을 때.
	bool is_loadable = (kairosman.GetLevel() >= 6) || _Auth().GetWorkAuthority(AUTH_BG_UPPER);
	if (is_loadable) 
	{
		if (IsArea())
		{
			//(eEllipsoid::T)_Env().GetEllipsoid()
			CMapDoubleArea Area = (m_ellipsoid == eEllipsoid::vGRS_80) ? kairosman.GetMapAreaByMapID(m_strMapID) : kairosman.GetMapArea(m_strMapID);

			client.send_request(new ReqLoadPolylinesArea_V1(m_ellipsoid, kairosman.GetLevel(), Area), 
				boost::bind(&FirstDataLoader::OnRequestSelectPolylineNew, this, _1, _2));
		}
		else
		{
			client.send_request(new ReqLoadPolylinesMapid_V1(m_strMapID.GetString()), 
				boost::bind(&FirstDataLoader::OnRequestSelectPolylineNew, this, _1, _2));

		}
	}


}




void FirstDataLoader::OnRequestSelectPolylineNew(const IMessage* msg, const boost::system::error_code& ec)
{
	if (ec) {
		MessageBoxA(NULL, ec.message().c_str(), __FUNCTION__, MB_OK);
		return;
	}

	if (msg->msg_id() == eNetID::Res_Error_V1) {
		MessageBoxW(NULL, msg->to_string().c_str(), _T(__FUNCTION__), MB_OK);
		return;
	}

	if (msg->msg_id() != eNetID::Res_LoadPolylines_V1)
	{
		_ASSERT(false);
		return;
	}

	const ResLoadPolylines_V1* response = dynamic_cast<const ResLoadPolylines_V1*>(msg);	

	CGeoObjectList objs;

	const std::vector<out::Polyline>& datas = response->getdatas();

	for (int i = 0 ;i < (int)datas.size(); i++)
	{
		const out::Polyline& data = datas[i];

		CGeoPolyline* pPolyline = new CGeoPolyline();

		UNIONID	unionID;
		unionID.dwObjectID = data.m_id;
		unionID.strMapID = ConvMapIDDword2String(data.m_mapid);
		
		pPolyline->SetObjID( unionID );

		pPolyline->SetKind(data.m_kind);
		
		wchar_t w_bes[out::Polyline::ID_LEN] = {0, };
		conv_UTF16(&data.m_bdgid_bes[0], sizeof(data.m_bdgid_bes), w_bes, sizeof(w_bes));
		pPolyline->Set3DBdgID(w_bes, BESSEL); //Cstring 내부에서 utf16변환
		
		wchar_t w_grs[out::Polyline::ID_LEN] = {0, };
		conv_UTF16(&data.m_bdgid_grs[0], sizeof(data.m_bdgid_grs), w_grs, sizeof(w_grs));

		pPolyline->Set3DBdgID(w_grs, GRS80); //CString 내부에서 utf16변환

		if (!data.m_xy_bes.empty()) {
			pPolyline->ConvWKBToGeoType((BYTE*)&data.m_xy_bes[0], BESSEL);
		}

		if (!data.m_xy_grs.empty()) {
			pPolyline->ConvWKBToGeoType((BYTE*)&data.m_xy_grs[0], GRS80);
		}
		
		pPolyline->SetSyncFlag((BYTE)data.m_syncflag);

		objs.Add( pPolyline );
	}

	if (eLoadVerifyTest::Use)
	{
		CGeoObjectList objsB;
		App::GetDataCommManager().GetPolylineDataByMapID(&objsB, m_strMapID);
		if (!util::is_equal_test(eEqualTest::polyline, objs, objsB)) 
		{
			ASSERT(false); //검증실패
			AELOG("결과값이 다릅니다.");
		}
	}
	
	CKairosDBManager& kairosman = App::GetKairosDBManager();
	CGeoObjectTable* pGeoTable = kairosman.CreateObjectTable(POLYLINE_OBJECT);
	if (pGeoTable)
	{
		objs.deleteList(pGeoTable->GetObjList());
		pGeoTable->lm_AddList(objs);
		pGeoTable->QSort();
	}
}
