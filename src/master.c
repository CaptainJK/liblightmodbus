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

#include <lightmodbus/master.h>
#include <lightmodbus/core.h>
#include <lightmodbus/parser.h>
#include <lightmodbus/master/mpregs.h>
#include <lightmodbus/master/mpcoils.h>

uint8_t modbusParseException( ModbusMaster *status, union ModbusParser *parser )
{
	//Parse exception frame and write data to MODBUSMaster structure

	//Check if given pointers are valid
	if ( status == NULL || parser == NULL ) return MODBUS_ERROR_OTHER;

	//Copy data (modbusParseResponse checked if length is 5 so it should be safe)
	status->exception.address = parser->exception.address;
	status->exception.function = parser->exception.function;
	status->exception.code = parser->exception.code;

	return MODBUS_ERROR_EXCEPTION;
}

uint8_t modbusParseResponse( ModbusMaster *status )
{
	//This function parses response from master
	//Calling it will lead to losing all data and exceptions stored in MODBUSMaster (space will be reallocated)

	//Note: crc is now checked here

	//If non-zero some parser failed its job
	uint8_t err = 0;

	//Check if given pointer is valid
	if ( status == NULL ) return MODBUS_ERROR_OTHER;

	//Reset output registers before parsing frame
	status->exception.address = 0;
	status->exception.function = 0;
	status->exception.code = 0;

	#ifndef LIGHTMODBUS_STATIC_MEM_MASTER_DATA
		free( status->data.coils );
		status->data.coils = NULL;
		status->data.regs = NULL;
	#endif
	status->data.length = 0;
	status->data.index = 0;
	status->data.count = 0;
	status->data.type = 0;
	status->data.address = 0;
	status->data.function = 0;

	//Check if frames are not too short and return error (to avoid problems with memory allocation)
	//That enables us to ommit the check in each parsing function
	if ( status->response.length < 4u || status->response.frame == NULL || \
	 	status->request.length < 4u || status->request.frame == NULL )
			return MODBUS_ERROR_OTHER;

	//Check both response and request frames CRC
	//Has to be done the weird way, which isn't safer at all, because avr-gcc didn't like it
	//warning: dereferencing type-punned pointer will break strict-aliasing rules
	uint16_t *crcresp = (uint16_t*)status->response.frame + status->response.length - 2;
	uint16_t *crcreq = (uint16_t*)status->request.frame + status->request.length - 2;
	if ( *crcresp != modbusCRC( status->response.frame, status->response.length - 2 ) ||\
		*crcreq != modbusCRC( status->request.frame, status->request.length - 2 ) )
			return MODBUS_ERROR_CRC;

	union ModbusParser *parser = (union ModbusParser*) status->response.frame;
	union ModbusParser *requestParser = (union ModbusParser*) status->request.frame;

	//Check if frame is exception response
	if ( parser->base.function & 128 && status->response.length == 5 )
	{
		err = modbusParseException( status, parser );
	}
	else
	{
		switch ( parser->base.function )
		{
			case 1: //Read multiple coils
			case 2: //Read multiple discrete inputs
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse0102( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 3: //Read multiple holding registers
			case 4: //Read multiple input registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse0304( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 5: //Write single coil
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse05( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 6: //Write single holding reg
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse06( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 15: //Write multiple coils
				if ( LIGHTMODBUS_MASTER_COILS ) err = modbusParseResponse15( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 16: //Write multiple holding registers
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse16( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			case 22: //Mask write holding register
				if ( LIGHTMODBUS_MASTER_REGISTERS ) err = modbusParseResponse22( status, parser, requestParser );
				else err = MODBUS_ERROR_PARSE;
				break;

			default: //function code not known by master
				err = MODBUS_ERROR_PARSE;
				break;
		}
	}
	return err;
}

uint8_t modbusMasterInit( ModbusMaster *status )
{
	//Check if given pointer is valid
	if ( status == NULL ) return MODBUS_ERROR_OTHER;

	//Very basic init of master side
	#ifndef LIGHTMODBUS_STATIC_MEM_MASTER_REQUEST
		status->request.frame = NULL;
	#endif

	#ifndef LIGHTMODBUS_STATIC_MEM_MASTER_DATA
		status->data.coils = NULL;
		status->data.regs = NULL;
	#endif

	status->request.length = 0;
	status->response.frame = NULL;
	status->response.length = 0;
	status->data.length = 0;
	status->data.count = 0;
	status->data.index = 0;
	status->data.type = 0;
	status->data.address = 0;

	status->exception.address = 0;
	status->exception.function = 0;
	status->exception.code = 0;

	return MODBUS_ERROR_OK;
}

uint8_t modbusMasterEnd( ModbusMaster *status )
{
	//Check if given pointer is valid
	if ( status == NULL ) return MODBUS_ERROR_OTHER;

	//Free memory
	#ifndef LIGHTMODBUS_STATIC_MEM_MASTER_REQUEST
		free( status->request.frame );
	#endif
	#ifndef LIGHTMODBUS_STATIC_MEM_MASTER_DATA
		free( status->data.coils );
		status->data.coils = NULL;
		status->data.regs = NULL;
	#endif

	return MODBUS_ERROR_OK;
}
