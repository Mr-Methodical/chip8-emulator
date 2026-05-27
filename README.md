// I am doing a fetch decode execute cycle:
fetch gets 2 bytes from memory, memory is uint8_t so we have to
go twice and bring those two bits together to form th 16 bit opcode
then using our function pointer table, we decode instruction to know
which function we want to do, we then execute by running that function
