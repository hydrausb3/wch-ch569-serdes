/*
 * Modified by B.VERNOUX -06 June 2022) for CRC-32 Serial ATA specification
 * SATA spec see https://www.seagate.com/support/disc/manuals/sata/sata_im.pdf
 */

#ifndef CRC32_SATA_H
#define CRC32_SATA_H

#include <stdint.h>

#define CRC32_SERDES_START 0x52325032L /* The same defined in CRC-32 SATA Spec */

/*
 * Prototype list of global functions
 */
//uint32_t crc_32(const unsigned char *input_str, size_t num_bytes);
uint32_t update_crc32_sata(uint32_t crc, uint32_t data_in);

#endif  // CRC32_SATA_H
