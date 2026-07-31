/**
 * GOAL: create a script that makes an api request to 3
 *       different weather APIs, averages their predictions
 *       then alerts me if there is rain in the forecast for
 *       my home in the next few days.
 * TODO:
 *  - [X] compile a c program with libcurl included
 *  - [X] compile a c program with cjson included
 *  - [X] write a barebones main fn entry point
 *  - [X] try to find curl fns to set up structs
 *  - [X] make a generic GET request with curl
 *  - [X] make a generic POST request with curl
 *  - [X] save POST response to a local variable
 *  - [X] parse the curl api call response with cjson
 *  - [ ] send a curl api call with a json payload
 *  - [ ] make an api call to openweathermap's api with curl
 *  - [ ] parse the openweathermap curl api call with cjson
 */
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
/* #include <stdio.h> */

void check_curl_error(CURLcode c) {
    if (0 != c) {
        printf("CURL exited with error code: %d", c);
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

void make_httpbin_api_call(char *url, enum HTTP_VERB v) {
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = {rb_data, 0, INIT_RB_SIZE};
    struct test_obj obj = { "Jack Aku", 42 };
    char body[128]; // Must live until perform() call
    CURL *handle = curl_easy_init();
    c = curl_easy_setopt(handle,
                         CURLOPT_WRITEDATA,
                         &rb);
    check_curl_error(c);
    printf("1st setopt CURLcode c was: %d\n", c);

    c = curl_easy_setopt(handle,
                         CURLOPT_URL,
                         url);
    check_curl_error(c);
    printf("2nd setopt CURLcode c was: %d\n", c);

    if (v == POST) {
        c = curl_easy_setopt(handle,
                             CURLOPT_POST,
                             true);
        check_curl_error(c);
        printf("3rd setopt CURLcode c was: %d\n", c);

        size_t bytes =
            snprintf(body,
                     sizeof(body)/sizeof(*body),
                     "name=%s&age=%d",
                     obj.name,
                     obj.age);
        c = curl_easy_setopt(handle,
                             CURLOPT_POSTFIELDS,
                             body);
        check_curl_error(c);
        printf("4th setopt CURLcode c was: %d\n", c);

        c = curl_easy_setopt(handle,
                             CURLOPT_POSTFIELDSIZE,
                             bytes);
        check_curl_error(c);
        printf("5th setopt CURLcode c was: %d\n", c);
    }

    c = curl_easy_setopt(handle,
                         CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c);
    printf("6th setopt CURLcode c was: %d\n", c);

    c = curl_easy_perform(handle);
    check_curl_error(c);
    printf("perform CURLcode c was: %d\n", c);

    long status_code;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status_code);
    printf("HTTP status: %ld\n", status_code);

    // Parse the response
    cJSON *json_handle = cJSON_Parse(rb.data);

    // Print the response
    /* printf("%s", rb.data); */
    /* char *json_str = cJSON_Print(json_handle); */
    /* printf("%s\n", json_str); */
    cJSON_bool has_form_data =
        cJSON_HasObjectItem(json_handle, "form");
    if (has_form_data) {
        printf("Form data found\n");
        cJSON *form_json_handle =
            cJSON_GetObjectItem(json_handle, "form");
        char *form_json_str = cJSON_Print(form_json_handle);
        printf("%s\n", form_json_str);
        free(form_json_str);
    }


    // Free memory
    /* free(json_str); */
    cJSON_Delete(json_handle);
    free(rb.data); // free the current not the original
    curl_easy_cleanup(handle);
}

#define HTTPBIN_GET "https://httpbin.org/get"
#define HTTPBIN_POST "https://httpbin.org/post"
#define HTTPBIN_STATUS "https://httpbin.org/status/200"
#define HTTPBIN_DELAY "https://httpbin.org/delay/3"
#define HTTPBIN_JSON "https://httpbin.org/json"

int main() {
    char url[256];
    int bytes = snprintf(url,
                         sizeof(url)/sizeof(*url),
                         HTTPBIN_POST);
    printf("sprintf bytes written was: %d\n", bytes);
    make_httpbin_api_call(url, POST);
    return 0;
}
