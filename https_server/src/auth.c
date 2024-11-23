#include <string.h>
#include "auth.h"

int authenticate(const char *header) {
    const char *correct_auth = "Basic aXNoYW5rOmlzaGFuaw=="; // "ishank:ishank" in Base64
    return strstr(header, correct_auth) != NULL;
}
