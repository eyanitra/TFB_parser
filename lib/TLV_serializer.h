/* This initial system for custom serialization
 * 
 * tag's rule:
 * - short form (1 byte):
 *  + class universal 00/application 01/specific 10/ private 11,
 *  	(2bits), primitive 0/constructed 1(1bit), short form(must 0), actual tag(4bits)
 * - long form (n byte)
 *  + first byte mask (1F)
 *  + most significant bit of 2nd,3rd.. must be 1 if there are additional byte
 *  + most significant bit 0, for last byte
 * 	
 *  constructed tag's mean you must find another tag inside it
 * 
 * length's rule:
 * - if bit7 (most significant bit) in most significant byte is 1 there is extension
 * - number of extension byte coded in value of binary value bit6 to bit0
 * 
 *
 * author : Evan
 */
 #ifndef TO_SERIAL_HEADER
 #define TO_SERIAL_HEADER

// classlessTag is tag number without class and data object type
// tagClass will change tag, but not affect byte needed for write down tag

int TLV_writeLength(unsigned int length, char *lengthBuffer);
int TLV_writeTag(unsigned char tagClass, unsigned int classlessTag, char *tagBuffer);
int TLV_writeTlv(unsigned char tagClass, unsigned int classlessTag, unsigned int length, const char *value, char *tlvBuffer);

unsigned int TLV_readLength(const char *lengthBuffer, int bufLen);
unsigned int TLV_readTag(const char *tagBuffer, int bufLen);

int TLV_lengthByte(const char *lengthBuffer, int bufLen);
int TLV_tagByte(const char *tagBuffer, int bufLen);
int TVL_tlvByte(unsigned int classlessTag, unsigned int length);
int TLV_valueOffset(const char *tlvBuffer, int bufLen);
 
 #endif