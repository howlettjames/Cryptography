//Date: 18th March 2019
//Programmers: Jaime Bastida and Daniel Solorio
//Abstract: This program makes use of the functionalities provided by Cryotp++ to do some practising with 3DES 
//Common Libraries
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
//Crypto++ Libraries
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h> 		//RandomNumberGenerator, AutoSeededRandomPool
#include <cryptopp/osrng.h> 		//OS_GenerateRandomBlock
#include <cryptopp/des.h> 			//DES and 3DES
#include <cryptopp/hex.h> 			//HexEncoder
#include <cryptopp/base64.h> 		//Base64Encoder
#include <cryptopp/files.h> 		//FileSink

using namespace std;
using namespace CryptoPP;

void do3DES(string variant, string mode_of_operation);

string iv_ciphertext_fn = "iv_ciphertext.txt";
string decipheredtext_fn = "decipheredtext.txt";
const char *key_fn = "key.txt";

int main(int argc, char *argv[])
{
	short option1, option2;
	bool correct_option = false;

	while(true)
	{
		cout << "\n|+|+|+| BLOCK CIPHERS |+|+|+|" << endl;
		cout << "1. EDE" << endl;
		cout << "2. EEE" << endl;
		cout << "3. Exit" << endl;
		cout << "Choose an option: ";
		cin >> option1;
		switch(option1)
		{
			case 1:
				do
				{
					cout << "\n|+|+| Modes of operation |+|+|" << endl;
					cout << "1. CBC" << endl;
					cout << "2. CTR" << endl;
					cout << "3. OFB" << endl;
					cout << "4. CFB" << endl;
					cout << "5. Return to main menu" << endl;
					cout << "Choose an option: ";
					cin >> option2;
					getchar(); 			//To absorb the '\n' entered by the user
					switch(option2)
					{
						case 1:
							do3DES("EDE", "CBC");
						break;
						case 2:
							do3DES("EDE", "CTR");
						break;
						case 3:
							do3DES("EDE", "OFB");
						break;
						case 4:
							do3DES("EDE", "CFB");
						break;
						case 5:
							correct_option = true;
						break;
						default:
							cout << "Choose a correct option." << endl << endl;
							correct_option = false;
						break;
						correct_option = true;
					}
				}
				while(!correct_option);
			break;
			case 2:
				do
				{
					cout << "\n|+|+| Modes of operation |+|+|" << endl;
					cout << "1. CBC" << endl;
					cout << "2. CTR" << endl;
					cout << "3. OFB" << endl;
					cout << "4. CFB" << endl;
					cout << "5. Return to main menu" << endl;
					cout << "Choose an option: ";
					cin >> option2;
					getchar(); 			//To absorb the '\n' entered by the user
					switch(option2)
					{
						case 1:
							do3DES("EEE", "CBC");
						break;
						case 2:
							do3DES("EEE", "CTR");
						break;
						case 3:
							do3DES("EEE", "OFB");
						break;
						case 4:
							do3DES("EEE", "CFB");
						break;
						case 5:
							correct_option = true;
						break;
						default:
							cout << "Choose a correct option." << endl << endl;
							correct_option = false;
						break;
						correct_option = true;
					}
				}
				while(!correct_option);
			break;
			case 3:
				exit(EXIT_SUCCESS);
			break;
			default:
				cout << "Choose a correct option." << endl << endl;
			break;
		}
	}
}

