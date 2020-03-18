/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "cJSON.h"
#include "cJSON_Utils.h"


#define PORT CONFIG_EXAMPLE_PORT



/* Used by some code below as an example datatype. */
//struct dipcommand
//{
//    const char *comando;
//    float aceleracion;
//    float velocidad;
//    float desplazamiento;
//    const char *address;
//    const char *city;
//    const char *state;
//    const char *zip;
//    const char *country;
//};




static const char *TAG = "example";
static const char *TAG_TEST = "example task test";





/* Used by some code below as an example datatype. */
struct dipstructjson
{
	const char *name_command;
    int  number_command;
    int velocity;
    int aceleration;
    int displacement;
};


///* Create a bunch of objects as demonstration. */
//static int print_preallocated(cJSON *root)
//{
//    /* declarations */
//    char *out = NULL;
//    char *buf = NULL;
//    char *buf_fail = NULL;
//    size_t len = 0;
//    size_t len_fail = 0;
//
//    /* formatted print */
//    out = cJSON_Print(root);
//
//    /* create buffer to succeed */
//    /* the extra 5 bytes are because of inaccuracies when reserving memory */
//    len = strlen(out) + 5;
//    buf = (char*)malloc(len);
//    if (buf == NULL)
//    {
//        printf("Failed to allocate memory.\n");
//        exit(1);
//    }
//
//    /* create buffer to fail */
//    len_fail = strlen(out);
//    buf_fail = (char*)malloc(len_fail);
//    if (buf_fail == NULL)
//    {
//        printf("Failed to allocate memory.\n");
//        exit(1);
//    }
//
//    /* Print to buffer */
//    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
//        printf("cJSON_PrintPreallocated failed!\n");
//        if (strcmp(out, buf) != 0) {
//            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
//            printf("cJSON_Print result:\n%s\n", out);
//            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
//        }
//        free(out);
//        free(buf_fail);
//        free(buf);
//        return -1;
//    }
//
//    /* success */
//    printf("%s\n", buf);
//
//    /* force it to fail */
//    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
//        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
//        printf("cJSON_Print result:\n%s\n", out);
//        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
//        free(out);
//        free(buf_fail);
//        free(buf);
//        return -1;
//    }
//
//    free(out);
//    free(buf_fail);
//    free(buf);
//    return 0;
//}

/* Create a bunch of objects as demonstration. */
static void create_objects(void)
{

	cJSON *root = NULL;
	cJSON *item = NULL;
	char *out = NULL;
	char *test = NULL;

	struct dipstructjson  field = {
		.name_command = "LOADPROGRAMSTANDARD",
		.number_command = 12,
		.velocity = 323232323,
		.aceleration= 22212,
		.displacement= 32232
	};


	struct dipstructjson  data_receive = {
		.name_command = "CACA",
		.number_command = 0,
		.velocity= 998989,
		.aceleration= 77777,
		.displacement= 354
	};

	root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "Name Command", field.name_command);
    cJSON_AddNumberToObject(root, "Number Command",field.number_command);
    cJSON_AddNumberToObject(root, "Velocity",field.velocity);
    cJSON_AddNumberToObject(root, "Aceleration",field.aceleration);
    cJSON_AddNumberToObject(root, "Displacement",field.displacement);

    out = cJSON_Print(root);
    printf("%s\r\n", out);    //-> Funciona


    item = cJSON_Parse(out);


    data_receive.aceleration= (int) cJSON_GetObjectItem(item, "Aceleration")->valueint;

    printf("velocidad  -> %d!! \r\n",data_receive.velocity);
    printf("aceleracion recibida por JSON -> %d !!\r\n",data_receive.aceleration);

    printf("nombre comando  ori  -> %s !!\r\n",data_receive.name_command);
    data_receive.name_command=(char *)cJSON_GetObjectItem(item, "Name Command")->valuestring;
    printf("nombre comando  recibida por JSON -> %s !!\r\n",data_receive.name_command);

//    printf("%s\r\n", test);


    cJSON_Delete(root);
    cJSON_Delete(item);

    free(out);
    free(test);

}



static void test_task(void *pvParameters){

	while (1){

		 ESP_LOGE(TAG_TEST, "Ejecuto tarea test!");
		 /* print the version */
		 printf("Version: %s\n", cJSON_Version());

		 /* Now some samplecode for building objects concisely: */
		 create_objects();

		 vTaskDelay(5000 / portTICK_RATE_MS);
	}

}


static void tcp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    int i;

    while (1) {

#ifdef CONFIG_EXAMPLE_IPV4
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
        struct sockaddr_in6 dest_addr;
        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket accepted");

        while (1) {

            printf("ANTES DEL RECV\r\n");
        	int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        	printf("DESPUES DEL RECV\r\n");
        	// Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Connection closed
            else if (len == 0) {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.sin6_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.sin6_family == PF_INET6) {
                    inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);

                for (i=0;i<len;i++){
                printf("Caracter: %c\r\n", rx_buffer[i] );

                }



                int err = send(sock, rx_buffer, len, 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);

            ////FIX sacado de internet  https://www.esp32.com/viewtopic.php?t=10335
            shutdown(listen_sock,0);
            close(listen_sock);
            vTaskDelay(5);

        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(test_task, "test", 4096, NULL, 5, NULL);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
}
