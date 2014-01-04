#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "jnxmem.h"
#include "jnxbase64.h"
void build_decoding_table();
void base64_cleanup();
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


char *jnx_base64_encode(const unsigned char *data,
		size_t input_length,
		size_t *output_length) {

	*output_length = 4 * ((input_length + 2) / 3);
	char *encoded_data = JNX_MEM_MALLOC(((*output_length) +1) * sizeof (char));
	if (encoded_data == NULL) return NULL;

	int i,j;

	for ( i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? data[i] : 0;
		i++;
		uint32_t octet_b = i < input_length ? data[i] : 0;
		i++;
		uint32_t octet_c = i < input_length ? data[i] : 0;
		i++;
		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	int l;
	for(l = 0; l < mod_table[input_length %3]; l++)
	{
		encoded_data[*output_length -1 -l] = '=';
	}
	encoded_data[*output_length ] = '\0';
	return encoded_data;
}


unsigned char *jnx_base64_decode(const char *data,
		size_t input_length,
		size_t *output_length) {

	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	unsigned char *decoded_data = JNX_MEM_CALLOC(*output_length, sizeof(unsigned char));
	bzero(decoded_data,*output_length);
	if (decoded_data == NULL) return NULL;

	int i,j;
	for (i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[(int)data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[(int)data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[(int)data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[(int)data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}
	return decoded_data;
}


void build_decoding_table() {

	decoding_table = JNX_MEM_MALLOC(256 * sizeof(char));

	int z;
	for(z = 0; z < 256;++z)
	{
		decoding_table[z] = (char)-1;
	}
	int i;
	for (i = 0; i < 65; i++)
		decoding_table[(unsigned char) encoding_table[i]] = i;
}


void jnx_base64_cleanup() {
	JNX_MEM_FREE(decoding_table);
}