void do3DES(string variant, string mode_of_operation)
{
	AutoSeededRandomPool auto_srp;
	string ciphertext, encodedciphertext, decodedtext;
	string key_decoded, recoveredtext, iv_encoded;						
	string plaintext_fn, plaintext, aux_line;
	clock_t begin_time, end_time;
	fstream plaintext_file;
	bool isInputCorrect;

	SecByteBlock key;
	byte *iv;
	if(variant.compare("EDE") == 0)
	{
		key.Assign(0x00, DES_EDE2::DEFAULT_KEYLENGTH);
		iv = (byte *) malloc(DES_EDE2::BLOCKSIZE);
	}
	else if(variant.compare("EEE") == 0)
	{
		key.Assign(0x00, DES_EDE3::DEFAULT_KEYLENGTH);
		iv = (byte *) malloc(DES_EDE3::BLOCKSIZE);	
	}	
	else
	{
		cout << "Variant of 3DES not recognized" << endl;
		exit(EXIT_FAILURE);
	}
	auto_srp.GenerateBlock(key, key.size());
	auto_srp.GenerateBlock(iv, sizeof(iv));

	// -------------------------------- READING PLAINTEXT -----------------------------
	isInputCorrect = false;	
	while(!isInputCorrect)
	{
		cout << "Write the plaintext file name: ";
		getline(cin, plaintext_fn);
		plaintext_file.open(plaintext_fn);
		if(plaintext_file.is_open())
			isInputCorrect = true;
		else
			cout << "Can't open: " << plaintext_fn << endl;
	}	
	while(!plaintext_file.eof())
	{
		getline(plaintext_file, aux_line);
		plaintext += aux_line; 
	}
	plaintext_file.close();

	// -------------------------------- CIPHERING ------------------------------------
	begin_time = clock();
	if(variant.compare("EDE") == 0)
	{
		if(mode_of_operation.compare("CBC") == 0)
		{
			try
			{
				CBC_Mode<DES_EDE2>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
				StringSource ss_plaintext(plaintext, true, new 
					StreamTransformationFilter(e, new StringSink(ciphertext), BlockPaddingSchemeDef::PKCS_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("CTR") == 0)
		{
			try
			{
				CTR_Mode<DES_EDE2>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext), BlockPaddingSchemeDef::DEFAULT_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("OFB") == 0) //No padding
		{
			try
			{
				OFB_Mode<DES_EDE2>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else if(mode_of_operation.compare("CFB") == 0) //No padding
		{
			try
			{
				CFB_Mode<DES_EDE2>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else
		{
			cout << "Cipher mode not recognized" << endl;
			exit(EXIT_FAILURE);		
		}
	}
	else if(variant.compare("EEE") == 0)
	{
		if(mode_of_operation.compare("CBC") == 0)
		{
			try
			{
				CBC_Mode<DES_EDE3>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
				StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext), BlockPaddingSchemeDef::PKCS_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("CTR") == 0)
		{
			try
			{
				CTR_Mode<DES_EDE3>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext), BlockPaddingSchemeDef::DEFAULT_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("OFB") == 0) //No padding
		{
			try
			{
				OFB_Mode<DES_EDE3>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else if(mode_of_operation.compare("CFB") == 0) //No padding
		{
			try
			{
				CFB_Mode<DES_EDE3>::Encryption e;
				e.SetKeyWithIV(key, key.size(), iv);
		    	StringSource ss_plaintext(plaintext, true, new StreamTransformationFilter(e, new StringSink(ciphertext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else
		{
			cout << "Cipher mode not recognized" << endl;
			exit(EXIT_FAILURE);		
		}
	}
	else
	{
		cout << "Variant of 3DES not recognized" << endl;
		exit(EXIT_FAILURE);
	}
	end_time = clock();
	cout << "Execution time: " << (double) (end_time - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;	

	// ------------------------------ ENCODING KEY, IV AND CIPHERTEXT --------------------------------------
	string key_string(reinterpret_cast<const char *> (key.BytePtr()));
	if(variant.compare("EEE") == 0) //Don't know why at casting the compiler adds another byte, only when using EEE
		key_string = key_string.substr(0, key_string.size() - 1);
	StringSource ss_key(key_string, true, new Base64Encoder(new FileSink(key_fn)));
	StringSource ss_iv(iv, (sizeof(iv)/sizeof(*iv)), true, new Base64Encoder(new StringSink(iv_encoded)));
	StringSource ss_ciphertext_encode(ciphertext, true, new Base64Encoder(new StringSink(encodedciphertext)));

	ofstream writer1(iv_ciphertext_fn);
	if(!writer1)
	{
		cout << "|+|ERROR: Can't open file: " << iv_ciphertext_fn << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		writer1 << iv_encoded << encodedciphertext;
		writer1.close();
	}

	// ------------------------------ DECODING KEY--------------------------------------
	FileSource sf_key(key_fn, true, new Base64Decoder(new StringSink(key_decoded)));

	// ------------------------------ DECIPHERING --------------------------------------
	if(variant.compare("EDE") == 0)
	{
		if(mode_of_operation.compare("CBC") == 0)
		{
			try
			{
				CBC_Mode<DES_EDE2>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext), BlockPaddingSchemeDef::PKCS_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("CTR") == 0)
		{
			try
			{
				CTR_Mode<DES_EDE2>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext), BlockPaddingSchemeDef::DEFAULT_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("OFB") == 0) //No padding
		{
			try
			{
				OFB_Mode<DES_EDE2>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else if(mode_of_operation.compare("CFB") == 0) //No padding
		{
			try
			{
				CFB_Mode<DES_EDE2>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else
		{
			cout << "Cipher mode not recognized" << endl;
			exit(EXIT_FAILURE);			
		}
	}
	else if(variant.compare("EEE") == 0)
	{
		if(mode_of_operation.compare("CBC") == 0)
		{
			try
			{
				CBC_Mode<DES_EDE3>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext), BlockPaddingSchemeDef::PKCS_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("CTR") == 0)
		{
			try
			{
				CTR_Mode<DES_EDE3>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext), BlockPaddingSchemeDef::DEFAULT_PADDING));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}
		}
		else if(mode_of_operation.compare("OFB") == 0) //No padding
		{
			try
			{
				OFB_Mode<DES_EDE3>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else if(mode_of_operation.compare("CFB") == 0) //No padding
		{
			try
			{
				CFB_Mode<DES_EDE3>::Decryption d;
				d.SetKeyWithIV((const byte *)key_decoded.c_str(), key_decoded.size(), iv);
			    StringSource ss_ciphertext(ciphertext, true, new StreamTransformationFilter(d, new StringSink(recoveredtext)));
			}
			catch(const CryptoPP::Exception& e)
			{
				cerr << e.what() << endl;
				exit(EXIT_FAILURE);
			}	
		}
		else
		{
			cout << "Cipher mode not recognized" << endl;
			exit(EXIT_FAILURE);			
		}
	}
	else
	{
		cout << "Variant of 3DES not recognized" << endl;
		exit(EXIT_FAILURE);
	}
	//Writting deciphered text to file
	ofstream writer2(decipheredtext_fn);
	if(!writer2)
	{
		cout << "|+|ERROR: Can't open file: " << decipheredtext_fn << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		writer2 << recoveredtext;
		writer2.close();
	}
}