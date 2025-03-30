#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#define SA struct sockaddr_in

char *parse_argv(int argc, char* argv[]){
	char *ret = malloc(1024);
	for(int i = 2; i < argc; i++){
		strcat(ret, argv[i]);
		strcat(ret, " ");
	}
	return ret;
}




int verify_post(char *buf, char *memo){
	// searches through the entire buffer (JSON list of memo objects)
	// and tries to find an object that contains 'memo'
	// returns the id of that object, or -1 if no object was found

	if(strstr(buf, memo) != NULL){
		// it's in there

		// go line by line
		char *lines = strtok(buf, "\n");
		char *prev = NULL;
		//printf("gunna luup\n");
		while(lines != NULL){
			//printf("%s\n", lines);
			if(strstr(lines, memo)){ // current line contains memo
				// then previous line contains id
				prev = &prev[7];
				prev[strlen(prev)-1] = '\0'; // remove ',' char
				return atoi(prev);
			}
			prev = lines;
			lines = strtok(NULL, "\n");
		}
		printf("loop, no findy\n");
		return -1;

	} else{
		printf("no findy\n");
		return -1;
	}



}

int main(int argc, char *argv[]){
	(void) argc;
	char* cookie = argv[1];
	char buffer[4096] = {0};
	SA server;

	char *memo = parse_argv(argc, argv);
	//printf("memo parsed")
	// connect to server
	// create client socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock > 0); // assert socket creation worked
	// create server struct

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(8139);

	//printf("%x\n",server.sin_port);

	inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));

	// ensure connection
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf("%s\n", strerror(errno));
	//assert( > 0);

	// post memo
	/*char *post_msg = malloc(2048);
	post_msg = "POST /api/memo HTTP/1.1\n"; //"Cookie: Session=",cookie,"\n\n",memo;
	strcat(post_msg, "Cookie: Session=");
	strcat(post_msg, cookie);
	strcat(post_msg, "\r\n\r\n");
	strcat(post_msg, memo);
	strcat(post_msg, "\n");*/

	char *post_msg = malloc(256);
	sprintf(post_msg, "%s%s%s%s", "POST /api/memo HTTP/1.1\nCookie: Session=", cookie, "\n\n", memo);
	//printf("msg made\n");
	printf("Sending POST message\n");
	send(sock, post_msg, strlen(post_msg), 0);
	printf("Receiving POST response from server\n");
	recv(sock, buffer, 4096, 0); // send 200 OK
	//close(sock);
	//printf("%s\n", buffer);
	// was able to post according to return
	assert(strstr(buffer, "HTTP/1.1 200 OK") != NULL);
	printf("POST response OK\n");
	// verify post
	printf("Verify post success\n");
	char *ver_post_msg = malloc(256);
	sprintf(ver_post_msg, "%s%s%s", "GET /api/memo HTTP/1.1\nCookie: Session=", cookie,"\n\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf("Requesting all memos\n");
	send(sock, ver_post_msg, strlen(ver_post_msg), 0);
	printf("Receiving all memos from server\n");
	recv(sock, buffer, 4096, 0); // send all memos
	//printf("%s\n", buffer);

	printf("Searching all memos for instance\n");
	int memo_id = verify_post(buffer, memo);
	printf("memo_id: %d\n", memo_id);
	assert(memo_id >= 0); // ensure that memo was found in return, AKA post worked
	printf("Memo instance found, POST confirmed\n");

	// delete memo
	char *del_msg = malloc(256);
	char *del_msg1 = "DELETE /api/memo/";
	sprintf(del_msg, "%s%d%s%s%s",del_msg1, memo_id, " HTTP/1.1\nCookie: Session=", cookie, "\n\n");


	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf("Sending DELETE message\n");
	send(sock, del_msg, strlen(del_msg), 0);
	printf("Receiving DELETE response from server\n");
	recv(sock, buffer, 4096, 0); // send all memos
	assert(strstr(buffer, "HTTP/1.1 200 OK") != NULL);
	printf("DELETE response OK\n");

	// verify delete
	char *ver_del_msg = malloc(256);
	sprintf(ver_del_msg, "%s%d%s%s%s", "PUT /api/memo/", memo_id, " HTTP/1.1\nCookie: Session=", cookie, "\n\nphooie");

	printf("Verify delete success\n");

	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf("Attempting PUT on memo instance\n");
	send(sock, ver_del_msg, strlen(ver_del_msg), 0);
	printf("Receiving PUT response from server\n");
	recv(sock, buffer, 2048, 0); // send all memos
	assert(strstr(buffer, "HTTP/1.1 404 Not Found") != NULL);
	printf("PUT response is a 404, DELETE confirmed\n");

	printf("Program finished, exiting...\n");

	return 0;
}

