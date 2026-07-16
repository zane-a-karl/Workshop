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
 *  - [ ] make a generic POST request with curl 
 *  - [ ] make an api call to openweathermap's api with curl
 *  - [ ] parse the curl api call with cjson
 */
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <stdlib.h>
/* #include <stdio.h> */

void check_curl_error(CURLcode c) {
    if (0 != c) {
        printf("CURL exited with error code: %d", c);
    }
}

void make_api_call() {
    CURLcode c;
    CURL *handle = curl_easy_init();
    c = curl_easy_setopt(handle, CURLOPT_WRITEDATA, stdout);
    check_curl_error(c);
    printf("1st setopt CURLcode c was: %d\n", c);
    
    char *url = malloc(256 * sizeof(*url));
    c = sprintf(url, "https://httpbin.org/get");
    printf("sprintf CURLcode c was: %d\n", c);
    
    c = curl_easy_setopt(handle, CURLOPT_URL, url);
    check_curl_error(c);
    printf("2nd setopt CURLcode c was: %d\n", c);
    
    c = curl_easy_perform(handle);
    check_curl_error(c);
    printf("perform CURLcode c was: %d\n", c);
}

int main() {
    make_api_call();
    return 0;
}
