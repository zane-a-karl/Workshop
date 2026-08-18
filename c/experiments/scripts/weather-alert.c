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
 *  - [X] send a curl api call with a json payload
 *  - [X] setup and read test api key from env variable
 *  - [X] make an api call to openweathermap's api with curl
 *  - [X] parse the openweathermap curl api call with cjson
 *  - [X] make an api call to weatherapi's api with curl
 *  - [X] parse the wapi curl api call with cjson
 *  - [X] make an api call to weatherbit's api with curl
 *  - [X] parse the wbit curl api call with cjson
 */
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
/* #include <stdio.h> */

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

struct alert_input {
    double lat;
    double lon;
};

struct alert_output {
    bool owm_will_rain;
    bool wapi_will_rain;
    bool wbit_will_rain;
};

bool make_owm_api_call(struct alert_input in) {

    printf("----------Begin OWM query----------\n");
    // Initialize handles and buffers
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = { rb_data, 0, INIT_RB_SIZE };
    CURL *curl_hdl = curl_easy_init();

    // Point a response buffer to tell curl where to write
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEDATA, &rb);
    check_curl_error(c, "CURLOPT_WRITEDATA");

    // Construct the url
    /* Go to https://home.openweathermap.org/myservices */
    /* > view > scroll down to Free Tier */
    char url[MAX_URL_LEN];
    char *base = "https://api.openweathermap.org/data/2.5/forecast";
    char *api_key = getenv("OWM_API_KEY");
    // Will auto-null-terminate
    /* int bytes = */
    snprintf(url, sizeof(url)/sizeof(*url),
             "%s?lat=%f&lon=%f&appid=%s",
             base, in.lat, in.lon, api_key);
    /* printf("<%d> bytes written to url: <%s>\n", bytes, url); */
    c = curl_easy_setopt(curl_hdl, CURLOPT_URL, url);
    check_curl_error(c, "CURLOPT_URL");

    // Give Curl custom callback to write to local resp buf
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c, "CURLOPT_WRITEFUNCTION");

    // Execute the call
    c = curl_easy_perform(curl_hdl);
    check_curl_error(c, "PERFORM");

    // Get response status code
    long status_code;
    curl_easy_getinfo(curl_hdl, CURLINFO_RESPONSE_CODE,
                      &status_code);
    /* printf("HTTP status: %ld\n", status_code); */

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(rb.data);
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    cJSON_bool has_list = cJSON_HasObjectItem(json_hdl, "list");
    if (!has_list) { return false; }
    cJSON *list_hdl = cJSON_GetObjectItem(json_hdl, "list");
    int list_size = cJSON_GetArraySize(list_hdl);
    time_t now = time(NULL);
    time_t day_in_sec = 60 * 60 * 24;
    for (int i = 0; i < list_size; i++) {
        cJSON *list_item_hdl = cJSON_GetArrayItem(list_hdl, i);
        cJSON_bool has_dt =
            cJSON_HasObjectItem(list_item_hdl, "dt");
        if (!has_dt) { return false; }
        cJSON *dt_hdl = cJSON_GetObjectItem(list_item_hdl, "dt");
        cJSON_bool dt_is_number = cJSON_IsNumber(dt_hdl);
        if (!dt_is_number) { return false; }
        double dt = cJSON_GetNumberValue(dt_hdl);

        // Skip if forecast is beyond 24 hours
        if ((time_t)(dt) > now + day_in_sec) { continue; }

        // Else check rain conditions

        //// First Main Weather Designation
        cJSON_bool has_weather =
            cJSON_HasObjectItem(list_item_hdl, "weather");
        if (!has_weather) { return false; }
        cJSON *weather_hdl =
            cJSON_GetObjectItem(list_item_hdl, "weather");
        int weather_size = cJSON_GetArraySize(weather_hdl);
        int main_rain = -1;
        for (int j = 0; i < weather_size; i++) {
            cJSON *weather_item_hdl =
                cJSON_GetArrayItem(weather_hdl, j);
            cJSON_bool has_main =
                cJSON_HasObjectItem(weather_item_hdl, "main");
            if (!has_main) { return false; }
            cJSON *main_hdl =
                cJSON_GetObjectItem(weather_item_hdl, "main");
            char *main_str = cJSON_Print(main_hdl);
            main_rain = strncmp(main_str, "Rain", 4 * sizeof(char));
            free(main_str);
            if (0 == main_rain) { break; }
        }

        //// Second Probability of Precipitation (pop)
        cJSON_bool has_pop =
            cJSON_HasObjectItem(list_item_hdl, "pop");
        if (!has_pop) { return false; }
        cJSON *pop_hdl = cJSON_GetObjectItem(list_item_hdl, "pop");
        cJSON_bool pop_is_number = cJSON_IsNumber(pop_hdl);
        if (!pop_is_number) { return false; }
        double pop = cJSON_GetNumberValue(pop_hdl);

        //// Third has a 'rain' item
        cJSON_bool has_rain =
            cJSON_HasObjectItem(list_item_hdl, "rain");
        if (!has_rain) { return false; }

        // Combine to check for rain
        if (0 == main_rain && pop > 0.55 && has_rain) {
            return true;
        }
    } // for loop

    // Free memory
    free(json_resp_str);
    cJSON_Delete(json_hdl);
    free(rb.data);
    curl_easy_cleanup(curl_hdl);

    printf("----------End OWM query----------\n");
    return false;
}

