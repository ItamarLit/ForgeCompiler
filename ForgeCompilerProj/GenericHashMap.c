#include "GenericHashMap.h"


void* getHashMapValue(void* key, HashMap* map)
{
	unsigned long hashCode = map->HashFunc(key);
	HashNode* current = map->arr[hashCode];

	while (current != NULL) {
		if (map->EqualFunc(key, current->key)) {
			return current->value;  
		}
		current = current->next;
	}
	return NULL;  
}


void insertNewValue(void* key, void* value, HashMap* map)
{
	// get the hashcode
	unsigned long hashCode = map->HashFunc(key);
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
	// no key found, need new node
	if (!map->arr[hashCode])
		map->usedSpaces++;
	HashNode* node = getHashNode(key, value);
	node->next = map->arr[hashCode];
	map->arr[hashCode] = node;
	// resize map if needed
	if ((float)map->usedSpaces / map->map_size >= map->load_factor) {
		resizeMap(map);
	}
}


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



HashMap* initHashMap(int size) {
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
	return map;
}


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
			free(temp->key);
			free(temp->value);
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


void resizeMap(HashMap* map) {
	// save old data
	int oldSize = map->map_size;
	HashNode** oldArr = map->arr;
	map->map_size *= 2;
	map->arr = (HashNode**)calloc(map->map_size, sizeof(HashNode*));
	if (map->arr == NULL) {
		printf("Memory allocation failed during resize\n");
		return;
	}
	map->usedSpaces = 0;
	for (int i = 0; i < oldSize; i++) {
		HashNode* current = oldArr[i];
		while (current != NULL) {
			HashNode* nextNode = current->next;
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
				//printf("-> [Key: %d, Char: '%c', NextState: %d] ", current->current_state, current->current_char, current->next_state);
				current = current->next;
			}
			printf("-> NULL");
		}
		printf("\n");
	}
}

