//u32 = unsigned 32-bit integer, and so on
typedef union
  {
  struct {
   u32 type : 10;
   u32 id : 22;
   }; //Okay in C99 with gcc -fms-extensions
  u32 guid;
  } GUID;

typedef struct
  {
  GUID guid;
  void *data;
  } GUIDTblEntry;