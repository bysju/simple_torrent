// testbencodeserialzation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <sstream>
#include "../becodeserialization/bencodearchive.h"

// bencode specification https://wiki.theory.org/index.php/BitTorrentSpecification

// Test Function

void TestInterger(int value, std::shared_ptr<bencode::BencodeType> bType)
{
	REQUIRE(bencode::BENCODE_TYPE::INTEGER == bType->GetType());

	bencode::BencodeInteger * pBencodeInteger = dynamic_cast<bencode::BencodeInteger*>(bType.get());


	INFO("Inteager is: " << value << " Bencode Type is: " << pBencodeInteger->GetValue());
	REQUIRE(pBencodeInteger->GetValue() == value);

}

void TestInteger(const std::string & strValue, std::shared_ptr<bencode::BencodeType> bType)
{
	REQUIRE(bencode::BENCODE_TYPE::INTEGER == bType->GetType());

	bencode::BencodeInteger * pBencodeInteger = dynamic_cast<bencode::BencodeInteger*>(bType.get());

	std::stringstream oss;
	bencode::BencodeOArchive bencodeOArchive(oss);


//	INFO("Inteager sring is: " << strValue << " Bencode Type is: " << pBencodeInteger->GetValue());
//	REQUIRE(pBencodeInteger->GetValue() == strValue);
}

SCENARIO("Bencode Interger Test/Integer to Bencode")
{
	// -99 -> i-99e
	// 0   -> i0e
	
	for (int i = -100; i < 101; ++i)
	{
		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		std::shared_ptr<bencode::BencodeType> bInteger(new bencode::BencodeInteger(i));
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
		std::shared_ptr<bencode::BencodeType> bInteger(pBencodeType);
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

		std::shared_ptr<bencode::BencodeType> bString(new bencode::BencodeString(str));
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

SCENARIO("Bencode List Test/List to Bencode")
{
	
		
	std::shared_ptr<bencode::BencodeType> bStrSpam(new bencode::BencodeString("spam"));
	std::shared_ptr<bencode::BencodeType> bStrEggs(new bencode::BencodeString("eggs"));
	std::shared_ptr<bencode::BencodeType> bInt9(new bencode::BencodeInteger(9));


	SECTION("string List")
	{
		// l4:spam4:eggse represents the list of two strings: [ "spam", "eggs" ] 
		std::string strTestResult = "l4:spam4:eggse";

		std::unique_ptr<bencode::BencodeList> bListTest(new bencode::BencodeList());
		bListTest->push_back(bStrSpam);
		bListTest->push_back(bStrEggs);

		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		bencodeOArchive << bListTest.get();

		REQUIRE(oss.str() == strTestResult);


	}
	
	SECTION("empty List")
	{
		// Example: le represents an empty list : []
		std::string strTestResult = "le";

		std::unique_ptr<bencode::BencodeList> bListTest(new bencode::BencodeList());

		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		bencodeOArchive << bListTest.get();

		REQUIRE(oss.str() == strTestResult);
	}

	SECTION("Strng/Integer List")
	{
		// l4:spam4:i9ee represents the list of two strings: [ "spam", 9 ] 
		std::string strTestResult = "l4:spami9ee";

		std::unique_ptr<bencode::BencodeList> bListTest(new bencode::BencodeList());
		bListTest->push_back(bStrSpam);
		bListTest->push_back(bInt9);

		std::stringstream oss;
		bencode::BencodeOArchive bencodeOArchive(oss);

		bencodeOArchive << bListTest.get();

		REQUIRE(oss.str() == strTestResult);
	}
	

}