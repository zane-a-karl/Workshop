#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void check_curl_error(CURLcode c, char *subject) {
    if (0 != c) {
        printf("Curl <%s> exited with error code: %d\n", subject, c);
    }
}

enum HTTP_VERB { GET, POST };

struct test_obj {
    char name[32];
    int age;
};

struct response_buffer {
    char *data;
    size_t size_assigned;
    size_t size_reserved;
};

size_t curl_fwrite_callback(const void *restrict ptr,
                            size_t size,
                            size_t nitems,
                            void *restrict userdata) {

    struct response_buffer *rb =
        (struct response_buffer *)userdata;
    size_t ptr_size = size * nitems;
    size_t new_size = rb->size_assigned + ptr_size;

    // Add 1 to obviate check during null termination
    while (new_size + 1 >= rb->size_reserved) {
        rb->data = realloc(rb->data, rb->size_reserved * 2);
        // Anything not eq to ptr_size is an error
        if (NULL == rb->data) { return 0; }
        rb->size_reserved *= 2;
    }

    memcpy(rb->data + rb->size_assigned,
           (char *)ptr,
           ptr_size);
    rb->size_assigned = new_size;
    // Resp buf must be null terminated
    rb->data[rb->size_assigned] = '\0';

    return ptr_size;
}

#define INIT_RB_SIZE 256
#define MAX_URL_LEN 256

void make_httpbin_api_call(char *url, enum HTTP_VERB v) {
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = {rb_data, 0, INIT_RB_SIZE};
    struct test_obj obj = { "Jack Aku", 42 };
    /* char body[128]; // Must live until perform() call */
    struct curl_slist *headers;
    CURL *handle = curl_easy_init();
    c = curl_easy_setopt(handle,
                         CURLOPT_WRITEDATA,
                         &rb);
    check_curl_error(c, "CURLOPT_WRITEDATA");

    c = curl_easy_setopt(handle,
                         CURLOPT_URL,
                         url);
    check_curl_error(c, "CURLOPT_URL");

    if (v == POST) {
        c = curl_easy_setopt(handle,
                             CURLOPT_POST,
                             true);
        check_curl_error(c, "CURLOPT_POST");

        /* "name=%s&age=%d" */
        /* "{\"name\": \"%s\", \"age\": %d}" */
        cJSON *obj_hdl = cJSON_CreateObject();
        cJSON *name_hdl =
            cJSON_AddStringToObject(obj_hdl, "name",
                                    obj.name);
        if (name_hdl == NULL) {
            printf("Failed to add name\n");
            return;
        }
        cJSON *age_hdl =
            cJSON_AddNumberToObject(obj_hdl, "age",
                                    (double)(obj.age));
        if (age_hdl == NULL) {
            printf("Failed to add age\n");
            return;
        }
        char *obj_json_str = cJSON_Print(obj_hdl);
        printf("obj_json_str: %s\n", obj_json_str);
        /* size_t bytes = */
        /*     snprintf(body, */
        /*              sizeof(body)/sizeof(*body), */
        /*              "name=%s&age=%d", */
        /*              obj.name, */
        /*              obj.age); */
        c = curl_easy_setopt(handle,
                             CURLOPT_POSTFIELDS,
                             obj_json_str);
        check_curl_error(c, "CURLOPT_POSTFIELDS");

        size_t bytes = strnlen(obj_json_str, 256);
        c = curl_easy_setopt(handle,
                             CURLOPT_POSTFIELDSIZE,
                             bytes);
        check_curl_error(c, "CURLOPT_POSTFIELDSIZE");

        headers = curl_slist_append(NULL, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        c = curl_easy_setopt(handle,
                             CURLOPT_HTTPHEADER,
                             headers);
        check_curl_error(c, "CURLOPT_HTTPHEADER");
    }

    c = curl_easy_setopt(handle,
                         CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c, "CURLOPT_WRITEFUNCTION");

    c = curl_easy_perform(handle);
    check_curl_error(c, "PERFORM");

    long status_code;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status_code);
    /* printf("HTTP status: %ld\n", status_code); */

    if (v == POST) {
        curl_slist_free_all(headers);
    }

    // Parse the response
    cJSON *json_handle = cJSON_Parse(rb.data);

    // Print the response
    /* printf("%s", rb.data); */
    char *json_str = cJSON_Print(json_handle);
    printf("%s\n", json_str);
    cJSON_bool has_json_field =
        cJSON_HasObjectItem(json_handle, "json");
    if (has_json_field) {
        printf("Json field found\n");
        cJSON *json_field_handle =
            cJSON_GetObjectItem(json_handle, "json");
        char *json_field_str = cJSON_Print(json_field_handle);
        printf("%s\n", json_field_str);
        free(json_field_str);
    }


    // Free memory
    free(json_str);
    cJSON_Delete(json_handle);
    free(rb.data); // free the current not the original
    curl_easy_cleanup(handle);
}

#define HTTPBIN_GET "https://httpbin.org/get"
#define HTTPBIN_POST "https://httpbin.org/post"
#define HTTPBIN_STATUS "https://httpbin.org/status/200"
#define HTTPBIN_DELAY "https://httpbin.org/delay/3"
#define HTTPBIN_JSON "https://httpbin.org/json"

/** NOTE: you must source and export your
 * .envrc to access api keys for this program
 * set -a
 * source ./.envrc
 * set +a
 */
int main() {

    char url[MAX_URL_LEN];
    int bytes = snprintf(url,
                         sizeof(url)/sizeof(*url),
                         HTTPBIN_POST);
    printf("sprintf bytes written was: %d\n", bytes);
    make_httpbin_api_call(url, POST);

    return 0;
}
