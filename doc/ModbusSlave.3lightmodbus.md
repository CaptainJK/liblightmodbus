# ModbusSlave 3lightmodbus "25 March 2017" "v1.3"

## NAME
**ModbusSlave** - data type containing all information about current slave device status and its configuration.

## SYNOPSIS
`  
	typedef struct
	{
		uint8_t address; //Slave address
		uint16_t *registers; //Slave holding registers
		uint16_t registerCount; //Slave register count
		uint16_t *inputRegisters; //Slave input registers
		uint16_t inputRegisterCount; //Slave input count
		uint8_t *coils; //Slave coils
		uint16_t coilCount; //Slave coil count
		uint8_t *discreteInputs; //Slave discrete input
		uint16_t discreteInputCount; //Slave discrete input count
		uint8_t *registerMask; //Masks for register write protection
		uint16_t registerMaskLength; //Masks length (each byte covers 8 registers)
		uint8_t *coilMask; //Masks for coil write protection
		uint16_t coilMaskLength; //Masks length (each byte covers 8 coils)
		struct //Slave response formatting status
		{
			uint8_t *frame;
			uint8_t length;
		} response;
		struct //Request from master should be put here
		{
			uint8_t *frame;
			uint8_t length;
		} request;
	} ModbusSlave; //Type containing slave device configuration data
`

## DESCRIPTION
The **ModbusSlave** contains slave device configuration and pointers do its data arrays.

| member name | description |
|---|---|
| `address` | the slave's address |
| `registers` | slave's holding registers |
| `registerCount`| slave's holding register count |
| `inputRegisters` | slave's input registers |
| `inputRegisterCount`| slave's input register count |
| `coils` | slave's coils |
| `coilCount`| slave's coil count |
| `discreteInputs` | slave's discrete inputs |
| `discreteInputCount`| slave's discrete input count |
| `registerMask` | registers write protection mask |
| `registerMaskLength`| registers write protection mask length (bytes) |
| `coilMask` | registers write protection mask |
| `coilMaskLength`| coil write protection mask length (bytes) |
| `response.frame` | response frame for master device |
| `response.length`| response frame length |
| `request.frame` | request frame for slave device |
| `request.length` | request frame length |

### Initialization
Firstly, user needs to set up register count values (for each data type obviously), assign pointers to suitable arrays, to store data and set up slave's address. After that, **modbusSlaveInit** should be called on the structure.

### Write protection
Holding registers and coils can be write-protected. To achieve that, accordingly set **bits** to 1 in *registerMask* or *coilMask* arrays (of *registerMaskLength* or *coilMaskLength* lengths accordingly). Mask shorter than registers/coils array will affect in all 'uncovered' registers being possible to write. For instance, setting 17th bit to 1, will result in 17th register being read-only.
To write and read masks more easily see modbusMaskRead(3lightmodbus) and modbusMaskWrite(3lightmodbus).

### Normal use
Each holding/input register is a single field in a **uint16_t** array.
In *coils* and *discreteInputs* each bit of a **uint8_t** array matches exactly one input/output.
The *discreteInputCount* and *coilCount* variables correspond to actual input/output count, not the array length!

Library calls operate directly on the data located in the arrays described above. They should not modify their content without a reason, but if such exception happens, please report it as a bug.

Simple initialization example:
`  
	ModbusSlave status;
	uint16_t arr[7];
	uint8_t arr2[5];
	status.address = 16;
	status.registers = arr;
	status.registerCount = 7;
	status.coils = arr2;
	status.coilCount = 5 * 8; //40
	status.inputRegisters = NULL;
	status.inputRegisterCount = 0;
	status.discreteInputs = NULL;
	status.discreteInputCount = 0;
	modbusSlaveInit( &status );
`

## SEE ALSO
modbusSlaveInit(3lightmodbus), modbusSlaveEnd(3lightmodbus)

## AUTHORS
Jacek Wieczorek (Jacajack) - mrjjot@gmail.com
