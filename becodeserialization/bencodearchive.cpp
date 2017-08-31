#include "stdafx.h"
#include <cassert>
#include <exception>
#include <vector>
#include "bencodearchive.h"
#include "define.h"

namespace bencode
{

//////////  BencodeOArchive ///////////////////////

	BencodeOArchive & BencodeOArchive::operator<< ( const BencodeType * pType)
	{
		BencodeArchiveImpl::SaveBencode( pType, m_os);
		return (*this);
	}

//////////  BencodeIArchive ///////////////////////

	BencodeIArchive & BencodeIArchive::operator>> ( BencodeType * & pType)
	{
		assert( NULL == pType);	
		BencodeArchiveImpl::LoadBencode( pType, m_is);
		return (*this);
	}

//////////  BencodeArchiveImpl ///////////////////////

// SAVE
	void BencodeArchiveImpl::SaveBencode(const BencodeType * pType, std::ostream & os )
	{
		switch(pType->GetType())
		{
		case STRING:
			saveBencodeString( pType, os );
			break;
		case INTEGER:
			saveBencodeInteger( pType, os );
			break;
		case LIST:
			saveBencodeList( pType, os );
			break;
		case DICTIONARY:
			saveBencodeDictionary( pType, os );
		default:
			break;
		}
	}
	void BencodeArchiveImpl::saveBencodeString(const BencodeType * pType, std::ostream & os )
	{
		assert( STRING == pType->GetType() );
		const BencodeString * pString = dynamic_cast< const BencodeString *>(pType);
		std::string value = pString->GetValue();
		os << value.length();
		os << BENCODE_TOKEN_STRING;
		os << value;
	}
	void BencodeArchiveImpl::saveBencodeInteger(const BencodeType * pType, std::ostream & os )
	{
		assert( INTEGER == pType->GetType() );

		const BencodeInteger * pInteger = dynamic_cast<const BencodeInteger *>( pType);
		os << BENCODE_PREFIX_INTEGER;	//prefix
		os << pInteger->GetValue();
		os << BENCODE_POSTFIX;	//postfix
	}
	void BencodeArchiveImpl::saveBencodeList( const BencodeType * pType, std::ostream & os )
	{
		assert( LIST == pType->GetType() );
		
		const BencodeList * pList = dynamic_cast<const BencodeList *>(pType);
		os << BENCODE_PREFIX_LIST; //prefix
		const BENCODE_LIST & list = pList->GetValue();

		BENCODE_LIST::const_iterator it = list.begin();
		BENCODE_LIST::const_iterator end_it = list.end();

		for( ; end_it != it; ++it )
			SaveBencode( (*it).get(), os );
		os << BENCODE_POSTFIX;	// postfix
	}

	void BencodeArchiveImpl::saveBencodeDictionary( const BencodeType * pType, std::ostream & os )
	{
		assert( DICTIONARY == pType->GetType() );

		const BencodeDictionary * pDictionary = dynamic_cast<const BencodeDictionary*>( pType );
		os << BENCODE_PREFIX_DICTIONARY; //prefix;
		const BENCODE_DICTIONARY & dictionary = pDictionary->GetValue();

		BENCODE_DICTIONARY::const_iterator it = dictionary.begin();
		BENCODE_DICTIONARY::const_iterator end_it = dictionary.end();

		for( ; end_it != it ; ++it )
		{
			SaveBencode( (it->first).get(), os );		//key
			SaveBencode( (it->second).get(), os );		//value
		}

		os << BENCODE_POSTFIX;	// postfix

	}

// LOAD
	void BencodeArchiveImpl::LoadBencode( BencodeType * & pType, std::istream & is )
	{
		BENCODE_TYPE type = checkType( is );
		switch( type )
		{
		case STRING:
			loadBencodeString( pType , is );
			break;
		case INTEGER:
			loadBencodeInteger( pType, is );
			break;
		case LIST:
			loadBencodeList( pType, is );
			break;
		case DICTIONARY:
			loadBencodeDictionary( pType, is );
			break;
		default:
			break;
		}
	}

