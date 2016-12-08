#include <stdio.h>
#include <memory.h>

void Memory__construct(Memory *this, uint start, uint end){

	char flag486 = 0;
	uint eflag, cr0;

	eflag = io_load_eflags();
	eflag |= EFLAGE_AC_BIT; //AC-bit = 1;
	io_store_eflags(eflag);
	eflag = io_load_eflags();
	if (eflag == 1)
	{
		flag486 = 1; //if AC-bit doesn's roll back to 0, the machine is 486
	}
	eflag &= ~EFLAGE_AC_BIT; //make AC-bit back to 0
	io_store_eflags(eflag);

	if (flag486 == 1) //disable the cache for Memory_check()
	{
		cr0 = io_load_cr0();
		cr0 |= CR0_CACHE_DISABLE; //cr0 0x60000000 set to 1, cache will be disabled
		io_store_cr0(cr0);
	}

	this->physize = Memory_check(start, end);

	if (flag486 == 1)
	{
		cr0 = io_load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; //enable the cache
		io_store_cr0(cr0);
	}

	this->freesize = 0;
	this->frees = 0;
	this->maxfrees = 0;
	this->usesize = 0;
	this->uses = 0;
	this->lostsize = 0;
	this->losts = 0;

	return;
}

uint Memory_useable(Memory *this){

	uint total = 0;
	for (uint i = 0; i < this->frees; ++i)
	{
		total += this->freeMemBlock[i].size;
	}

	return total;
}

uint Memory_alloc(Memory *this, uint size){

	uint addr = 0; //memory pointer for the allocation result
	for (uint i = 0; i < this->frees; ++i)
	{
		if (this->freeMemBlock[i].size >= size)
		{
			//memory avilable
			addr = this->freeMemBlock[i].addr;
			this->freeMemBlock[i].addr += size;
			this->freeMemBlock[i].size -= size;

			if (this->freeMemBlock[i].size == 0)
			{
				//one block has been allocated, delete from memory->frees
				this->frees--;
				for (; i < this->frees; ++i)
				{
					//fill the blank field of freeMemBlock array
					this->freeMemBlock[i] = this->freeMemBlock[i + 1];
				}
			}
			this->freesize -= size;
			return addr;
		}
	}
	//no avilable memory
	return 0;
}


int Memory_free(Memory *this, uint addr, uint size){

	uint i;
	for (i = 0; i < this->frees; ++i)
	{
		if (this->freeMemBlock[i].addr > addr)
		{
			break;
		}
	}
	//freeMemBlock[i -1].addr < addr < freeMemBlock[i].addr
	
	if (i > 0)
	//there are blocks before this freeing block
	{
		if (this->freeMemBlock[i - 1].addr + this->freeMemBlock[i - 1].size == addr)
		//freeing block can make up with the one in the front of
		{
			this->freeMemBlock[i - 1].size += size;
			if (addr + size == this->freeMemBlock[i].addr)
			//freeing block can also make up with the one right behind
			{
				this->freeMemBlock[i -1].size += this->freeMemBlock[i].size;
				this->frees--;
				for (; i < this->frees; ++i)
				{
					this->freeMemBlock[i] = this->freeMemBlock[i + 1];
				}
			}
		//free complished
		this->freesize += size;
		return 0;
		}
	}

	if (i < this->frees)
	//there are blocks after this freeing block 
	{
		if (addr + size == this->freeMemBlock[i].addr)
		//freeing block can make up with the one right behind
		{
			this->freeMemBlock[i].addr += addr;
			this->freeMemBlock[i].size += size;
			//free complished
			this->freesize += size;
			return 0;
		}
	}

	if (this->frees < MEM_BLOCK_MAX)
	//isolated block
	{
		//right shift blocks from i to frees
		for (uint j = this->frees; j > i; --j)
		{
			this->freeMemBlock[j] = this->freeMemBlock[j - 1];
		}
		//frees increas
		this->frees++;
		if (this->frees > this->maxfrees)
		{
			this->maxfrees = this->frees;
		}

		this->freeMemBlock[i].addr = addr;
		this->freeMemBlock[i].size = size;
		this->freesize += size;
		return 0;
	}

	//block buffer overflow
	this->losts++;
	this->lostsize += size;
	return -1;
}

uint Memory_alloc_4k(Memory *this, uint size){

	uint addr;
	size = (size + 0xfff) & 0xfffff000;
	Memory_alloc(this, size);

	return addr;
}

int Memory_free_4k(Memory *this, uint addr, uint size){

	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = Memory_free(this, addr, size);

	return i;
}
