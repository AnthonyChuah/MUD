/* CircularBuffer may contain Size - 1 elements of any object,
given that the object has a copy constructor
*/

template <typename T, int Size>
class CircularBuffer {
  T* head;
  T* tail;
  T* endOfBuffer;
  T buffer[Size];
  int numel;
public:
  CircularBuffer() {
    head = buffer;
    tail = buffer;
    endOfBuffer = buffer + Size;
    numel = 0;
  }
  T pop() {
    if (head == tail) {
      // Warning: CircularBuffer should NOT be popped when empty!
      return *head;
    }
    T returned = *(head++);
    if (head == endOfBuffer) head = buffer;
    --numel;
    return returned;
  }
  bool push(T _toPush) {
    bool isFullBufferWrapAround = (head == buffer) && (tail == endOfBuffer - 1);
    bool isFullBuffer = head == tail + 1 || isFullBufferWrapAround;
    if (isFullBuffer) return false;
    *tail++ = _toPush;
    if (tail == endOfBuffer) tail = buffer;
    ++numel;
    return true;
  }
  int getNumel() const { return numel; }
};