bool make_wapi_api_call(struct alert_input in) {

    printf("----------Begin WAPI query----------\n");
    // Initialize handles and buffers
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = { rb_data, 0, INIT_RB_SIZE };
    CURL *curl_hdl = curl_easy_init();

    // Point a response buffer to tell curl where to write
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEDATA, &rb);
    check_curl_error(c, "CURLOPT_WRITEDATA");

    // Construct the url
    char url[MAX_URL_LEN];
    char *base = "http://api.weatherapi.com/v1";
    char *method = "forecast.json";
    char *city = "Irvine";
    int days = 2;
    char *api_key = getenv("WAPI_API_KEY");
    // Will auto-null-terminate
    /* int bytes = */
    snprintf(url, sizeof(url)/sizeof(*url),
             "%s/%s?key=%s&q=%s&days=%d",
             base, method, api_key, city, days);
    /* printf("<%d> bytes written to url: <%s>\n", bytes, url); */
    c = curl_easy_setopt(curl_hdl, CURLOPT_URL, url);
    check_curl_error(c, "CURLOPT_URL");

    // Give Curl custom callback to write to local resp buf
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c, "CURLOPT_WRITEFUNCTION");

    // Execute the call
    c = curl_easy_perform(curl_hdl);
    check_curl_error(c, "PERFORM");

    // Get response status code
    long status_code;
    curl_easy_getinfo(curl_hdl, CURLINFO_RESPONSE_CODE,
                      &status_code);
    /* printf("HTTP status: %ld\n", status_code); */

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(rb.data);
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    cJSON_bool has_forecast =
        cJSON_HasObjectItem(json_hdl, "forecast");
    if (!has_forecast) { return false; }
    cJSON *forecast_hdl = cJSON_GetObjectItem(json_hdl, "forecast");
    cJSON_bool has_forecastday =
        cJSON_HasObjectItem(forecast_hdl, "forecastday");
    if (!has_forecastday) { return false; }
    cJSON *forecastday_hdl =
        cJSON_GetObjectItem(forecast_hdl, "forecastday");
    int forecastday_size = cJSON_GetArraySize(forecastday_hdl);
    time_t now = time(NULL);
    time_t day_in_sec = 60 * 60 * 24;
    for (int i = 0; i < forecastday_size; i++) {
        cJSON *forecastday_item_hdl =
            cJSON_GetArrayItem(forecastday_hdl, i);
        cJSON_bool has_hour =
            cJSON_HasObjectItem(forecastday_item_hdl, "hour");
        if (!has_hour) { return false; }
        cJSON *hour_hdl =
            cJSON_GetObjectItem(forecastday_item_hdl, "hour");
        int hour_size = cJSON_GetArraySize(hour_hdl);
        for (int j = 0; j < hour_size; j++) {
            cJSON *hour_item_hdl = cJSON_GetArrayItem(hour_hdl, j);
            cJSON_bool has_time_epoch =
                cJSON_HasObjectItem(hour_hdl, "time_epoch");
            if (!has_time_epoch) { return false; }
            cJSON *time_epoch_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "time_epoch");
            cJSON_bool time_epoch_is_number =
                cJSON_IsNumber(time_epoch_hdl);
            if (!time_epoch_is_number) { return false; }
            double time_epoch =
                cJSON_GetNumberValue(time_epoch_hdl);

            // Skip if forecast is beyond 24 hours
            if ((time_t)(time_epoch) > now + day_in_sec) { continue; }

            // Else check rain conditions

            //// First Main Weather Designation
            cJSON_bool has_condition =
                cJSON_HasObjectItem(hour_item_hdl, "condition");
            if (!has_condition) { return false; }
            cJSON *condition_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "condition");
            cJSON_bool has_text =
                cJSON_HasObjectItem(condition_hdl, "text");
            if (!has_text) { return false; }
            cJSON *text_hdl =
                cJSON_GetObjectItem(condition_hdl, "text");
            char *text_str = cJSON_Print(text_hdl);

            int text_rain = strncmp(text_str, "Rain",
                                    4 * sizeof(char));
            free(text_str);
            if (0 == text_rain) { break; }

            //// Second Probability of Precipitation (pop)
            cJSON_bool has_chance_of_rain =
                cJSON_HasObjectItem(hour_item_hdl, "chance_of_rain");
            if (!has_chance_of_rain) { return false; }
            cJSON *chance_of_rain_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "chance_of_rain");
            cJSON_bool chance_of_rain_is_number =
                cJSON_IsNumber(chance_of_rain_hdl);
            if (!chance_of_rain_is_number) { return false; }
            double chance_of_rain =
                cJSON_GetNumberValue(chance_of_rain_hdl);

            //// Third will_it_rain is 1
            cJSON_bool has_will_it_rain =
                cJSON_HasObjectItem(hour_item_hdl, "will_it_rain");
            if (!has_will_it_rain) { return false; }
            cJSON *will_it_rain_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "will_it_rain");
            cJSON_bool will_it_rain_is_number =
                cJSON_IsNumber(will_it_rain_hdl);
            if (!will_it_rain_is_number) { return false; }
            double will_it_rain =
                cJSON_GetNumberValue(will_it_rain_hdl);

            // Combine to check for rain
            if (0 == text_rain &&
                chance_of_rain > 0.55 &&
                will_it_rain != 0) {
                return true;
            }
        } // for loop hour
    } // for loop forecastday

    // Free memory
    free(json_resp_str);
    cJSON_Delete(json_hdl);
    free(rb.data);
    curl_easy_cleanup(curl_hdl);

    printf("----------End WAPI query----------\n");
    return false;
}

