#pragma once

#include <sstream>
#include <string>

class ae_binary_iarchive
{
public:
	ae_binary_iarchive(std::istringstream& stream) : m_stream(stream) {}

	void read(char& value) {stream_read<char>(value);}
	void read(BYTE& value) {stream_read<BYTE>(value);}
	void read(short& value) {stream_read<short>(value);}
	void read(USHORT& value) {stream_read<USHORT>(value);}
	void read(int& value) {stream_read<int>(value);}
	void read(UINT& value) {stream_read<UINT>(value);}
	void read(long& value) {stream_read<long>(value);}
	void read(ULONG& value) {stream_read<ULONG>(value);}
	void read(float& value) {stream_read<float>(value);}
	void read(double& value) {stream_read<double>(value);}
	
	void read(std::string& value)
	{
		read_iteratable_dump(value);
	}

	void read(std::wstring& value)
	{
		read_iteratable_dump(value);
	}

	template<class POD>
	void read_struct(POD& value) {
		stream_read<POD>(value);	
	};

	void read_buf(char* buf, int size) {
		m_stream.read(buf, size);	
	}

	template<class Iteratorable>
	void read_iteratable(Iteratorable& datas)
	{
		int count = 0;
		stream_read<int>(count);
		datas.resize(count);
		for (Iteratorable::iterator it = datas.begin(); it != datas.end(); it++)
		{
			it->on_load_from(*this);
		}
	}

	template<class Iteratorable>
	void read_iteratable_dump(Iteratorable& datas)
	{
		int count = 0;
		stream_read<int>(count);
		datas.resize(count);
		if (count > 0) {
			m_stream.read((char*)&datas[0], count * sizeof(Iteratorable::value_type));
		}
	}
	
private:
	template<class POD>
	void stream_read(POD& value) {
		m_stream.read((char*)&value, sizeof(POD));
	}

private:
	
	std::istringstream& m_stream;
};


