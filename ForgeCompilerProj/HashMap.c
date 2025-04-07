#include "HashMap.h"

/// <summary>
/// This func will get a value from the hashmap based on the key, if there is no value it returns NULL
/// </summary>
/// <param name="key"></param>
/// <param name="map"></param>
/// <returns>The func will return a value or Null </returns>
void* get_hashmap_value(void* key, HashMap* map)
{
	// Get the hashcode for the key
	unsigned long hashCode = map->hash_func(key, map->map_size);
	HashNode* current = map->arr[hashCode];
	// Go over the linked list until we find the same key
	while (current != NULL) {
		if (map->equal_func(key, current->key)) {
			return current->value;  
		}
		current = current->next;
	}
	return NULL;  
}

/// <summary>
/// This func is used to insert a new value into the hashmap
/// </summary>
/// <param name="key"></param>
/// <param name="value"></param>
/// <param name="map"></param>
void insert_new_value(void* key, void* value, HashMap* map)
{
	// get the hashcode
	unsigned long hashCode = map->hash_func(key, map->map_size);
	HashNode* current = map->arr[hashCode];
	// check if the key exists
	while (current) {
		if (map->equal_func(current->key, key)) {
			// overite the value if found key
			map->FreeValue(current->value);
			current->value = value;
			return; 
		}
		current = current->next;
	}
	// if we fill a new bucket update the used spaces
	if (!map->arr[hashCode])
		map->usedSpaces++;
	// no key found, need new node
	HashNode* node = get_hash_node(key, value);
	node->next = map->arr[hashCode];
	map->arr[hashCode] = node;
	// resize map if needed
	if ((float)map->usedSpaces / map->map_size >= map->load_factor) {
		resize_map(map);
	}
}

/// <summary>
/// This func creates a new hash node and sets its key and value
/// </summary>
/// <param name="key"></param>
/// <param name="value"></param>
/// <returns>Returns a HashNode* or Null if it wasnt able to allocate memory</returns>
HashNode* get_hash_node(void* key, void* value) 
{
	HashNode* node = (HashNode*)malloc(sizeof(HashNode));
	if (!node) {
		fprintf(stderr, "Memory allocation failed for HashNode");
		return NULL;
	}
	node->key = key;
	node->value = value;
	node->next = NULL;
	return node;
}


/// <summary>
/// This func inits the starting hashmap, it creates a hashmap based on a specific size and also sets the hash and equals func
/// </summary>
/// <param name="size"></param>
/// <param name="hash_func"></param>
/// <param name="equal_func"></param>
/// <returns>Returns a pointer to a new hashmap or NULL if it wasnt able to allocate memory</returns>
HashMap* init_hashmap(int size, unsigned long (*hash_func)(void*, int), int (*equal_func)(void*, void*),
	void (*print_key)(void*), void (*print_value)(void*), void (*free_key)(void*), void (*FreeValue)(void*))
{
	// create the map
	HashMap* map = (HashMap*)malloc(sizeof(HashMap));
	if (!map) {
		fprintf(stderr, "Memory allocation failed for the map\n");
		return NULL;
	}
	// create the arr in the map
	map->arr = (HashNode**)calloc(size, sizeof(HashNode*));
	if (!map->arr) {
		fprintf(stderr, "Memory allocation failed for the map array\n");
		free(map);
		return NULL;
	}
	// set the map size and load factor
	map->map_size = size;
	map->load_factor = 0.75f;
	map->usedSpaces = 0;
	map->hash_func = hash_func;
	map->equal_func = equal_func;
	map->free_key = free_key;
	map->FreeValue = FreeValue;
	map->print_key = print_key;
	map->print_value = print_value;
	return map;
}

/// <summary>
/// This func will free the hashmap and all of the pointers in it
/// </summary>
/// <param name="map"></param>
void free_hashmap(HashMap** map) {
	// check if we need to free anything
	if (map == NULL || *map == NULL) {
		return;
	}
	// go over all the arr 
	for (int i = 0; i < (*map)->map_size; i++) {
		// get the head of the linked list
		HashNode* currentHead = (*map)->arr[i];
		while (currentHead != NULL) {
			HashNode* temp = currentHead;
			currentHead = currentHead->next;
			// free the pointers to the data
			(*map)->free_key(temp->key);
			(*map)->FreeValue(temp->value);
			// free the node itself
			free(temp);
		}
		(*map)->arr[i] = NULL; // set the pointer in the arr to null
	}
	// free the arr memory
	free((*map)->arr);
	// set the arr pointer to null
	(*map)->arr = NULL;
	// free the map itself
	free(*map);
	// set the map pointer to null
	*map = NULL;
}

/// <summary>
/// This func is used to resize the hashmap
/// </summary>
/// <param name="map"></param>
void resize_map(HashMap* map) {
	// save old data
	int oldSize = map->map_size;
	HashNode** oldArr = map->arr;
	// double the size
	map->map_size *= 2;
	map->arr = (HashNode**)calloc(map->map_size, sizeof(HashNode*));
	if (map->arr == NULL) {
		fprintf(stderr, "Memory allocation failed during resize\n");
		return;
	}
	// refill the old data
	map->usedSpaces = 0;
	for (int i = 0; i < oldSize; i++) {
		HashNode* current = oldArr[i];
		while (current != NULL) {
			HashNode* nextNode = current->next;
			// reinsert the hash nodes
			insert_new_value(current->key, current->value, map);
			// free the current node
			free(current);
			current = nextNode;
		}
	}
	// free the old arr
	free(oldArr);
}

/// <summary>
/// This is a print function for the hashmap
/// </summary>
/// <param name="map"></param>
void print_hashmap(HashMap* map) {
	if (map == NULL) {
		printf("HashMap is NULL.\n");
		return;
	}
	printf("HashMap Contents:\n");
	printf("Map Size: %d, Used Spaces: %d, Load Factor: %.2f\n", map->map_size, map->usedSpaces, map->load_factor);
	for (int i = 0; i < map->map_size; i++) {
		if (map->arr[i] != NULL) {
			printf("Bucket %d: ", i);
			HashNode* current = map->arr[i];
			
			while (current != NULL) {
				map->print_key(current->key);
				map->print_value(current->value);
				current = current->next;
			}
			printf("-> NULL");
			
			printf("\n");
		}
	}
}

/// <summary>
/// This is the hash func for the action string, it is a djb2 hash func
/// </summary>
/// <param name="str"></param>
/// <returns>Returns a hash for a given string</returns>
unsigned long djb2Hash(const char* str) {
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}