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
