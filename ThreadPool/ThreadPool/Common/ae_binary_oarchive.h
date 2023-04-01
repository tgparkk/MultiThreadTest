#pragma once

#include <sstream>
#include <string>
#include <boost/array.hpp>

class ae_binary_oarchive
{
public:
	ae_binary_oarchive(std::ostringstream& stream) : m_stream(stream) {}

	void write(char value)		{stream_write<char>(value);}
	void write(UCHAR value)		{stream_write<UCHAR>(value);}
	void write(short value)		{stream_write<short>(value);}
	void write(USHORT value)	{stream_write<USHORT>(value);}
	void write(int value)		{stream_write<int>(value);}
	void write(UINT value)		{stream_write<UINT>(value);}
	void write(long value)		{stream_write<long>(value);}
	void write(ULONG value)		{stream_write<ULONG>(value);}
	void write(float value)		{stream_write<float>(value);}
	void write(double value)	{stream_write<double>(value);}

	void write(const std::string& value)
	{
		write_iteratable_dump(value);
	}

	void write(const std::wstring& value)
	{
		write_iteratable_dump(value);
	}

	template<class POD>
	void write_struct(const POD& value) {
		stream_write<POD>(value);	
	};

	void write_buf(const char* buf, int cbsize) {
		if (cbsize > 0) {
			m_stream.write((char*)buf, cbsize);
		}
	};


	template<class Iteratorable>
	void write_iteratable(const Iteratorable& datas)
	{
		int count = (int)datas.size();
		stream_write<int>(count);

		for (Iteratorable::const_iterator it = datas.begin(); it != datas.end(); it++)
		{
			it->on_save_to(*this);
		}
	}

	template<class Iteratorable>
	void write_iteratable_dump(const Iteratorable& datas)
	{
		int count = (int)datas.size();
		stream_write<int>(count);

		if (count > 0) {
			m_stream.write((char*)&datas[0], datas.size() * sizeof(Iteratorable::value_type));
		}
	}

private:
	template<class POD>
	void stream_write(const POD& value) {
		m_stream.write((char*)&value, sizeof(POD));
	}

private:

	std::ostringstream& m_stream;
};