bool make_wbit_api_call(struct alert_input in) {

    printf("----------Begin WBIT query----------\n");
    // Initialize handles and buffers
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = { rb_data, 0, INIT_RB_SIZE };
    CURL *curl_hdl = curl_easy_init();

    // Point a response buffer to tell curl where to write
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEDATA, &rb);
    check_curl_error(c, "CURLOPT_WRITEDATA");

    // Construct the url
    char url[MAX_URL_LEN];
    char *base = "https://api.weatherbit.io/v2.0/forecast/daily";
    int days = 2;
    char *api_key = getenv("WBIT_API_KEY");
    // Will auto-null-terminate
    /* int bytes = */
    snprintf(url, sizeof(url)/sizeof(*url),
             "%s?key=%s&days=%d&lat=%f&lon=%f",
             base, api_key, days, in.lat, in.lon);
    /* printf("<%d> bytes written to url: <%s>\n", bytes, url); */
    c = curl_easy_setopt(curl_hdl, CURLOPT_URL, url);
    check_curl_error(c, "CURLOPT_URL");

    // Give Curl custom callback to write to local resp buf
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c, "CURLOPT_WRITEFUNCTION");

    // Execute the call
    c = curl_easy_perform(curl_hdl);
    check_curl_error(c, "PERFORM");

    // Get response status code
    long status_code;
    curl_easy_getinfo(curl_hdl, CURLINFO_RESPONSE_CODE,
                      &status_code);
    /* printf("HTTP status: %ld\n", status_code); */

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(rb.data);
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    cJSON_bool has_data =
        cJSON_HasObjectItem(json_hdl, "data");
    if (!has_data) { return false; }
    cJSON *data_hdl = cJSON_GetObjectItem(json_hdl, "data");
    int data_size = cJSON_GetArraySize(data_hdl);
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
    for (int i = 0; i < data_size; i++) {
        cJSON *data_item_hdl = cJSON_GetArrayItem(data_hdl, i);
        cJSON_bool has_datetime =
            cJSON_HasObjectItem(data_item_hdl, "datetime");
        if (!has_datetime) { return false; }
        cJSON *datetime_hdl =
            cJSON_GetObjectItem(data_item_hdl, "datetime");
        cJSON_bool datetime_is_string =
            cJSON_IsString(datetime_hdl);
        if (!datetime_is_string) { return false; }
        char *datetime = cJSON_GetStringValue(datetime_hdl);
        int year, month, day;
        sscanf(datetime, "%d-%d-%d", &year, &month, &day);

        // Skip if forecast is beyond 1 day
        if (today->tm_year > year ||
            today->tm_mon > month ||
            today->tm_mday > day) {
            continue;
        }

        // Else check weather conditions
        //// First Weather Description
        cJSON_bool has_weather =
            cJSON_HasObjectItem(data_item_hdl, "weather");
        if (!has_weather) { return false; }
        cJSON *weather_hdl =
            cJSON_GetObjectItem(data_item_hdl, "weather");
        cJSON_bool has_description =
            cJSON_HasObjectItem(weather_hdl, "description");
        if (!has_description) { return false; }
        cJSON *description_hdl =
            cJSON_GetObjectItem(weather_hdl, "description");
        cJSON_bool description_is_string =
            cJSON_IsString(description_hdl);
        if (!description_is_string) { return false; }
        char *description =
            cJSON_GetStringValue(description_hdl);
        int description_rain = strncmp(description, "Rain",
                                       4 * sizeof(char));
        if (0 == description_rain) { break; }

        //// Second Probability of Precipitation (pop)
        cJSON_bool has_pop =
            cJSON_HasObjectItem(data_item_hdl, "pop");
        if (!has_pop) { return false; }
        cJSON *pop_hdl = cJSON_GetObjectItem(data_item_hdl, "pop");
        cJSON_bool pop_is_number = cJSON_IsNumber(pop_hdl);
        if (!pop_is_number) { return false; }
        double pop = cJSON_GetNumberValue(pop_hdl);

        //// Third will_it_rain is 1
        cJSON_bool has_precip =
            cJSON_HasObjectItem(data_item_hdl, "precip");
        if (!has_precip) { return false; }
        cJSON *precip_hdl =
            cJSON_GetObjectItem(data_item_hdl, "precip");
        cJSON_bool precip_is_number = cJSON_IsNumber(precip_hdl);
        if (!precip_is_number) { return false; }
        double precip = cJSON_GetNumberValue(precip_hdl);

        // Combine to check for rain
        if (0 == description_rain &&
            pop > 0.55 &&
            precip > 0) {
            return true;
        }
    } // for loop data

    // Free memory
    free(json_resp_str);
    cJSON_Delete(json_hdl);
    free(rb.data);
    curl_easy_cleanup(curl_hdl);

    printf("----------End WBIT query----------\n");
    return false;
}

int main() {

    /* The latitude of Irvine, California is approximately */
    /* 33.6695° N and the longitude is about -117.823° W. */
    struct alert_input in = { 33.6695, -117.823 };
    printf("Query for lat={%f}, lon={%f}\n", in.lat, in.lon);
    struct alert_output out = { false, false };
    /* char url[MAX_URL_LEN]; */
    /* int bytes = snprintf(url, */
    /*                      sizeof(url)/sizeof(*url), */
    /*                      HTTPBIN_POST); */
    /* printf("sprintf bytes written was: %d\n", bytes); */
    /* make_httpbin_api_call(url, POST); */

    // Make openweathermap api call
    out.owm_will_rain = make_owm_api_call(in);
    printf("OWM says it will rain: %d\n", out.owm_will_rain);
    out.wapi_will_rain = make_wapi_api_call(in);
    printf("WAPI says it will rain: %d\n", out.wapi_will_rain);
    out.wbit_will_rain = make_wbit_api_call(in);
    printf("WBIT says it will rain: %d\n", out.wbit_will_rain);

    // set -a
    // source ./.envrc
    // set +a
    /* char *test_api_key = getenv("TEST_API_KEY"); */
    /* printf("test api key = %s\n", test_api_key); */

    return 0;
}
