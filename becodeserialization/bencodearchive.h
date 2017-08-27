#ifndef _BENCODE_ARCHIVE_H_
#define _BENCODE_ARCHIVE_H_

#include <ostream>
#include "bencodetype.h"

namespace bencode
{

	class BencodeOArchive
	{
	public:
		BencodeOArchive( std::ostream & os ) : m_os(os) {}

		BencodeOArchive & operator << (const BencodeType * pType );
		
	private:
		std::ostream & m_os;

	};

	class BencodeIArchive
	{
	public:
		BencodeIArchive( std::istream & is ) : m_is(is) {}
		BencodeIArchive & operator >> ( BencodeType * & pType);
	private:
		std::istream & m_is;
	};

	class BencodeArchiveImpl
	{
	private:
		BencodeArchiveImpl(){}	// static class
	public:
		static void SaveBencode( const BencodeType * pType, std::ostream & os );
		static void LoadBencode( BencodeType * & pType, std::istream & is );
	protected:
		static void saveBencodeInteger( const BencodeType * pType, std::ostream & os );
		static void saveBencodeString ( const BencodeType * pType, std::ostream & os );
		static void saveBencodeList( const BencodeType * pType, std::ostream & os );
		static void saveBencodeDictionary( const BencodeType * pType, std::ostream & os );

		static void loadBencodeString( BencodeType *& pType, std::istream & is );
		static void loadBencodeInteger(BencodeType *& pType, std::istream & is );
		static void loadBencodeList( BencodeType *& pType, std::istream & is );	
		static void loadBencodeDictionary( BencodeType *& pType, std::istream & is );	

		static BENCODE_TYPE checkType( std::istream & is );
	};


}

#endif //_BENCODE_ARCHIVE_H_