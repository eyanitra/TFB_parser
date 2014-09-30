/* This initial system for custom serialization
 * 
 * tag's rule:
 * - short form (1 byte):
 *  + class (2bits), primitive/constructed(1bit), short form(must 0), actual tag(4bits)
 * - long form (n byte)
 *  + first byte mask (1F)
 *  + most significant bit of 2nd,3rd.. must be 1 if there are additional byte
 *  + most significant bit 0, for last byte
 * 	
 *  constructed tag's mean you must find another tag inside it
 * 
 * length's rule:
 * - most significant bit in last byte must be 1
 * - if zero, need additional byte 
 * 
 *
 * author : Evan
 */
 #ifndef TO_SERIAL_HEADER
 #define TO_SERIAL_HEADER
 
int TLV_writeLength(unsigned int length, char *lengthBuffer);
int TLV_writeTag(unsigned int tag, char *tagBuffer);
int TLV_writeTlv(unsigned int tag, unsigned int length, char *value);

int TLV_readLength(char *lengthBuffer, int bufLen);
int TLV_readTag(char *tagBuffer, int bufLen);

int TLV_lengthByte(char *lengthBuffer, int bufLen);
int TLV_tagByte(char *tagBuffer, int bufLen);
int TVL_byte(unsigned int tag, unsigned int length);
int TLV_valueOffset(char *tlvBuffer, int bufLen);
 
 #endif