#ifndef FOS_KERN_KHEAP_H_
#define FOS_KERN_KHEAP_H_

#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

//Values for user heap placement strategy
#define KHP_PLACE_CONTALLOC 0x0
#define KHP_PLACE_FIRSTFIT 	0x1
#define KHP_PLACE_BESTFIT 	0x2
#define KHP_PLACE_NEXTFIT 	0x3
#define KHP_PLACE_WORSTFIT 	0x4


void* kmalloc(unsigned int size);
void kfree(void* virtual_address);
void *krealloc(void *virtual_address, uint32 new_size);
unsigned int kheap_virtual_address(unsigned int physical_address);
unsigned int kheap_physical_address(unsigned int virtual_address);
void Coalescing_heap(struct Block_Info *take);
void *cont_allocation(unsigned int size);
void *first_fit_allocation(unsigned int size);
void *next_fit_allocation(unsigned int size);
void insert_sorted(struct Linked_List_block *curr_list,struct Block_Info *elem);
#endif // FOS_KERN_KHEAP_H_
