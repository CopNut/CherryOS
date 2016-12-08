//----------object memory----------
typedef struct MemBlock{
	uint addr, size;
}MemBlock, *MemBlockPtr;

//32K
#define MEM_BLOCK_MAX 		4092 //4096 * (4 + 4) - (4 * 8) / 8 = 4092
typedef struct Memory{
	uint physize;
	uint freesize;
	uint frees;
	uint maxfrees;
	uint usesize;
	uint uses;
	uint lostsize;
	uint losts;	
	MemBlock freeMemBlock[MEM_BLOCK_MAX];
}Memory, *MemoryPtr;


void Memory__construct(Memory *this, uint start, uint end);

//check how much RAM are useful from start to end
uint Memory_check(uint start, uint end);

uint Memory_alloc(Memory *this, uint size);

int Memory_free(Memory *this, uint addr, uint size);

uint Memory_alloc_4k(Memory *this, uint size);

int Memory_free_4k(Memory *this, uint addr, uint size);


#define ADDR_MEMBUF			0x003c0000
#define EFLAGE_AC_BIT 		0x00040000
#define CR0_CACHE_DISABLE 	0x60000000
#define MEM_CHECK_START 	0x00400000
#define MEM_CHECK_END 		0xffffffff