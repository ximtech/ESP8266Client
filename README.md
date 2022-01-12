# ESP8266 Client

***STM32*** Low-Layer(LL) library. Micro-sized HTTP client for ESP8266 using AT commands.

### Features

- Non-blocking http request sending
- Easy request formatting
- URL parsing and validation
- Small memory footprint

### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)
```cmake
CPMAddPackage(
        NAME ESP8266Client
        GITHUB_REPOSITORY ximtech/ESP8266Client
        GIT_TAG origin/main)
```

### Project configuration

1. Start project with STM32CubeMX:
    * [USART configuration](https://github.com/ximtech/ESP8266Client/blob/main/example/config_base.PNG)
    * [NVIC configuration](https://github.com/ximtech/ESP8266Client/blob/main/example/config_nvic.PNG)
    * [DMA configuration](https://github.com/ximtech/ESP8266Client/blob/main/example/config_dma.PNG)
2. Select: Project Manager -> Advanced Settings -> USART -> LL
3. Generate Code
4. Add sources to project:

```cmake
add_subdirectory(${STM32_CORE_SOURCE_DIR}/USART/DMA)

include_directories(${includes}
        ${ESP8266_CLIENT_DIRECTORY})   # source directories

file(GLOB_RECURSE SOURCES ${sources}
        ${ESP8266_CLIENT_SOURCES})    # source files

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT}) # executable declaration should be before libraries

target_link_libraries(${PROJECT_NAME}.elf Ethernet)   # add library dependencies to project
target_link_libraries(${PROJECT_NAME}.elf Regex)
target_link_libraries(${PROJECT_NAME}.elf Vector)
target_link_libraries(${PROJECT_NAME}.elf HTTPClient)
```

3. Then Build -> Clean -> Rebuild Project

### Wiring

- <img src="https://github.com/ximtech/ESP8266Client/blob/main/example/pinout.PNG" alt="image" width="300"/>
- <img src="https://github.com/ximtech/ESP8266Client/blob/main/example/wiring.PNG" alt="image" width="300"/>
- <img src="https://github.com/ximtech/ESP8266Client/blob/main/example/wiring_2.PNG" alt="image" width="300"/>

## Usage

***Provide interrupt handler***

- Full example: [link](https://github.com/ximtech/ESP8266Client/blob/main/example/stm32f4xx_it.c)

```c
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void) {
    transferCompleteCallbackUSART_DMA(DMA2, LL_DMA_STREAM_2);    // USART1_RX
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void) {
    transferCompleteCallbackUSART_DMA(DMA2, LL_DMA_STREAM_7);    // USART1_TX
}
```

***The following example for base application***
```c
char *jsonExample = "{\"widget\": {\n"
                    "    \"debug\": \"on\",\n"
                    "    \"window\": {\n"
                    "        \"title\": \"Sample Konfabulator Widget\",\n"
                    "        \"name\": \"main_window\",\n"
                    "        \"width\": 500,\n"
                    "        \"height\": 500\n"
                    "    }
                    "}}";

    WiFi *wifi = initWifiESP8266(USART1, DMA2, LL_DMA_STREAM_2, LL_DMA_STREAM_7, 2000, 1000);
    if (wifi == NULL) while (true);

    APConnectionStatus connectionStatus = beginESP8266(wifi, "BALTICOM2G46", "adzdm99qmjwg");
    while (connectionStatus == ESP8266_WIFI_WAITING_FOR_CONNECTION) {
        connectionStatus = getAccessPointConnectionStatusESP8266(wifi);
        delay_ms(200);
    }

    if (connectionStatus == ESP8266_WIFI_CONNECTED) {
    HTTP http = getESP8266HttpClient(wifi);

    HTTPResponse response = http.POST("https://httpbin.org/post")
            .addHeader(CONNECTION, "close")
            .addHeader(ACCEPT, "application/json")
            .addHeader(CONTENT_TYPE, "application/json")
            .addHeader(USER_AGENT, "FakeAgent")
            .bindJson(jsonExample)  // add json string to request
            .execute(&http);
    
    HTTPParser httpParser = getHttpParserInstance();
    parseHttpBuffer(response.rawResponse, httpParser, HTTP_RESPONSE);
    parseHttpHeaders(httpParser, response.rawResponse);
    
    HashMap headers = httpParser->headers;
    HashMapIterator iterator = getHashMapIterator(headers);
    while (hashMapHasNext(&iterator)) {
        printf("[%s] -> [%s]\n", iterator.key, (char *) iterator.value);
    }

    printf("%s\n", httpParser.messageBody);
    while (1) {
    }
```