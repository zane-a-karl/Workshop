#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <time.h>

int main() {
    int str[128];
    printf("str[len-1] = %d\n", str[sizeof(str) - 1]);
    printf("The size of str = %lu\n", sizeof(str));
    printf("The size of str divided = %lu\n",
           sizeof(str)/sizeof(*str));

    double x = 0.0;
    int y = 0;
    if ( x == y) {
        printf("TRUE: 0.0 == 0\n");
    }

    char *raw_json = "{\"a_bool\": 0}";
    cJSON *json_hdl = cJSON_Parse(raw_json);
    char *json_resp_str = cJSON_Print(json_hdl);
    printf("response: %s\n", json_resp_str);
    cJSON_bool has_a_bool =
        cJSON_HasObjectItem(json_hdl, "a_bool");
    if (!has_a_bool) { printf("Can't find a_bool\n"); }
    cJSON *a_bool_hdl = cJSON_GetObjectItem(json_hdl, "a_bool");
    cJSON_bool a_bool_is_bool = cJSON_IsBool(a_bool_hdl);
    if (!a_bool_is_bool) { printf("a_bool isn't a bool\n"); }
    bool a_bool = cJSON_IsFalse(a_bool_hdl);
    free(json_resp_str);
    cJSON_Delete(json_hdl);


    time_t now = time(NULL);
    struct tm *dt = localtime(&now);
    printf("YYYY-MM-DD: %d-%d-%d\n", dt->tm_year  + 1900,
           dt->tm_mon  + 1, dt->tm_mday);
    printf("Timezone: %s\n", dt->tm_zone);

    char *string = "this is a string"
        " this is the same string";
    printf("%s\n", string);

    printf("(-1 mod 5) = %i\n", (-1 % 5));

    return 0;
}
