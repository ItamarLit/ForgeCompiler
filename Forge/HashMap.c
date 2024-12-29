#include "HashMap.h"


HashNode* getHashNode(int current_state, char current_char, int next_state) {
	HashNode* node = (HashNode*)malloc(sizeof(HashNode));
	if (node == NULL) {
		fprintf(stderr, "Memory allocation failed for HashNode\n");
		return NULL;
	}
	node->current_state = current_state;
	node->current_char = current_char;
	node->next_state = next_state;
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


getHashCode(int currentState, char currentChar, HashMap* map) {
	int asciiValue = currentChar;
	// get 2 prime numbers to distribute the hash values more
	int prime1 = 31;
	int prime2 = 37;
	// return the hash for the state char pair
	return ((asciiValue * prime1 + currentState * prime2) % map->map_size);
}

int getNextState(int currentState, char currentChar, HashMap* map)
{
	int hashCode = getHashCode(currentState, currentChar, map);
	// get the head of the linked list at the index
	HashNode* indexHead = map->arr[hashCode];
	while (indexHead != NULL)
	{
		// check for the correct state-char pair
		if (indexHead->current_state == currentState && indexHead->current_char == currentChar) {
			return indexHead->next_state;
		}
		indexHead = indexHead->next;
	}
	// if we didn't find a value that matches in the linked list
	return -1;
}


void putState(int currentState, char currentChar, int nextState, HashMap* map)
{
	// check if we need to resize the map
	if ((float)map->usedSpaces / map->map_size >= map->load_factor) {
		resizeMap(map);
	}
	int hashCode = getHashCode(currentState, currentChar, map);
	// get the head of the linked list
	HashNode* indexHead = map->arr[hashCode];
	// set up the node
	HashNode* temp = getHashNode(currentState, currentChar, nextState);
	if (temp == NULL) {
		printf("Failed to allocate memory for new HashNode\n");
		return;
	}
	// insert at head of list
	temp->next = indexHead;
	map->arr[hashCode] = temp;
	// increment spaces used if it is a new list
	if (indexHead == NULL) {
		map->usedSpaces++;
	}
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
			putState(current->current_state, current->current_char, current->next_state, map);
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
				printf("-> [State: %d, Char: '%c', NextState: %d] ", current->current_state, current->current_char, current->next_state);
				current = current->next;
			}
			printf("-> NULL");
		}
		printf("\n");
	}
}

