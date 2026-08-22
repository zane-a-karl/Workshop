/**
 * GOAL: create a script that makes an api request to 3
 *       different weather APIs, averages their predictions
 *       then alerts me if there is rain in the forecast for
 *       my home in the next few days.
 */
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <curl/easy.h>

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INIT_RB_SIZE 256
#define MAX_URL_LEN 512

struct alert_input {
    double lat;
    double lon;
};

struct alert_output {
    bool owm_will_rain;
    bool wapi_will_rain;
    bool wbit_will_rain;
};

struct response_buffer {
    char *data;
    size_t size_assigned;
    size_t size_reserved;
};

enum json_step_type { KEY, INDEX };

struct json_path_step {
    enum json_step_type type;
    union {
        const char *key;
        double    index;
    };
};

enum json_field_type { STRING, NUMBER, NOT_FOUND };

struct json_result_field {
    enum json_field_type type;
    union {
        const char *string;
        const double number;
    };
};

struct json_result_field *
get_json_path_field(cJSON *root,
                    struct json_path_step *path,
                    size_t path_length) {

    // loop through the path until path_length
    // check that the next step exists
    // check that the next step's type is correct
    // get a handle to the step and save it for the next iteration
    return NULL;
}

void check_curl_error(CURLcode c, char *subject) {
    if (c != CURLE_OK) {
        printf("Curl <%s> exited with error code: %d\n", subject, c);
    }
}

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

char *make_http_get(char *url) {

    // Initialize handles and buffers
    CURLcode c;
    char *rb_data = malloc(INIT_RB_SIZE * sizeof(*rb_data));
    struct response_buffer rb = { rb_data, 0, INIT_RB_SIZE };
    CURL *curl_hdl = curl_easy_init();
    if (curl_hdl == NULL) { free(rb_data); return NULL; }

    // Point to a response buffer to tell curl where to write
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEDATA, &rb);
    check_curl_error(c, "CURLOPT_WRITEDATA");

    c = curl_easy_setopt(curl_hdl, CURLOPT_URL, url);
    check_curl_error(c, "CURLOPT_URL");

    // Give Curl custom callback to write to local resp buf
    c = curl_easy_setopt(curl_hdl, CURLOPT_WRITEFUNCTION,
                         curl_fwrite_callback);
    check_curl_error(c, "CURLOPT_WRITEFUNCTION");

    // Follow HTTP redirects
    c = curl_easy_setopt(curl_hdl, CURLOPT_FOLLOWLOCATION, 1L);
    check_curl_error(c, "CURLOPT_FOLLOWLOCATION");

    // Execute the call
    c = curl_easy_perform(curl_hdl);
    check_curl_error(c, "PERFORM");

    // Get response status code
    long status_code;
    curl_easy_getinfo(curl_hdl, CURLINFO_RESPONSE_CODE,
                      &status_code);
    if (status_code != 200) {
        printf("HTTP status: %ld\n", status_code);
        curl_easy_cleanup(curl_hdl);
        return NULL;
    }

    curl_easy_cleanup(curl_hdl);
    return rb.data; /* Caller must free this memory */
}

