#include "stdafx.h"
#include "imessage.h"
#include <strsafe.h>
#include <sstream>
#include "zlibmanager.h"
#include "util_utf_conv.h"
#include "ae_binary_oarchive.h"
#include "ae_binary_iarchive.h"
#include "LzmaLib.h"
#include <boost/chrono.hpp>

namespace ch = boost::chrono;

//압축방식 지정.

namespace eCompAlgo
{
	enum T
	{
		zlib,	// zlib 압축알고리즘
		lzma,	// 7zip 압축알고리즘
	};
}

//압축테스트 결과 (폴리곤 54메가(54,822,298)) (64비트가 좀더 빨라 64비트를 사용)
//
// 압축종류		크기		시간
//
// zlib_1		26631127	0.912411
// zlib_9		25923267	7.169681
//
// lzma_0		23571920	1.868200  <-- 시간 대비 압축율 높아 선택
// lzma_1		23674365	2.386392
// lzma_2		23820781	3.572940
// lzma_3		24035350	5.927576
// lzma_4		24206762	6.553409
// lzma_5(def)	22149899	10.422174
// lzma_9		21784248	13.895567

void IMessage::save_to(OUT std::string& buf) const
{
	std::ostringstream stream;
	ae_binary_oarchive archive(stream);
	on_save_to(archive);
	
	std::string ori_buf = stream.str();
	int ori_size = (int)ori_buf.size();
	if (ori_size > m_comp_start_size)
	{
		eCompAlgo::T com_type = eCompAlgo::lzma;
		
		if (com_type == eCompAlgo::zlib) //comp_zip
		{
			ULONG comp_buf_size = CZlibManager::GetOutBufferSize(ori_size);
			std::vector<BYTE> comp_buf(comp_buf_size, 0); // resize
			
			//ch::system_clock::time_point zlib_start = ch::system_clock::now();
			CZlibManager::Compress(&comp_buf[0], &comp_buf_size, (BYTE*)&ori_buf[0], ori_size);
			//ch::duration<double> zlib_gap = ch::system_clock::now() - zlib_start;

			comp_buf.resize(comp_buf_size);
			
			BYTE* size_begin = (BYTE*)&ori_size;

			//압축시 버퍼 구조
			buf.push_back('Z'); //첫문자에 압축종류(L)
			buf.append(size_begin, size_begin + sizeof(int)); //원본크기
			buf.append(comp_buf.begin(), comp_buf.end()); //serizliaed 데이터
		}
		else if (com_type == eCompAlgo::lzma)
		{
			size_t comp_buf_sizeB = (size_t)(ori_size * 1.2f);
			if (comp_buf_sizeB < 1024) {comp_buf_sizeB = 1024;}

			std::vector<BYTE> comp_bufB(comp_buf_sizeB, 0); // resize

			size_t propsSize = LZMA_PROPS_SIZE;

			//https://www.asawicki.info/news_1368_lzma_sdk_-_how_to_use
			//전체버퍼의 앞5바이트를 prop 버퍼로 사용
			//ch::system_clock::time_point lzma_start = ch::system_clock::now();
			int res = LzmaCompress(
				&comp_bufB[LZMA_PROPS_SIZE], &comp_buf_sizeB, 
				(BYTE*)&ori_buf[0], ori_size, 
				&comp_bufB[0], &propsSize, 
				0, 0, -1, -1, -1, -1, -1); // 0 LEVEL, 가장빠르게

			//ch::duration<double> lzma_gap = ch::system_clock::now() - lzma_start;
			
			assert(propsSize == LZMA_PROPS_SIZE);
			assert(res == SZ_OK);

			comp_bufB.resize(propsSize + comp_buf_sizeB);

			//AELOG("** comp, ori:%d, zip:%d(t:%f), lzma:%d(t:%f)", ori_size, comp_buf.size(), zlib_gap, comp_bufB.size(), lzma_gap);

			BYTE* size_begin = (BYTE*)&ori_size;

			//압축시 버퍼 구조
			buf.push_back('L'); //첫문자에 압축종류(Z)
			buf.append(size_begin, size_begin + sizeof(int)); //원본크기
			buf.append(comp_bufB.begin(), comp_bufB.end()); //serizliaed 데이터
		}
		else 
		{
			ASSERT(false);
		}
	}
	else
	{
		//비압축시 버퍼 구조
		buf.push_back('N'); //첫문자에 압축여부
		buf.append(ori_buf.begin(), ori_buf.end()); //serizliaed 데이터
	}

	//AELOG("Message 직렬화(저장)에 실패했습니다.");
}

void IMessage::load_from(const std::string& buf)
{
	if (buf.empty()) {
		ASSERT(false);
		return;
	}

	std::istringstream stream(buf);
	char comp = stream.get();
	if (comp == 'N')
	{
		ae_binary_iarchive archive(stream);
		on_load_from(archive);	
	}
	else if (comp == 'Z')
	{
		int ori_size;
		stream.read((char*)&ori_size, sizeof(int));

		//나머지는 압축된 버퍼로 압축해제.
		std::vector<BYTE> comp_buf(buf.size() - stream.tellg(), 0);
		stream.read((char*)&comp_buf[0], comp_buf.size());

		std::string ori_buf(ori_size, 0);
		ULONG ori_buf_size = (ULONG)ori_buf.size();
		CZlibManager::UnCompress((BYTE*)&ori_buf[0], &ori_buf_size, &comp_buf[0], (ULONG)comp_buf.size());
		ASSERT(ori_buf_size == ori_buf.size());
		
		std::istringstream ori_stream(ori_buf);
		ae_binary_iarchive ori_archive(ori_stream);
		on_load_from(ori_archive);	
	}
	else if (comp == 'L')
	{
		int ori_size;
		stream.read((char*)&ori_size, sizeof(int));

		//나머지는 압축된 버퍼로 압축해제.
		std::vector<BYTE> comp_buf(buf.size() - stream.tellg(), 0);
		stream.read((char*)&comp_buf[0], comp_buf.size());

		std::string ori_buf(ori_size, 0);
		size_t ori_buf_size = ori_buf.size();
		
		//https://www.asawicki.info/news_1368_lzma_sdk_-_how_to_use

		size_t comp_src_len = comp_buf.size() - LZMA_PROPS_SIZE; // 첫5바이트는 prop

		ch::system_clock::time_point uncomp_start = ch::system_clock::now();
		SRes res = LzmaUncompress(
			(BYTE*)&ori_buf[0], &ori_buf_size,
			&comp_buf[LZMA_PROPS_SIZE], &comp_src_len,
			&comp_buf[0], LZMA_PROPS_SIZE);
		ch::duration<double> lzma_gap = ch::system_clock::now() - uncomp_start;
		AELOG("** uncomp, lzma:%d, ori:%d(t:%f)", comp_buf.size(), ori_buf_size, lzma_gap);

		assert(res == SZ_OK);

		//CZlibManager::UnCompress((BYTE*)&ori_buf[0], &ori_buf_size, &comp_buf[0], (ULONG)comp_buf.size());
		ASSERT(ori_buf_size == ori_buf.size());

		std::istringstream ori_stream(ori_buf);
		ae_binary_iarchive ori_archive(ori_stream);
		on_load_from(ori_archive);	
	}
	else
	{
		ASSERT(false);
		AELOG("Message 직렬화(로딩)에 실패했습니다.");
	}
}
