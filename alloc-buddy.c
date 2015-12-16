/*
 * Papers:
 *  - Fast Allocation and Deallocation with an Improved Buddy System
 *     Erik D. Demaine and J.  Ian Munro
 *
 *  -
 */

struct buddy {
	void *base;
	size_t block_sz;
	size_t block_ct;

	/* we need a quick way to determine if a given block is allocated (to
	 * make free() fast)*/

	/* we need a quick way to find the "best" free blocks to select when the
	 * allocator asks for N blocks (to make alloc() fast)
	 */
	
	/* provides some of the same functionality as linux's page bits */
	/* TODO: consider if we need/should store "order" here */
	void *free_bitmap;
};

struct buddy_free_block_data {
	size_t order;
};


size_t buddy_free_bitmap_len(size_t block_ct)
{
	return block_ct / CHAR_BIT;
}

/*
 * Assume:
 *  - a contiguous region (we can't pair blocks between split regions anyhow)
 *
 * TODO:
 *  - provide an initializer that takes it's free_bitmap from the memory supplied
 *  - provide an initializer that can put struct buddy in the memory supplied
 *
 * XXX:
 *  - common designs use a set of lists, one for each order
 */
int buddy_init(struct buddy *b, void *free_bitmap, void *base, size_t block_sz, size_t block_ct)
{
	/* TODO: check that base is aligned to block_sz */

	/* block_sz * 2**x == block_sz * block_ct
	 * 2**x == block_ct
	 * x = log2(block_ct)
	 *
	 */
	size_t high_order = 

	*b = (struct buddy) {
		.base = base,
		.block_sz = block_sz,
		.block_ct = block_ct,
		.free_bitmap = free_bitmap
		.high_order = 
	};
}

void *buddy_alloc(struct buddy *b, size_t order)
{
	/* find a block with:
	 *  - with the given order
	 *  - or if no such block exists, split a larger order block
	 */

	/*
	 * Iterate over top-level blocks, then follow them down.
	 * If things get too small before finding a free block of the appropriate size, go up a level
	 * If the first free block found is too large, split it
	 *
	 * This means:
	 *  - we pick the first free block (instead of the "best" or "highest"), which might cause fragmentation.
	 *  - could search in reverse to lower fragmentation
	 */
	size_t i_order = 
	for (;;) {
		
	}
}

/*
 * Unclear:
 *  - block count might not be required, but probably is
 */
void *buddy_free(struct buddy *b, size_t order)
{

}