bool make_owm_api_call(struct alert_input in) {

    // Construct the url
    /* Go to https://home.openweathermap.org/myservices */
    /* > view > scroll down to Free Tier */
    char url[MAX_URL_LEN];
    char *api_key = getenv("OWM_API_KEY");
    // -1 b/c will auto-null-terminate
    int bytes =
        snprintf(url, MAX_URL_LEN - 1,
                 "https://api.openweathermap.org/data/2.5/forecast"
                 "?lat=%f&lon=%f&appid=%s",
                 in.lat, in.lon, api_key);
    if (bytes >= MAX_URL_LEN || bytes < 0) {
        printf("<%d> bytes written to url: <%s>\n", bytes, url);
        goto CLEANUP_OWM3;
    }
    char *resp = make_http_get(url);
    if (resp == NULL) { printf("owm make_http_get\n"); goto CLEANUP_OWM2; }

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(resp);
    if (json_hdl == NULL) { printf("json_hdl\n"); goto CLEANUP_OWM1; }
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    free(json_resp_str);
    cJSON_bool has_list = cJSON_HasObjectItem(json_hdl, "list");
    if (!has_list) { printf("has_list\n"); goto CLEANUP_OWM1; }
    cJSON *list_hdl = cJSON_GetObjectItem(json_hdl, "list");
    int list_size = cJSON_GetArraySize(list_hdl);
    time_t now = time(NULL);
    time_t day_in_sec = 60 * 60 * 24;

    for (int i = 0; i < list_size; i++) {
        cJSON *list_item_hdl = cJSON_GetArrayItem(list_hdl, i);
        cJSON_bool has_dt =
            cJSON_HasObjectItem(list_item_hdl, "dt");
        if (!has_dt) { printf("has_dt\n"); goto CLEANUP_OWM1; }
        cJSON *dt_hdl = cJSON_GetObjectItem(list_item_hdl, "dt");
        cJSON_bool dt_is_number = cJSON_IsNumber(dt_hdl);
        if (!dt_is_number) { printf("dt_is_number\n"); goto CLEANUP_OWM1; }
        double dt = cJSON_GetNumberValue(dt_hdl);

        // Skip if forecast is beyond 24 hours
        if ((time_t)(dt) > now + day_in_sec) { continue; }

        // Else check rain conditions
        //// First Probability of Precipitation (pop)
        cJSON_bool has_pop =
            cJSON_HasObjectItem(list_item_hdl, "pop");
        if (!has_pop) { printf("has_pop\n"); goto CLEANUP_OWM1; }
        cJSON *pop_hdl = cJSON_GetObjectItem(list_item_hdl, "pop");
        cJSON_bool pop_is_number = cJSON_IsNumber(pop_hdl);
        if (!pop_is_number) { printf("pop_is_number\n"); goto CLEANUP_OWM1; }
        // Is a probability 0-1
        double pop = cJSON_GetNumberValue(pop_hdl);

        // Combine to check for rain
        if (pop > 0.55) {
            free(resp);
            cJSON_Delete(json_hdl);
            return true;
        }
    } // for loop

    // Free memory
 CLEANUP_OWM1:
    cJSON_Delete(json_hdl);
 CLEANUP_OWM2:
    free(resp);
 CLEANUP_OWM3:
    return false;
}

