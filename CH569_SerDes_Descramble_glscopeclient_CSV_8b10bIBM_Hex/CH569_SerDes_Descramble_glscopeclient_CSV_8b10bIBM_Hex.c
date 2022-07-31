/*
CH569_SerDes_Descramble_glscopeclient_CSV_8b10bIBM_Hex.c
Code by B.VERNOUX 8 June 2022
* SATA spec see https://www.seagate.com/support/disc/manuals/sata/SERDES_im.pdf

Code used:
* https://github.com/jandoczy/csv-fast-reader.git
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "csv.h"
#include "crc32_SATA.h"

/* For more details see SATA spec https://www.seagate.com/support/disc/manuals/sata/SERDES_im.pdf */
#define SERDES_PRIMITIVE_SOF   (0x7CB53737)
#define SERDES_PRIMITIVE_EOF   (0x7CB5D5D5)

#define SERDES_PRIMITIVE_SYNC  (0x7C95B5B5)
#define SERDES_PRIMITIVE_CONT  (0x7CAA9999)

#define SERDES_PRIMITIVE_ALIGN (0xBC4A4A7B)

#define TIME_S_TO_MICROSEC (1000*1000)

static int serdes_start_data = 0; /* 1=SOF received (reset to 0 after EOF) */
static int serdes_first_end_data = 0; /* 1=1st EOF received */
#define MAX_NB_32BITS (65536)
static uint32_t scrambler_data[MAX_NB_32BITS];
static uint32_t data_u32_buf[MAX_NB_32BITS];

