//Date: 18th March 2019
//Programmers: Jaime Bastida and Daniel Solorio
//Abstract: This program makes use of the functionalities provided by Cryotp++ to do some practising with 3DES 

//Common Libraries
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
//Crypto++ Libraries
#include "cryptopp/modes.h"
#include "cryptopp/filters.h"
#include <cryptopp/cryptlib.h> 		//RandomNumberGenerator, AutoSeededRandomPool
#include <cryptopp/osrng.h> 		//OS_GenerateRandomBlock
#include <cryptopp/des.h> 			//DES and 3DES
#include <cryptopp/hex.h> 			//HexEncoder

using namespace std;
using namespace CryptoPP;

int main(int argc, char *argv[])
{
	AutoSeededRandomPool prng;

	SecByteBlock key(0x00, DES_EDE2::DEFAULT_KEYLENGTH);
	prng.GenerateBlock(key, key.size());

	byte iv[DES_EDE2::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	string plain = "CBC Mode Test";
	string cipher, encoded, recovered;

	/*********************************\
	\*********************************/

	try
	{
	    cout << "plain text: " << plain << endl;

	    CBC_Mode< DES_EDE2 >::Encryption e;
	    e.SetKeyWithIV(key, key.size(), iv);

	    // The StreamTransformationFilter adds padding
	    //  as required. ECB and CBC Mode must be padded
	    //  to the block size of the cipher.
	    StringSource ss1(plain, true, 
	        new StreamTransformationFilter(e,
	            new StringSink(cipher)
	        ) // StreamTransformationFilter      
	    ); // StringSource
	}
	catch(const CryptoPP::Exception& e)
	{
	    cerr << e.what() << endl;
	    exit(1);
	}

	/*********************************\
	\*********************************/

	// Pretty print
	StringSource ss2(cipher, true,
	    new HexEncoder(
	        new StringSink(encoded)
	    ) // HexEncoder
	); // StringSource

	cout << "cipher text: " << encoded << endl;

	/*********************************\
	\*********************************/

	try
	{
	    CBC_Mode< DES_EDE2 >::Decryption d;
	    d.SetKeyWithIV(key, key.size(), iv);

	    // The StreamTransformationFilter removes
	    //  padding as required.
	    StringSource ss3(cipher, true, 
	        new StreamTransformationFilter(d,
	            new StringSink(recovered)
	        ) // StreamTransformationFilter
	    ); // StringSource

	    cout << "recovered text: " << recovered << endl;
	}
	catch(const CryptoPP::Exception& e)
	{
	    cerr << e.what() << endl;
	    exit(1);
	}
}