bool make_wapi_api_call(struct alert_input in) {

    // Construct the url
    char url[MAX_URL_LEN];
    char *method = "forecast.json";
    char *city = "Irvine";
    int days = 2;
    char *api_key = getenv("WAPI_API_KEY");
    // -1 b/c will auto-null-terminate
    int bytes =
        snprintf(url, MAX_URL_LEN - 1,
                 "http://api.weatherapi.com/v1/"
                 "%s?key=%s&q=%s&days=%d",
                 method, api_key, city, days);
    if (bytes >= MAX_URL_LEN || bytes < 0) {
        printf("<%d> bytes written to url: <%s>\n", bytes, url);
        goto CLEANUP_WAPI3;
    }
    char *resp = make_http_get(url);
    if (resp == NULL) { printf("wapi make_http_get\n"); goto CLEANUP_WAPI2; }

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(resp);
    if (json_hdl == NULL) { printf("json_hdl\n"); goto CLEANUP_WAPI1; }
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    free(json_resp_str);
    cJSON_bool has_forecast =
        cJSON_HasObjectItem(json_hdl, "forecast");
    if (!has_forecast) { printf("has_forecast\n"); goto CLEANUP_WAPI1; }
    cJSON *forecast_hdl = cJSON_GetObjectItem(json_hdl, "forecast");
    cJSON_bool has_forecastday =
        cJSON_HasObjectItem(forecast_hdl, "forecastday");
    if (!has_forecastday) { printf("has_forecastday\n"); goto CLEANUP_WAPI1; }
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
        if (!has_hour) { printf("has_hour\n"); goto CLEANUP_WAPI1; }
        cJSON *hour_hdl =
            cJSON_GetObjectItem(forecastday_item_hdl, "hour");
        int hour_size = cJSON_GetArraySize(hour_hdl);

        for (int j = 0; j < hour_size; j++) {
            cJSON *hour_item_hdl = cJSON_GetArrayItem(hour_hdl, j);
            cJSON_bool has_time_epoch =
                cJSON_HasObjectItem(hour_item_hdl, "time_epoch");
            if (!has_time_epoch) { printf("has_time_epoch\n"); goto CLEANUP_WAPI1; }
            cJSON *time_epoch_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "time_epoch");
            cJSON_bool time_epoch_is_number =
                cJSON_IsNumber(time_epoch_hdl);
            if (!time_epoch_is_number) { printf("time_epoch_is_number\n"); goto CLEANUP_WAPI1; }
            double time_epoch =
                cJSON_GetNumberValue(time_epoch_hdl);

            // Skip if forecast is beyond 24 hours
            if ((time_t)(time_epoch) > now + day_in_sec) { continue; }

            // Else check rain conditions
            //// First Probability of Precipitation (pop)
            cJSON_bool has_chance_of_rain =
                cJSON_HasObjectItem(hour_item_hdl,
                                    "chance_of_rain");
            if (!has_chance_of_rain) { printf("has_chance_of_rain\n"); goto CLEANUP_WAPI1; }
            cJSON *chance_of_rain_hdl =
                cJSON_GetObjectItem(hour_item_hdl,
                                    "chance_of_rain");
            cJSON_bool chance_of_rain_is_number =
                cJSON_IsNumber(chance_of_rain_hdl);
            if (!chance_of_rain_is_number) { printf("chance_of_rain_is_number\n"); goto CLEANUP_WAPI1; }
            double chance_of_rain =
                cJSON_GetNumberValue(chance_of_rain_hdl);

            //// Third will_it_rain is 1
            cJSON_bool has_will_it_rain =
                cJSON_HasObjectItem(hour_item_hdl, "will_it_rain");
            if (!has_will_it_rain) { printf("has_will_it_rain\n"); goto CLEANUP_WAPI1; }
            cJSON *will_it_rain_hdl =
                cJSON_GetObjectItem(hour_item_hdl, "will_it_rain");
            cJSON_bool will_it_rain_is_number =
                cJSON_IsNumber(will_it_rain_hdl);
            if (!will_it_rain_is_number) { printf("will_it_rain_is_number\n"); goto CLEANUP_WAPI1; }
            double will_it_rain =
                cJSON_GetNumberValue(will_it_rain_hdl);

            // Combine to check for rain
            if (chance_of_rain > 55 && will_it_rain != 0) {
                free(resp);
                cJSON_Delete(json_hdl);
                return true;
            }
        } // for loop hour
    } // for loop forecastday

    // Free memory
 CLEANUP_WAPI1:
    cJSON_Delete(json_hdl);
 CLEANUP_WAPI2:
    free(resp);
 CLEANUP_WAPI3:
    return false;
}

