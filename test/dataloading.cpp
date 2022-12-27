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