	void BencodeArchiveImpl::loadBencodeString( BencodeType *& pType, std::istream & is )
	{
		char nextChar = is.get();
		if( !isdigit(nextChar) )
			throw std::invalid_argument("parsing error : first char is not digit in becodestring");

		std::vector<char> numVec;
		numVec.push_back( nextChar);

		while( true)
		{
			nextChar = is.get();
			if(BENCODE_TOKEN_STRING == nextChar )
			{
				numVec.push_back('\0');	// end charactor
				break;
			}
			else if(!isdigit(nextChar) )
				throw std::invalid_argument("parsing error : char is not digit in becodestring");
			else
				numVec.push_back( nextChar);
		}

		int num = atoi( &numVec[0] );

		if (0 == num)
			pType = new BencodeString(std::string("")); //empty value
		else
		{
			std::vector<char> data(num);
			is.read(&data[0], num);

			pType = new BencodeString(std::string(&data[0], num));
		}
	}

	void BencodeArchiveImpl::loadBencodeInteger( BencodeType *& pType, std::istream & is )
	{
		char nextChar = is.get();

		if( BENCODE_PREFIX_INTEGER != nextChar )
			throw std::invalid_argument("parsing error : prefix char is not 'i' in becodeinteger");

		std::vector<char> numVec;
		nextChar = is.get();

		if( !('-' == nextChar || isdigit(nextChar)) )
			throw std::invalid_argument("parsing error : first char is not '-' or digit in becodeinteger");

		numVec.push_back(nextChar);

		while( true)
		{
			nextChar = is.get();

			if( BENCODE_POSTFIX == nextChar )	// end
				break;
			else if( !isdigit(nextChar ) )
				throw std::invalid_argument("parsing error : char is not  digit in becodeinteger");
			else
				numVec.push_back(nextChar );
		}

		numVec.push_back('\0');
		
		pType = new BencodeInteger( atoi(&numVec[0]) );
	}

	void BencodeArchiveImpl::loadBencodeList( BencodeType *& pType, std::istream & is )
	{
		char nextChar = is.get();

		if( BENCODE_PREFIX_LIST != nextChar )
			throw std::invalid_argument("parsing error : prefix char is not 'l' in becodeList");


		pType = new BencodeList;

		try
		{
			BencodeList * pRawList = dynamic_cast<BencodeList*>(pType);
		

			while( true )
			{
				nextChar = is.peek();

				if( EOF == nextChar )
					throw std::ios_base::failure("invalid bencode list data");
				else if( BENCODE_POSTFIX == nextChar )	// end
					break;

				BencodeType * pRawType = NULL;
				LoadBencode( pRawType, is );
				BENCODE_SHARED_TYPE pType( pRawType );

				pRawList->push_back( pType);

			}

			//delete list postfix
			is.get();
		}
		catch(...)
		{
			delete pType;
			throw;
		}

	}

	void BencodeArchiveImpl::loadBencodeDictionary( BencodeType *& pType, std::istream & is )
	{
		char nextChar = is.get();

		if( BENCODE_PREFIX_DICTIONARY != nextChar )
			throw std::invalid_argument("parsing error : prefix char is not 'd' in becodeDictionary");

		pType = new BencodeDictionary;

		try
		{
			BencodeDictionary * pRawDictionary = dynamic_cast<BencodeDictionary*>(pType);
		

			while( true )
			{
				nextChar = is.peek();

				if( EOF == nextChar )
					throw std::ios_base::failure("invalid bencode key dictionary data");
				else if( BENCODE_POSTFIX == nextChar )	// end
					break;

				BencodeType * pRawType = NULL;
				LoadBencode( pRawType, is );
				BENCODE_SHARED_TYPE pKey( pRawType );

				nextChar = is.peek();

				if( EOF == nextChar )
					throw std::ios_base::failure("invalid bencode key dictionary data");
				else if( BENCODE_POSTFIX == nextChar )	// end
					throw std::invalid_argument("not exist dictionary value");

				pRawType = NULL;
				LoadBencode( pRawType, is );
				BENCODE_SHARED_TYPE pValue( pRawType );

				pRawDictionary->insert( BENCODE_PAIR( pKey, pValue ) );

			}

			//delete list postfix
			is.get();
		}
		catch(...)
		{
			delete pType;
			throw;
		}
	}

	BENCODE_TYPE BencodeArchiveImpl::checkType( std::istream & is )
	{
		char nextChar = is.peek();
		
		if( BENCODE_PREFIX_INTEGER == nextChar )
			return INTEGER;
		else if( BENCODE_PREFIX_LIST == nextChar )
			return LIST;
		else if( BENCODE_PREFIX_DICTIONARY == nextChar )
			return DICTIONARY;
		else if( isdigit(nextChar) )
			return STRING;
		else
			return NOT_EXPECTED;
	}


	
}