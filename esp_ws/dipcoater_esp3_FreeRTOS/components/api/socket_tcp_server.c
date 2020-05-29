/*
 * wifi.c
 *
 *  Created on: 29 may. 2020
 *      Author: martin
 */


#include "socket_tcp_server.h"

#include "wifi_manager.h"




#include "esp_wifi.h"
#include "esp_system.h"

#include "http_server.h"
#include "dns_server.h"


#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>



#define PORT 3333


static const char *TAG_TASK_SOCKET = "task_tcp_socket_server";

extern int sock_global;


void socket_tcp_run(void){

	char rx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	int i;

	while (1) {

//#ifdef CONFIG_EXAMPLE_IPV4
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(PORT);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;
		inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

//#else 	//IPV6
//        struct sockaddr_in6 dest_addr;
//        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
//        dest_addr.sin6_family = AF_INET6;
//        dest_addr.sin6_port = htons(PORT);
//        addr_family = AF_INET6;
//        ip_protocol = IPPROTO_IPV6;
//        inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
//#endif

		int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Unable to create socket: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket created");

		int err = bind(listen_sock, (struct sockaddr*) &dest_addr,
				sizeof(dest_addr));
		if (err != 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Socket unable to bind: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket bound, port %d", PORT);

		err = listen(listen_sock, 1);
		if (err != 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Error occurred during listen: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket listening");

		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		uint addr_len = sizeof(source_addr);
//      int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
//      int sock_global = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		sock_global = accept(listen_sock, (struct sockaddr*) &source_addr,
				&addr_len);

		printf("Numero de Socket: %d\r\n", sock_global);

		if (sock_global < 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket accepted");

		while (1) {

			int len = recv(sock_global, rx_buffer, sizeof(rx_buffer) - 1, 0);

			// Error occurred during receiving
			if (len < 0) {
				ESP_LOGE(TAG_TASK_SOCKET, "recv failed: errno %d", errno);

				///////////****************////////////////////////////
				/////prueba por desconexion aleatoria mensaje -->  recv failed: errno 104
				close(sock_global);
				close(listen_sock);
				///////////////***********/////////////////////////
				break;
			}
			// Connection closed
			else if (len == 0) {
				ESP_LOGI(TAG_TASK_SOCKET, "Connection closed");
				break;
			}
			// Data received
			else {
				// Get the sender's ip address as string
				if (source_addr.sin6_family == PF_INET) {
					inet_ntoa_r(
							((struct sockaddr_in*) &source_addr)->sin_addr.s_addr,
							addr_str, sizeof(addr_str) - 1);
				} else if (source_addr.sin6_family == PF_INET6) {
					inet6_ntoa_r(source_addr.sin6_addr, addr_str,
							sizeof(addr_str) - 1);
				}

				rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
				ESP_LOGI(TAG_TASK_SOCKET, "Received %d bytes from %s:", len, addr_str);
				ESP_LOGI(TAG_TASK_SOCKET, "%s", rx_buffer);


				for (i = 0; i < len; i++) {
					tinysh_char_in(rx_buffer[i]);
				}
				//Para que  valide lo recibido!
				tinysh_char_in('\n');


				if (err < 0) {
				ESP_LOGE(TAG_TASK_SOCKET, "Error occurred during sending: errno %d",errno);
				break;
				}
			}
		}

		if (sock_global != -1) {
			ESP_LOGE(TAG_TASK_SOCKET, "Shutting down socket and restarting...");
			//shutdown(sock_global, 0);   /*TEST MARTES SOCKET*/
			close(sock_global); /*TEST MARTES SOCKET*/

			////FIX sacado de internet  https://www.esp32.com/viewtopic.php?t=10335
			//shutdown(listen_sock,0);  /*TEST MARTES SOCKET*/
			close(listen_sock);
			vTaskDelay(10);

		}
	}

	//vTaskDelete(NULL); /*TEST MARTES SOCKET*/

}





