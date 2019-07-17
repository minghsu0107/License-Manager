#ifndef XXTEA_H 
#define XXTEA_H 
#include <stddef.h> /* for size_t & NULL declarations */ 
#include <string>
#include <stdint.h> 
#define XXTEA_MX (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z) 
#define XXTEA_DELTA 0x9E3779B9 
typedef uint32_t xxtea_uint; 
using namespace std;

class XXTEA {
public:
	string xxtea_encrypt(string data, string key); 
	string xxtea_decrypt(string data, string key);
	
private:
	string base64_encode(unsigned char const* , unsigned int len);
	string base64_decode(std::string const& s);

	unsigned char *xxtea_encrypt(unsigned char *data, xxtea_uint data_len, unsigned char *key, xxtea_uint key_len, xxtea_uint *ret_length); 
	unsigned char *xxtea_decrypt(unsigned char *data, xxtea_uint data_len, unsigned char *key, xxtea_uint key_len, xxtea_uint *ret_length); 
};
#endif
