#include "ESP8266Client.h"

#define HTTP_DEFAULT_PORT 80
#define ESP8266_DATA_MARKER_MAX_LENGTH 20

static WiFi *wifiInstance = NULL;

static inline uint16_t getPortOrDefault(URL *url);
static HTTPResponse sendRequestCallback(URL *url, char *requestBuffer, uint32_t dataLength, bool isBlockingExecute);
static void removeESP8266DataMarker(char *rawResponse, const char *marker);


HTTP getESP8266HttpClient(WiFi *wifi) {
    HTTP http = { NULL };
    if (wifi != NULL) {
        wifiInstance = wifi;
        http = initHttpInstance(wifi->request->requestBody, wifi->request->bufferSize);
        registerHttpCallback(&http, sendRequestCallback);
    }
    return http;
}

void cleanESP8266Response(char *rawResponse) {
    removeESP8266DataMarker(rawResponse, "\r\n+IPD,");
    removeESP8266DataMarker(rawResponse, "\nCLOSED");
}

HTTPResponse sendRequestCallback(URL *url, char *requestBuffer, uint32_t dataLength, bool isBlockingExecute) {
    HTTPResponse httpResponse = {HTTP_NO_STATUS, NULL, NULL};
    if (wifiInstance != NULL) {
        httpResponse.rawResponse = wifiInstance->response->responseBody;
        ResponseStatus status = connectESP8266(wifiInstance, url->host, getPortOrDefault(url));
        if (isResponseStatusTimeout(status) || isResponseStatusError(status)) {
            httpResponse.statusCode = HTTP_SERVICE_UNAVAILABLE;
        }
        if (isResponseStatusSuccess(status)) {
            wifiInstance->request->dataLength = dataLength;
            sendRequestBodyESP8266(wifiInstance);

            if (isBlockingExecute) {
                status = waitForResponseESP8266(wifiInstance);
                if (isResponseStatusSuccess(status)) {
                    cleanESP8266Response(wifiInstance->response->responseBody);
                }
            }
        }
    }
    return httpResponse;
}

static void removeESP8266DataMarker(char *rawResponse, const char *marker) {
    char *subStringStart;
    char *subStringEnd;
    while ((subStringStart = strstr(rawResponse, marker)) != NULL) {
        subStringEnd = subStringStart;

        int replacementLength = 0;
        while (*subStringEnd != ':' && *subStringEnd != '\0' && replacementLength < ESP8266_DATA_MARKER_MAX_LENGTH) {
            replacementLength++;
            subStringEnd++;
        }
        replacementLength++; // remove also ":"

        char buffer[ESP8266_DATA_MARKER_MAX_LENGTH] = { [0 ... ESP8266_DATA_MARKER_MAX_LENGTH - 1] = 0 };
        strncpy(buffer, subStringStart, replacementLength);
        replaceString(rawResponse, buffer, "");
    }
}

static inline uint16_t getPortOrDefault(URL *url) {
    return url->port > 0 ? url->port : HTTP_DEFAULT_PORT;
}