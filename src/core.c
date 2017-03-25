/*
	liblightmodbus - a lightweight, multiplatform Modbus library
	Copyright (C) 2016 Jacek Wieczorek <mrjjot@gmail.com>

	This file is part of liblightmodbus.

	Liblightmodbus is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Liblightmodbus is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <lightmodbus/core.h>

uint8_t modbusMaskRead( uint8_t *mask, uint16_t maskLength, uint16_t bit )
{
	//Return nth bit from uint8_t array
	//When 255 value is returned, an error occured

	if ( mask == NULL ) return MODBUS_ERROR_OTHER;
	if ( ( bit >> 3 ) >= maskLength ) return MODBUS_ERROR_OTHER;
	return ( mask[bit >> 3] & ( 1 << ( bit % 8 ) ) ) >> ( bit % 8 );
}

uint8_t modbusMaskWrite( uint8_t *mask, uint16_t maskLength, uint16_t bit, uint8_t value )
{
	//Write nth bit in uint8_t array
	//When 255 value is returned, an error occured

	if ( mask == NULL ) return MODBUS_ERROR_OTHER;
	if ( ( bit >> 3 ) >= maskLength ) return MODBUS_ERROR_OTHER;
	if ( value )
		mask[bit >> 3] |= ( 1 << ( bit % 8 ) );
	else
		mask[bit >> 3] &= ~( 1 << ( bit % 8 ) );
	return value != 0;
}

uint16_t modbusSwapEndian( uint16_t data )
{
	//Change big-endian to little-endian and vice versa

	uint8_t swap;

	//Create 2 bytes long union
	union
	{
		uint16_t data;
		uint8_t bytes[2];
	} conversion;

	//Swap bytes
	conversion.data = data;
	swap = conversion.bytes[0];
	conversion.bytes[0] = conversion.bytes[1];
	conversion.bytes[1] = swap;

	return conversion.data;
}

uint16_t modbusCRC( uint8_t *data, uint16_t length )
{
	//Calculate CRC16 checksum using given data and length

	uint16_t crc = 0xFFFF;
	uint16_t i;
	uint8_t j;

	if ( data == NULL ) return 0;

	for ( i = 0; i < length; i++ )
	{
		crc ^= (uint16_t) data[i]; //XOR current data byte with crc value

		for ( j = 8; j != 0; j-- )
		{
			//For each bit
			//Is least-significant-bit is set?
			if ( ( crc & 0x0001 ) != 0 )
			{
				crc >>= 1; //Shift to right and xor
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}
