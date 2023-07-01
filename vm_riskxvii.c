#include "vm.h"
#include "kernel.h"

#include ".tests/dump.h"

i32 main(int argc, char* argv[])
{
    if (argc != PROGRAM_ARGS) TERMINATE;
    FILE *fp = fopen( BINARY_IMAGE, "rb" );
    fread( VM.M, TOTAL_INSTRUCTIONS, sizeof(u32), fp );
    for(;;) { $zero = 0; execute( INSTRUCTION ); }
    fclose( fp );
    return 0;
}

