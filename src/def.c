// def.c - Various typedefs and definitions

#include "def.h"

void* ListGet(list* l, int index) {
	listEntry* cur = l->first;
	int i = 0;
	
	if (index < 0) return NULL;
	
	while (cur) {
		if (i == index) return cur->value;
		
		i++;
		cur = cur->next;
	}
	
	return NULL;
}

int ListSize(list* l) {
	listEntry* cur = l->first;
	int i = 0;
	
	while (cur) {
		cur = cur->next;
		i++;
	}
	
	l->size = i;
	
	return i;
}

void ListAdd(list* l, void* value) {
	listEntry* cur = l->first;
	if (cur) {
		while (cur->next) 
			cur = cur->next;
		cur->next = malloc(sizeof(listEntry));
		cur->next->value = value;
		cur->next->prev = cur;
		cur->next->next = NULL;
	}else {
		l->first = malloc(sizeof(listEntry));
		l->first->value = value;
		l->first->prev = l->first->next = NULL;
	}
	
	l->size++;
}

void ListRemove(list* l, int index) {
	listEntry* cur = l->first;
	int i = 0;
	
	if (index < 0) return;
	
	while (cur && i < index) {
		cur = cur->next;
		i++;
	}
	
	if (!cur) return;
	
	if (i == 0)		l->first = cur->next;
	if (cur->prev)	cur->prev->next = cur->next;
	if (cur->next)	cur->next->prev = cur->prev;
	free(cur);
	
	l->size--;
}
