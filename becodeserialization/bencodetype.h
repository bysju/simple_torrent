#ifndef _BECODETYPE_H_
#define _BECODETYPE_H_

#include <string>
#include <list>
#include <map>
#include <memory>

#ifdef _DEBUG
#include "objectcounter.h"
#define  OBJECTCOUNTER(x)  , public ObjectCounter<x>
#else
#define  OBJECTCOUNTER(x)
#endif


namespace bencode
{
	enum BENCODE_TYPE
	{
		STRING,
		INTEGER,
		LIST,
		DICTIONARY,
		NOT_EXPECTED
	};

	class BencodeType 
	{
	public:
		virtual BENCODE_TYPE GetType() const = 0;
		virtual ~BencodeType() { }
	};

	class BencodeInteger : public BencodeType OBJECTCOUNTER(BencodeInteger)
	{
	public:
		BencodeInteger(int value) : m_value(value) {}
		virtual BENCODE_TYPE GetType() const { return INTEGER; }
		int GetValue() const { return m_value; }
	private:
		int m_value;
	};

	class BencodeString : public BencodeType OBJECTCOUNTER(BencodeString)
	{
	public:
		BencodeString( const std::string & value ) : m_value(value) {}
		virtual BENCODE_TYPE GetType() const { return STRING; }
		const std::string & GetValue() const { return m_value; }
	private:
		std::string m_value;
	};

	typedef std::tr1::shared_ptr<BencodeType>	BENCODE_SHARED_TYPE; 
	typedef std::list< BENCODE_SHARED_TYPE > BENCODE_LIST;
	typedef std::map< BENCODE_SHARED_TYPE, BENCODE_SHARED_TYPE > BENCODE_DICTIONARY;
	typedef std::pair<BENCODE_SHARED_TYPE, BENCODE_SHARED_TYPE> BENCODE_PAIR;

	class BencodeList : public BencodeType OBJECTCOUNTER(BencodeList)
	{
	public:
		BencodeList(){}
		BencodeList( const BENCODE_LIST & value ) : m_value( value ) {}
		virtual BENCODE_TYPE GetType() const { return LIST; }
		const BENCODE_LIST & GetValue() const { return m_value; }

		// list like operation
		void push_back( const BENCODE_SHARED_TYPE & val ) { m_value.push_back( val ); }
	private:
		BENCODE_LIST m_value;
	};

	class BencodeDictionary : public BencodeType OBJECTCOUNTER(BencodeDictionary)
	{
	public:
		BencodeDictionary(){}
		BencodeDictionary( const BENCODE_DICTIONARY & value ) : m_value(value) {}
		virtual BENCODE_TYPE GetType() const { return DICTIONARY; }
		const BENCODE_DICTIONARY & GetValue() const { return m_value; }

		// map like operation
		std::pair< BENCODE_DICTIONARY::iterator, bool> insert( const BENCODE_PAIR & val ) { return m_value.insert( val); }
	private:
		BENCODE_DICTIONARY m_value;
	};
}

#endif