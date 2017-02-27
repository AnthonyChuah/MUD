/* Test for CircularBuffer:
(1) Test with ints
    (a) Try to push beyond limit
    (b) Push a sequence and pop the sequence, ensure FIFO
    (c) Ensure getNumel() correctly reports
(2) Test with std::string, (a) (b) and (c)
*/

#include <iostream>
#include <string>
#include "CircularBuffer.hpp"

int main() {
  using namespace std;
  cout << "Testing CircularBuffer template class\n";

  CircularBuffer<int, 4> cb1;
  bool successfulPush = false;
  successfulPush = cb1.push(1);
  if (!successfulPush) cout << "Unsuccessful push of 1.\n";
  successfulPush = cb1.push(2);
  if (!successfulPush) cout << "Unsuccessful push of 2.\n";
  successfulPush = cb1.push(3);
  if (!successfulPush) cout << "Unsuccessful push of 3.\n";
  successfulPush = cb1.push(4);
  if (!successfulPush) cout << "Unsuccessful push of 4.\n";
  int numel = cb1.getNumel();
  cout << "Number of elements should be 3: " << numel << "\n";
  cout << "Now pop them in FIFO order, should get 1 2 3: ";
  while (cb1.getNumel() != 0) {
    cout << cb1.pop() << " with numel " << cb1.getNumel() << "\n";
  }
  cout << "Numel should now be 0: " << cb1.getNumel() << "\n";
  cout << "Now test that pushing will wrap the tail pointer around to start of buffer\n";
  successfulPush = cb1.push(4);
  if (!successfulPush) cout << "Unsuccessful push of 4.\n";
  successfulPush = cb1.push(5);
  if (!successfulPush) cout << "Unsuccessful push of 5.\n";
  successfulPush = cb1.push(6);
  if (!successfulPush) cout << "Unsuccessful push of 6.\n";
  successfulPush = cb1.push(7);
  if (!successfulPush) cout << "Unsuccessful push of 7.\n";
  numel = cb1.getNumel();
  cout << "Number of elements back to 3: " << cb1.getNumel() << "\n";
  cout << "Now pop them again in FIFO order, should get 4 5 6: ";
  while (cb1.getNumel() != 0) {
    cout << cb1.pop() << " with numel " << cb1.getNumel() << "\n";
  }

  cout << "\nNow testing with STRINGS!\n\n";
  CircularBuffer<std::string, 4> cb2;
  successfulPush = false;
  successfulPush = cb2.push("one");
  if (!successfulPush) cout << "Unsuccessful push of one.\n";
  successfulPush = cb2.push("two");
  if (!successfulPush) cout << "Unsuccessful push of two.\n";
  successfulPush = cb2.push("three");
  if (!successfulPush) cout << "Unsuccessful push of three.\n";
  successfulPush = cb2.push("four");
  if (!successfulPush) cout << "Unsuccessful push of four.\n";
  numel = cb2.getNumel();
  cout << "Number of elements should be 3: " << numel << "\n";
  cout << "Now pop them in FIFO order, should get one two three: ";
  while (cb2.getNumel() != 0) {
    cout << cb2.pop() << " with numel " << cb2.getNumel() << "\n";
  }
  cout << "Numel should now be 0: " << cb2.getNumel() << "\n";
  cout << "Now test that pushing will wrap the tail pointer around to start of buffer\n";
  successfulPush = cb2.push("four");
  if (!successfulPush) cout << "Unsuccessful push of four.\n";
  successfulPush = cb2.push("five");
  if (!successfulPush) cout << "Unsuccessful push of five.\n";
  successfulPush = cb2.push("six");
  if (!successfulPush) cout << "Unsuccessful push of six.\n";
  successfulPush = cb2.push("seven");
  if (!successfulPush) cout << "Unsuccessful push of seven.\n";
  numel = cb2.getNumel();
  cout << "Number of elements back to 3: " << cb2.getNumel() << "\n";
  cout << "Now pop them again in FIFO order, should get four five six: ";
  while (cb2.getNumel() != 0) {
    cout << cb2.pop() << " with numel " << cb2.getNumel() << "\n";
  }
  return 0;
}
