// testbencodeserialzation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <sstream>
#include "../becodeserialization/bencodearchive.h"

// bencode specification https://wiki.theory.org/index.php/BitTorrentSpecification

SCENARIO("Bencode Interger Test/Integer to Bencode")
{
	// -99 -> i-99e
	// 0   -> i0e
	
	for (int i = -100; i < 101; ++i)
	{
		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		std::unique_ptr<bencode::BencodeType> bInteger(new bencode::BencodeInteger(i));
		bencodeOArchive << bInteger.get();

		// make correct result
		std::stringstream correctVal;
		correctVal << "i" << i << "e";

		INFO("Inteager is: " << i << " String is: " << correctVal.str());
		REQUIRE(oss.str() == correctVal.str());
	}
	

}

SCENARIO("Bencode Interger Test/Bencode to Integer")
{
	//  i-99e -> -99 
	//  i0e   -> 0

	for (int i = -100; i < 101; ++i)
	{
		std::stringstream iss;
		iss << "i" << i << "e";
		bencode::BencodeIArchive bencodeIArchive(iss);
		
		bencode::BencodeType * pBencodeType = nullptr;
		bencodeIArchive >> pBencodeType;

		REQUIRE(pBencodeType->GetType() == bencode::BENCODE_TYPE::INTEGER);
		std::unique_ptr<bencode::BencodeType> bInteger(pBencodeType);
		bencode::BencodeInteger * pBencodeInteger = dynamic_cast<bencode::BencodeInteger*>(pBencodeType);


		INFO("Inteager is: " << pBencodeInteger->GetValue() << " String is: " << iss.str());
		REQUIRE(pBencodeInteger->GetValue() == i);
	}
}

SCENARIO("Bencode String Test/String to Bencode")
{
	// spam -> 4:spam
	// "    -> 0:

	std::vector<std::string> vec;

	vec.push_back("");
	vec.push_back("spam");

	for (const std::string & str : vec)
	{
		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		// make correct result
		std::stringstream correctVal;
		correctVal << str.size() << ":" << str;

		std::unique_ptr<bencode::BencodeType> bString(new bencode::BencodeString(str));
		bencodeOArchive << bString.get();

		INFO("String is: " << str << " String is: " << correctVal.str());
		REQUIRE(oss.str() == correctVal.str());

	}

	
}

SCENARIO("Bencode String Test/Bencode to String")
{
	// 4:spam -> spam
	// 0:     -> ""  

	std::vector<std::string> vec;

	vec.push_back("");
	vec.push_back("spam");

	for (const std::string & str : vec)
	{
		std::stringstream iss;
		iss << str.size() << ":" << str;
		bencode::BencodeIArchive bencodeIArchive(iss);

		bencode::BencodeType * pBencodeType = nullptr;
		bencodeIArchive >> pBencodeType;

		REQUIRE(pBencodeType->GetType() == bencode::BENCODE_TYPE::STRING);
		std::unique_ptr<bencode::BencodeType> bString(pBencodeType);
		bencode::BencodeString * pBencodeString = dynamic_cast<bencode::BencodeString*>(pBencodeType);


		INFO("String is: " << pBencodeString->GetValue() << " String is: " << iss.str());
		REQUIRE(pBencodeString->GetValue() == str);
	}
}