bool make_wbit_api_call(struct alert_input in) {

    // Construct the url
    char url[MAX_URL_LEN];
    int days = 2;
    char *api_key = getenv("WBIT_API_KEY");
    // -1 b/c will auto-null-terminate
    int bytes =
        snprintf(url, MAX_URL_LEN - 1,
                 "https://api.weatherbit.io/v2.0/forecast/daily?"
                 "key=%s&days=%d&lat=%f&lon=%f",
                 api_key, days, in.lat, in.lon);
    if (bytes >= MAX_URL_LEN || bytes < 0) {
        printf("<%d> bytes written to url: <%s>\n", bytes, url);
        goto CLEANUP_WBIT3;
    }
    char *resp = make_http_get(url);
    if (resp == NULL) { printf("wbit make_http_get\n"); goto CLEANUP_WBIT2; }

    // Parse the JSON response
    cJSON *json_hdl = cJSON_Parse(resp);
    if (json_hdl == NULL) { printf("json_hdl\n"); goto CLEANUP_WBIT1; }
    char *json_resp_str = cJSON_Print(json_hdl);
    /* printf("response: %s\n", json_resp_str); */
    free(json_resp_str);
    cJSON_bool has_data =
        cJSON_HasObjectItem(json_hdl, "data");
    if (!has_data) { printf("has_data\n"); goto CLEANUP_WBIT1; }
    cJSON *data_hdl = cJSON_GetObjectItem(json_hdl, "data");
    int data_size = cJSON_GetArraySize(data_hdl);
    time_t now = time(NULL);
    time_t day_in_sec = 60 * 60 * 24;
    /* struct tm *today = localtime(&now); */

    for (int i = 0; i < data_size; i++) {
        cJSON *data_item_hdl = cJSON_GetArrayItem(data_hdl, i);
        cJSON_bool has_ts =
            cJSON_HasObjectItem(data_item_hdl, "ts");
        if (!has_ts) { printf("has_ts\n"); goto CLEANUP_WBIT1; }
        cJSON *ts_hdl = cJSON_GetObjectItem(data_item_hdl, "ts");
        cJSON_bool ts_is_number = cJSON_IsNumber(ts_hdl);
        if (!ts_is_number) { printf("ts_is_number\n"); goto CLEANUP_WBIT1; }
        time_t ts = (time_t)cJSON_GetNumberValue(ts_hdl);

        // Skip if forecast is beyond 1 day
        double diff = difftime(ts, now);
        if (diff > day_in_sec) {
            continue;
        }

        // Else check weather conditions
        //// First Probability of Precipitation (pop)
        cJSON_bool has_pop =
            cJSON_HasObjectItem(data_item_hdl, "pop");
        if (!has_pop) { printf("has_pop\n"); goto CLEANUP_WBIT1; }
        cJSON *pop_hdl = cJSON_GetObjectItem(data_item_hdl, "pop");
        cJSON_bool pop_is_number = cJSON_IsNumber(pop_hdl);
        if (!pop_is_number) { printf("pop_is_number\n"); goto CLEANUP_WBIT1; }
        // is a percentage 0-100
        double pop = cJSON_GetNumberValue(pop_hdl);

        //// Third will_it_rain is 1
        cJSON_bool has_precip =
            cJSON_HasObjectItem(data_item_hdl, "precip");
        if (!has_precip) { printf("has_precip\n"); goto CLEANUP_WBIT1; }
        cJSON *precip_hdl =
            cJSON_GetObjectItem(data_item_hdl, "precip");
        cJSON_bool precip_is_number = cJSON_IsNumber(precip_hdl);
        if (!precip_is_number) { printf("precip_is_number\n"); goto CLEANUP_WBIT1; }
        double precip = cJSON_GetNumberValue(precip_hdl);

        // Combine to check for rain
        if (pop > 55 && precip > 0) {
            free(resp);
            cJSON_Delete(json_hdl);
            return true;
        }
    } // for loop data

    // Free memory
 CLEANUP_WBIT1:
    cJSON_Delete(json_hdl);
 CLEANUP_WBIT2:
    free(resp);
 CLEANUP_WBIT3:
    return false;
}

/** NOTE: you must source and export your
 * .envrc to access api keys for this program
 * set -a
 * source ./.envrc
 * set +a
 */
int main() {

    // Init libcurl
    CURLcode c = curl_global_init(CURL_GLOBAL_DEFAULT);
    check_curl_error(c, "curl_global_init");

    /* The latitude of Irvine, California is approximately */
    /* 33.6695° N and the longitude is about -117.823° W. */
    struct alert_input in = { 33.6695, -117.823 };
    /* printf("Query for lat={%f}, lon={%f}\n", in.lat, in.lon); */
    struct alert_output out = { false, false, false };

    // Make api calls
    out.owm_will_rain = make_owm_api_call(in);
    out.wapi_will_rain = make_wapi_api_call(in);
    out.wbit_will_rain = make_wbit_api_call(in);

    // If a majority of calls forecast rain return RAIN
    if ( (out.owm_will_rain && out.wapi_will_rain) ||
         (out.owm_will_rain && out.wbit_will_rain) ||
         (out.wapi_will_rain && out.wbit_will_rain) ) {
        printf("RAIN!!!\n");
    } else {
        printf("No rain\n");
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}