/*
This code generates the entire sequence of 65535 Dwords produced
by the scrambler defined in the Serial ATA specification
*/
void scramble(void)
{
	int i, j;
	uint16_t context; /* The 16 bit register that holds the context or state */
	uint32_t scrambler; /* The 32 bit output of the circuit */
	uint8_t now[16]; /* The individual bits of context */
	uint8_t next[32]; /* The computed bits of scrambler */
	/* Parallelized versions of the scrambler are initialized to a value */
	/* derived from the initialization value of 0xFFFF defined in the */
	/* specification. This implementation is initialized to 0xF0F6. Other */
	/* parallel implementations will have different initial values. The */
	/* important point is that the first Dword output of any implementation */
	/* must equal 0xC2D2768D. */
	context = 0xF0F6;
	for (i = 0; i < 65535; ++i)
	{
		/* Split the register contents (the variable context) up into its */
		/* individual bits for easy handling. */
		for (j = 0; j < 16; ++j)
		{
			now[j] = (context >> j) & 0x01;
		}
		/* The following 16 assignments implement the matrix multiplication */
		/* performed by the box labeled *M1. */
		/* Notice that there are lots of shared terms in these assignments. */
		next[31] = now[12] ^ now[10] ^ now[7] ^ now[3] ^ now[1] ^ now[0];
		next[30] = now[15] ^ now[14] ^ now[12] ^ now[11] ^ now[9] ^ now[6] ^ now[3] ^ now[2] ^ now[0];
		next[29] = now[15] ^ now[13] ^ now[12] ^ now[11] ^ now[10] ^ now[8] ^ now[5] ^ now[3] ^ now[2] ^ now[1];
		next[28] = now[14] ^ now[12] ^ now[11] ^ now[10] ^ now[9] ^ now[7] ^ now[4] ^ now[2] ^ now[1] ^ now[0];
		next[27] = now[15] ^ now[14] ^ now[13] ^ now[12] ^ now[11] ^ now[10] ^ now[9] ^ now[8] ^ now[6] ^ now[1] ^ now[0];
		next[26] = now[15] ^ now[13] ^ now[11] ^ now[10] ^ now[9] ^ now[8] ^ now[7] ^ now[5] ^ now[3] ^ now[0];
		next[25] = now[15] ^ now[10] ^ now[9] ^ now[8] ^ now[7] ^ now[6] ^ now[4] ^ now[3] ^ now[2];
		next[24] = now[14] ^ now[9] ^ now[8] ^ now[7] ^ now[6] ^ now[5] ^ now[3] ^ now[2] ^ now[1];
		next[23] = now[13] ^ now[8] ^ now[7] ^ now[6] ^ now[5] ^ now[4] ^ now[2] ^ now[1] ^ now[0];
		next[22] = now[15] ^ now[14] ^ now[7] ^ now[6] ^ now[5] ^ now[4] ^ now[1] ^ now[0];
		next[21] = now[15] ^ now[13] ^ now[12] ^ now[6] ^ now[5] ^ now[4] ^ now[0];
		next[20] = now[15] ^ now[11] ^ now[5] ^ now[4];
		next[19] = now[14] ^ now[10] ^ now[4] ^ now[3];
		next[18] = now[13] ^ now[9] ^ now[3] ^ now[2];
		next[17] = now[12] ^ now[8] ^ now[2] ^ now[1];
		next[16] = now[11] ^ now[7] ^ now[1] ^ now[0];
		/* The following 16 assignments implement the matrix multiplication */
		/* performed by the box labeled *M2. */
		next[15] = now[15] ^ now[14] ^ now[12] ^ now[10] ^ now[6] ^ now[3] ^ now[0];
		next[14] = now[15] ^ now[13] ^ now[12] ^ now[11] ^ now[9] ^ now[5] ^ now[3] ^ now[2];
		next[13] = now[14] ^ now[12] ^ now[11] ^ now[10] ^ now[8] ^ now[4] ^ now[2] ^ now[1];
		next[12] = now[13] ^ now[11] ^ now[10] ^ now[9] ^ now[7] ^ now[3] ^ now[1] ^ now[0];
		next[11] = now[15] ^ now[14] ^ now[10] ^ now[9] ^ now[8] ^ now[6] ^ now[3] ^ now[2] ^ now[0];
		next[10] = now[15] ^ now[13] ^ now[12] ^ now[9] ^ now[8] ^ now[7] ^ now[5] ^ now[3] ^ now[2] ^ now[1];
		next[9] = now[14] ^ now[12] ^ now[11] ^ now[8] ^ now[7] ^ now[6] ^ now[4] ^ now[2] ^ now[1] ^ now[0];
		next[8] = now[15] ^ now[14] ^ now[13] ^ now[12] ^ now[11] ^ now[10] ^ now[7] ^ now[6] ^ now[5] ^ now[1] ^ now[0];
		next[7] = now[15] ^ now[13] ^ now[11] ^ now[10] ^ now[9] ^ now[6] ^ now[5] ^ now[4] ^ now[3] ^ now[0];
		next[6] = now[15] ^ now[10] ^ now[9] ^ now[8] ^ now[5] ^ now[4] ^ now[2];
		next[5] = now[14] ^ now[9] ^ now[8] ^ now[7] ^ now[4] ^ now[3] ^ now[1];
		next[4] = now[13] ^ now[8] ^ now[7] ^ now[6] ^ now[3] ^ now[2] ^ now[0];
		next[3] = now[15] ^ now[14] ^ now[7] ^ now[6] ^ now[5] ^ now[3] ^ now[2] ^ now[1];
		next[2] = now[14] ^ now[13] ^ now[6] ^ now[5] ^ now[4] ^ now[2] ^ now[1] ^ now[0];
		next[1] = now[15] ^ now[14] ^ now[13] ^ now[5] ^ now[4] ^ now[1] ^ now[0];
		next[0] = now[15] ^ now[13] ^ now[4] ^ now[0];
		/* The 32 bits of the output have been generated in the "next" array. */
		/* Reassemble the bits into a 32 bit Dword. */
		scrambler = 0;
		for (j = 31; j >= 0; --j)
		{
			scrambler = scrambler << 1;
			scrambler |= next[j];
		}
		/* The upper half of the scrambler output is stored backed into the */
		/* register as the saved context for the next cycle. */
		context = scrambler >> 16;
		scrambler_data[i] = scrambler;
	}
	return 0;
}

