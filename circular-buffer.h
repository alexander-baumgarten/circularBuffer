struct Buffer {
    int readIndex;      // Reading Index
    int writeIndex;     // Writing Index
    int countIndex;     // Counting Index
    int size;           // Desired Buffer size
    int count;          // Count of items in buffer
    struct CharPos* elements; 
};

struct CharPos {
    char character;
    int counted;        // 0 if not counted, 1 if counted
};

struct Buffer* initBuffer(int sizeP);
void freeBuf(struct Buffer *buf);
int canAdd(struct Buffer *buf);
int add(struct Buffer *buf, char c);
char count(struct Buffer *buf);
int canRead(struct Buffer *buf);
char readBuf(struct Buffer *buf);
void printHelp(struct Buffer *buf, char c, char type);

