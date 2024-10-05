#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "build/map_with_http.pb-c.h"

void pack(void **buf, int *len) {
  Http http = HTTP__INIT;
  http.uri = "/newsfeed";
  http.body = "<no data in body>";
  http.n_headers = 2;
  http.headers = malloc(sizeof(Http__HeadersEntry*) * http.n_headers);
  for(int i=0; i < http.n_headers; i++) {
    http.headers[i] = malloc(sizeof(Http__HeadersEntry));
    http__headers_entry__init(http.headers[i]);
  }
  
  http.headers[0]->key =  strdup("Host");
  http.headers[0]->value =  strdup("example.com");

  http.headers[1]->key =  strdup("Content-Type");
  http.headers[1]->value =  strdup("application/json");

  *len = http__get_packed_size(&http);
  *buf = malloc(*len);
  http__pack(&http, *buf);
  for(int i=0; i < http.n_headers; i++) {
    free(http.headers[i]);
  }
  free(http.headers);
  printf("Packed protobuf message with len: %d.\n", *len);
}

void unpack(void *buf, int len) {

  Http *received;
  received = http__unpack(NULL, len, buf); // Need 'len' here!
  if (received == NULL)
    exit(1); // Error unpacking incoming message.
  printf("\nUnpacked: uri: %s, body: %s\n", received->uri, received->body);
  
  for(int i=0; i < received->n_headers; i++) {
    printf("Header %d. %s: %s\n",i, received->headers[i]->key, received->headers[i]->value);
  }

  http__free_unpacked(received, NULL);
}

int main(int argc, const char *argv[])
{
  void *buf;    // Buffer to store serialized data.
  unsigned len; // Length of serialized data.
  pack(&buf, &len);
  unpack(buf, len);
  free(buf);
  return 0;
}