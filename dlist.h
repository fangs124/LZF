typedef struct DLISTS Dlist_t; // doubly linked list
struct DLISTS {
	void* data;
	Dlist_t next;
	Dlist_t prev;
};

