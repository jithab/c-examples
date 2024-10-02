#include <stdio.h>
#include <stdlib.h>
#include "build/user.pb-c.h"

int main(int argc, const char *argv[])
{
  void *buf;    // Buffer to store serialized data.
  unsigned len; // Length of serialized data.

  // Prepare the data.
  int32_t id = 123;
  char *name = "MyNameIsProtobuf!";

  // Serialize the data.
  User send = USER__INIT;
  send.id = id;
  send.name = name;
  len = user__get_packed_size(&send);
  buf = malloc(len);
  user__pack(&send, buf);
  printf("Packed protobuf message.\n");

  User *received;
  received = user__unpack(NULL, len, buf); // Need 'len' here!
  if (received == NULL)
    exit(1); // Error unpacking incoming message.
  printf("Unpacked: id: %d, name: %s\n", received->id, received->name);

  user__free_unpacked(received, NULL);
  free(buf);
  return 0;
}