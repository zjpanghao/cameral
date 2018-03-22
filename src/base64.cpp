#include "base64.h"
#include <openssl/pem.h>                                                        
#include <openssl/bio.h>                                                        
#include <openssl/evp.h> 

void base64Encry(char *in, int len, std::string *out) {
  BIO *bmem = BIO_new(BIO_s_mem());  
  BIO *b64 = BIO_new(BIO_f_base64());
  b64 = BIO_push(b64, bmem); 
  BIO_write(b64, in, len);
  BIO_flush(b64);
  BUF_MEM *bptr = NULL;
  BIO_get_mem_ptr(b64, &bptr); 
  out->assign(bptr->data, bptr->length);
  BIO_free_all(b64);
}
