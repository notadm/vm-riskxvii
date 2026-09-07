#ifndef ALLOC_H 
#define ALLOC_H

#include "vm.h"

#define HEAP_MID        4096	
#define MAX_BANKS       128 
#define HALF_BANKS      64
#define MIN_ALLOC       64
#define BANK_SIZE       64
#define BASE_OFFSET     6
#define MAX_ALLOC       8192	
#define NO_SPACE        0
#define UNALLOCATED     0
#define ZERO_MASK       0x0000000000000000
#define FULL_MASK       0xFFFFFFFFFFFFFFFF 
#define INITIAL_MASK    0x0000000000000001

typedef enum region 
{
	UPPER,
	LOWER,
}
region;

typedef enum mem 
{
    	BYTES_64   = 	1,
    	BYTES_128  = 	2,
    	BYTES_256  = 	3,
    	BYTES_512  = 	4,
    	BYTES_1024 = 	5,
    	BYTES_2048 = 	6,
    	BYTES_4096 = 	7,
    	BYTES_8196 = 	8,
}
mem;


region bitmap_region(bitmap x) 
{
    return x.upper ? UPPER : LOWER;
}

// ALLOC OPS
i32 blocksize(u32 x) 
{
    if (x < MIN_ALLOC) return MIN_ALLOC;
    if (x > MAX_ALLOC) return NO_SPACE;
    x--;x|=x>>1;x|=x>>2;x|=x>>4;x|=x>>8;x|=x>>16;x++;
    return x;
}

u32 positional_offset(u64 positional_encoding) 
{
    u32 offset = 0;
    while( (positional_encoding & 1) != 1 ) 
    { 
	    positional_encoding >>= 1; offset++; 
    } 
    return offset * BANK_SIZE;
}

u64 kernel_encoding(u64 positional_encoding, u32 size) 
{
    u32     blocks      = size / BANK_SIZE;
    u64     kernel      = ZERO_MASK;
    for (int i = 0; i < blocks; i++) 
    {
	    kernel |= positional_encoding;
	    positional_encoding <<= 1;
    }
    return kernel;
}

u64 positional_encoding(u64 allocation, u32 size) 
{
    u64 	kernel 		= kernel_encoding(INITIAL_MASK, size);
    u64 	encoding 	= INITIAL_MASK;
    u32 	stride 		= size / BANK_SIZE;

    for (int i = 0; i < BANK_SIZE / stride; i++)
    { 
	if ( ((kernel & allocation) ^ kernel) == kernel ) return encoding;
	kernel 		<<= 	stride;
	encoding 	<<= 	stride;
    }

    return 0;
}

u32 encode_size(region block, u64 positional_mask, u32 size)
{
    mem     allocated   = BYTES_64; size >>= BASE_OFFSET; 
    while (((1 & size) != 1)) { size >>= 1; allocated++; }

    for (int i = 0; i < HEAP_BITMAP_DEPTH; i++) 
    { 	
	    switch (block) 
	    {
	        case LOWER: VM.allocsize[i].lower |= (allocated&1) ? positional_mask:ZERO_MASK; break;
	        case UPPER: VM.allocsize[i].upper |= (allocated&1) ? positional_mask:ZERO_MASK; break;
    	}
	    allocated >>= 1;
    }
    return 0;
}

u32 alloc(region block, u32 size) 
{
    u64     positional_mask = ZERO_MASK;
    u64     kernel_mask	    = ZERO_MASK;
    switch (block) 
    {
        case LOWER:  	
            positional_mask	 = positional_encoding(VM.allocation.lower, size);
            kernel_mask		 = kernel_encoding(positional_mask, size);
            VM.allocation.lower |= kernel_mask; 
            encode_size(block, positional_mask, size);
            return HEAP_OFFSET + positional_offset(positional_mask);
        case UPPER: 
            positional_mask	 = positional_encoding(VM.allocation.upper, size);
            kernel_mask		 = kernel_encoding(positional_mask, size);
            encode_size(block, positional_mask, size);
            VM.allocation.upper |= kernel_mask; 
            return HEAP_OFFSET + positional_offset(positional_mask) + HEAP_MID;
    }
    return 0;
}