char* read_data_str(CsvHandle handle)
{
	char* row = NULL;
	row = CsvReadNextRow(handle);
	if(row)
	{
		char* time_s;		
		 // 1st col "Time (s)"
		time_s = CsvReadNextCol(row, handle);
		if(time_s != NULL)
		{
			// 2nd Col "8b10bIBM_Hex"
			return CsvReadNextCol(row, handle);
		}else
		{
			return NULL;
		}
	}else
	{
		return NULL;
	}
}

/*
 Return
	0=End of file
	1=PRIMITIVE
	2=DATA(SATA SOF detected)
	3=NO DATA(SATA No SOF detected or EOF)
	4=ERROR on DATA
*/
int read_data(CsvHandle handle, uint32_t* data_u32, double* data_time_s)
{
	static int start_data = 0; /* 1=SOF received (reset to 0 after EOF) */
	#define DATA32BITS_ASCII_HEX_LEN (8*2)
	char* data32bits_ASCII_HEX[DATA32BITS_ASCII_HEX_LEN+1] = { 0 };		
	char* row = NULL;

	row = CsvReadNextRow(handle);
	if(row)
	{
		const char* time;
		const char* data;
		time = CsvReadNextCol(row, handle); // 1st col "Time (s)"
		if(time == NULL)
		{
			return 0; /* Error end of file */
		}
		*data_time_s = atof(time);
		data = CsvReadNextCol(row, handle); // 2nd Col "8b10bIBM_Hex"
		if(data != NULL)
		{		
			if(data[0] == 'K' && data[1] == '.') //
			{
				int i;
				/* Read 4 Bytes */
				strncat(data32bits_ASCII_HEX, &data[2], 2);
				for(i = 1; i < 4; i++)
				{
					data = read_data_str(handle);
					if(data != NULL)
					{
						if( strcmp(data, "ERROR") != 0 )
						{
							strncat(data32bits_ASCII_HEX, data, 2);
						}else
						{
							return 4; /* ERROR on Data */
						}
					}else
					{
						return 0; /* Error end of file */
					}
				}
				*data_u32 = strtoul(data32bits_ASCII_HEX, NULL, 16);
				switch(*data_u32)
				{
					case SERDES_PRIMITIVE_SOF:
						start_data = 1;
					break;

					case SERDES_PRIMITIVE_EOF:
						start_data = 0;
					break;

					default: /* Nothing to do */
					break;
				}
				return 1; // PRIMITIVE
			}

			if( strcmp(data, "ERROR") != 0 )
			{
				const char* Bytes[4]; /* SATA Primitive encoding Byte0 to Byte3 Little Endian */
				int i;
				if(start_data != 1)
				{
					return 3; /* NO DATA (SATA SOF not detected or EOF) */
				}
				/* Read 4 Bytes */
				Bytes[0] = &data[0];
				for(i = 1; i < 4; i++)
				{
					data = read_data_str(handle);
					if(data != NULL)
					{
						if( strcmp(data, "ERROR") != 0 )
						{
							Bytes[i] = data;
						}else
						{
							return 4; /* ERROR on Data */
						}
					}else
					{
						return 0; /* Error end of file */
					}
				}
				/* Reorder bytes MSB to LSB */
				snprintf(data32bits_ASCII_HEX, DATA32BITS_ASCII_HEX_LEN, "%s%s%s%s",
							Bytes[3], Bytes[2], Bytes[1], Bytes[0]);
				*data_u32 = strtoul(data32bits_ASCII_HEX, NULL, 16);
				return 2; // DATA(SATA SOF detected)
			}else
			{
				return 4; /* ERROR on Data */
			}
		}else
		{
			return 0; /* Error end of file */
		}
	}else
	{
		return 0; /* Error end of file */
	}
}

