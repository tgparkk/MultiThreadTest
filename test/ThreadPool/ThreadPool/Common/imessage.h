#pragma once

#include "net_messages.h"
#include <boost/noncopyable.hpp>

class ae_binary_oarchive;
class ae_binary_iarchive;

class MsgHeader
{
public:
	MsgHeader() : m_msgid(eNetID::None), m_bodysize(0) {}
	MsgHeader(eNetID::T msgid, int bodysize) : m_msgid(msgid), m_bodysize(bodysize) {}

	bool IsValid() const {
		return (eNetID::None <= m_msgid && m_msgid < eNetID::Max) &&
			0 < m_bodysize && m_bodysize < 100 * 1024 * 1024;
	}
	
	eNetID::T msg_id() const {return m_msgid;}
	int body_size() const {return m_bodysize;}

private:
	eNetID::T m_msgid;
	int m_bodysize; //count bytes
};

class IMessage : public boost::noncopyable
{
public:
	explicit IMessage(eNetID::T id) 
		: m_msgid(id), m_comp_start_size(8 * 1024) {}
	virtual ~IMessage() {}
	
	eNetID::T msg_id() const {return m_msgid;}
	virtual std::wstring to_string() const = 0;
 	
	//메시지 압축을 시작할 크기를 지정, 매우작은값을 지정하면 항상 압축을 진행.
	void compress_start_size(int start_size) {m_comp_start_size = start_size;}
	int compress_start_size() const {return m_comp_start_size;}

	void save_to(OUT std::string& buf) const;
	void load_from(const std::string& buf);
private:
	virtual void on_save_to(ae_binary_oarchive& archive) const = 0;
	virtual void on_load_from(ae_binary_iarchive& archive) = 0;

	eNetID::T m_msgid;
	int m_comp_start_size;
	
};

//typedef boost::shared_ptr<IMessage> spIMessage;
typedef IMessage* pIMessage;
