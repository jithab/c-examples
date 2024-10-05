#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "build/google/protobuf/any.pb-c.h"
#include "build/any_with_http.pb-c.h"

void pack_user(void **buf, int *len) {
  
  User user = USER__INIT;
  user.id = 123;
  user.name = "Indila";
  size_t user_size = user__get_packed_size(&user);
  uint8_t *user_buffer = malloc(user_size);
  if (!user_buffer) exit(__LINE__);
  user__pack(&user, user_buffer);
  
  Google__Protobuf__Any anymsg = GOOGLE__PROTOBUF__ANY__INIT;

  anymsg.type_url = strdup("RCS/User");
  anymsg.value.len = user_size;
  anymsg.value.data = user_buffer;

  Http http = HTTP__INIT;
  http.body = &anymsg;

  *len = http__get_packed_size(&http);
  *buf = malloc(*len);
  http__pack(&http, *buf);

  printf("Packed protobuf message: %lu / %d.\n", user_size, *len);
}

void pack_login(void **buf, int *len) {
  
  Login login = LOGIN__INIT;
  login.username = strdup("lamiskan");
  login.password = "mesaytara";
  size_t login_size = login__get_packed_size(&login);
  uint8_t *login_buffer = malloc(login_size);
  if (!login_buffer) exit(__LINE__);
  login__pack(&login, login_buffer);
  
  Google__Protobuf__Any anymsg = GOOGLE__PROTOBUF__ANY__INIT;

  anymsg.type_url = strdup("RCS/Login");
  anymsg.value.len = login_size;
  anymsg.value.data = login_buffer;

  Http http = HTTP__INIT;
  http.body = &anymsg;

  *len = http__get_packed_size(&http);
  *buf = malloc(*len);
  http__pack(&http, *buf);

  printf("Packed protobuf message: %lu / %d.\n", login_size, *len);
}

void unpack(void *buf, int len) {

  Http *received;
  received = http__unpack(NULL, len, buf);
  if (received == NULL) exit(__LINE__);

  if (strcmp(received->body->type_url, "RCS/User") == 0) {
      User *user = user__unpack(NULL, received->body->value.len,
                                                received->body->value.data);
      if (user == NULL) exit(__LINE__);
      printf("Unpacked User: id = %d, name = %s\n", user->id, user->name);
      user__free_unpacked(user, NULL);
  } else if (strcmp(received->body->type_url, "RCS/Login") == 0) {
      Login *login = login__unpack(NULL, received->body->value.len,
                                                received->body->value.data);
      if (login == NULL) exit(__LINE__);
      printf("Unpacked Login: username = %s, password = %s\n", login->username, login->password);
      login__free_unpacked(login, NULL);
  } else {
      printf("Unknown message type: %s\n", received->body->type_url);
  }

  http__free_unpacked(received, NULL);
}

int main(int argc, const char *argv[])
{
  void *buf;    // Buffer to store serialized data.
  unsigned len; // Length of serialized data.
  
  pack_user(&buf, &len);
  unpack(buf, len);
  printf("- - - -\n");
  pack_login(&buf, &len);
  unpack(buf, len);

  free(buf);
  return 0;
}