u32 vmalloc(u32 x)
{
    u32 size = blocksize(x);
    if ( size == NO_SPACE  )  return NO_SPACE;
    if ( size == MAX_ALLOC ) 
    {	
	if (( VM.allocation.upper & FULL_MASK ) == ZERO_MASK && 
	    ( VM.allocation.lower & FULL_MASK ) == ZERO_MASK) 
	{ 
	    encode_size(LOWER, INITIAL_MASK, MAX_ALLOC);
	    VM.allocation.upper |= FULL_MASK;
	    VM.allocation.lower |= FULL_MASK;
	    return HEAP_OFFSET; 
	}
	    return NO_SPACE;
    }
    if (positional_encoding(VM.allocation.lower,size)) return alloc(LOWER,size);
    if (positional_encoding(VM.allocation.upper,size)) return alloc(UPPER,size);
    return 0; 
}

u32 bytemap(mem x) 
{
    switch(x)
    {
        case BYTES_64: 		return 64;
        case BYTES_128: 	return 128;
        case BYTES_256: 	return 256;
        case BYTES_512: 	return 512;
        case BYTES_1024: 	return 1024;
        case BYTES_2048: 	return 2048;
        case BYTES_4096: 	return 4096;
        case BYTES_8196: 	return 8196;
    }
    return 0;
}


// FREE OPS
i32 is_allocated(u32 x)
{
    region block;
    if (x < HEAP_OFFSET || x > MEMORY_LIMIT) return UNALLOCATED;
    if (x >= (HEAP_OFFSET+HEAP_MID))    { x -= HEAP_MID; 	block = UPPER; }
    else                                { block = LOWER;                       }
    u32     offset          = ((x - HEAP_OFFSET) / BANK_SIZE);
    u64     positional_mask = INITIAL_MASK << offset;
    switch (block) 
    {
        case UPPER: return VM.allocation.upper & positional_mask;
        case LOWER: return VM.allocation.lower & positional_mask;
    }
    return UNALLOCATED;
}

u32 decode_size(region block, u64 positional_mask) 
{
    mem size = 0;
    for (int i = HEAP_BITMAP_DEPTH - 1; i >= 0; i--) 
    {
	switch(block) 
	{
	    case UPPER: size |= (positional_mask & VM.allocsize[i].upper) ? 1 : 0; break;
	    case LOWER: size |= (positional_mask & VM.allocsize[i].lower) ? 1 : 0; break;
	}
	    size <<= 1;
    }
    size >>= 1;
    return size;
}


u32 delloc(region block, u32 x) 
{
    if ( (x - HEAP_OFFSET) % BANK_SIZE  )        illegal_operation();
    if ( x < HEAP_OFFSET || x > MEMORY_LIMIT )   illegal_operation();

    u32 offset          = (x - HEAP_OFFSET) / BANK_SIZE;
    u64 positional_mask = INITIAL_MASK << offset;

    u32 bytes = bytemap( decode_size(block, positional_mask) );
    if (bytes == UNALLOCATED) illegal_operation();

    u64 kernel = ~kernel_encoding(positional_mask, bytes);

    for (int i = 0; i < HEAP_BITMAP_DEPTH; i++) 
    { 	
        switch (block) 
        {
            case UPPER: VM.allocsize[i].upper &= ~positional_mask;      break;
            case LOWER: VM.allocsize[i].lower &= ~positional_mask;      break;
        }
    }
    switch (block) 
    {
        case UPPER: 	VM.allocation.upper &= kernel;                  break;
        case LOWER: 	VM.allocation.lower &= kernel;                  break;
    }
    return 0;
}

i32 vmfree(u32 x)
{
    return x>=(HEAP_OFFSET+HEAP_MID) ? delloc(UPPER,x-HEAP_MID):delloc(LOWER,x);
}

#endif
