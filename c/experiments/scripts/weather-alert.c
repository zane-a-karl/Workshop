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
 *  - [ ] make an api call to openweathermap's api with curl
 *  - [ ] parse the curl api call with cjson
 */
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <stdlib.h>
#include <stdbool.h>
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

void make_httpbin_api_call(char *url, enum HTTP_VERB v) {
    CURLcode c;
    CURL *handle = curl_easy_init();
    c = curl_easy_setopt(handle, CURLOPT_WRITEDATA, stdout);
    check_curl_error(c);
    printf("1st setopt CURLcode c was: %d\n", c);

    c = curl_easy_setopt(handle, CURLOPT_URL, url);
    check_curl_error(c);
    printf("2nd setopt CURLcode c was: %d\n", c);

    if (v == POST) {
        c = curl_easy_setopt(handle, CURLOPT_POST, true);
        check_curl_error(c);
        printf("3rd setopt CURLcode c was: %d\n", c);

        struct test_obj obj = { "Jack Aku", 42 };
        char body[128];
        size_t bytes =
            sprintf(body,"name=%s&age=%d", obj.name, obj.age);
        c = curl_easy_setopt(handle,CURLOPT_POSTFIELDS, body);
        check_curl_error(c);
        printf("4th setopt CURLcode c was: %d\n", c);

        c = curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,bytes);
        check_curl_error(c);
        printf("5th setopt CURLcode c was: %d\n", c);
    }

    c = curl_easy_perform(handle);
    check_curl_error(c);
    printf("perform CURLcode c was: %d\n", c);
}

#define HTTPBIN_GET "https://httpbin.org/get"
#define HTTPBIN_POST "https://httpbin.org/post"
#define HTTPBIN_STATUS "https://httpbin.org/status/200"
#define HTTPBIN_DELAY "https://httpbin.org/delay/3"
#define HTTPBIN_JSON "https://httpbin.org/json"

int main() {
    char *url = malloc(256 * sizeof(*url));
    int bytes = sprintf(url, HTTPBIN_POST);
    printf("sprintf bytes written was: %d\n", bytes);
    make_httpbin_api_call(url, POST);
    return 0;
}