int error_exit_invalid_csv_file(CsvHandle handle)
{
	printf("Error invalid csv file it shall contains \"Time (s),8b10bIBM_Hex\" data\n");
	fflush(stdout);
	CsvClose(handle);
	exit(-1);
}

int main(int argc, char **argv)
{
	int i, packet_no, ret;
	CsvHandle handle;
	uint32_t data_u32;
	uint32_t crc32_ret;
	uint32_t crc32_expected;

	double data_time_s;
	char* data = NULL;
	char* time_s = NULL;
	char* row = NULL;
	char* csvname = NULL;
	int verbose = 0;

	printf("CH569 SerDes data capture extractor(Descramble/check CRC32) v1.0 by B.VERNOUX 08 Jun 2022\n");
	if(argc < 2)
	{
		printf("First parameter is mandatory and shall be set to fullpath *.csv file\nThe csv file shall contain only \"Time (s)\" & \"8b10bIBM_Hex\" data exported from glscopeclient\n");
		exit(-1);
	}
	csvname = argv[1];
	if(argc == 3)
	{
		if(strcmp(argv[2], "-v") == 0)
		{
			printf("Verbose mode\n");
			/* Verbose mode enabled */
			verbose = 1;
		}
	}
	scramble();

	/* SATA CRC-32 Test */
#if 0
	data_u32_buf[0] = 0x00308027;
	data_u32_buf[1] = 0xE1234567;
	data_u32_buf[2] = 0x00000000;
	data_u32_buf[3] = 0x00000002;
	data_u32_buf[4] = 0x00000000;
	/* Expected results
		Running CRC value is 0x11E353FD
		Running CRC value is 0x0F656DA7
		Running CRC value is 0x3D14369C
		Running CRC value is 0x92D0D681
		Running CRC value is 0x319FFF6F
		The total number of data words processed was 5
		The CRC is 0x319FFF6F
	*/
	crc32_ret = CRC32_SERDES_START;
	for(i = 0; i < 5; i++)
	{
		crc32_ret = update_crc32_sata(crc32_ret, data_u32_buf[i]);
		printf("Running CRC value is 0x%08X\n", crc32_ret);
	}
#endif

	handle = CsvOpen2(csvname, ',', '"', '\\');
	if (!handle)
	{
		printf("Error can not open '%s' file\n", csvname);
		return -1;
	}

	/* Basic check of csv file format (headers) */
	row = CsvReadNextRow(handle);
	if(row)
	{
		 // 1st col "Time (s)"
		time_s = CsvReadNextCol(row, handle);
		if(time_s != NULL)
		{
			if(strcmp(time_s, "Time (s)") == 0)
			{
				// 2nd Col "8b10bIBM_Hex"
				data = CsvReadNextCol(row, handle);
				if(strcmp(data, "8b10bIBM_Hex") != 0)
				{
					error_exit_invalid_csv_file(handle);
				}
			}else
			{
				error_exit_invalid_csv_file(handle);
			}
		} else
		{
			error_exit_invalid_csv_file(handle);
		}
	}else
	{
		error_exit_invalid_csv_file(handle);
	}

	i = 0;
	packet_no = 0;
	do
	{
		ret = read_data(handle, &data_u32, &data_time_s);
		switch(ret)
		{
			case 0: // 0=End Of file
			{
				break;
			}
			case 1: // 1=PRIMITIVE
			{
				//printf("K %06d 0x%08X ", i, data_u32);
				switch(data_u32)
				{
					case SERDES_PRIMITIVE_SOF:
					{
						serdes_start_data = 1;
						i = 0;
						packet_no++;
						printf("\nSerDes SOF data(HEX) packet%d at %05.08fs/%.02fus:\n", packet_no, data_time_s, data_time_s*TIME_S_TO_MICROSEC);
					}
					break;

					case SERDES_PRIMITIVE_EOF:
					{
						if(serdes_start_data == 1)
						{
							serdes_start_data = 0;
							if(i > 0)
							{
								int j;
								crc32_expected = data_u32_buf[i-1];
								/* Check CRC-32 */
								crc32_ret = CRC32_SERDES_START;
								for(j = 0; j < (i-1); j++)
								{
									crc32_ret = update_crc32_sata(crc32_ret, data_u32_buf[j]);
								}
								if(crc32_ret == crc32_expected)
								{
									printf("(SerDes CRC32 OK(Nb32bits=%d) at %05.08fs/%.02fus)\n",i-1, data_time_s, data_time_s*TIME_S_TO_MICROSEC);
								}else
								{
									printf("(SerDes CRC32 ERROR !!)\n");
									printf("crc32_expected=0x%08X crc32_ret=0x%08X (Nb32bits=%d)\n", crc32_expected, crc32_ret, i-1);
								}
								i = 0;
							}
						}else
						{
							if(serdes_first_end_data == 1)
								printf("\n");
						}
						serdes_first_end_data = 1;						
					}
					break;

					case SERDES_PRIMITIVE_SYNC:
					{
						if(verbose)
						{
							if(serdes_start_data)
								printf("\n");
							printf("SYNC at %05.08fs/%.02fus\n", data_time_s, data_time_s*TIME_S_TO_MICROSEC);
						}
					}
					break;

					case SERDES_PRIMITIVE_CONT:
					{
						if(verbose)
						{
							if(serdes_start_data)
								printf("\n");
							printf("CONT at %05.08fs/%.02fus\n", data_time_s, data_time_s*TIME_S_TO_MICROSEC);
						}
					}
					break;

					case SERDES_PRIMITIVE_ALIGN:
					{
						if(verbose)
						{
							if(serdes_start_data)
								printf("\n");
							printf("ALIGN at %05.08fs/%.02fus\n", data_time_s, data_time_s*TIME_S_TO_MICROSEC);
						}
					}
					break;

					default:
					{
						if(verbose)
							printf("\nSerDes unknown primitive: K 0x%08X at %05.08fs/%.02fus\n",data_u32, data_time_s, data_time_s*TIME_S_TO_MICROSEC);
					}
				}
			}
			break;

			case 2: // 2=DATA
			{
				data_u32_buf[i] = (data_u32 ^ scrambler_data[i]);
				//printf("D %06d 0x%08X => Descramble 0x%08X\n", i, data_u32, data_u32_buf[i]);
				if(i == 0) /* First 32bits is Header => 8bits(including 4bits packet counter) + 24bits Identifier */
				{
					printf("%08X (SerDes HDR at %05.08fs/%.02fus)\n", data_u32_buf[i], data_time_s, data_time_s*TIME_S_TO_MICROSEC);
				}else
				{
					printf("%08X ", data_u32_buf[i]);
					if( !(i%8) )
					{
						printf("\n");
					}
				}
				if(i < MAX_NB_32BITS)
					i++;
			}
			break;

			case 3: // 3=NO DATA(SATA No SOF detected or EOF)
			{
				if(verbose)
					printf("N 0x%08X at %05.08fs/%.02fus\n",data_u32, data_time_s, data_time_s*TIME_S_TO_MICROSEC);
			}
			break;

			case 4: // 4=ERROR
			{
				if(verbose)
					printf("\nSerDes DATA ERROR at %05.08fs/%.02fus\n", data_time_s, data_time_s*TIME_S_TO_MICROSEC);
			}
			break;

			default: // Unknown state
			{
				if(verbose)
					printf("Unknown state %d at %05.08fs/%.02fus\n", ret, data_time_s, data_time_s*TIME_S_TO_MICROSEC);
			}
		}
	}while(ret);

	CsvClose(handle);
	if(verbose)
		printf("\nEnd of csv file, exit\n");
	return 0;
}
