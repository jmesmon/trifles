#pragma once
/*
 * alloc-buddy is a general buddy allocator capable of using arbitrary (power of 2) block sizes
 *
 * Only designed to handle contiguous memory regions.
 *
 */
#include <stdlib.h>
#include <stdint.h>

struct buddy_block_info {
	/*
	 * if 1, this block is allocated
	 * if 0, it is free
	 */
	uint8_t allocated:1;

	/*
	 * we probably don't need this many bits
	 * max block order = 2**128
	 *
	 * If this is set to `SIZE_IN_BLOCK`, the `order_upper` and
	 * `order_lower` fields in the block itself must be examined.
	 *
	 * `SIZE_IN_BLOCK is not legal for allocated blocks, as we can't store
	 * anything in them (they are in use by others).
	 */
	uint8_t order:7;
};

struct buddy {
	/*
	 * first block
	 */
	void *base;
	
	/*
	 * Size of each block, given as a power of 2
	 *
	 * block_size_in_bytes = 1 << block_bytes_order;
	 */
	size_t block_bytes_order;

	/*
	 * number of blocks, each containing @block_size_in_bytes bytes
	 */
	size_t block_ct;

	/*
	 * Largest pairing of blocks together, limits size of maximum allocation.
	 * Power of 2
	 *
	 * max_allocation_in_blocks = 1 << max_blocks_order;
	 * max_allocation_in_bytes = block_size_in_bytes * max_allocation_in_blocks;
	 */
	size_t max_blocks_order;

	/*
	 * [0] = order 0 blocks
	 * [1] = order 1 blocks (2^1 * block_sz)
	 * [n] = order n blocks (2^n * block_sz)
	 * ...
	 * [max_blocks_order] = ...
	 *
	 * this points to an array with max_block_order entries
	 * The index in the array is the order of page stored in that list
	 *
	 * ie: pages double in size each time you move forward.
	 *
	 * This gives us forward scanning on allocation.
	 */
	//struct buddy_free_block *(*free_lists)[/* static max_blocks_order */];
	struct buddy_free_block **free_lists;
	
	/* 
	 * We need a quick way to determine if a given block is allocated (to
	 * make free() fast).
	 *
	 * Further, to allow the free-er to not need to know the order, it is
	 * useful to be able to look up the order.
	 */
	//struct buddy_block_info (*info)[/* static block_ct */];
	struct buddy_block_info *info;
};


// Initialize a buddy instance in `b`, with `base` as the base address, and `size_bytes` bytes in `base`.
//
// `block_bytes_order` is  log2(block_size), such that `1 << block_bytes_order`
// is the block size
//
// `max_blocks_order` is the maximum block order tracked, such that `1 <<
// max_blocks_order` is the largest block that can be allocated from this
// buddy.
int buddy_init(struct buddy *b, void *base, size_t size_bytes, size_t block_bytes_order, size_t max_blocks_order,
		// an array of pointers to `struct buddy_free_block`, array size is `max_blocks_order`.
		struct buddy_free_block *(*free_lists)[],
		// `buddy_block_count(size_bytes, block_bytes_order)`
		struct buddy_block_info (*info)[]);

// Allocate a single block with order `block_order`.
//
// XXX: how do we want to handle fractional allocations?
void *buddy_alloc_block(struct buddy *b, size_t block_order);
void buddy_free(struct buddy *b, void *block);

size_t buddy_block_count(size_t size_bytes, size_t block_bytes_order);
