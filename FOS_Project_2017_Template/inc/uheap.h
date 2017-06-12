#ifndef FOS_INC_MALLOC_H
#define FOS_INC_MALLOC_H 1

//Values for user heap placement strategy
#define UHP_PLACE_FIRSTFIT 	0x1
#define UHP_PLACE_BESTFIT 	0x2
#define UHP_PLACE_NEXTFIT 	0x3
#define UHP_PLACE_WORSTFIT 	0x4
void* first_fit_user(uint32 size);
void *malloc(uint32 size);
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable);
void* sget(int32 ownerEnvID, char *sharedVarName);
void free(void* virtual_address);
void sfree(void* virtual_address);
void *realloc(void *virtual_address, uint32 new_size);
void Coalescing_heap_user(struct Block_Info *take);
void allocate_block(struct Block_Info**neww);
void insert_sorted_user(struct Linked_List_block *curr_list,struct Block_Info *elem);
#endif
