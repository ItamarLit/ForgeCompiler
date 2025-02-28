#include "HashMap.h"

/// <summary>
/// This func will get a value from the hashmap based on the key, if there is no value it returns NULL
/// </summary>
/// <param name="key"></param>
/// <param name="map"></param>
/// <returns>The func will return a value or Null </returns>
void* getHashMapValue(void* key, HashMap* map)
{
	// Get the hashcode for the key
	unsigned long hashCode = map->HashFunc(key, map->map_size);
	HashNode* current = map->arr[hashCode];
	// Go over the linked list until we find the same key
	while (current != NULL) {
		if (map->EqualFunc(key, current->key)) {
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
void insertNewValue(void* key, void* value, HashMap* map)
{
	// get the hashcode
	unsigned long hashCode = map->HashFunc(key, map->map_size);
	HashNode* current = map->arr[hashCode];
	// check if the key exists
	while (current) {
		if (map->EqualFunc(current->key, key)) {
			// overite the value if found key
			current->value = value;
			return; 
		}
		current = current->next;
	}
	// if we fill a new bucket update the used spaces
	if (!map->arr[hashCode])
		map->usedSpaces++;
	// no key found, need new node
	HashNode* node = getHashNode(key, value);
	node->next = map->arr[hashCode];
	map->arr[hashCode] = node;
	// resize map if needed
	if ((float)map->usedSpaces / map->map_size >= map->load_factor) {
		resizeMap(map);
	}
}

/// <summary>
/// This func creates a new hash node and sets its key and value
/// </summary>
/// <param name="key"></param>
/// <param name="value"></param>
/// <returns>Returns a HashNode* or Null if it wasnt able to allocate memory</returns>
HashNode* getHashNode(void* key, void* value) 
{
	HashNode* node = (HashNode*)malloc(sizeof(HashNode));
	if (!node) {
		printf("Memory allocation failed for HashNode");
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
/// <param name="HashFunc"></param>
/// <param name="EqualFunc"></param>
/// <returns>Returns a pointer to a new hashmap or NULL if it wasnt able to allocate memory</returns>
HashMap* initHashMap(int size, unsigned long (*HashFunc)(void*, int), int (*EqualFunc)(void*, void*),
	void (*PrintKey)(void*), void (*PrintValue)(void*), void (*FreeKey)(void*), void (*FreeValue)(void*))
{
	// create the map
	HashMap* map = (HashMap*)malloc(sizeof(HashMap));
	if (!map) {
		printf("Memory allocation failed for the map\n");
		return NULL;
	}
	// create the arr in the map
	map->arr = (HashNode**)calloc(size, sizeof(HashNode*));
	if (!map->arr) {
		printf("Memory allocation failed for the map array\n");
		free(map);
		return NULL;
	}
	// set the map size and load factor
	map->map_size = size;
	map->load_factor = 0.75f;
	map->usedSpaces = 0;
	map->HashFunc = HashFunc;
	map->EqualFunc = EqualFunc;
	map->FreeKey = FreeKey;
	map->FreeValue = FreeValue;
	map->PrintKey = PrintKey;
	map->PrintValue = PrintValue;
	return map;
}

/// <summary>
/// This func will free the hashmap and all of the pointers in it
/// </summary>
/// <param name="map"></param>
void freeHashMap(HashMap** map) {
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
			(*map)->FreeKey(temp->key);
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
void resizeMap(HashMap* map) {
	// save old data
	int oldSize = map->map_size;
	HashNode** oldArr = map->arr;
	// double the size
	map->map_size *= 2;
	map->arr = (HashNode**)calloc(map->map_size, sizeof(HashNode*));
	if (map->arr == NULL) {
		printf("Memory allocation failed during resize\n");
		return;
	}
	// refill the old data
	map->usedSpaces = 0;
	for (int i = 0; i < oldSize; i++) {
		HashNode* current = oldArr[i];
		while (current != NULL) {
			HashNode* nextNode = current->next;
			// reinsert the hash nodes
			insertNewValue(current->key, current->value, map);
			// free the current node
			free(current);
			current = nextNode;
		}
	}
	// free the old arr
	free(oldArr);
}

void printHashMap(HashMap* map) {
	if (map == NULL) {
		printf("HashMap is NULL.\n");
		return;
	}
	printf("HashMap Contents:\n");
	printf("Map Size: %d, Used Spaces: %d, Load Factor: %.2f\n", map->map_size, map->usedSpaces, map->load_factor);
	for (int i = 0; i < map->map_size; i++) {
		printf("Bucket %d: ", i);
		HashNode* current = map->arr[i];
		if (current == NULL) {
			printf("NULL");
		}
		else {
			while (current != NULL) {
				map->PrintKey(current->key);
				map->PrintValue(current->value);
				current = current->next;
			}
			printf("-> NULL");
		}
		printf("\n");
	}
}

