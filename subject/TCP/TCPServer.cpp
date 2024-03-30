#include "Common.h" // common.h 헤더파일을 include 하여 자주사용하는 헤더파일 및 오류처리를 위한 함수 사용

#define SERVERPORT 9000		// 서버 포트 번호 9000
#define BUFSIZE    512		// 서버 버퍼 사이즈는 512

int main(int argc, char *argv[])
{
	int retval;		// int형의 retval 변수 선언

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);	// listen_sock라는 이름의 소캣 생성
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");	// 제대로 생성되지 않으면 "socket()" 출력하고 프로그램 종료 -> err_quit는 오류 메세지 출력 후 exit(1)함수 호출해 프로그램 종료

	// bind()
	struct sockaddr_in serveraddr;	// sockaddr_in이라는 구조체를 serveraddr이라는 이름으로 생성
	memset(&serveraddr, 0, sizeof(serveraddr));	// 메모리를 원하는 만큼 값을 채움 memset(서버주소 시작주소, 채우길 원하는 값, 채울 크기)
	serveraddr.sin_family = AF_INET;	// sin_family변수를 AF_INET(IPv4)으로 저장 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);	// serveraddr에 sin_addr 구조체에있는 s_addr에 INADDR_ANY(소켓이 모든 로컬 인터페이스에 바인딩 될 때 사용되는 특별한 IP주소)를 네트워크 바이트 순서로 변환하여 저장
	serveraddr.sin_port = htons(SERVERPORT); // 서버 포트를 네트워크 바이트 순서로 변환하여 sin_port에 저장
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));	// 소켓에 바인딩 해주며 성공할 시 0을 반환, bind(소켓이름, 저장할 IP주소와 port번호의 변수를 가지는 구조체, 그 구조체의 데이터 크기)
	if (retval == SOCKET_ERROR) err_quit("bind()");	// 에러나면 "bind()"출력 후 프로그램 종료

	// listen()
	retval = listen(listen_sock, SOMAXCONN);	// 서버가 클라이언트와 연결 가능한지 체크하기위해 listen함수 사용, 클라이언트의 요청은 대기 queue에 push, listen(소켓이름, 대기 queue의 크기) 
	if (retval == SOCKET_ERROR) err_quit("listen()");	// 에러나면 "listen()"출력 후 프로그램 종료

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;	// client_sock라는 소켓 생성
	struct sockaddr_in clientaddr;	// clientaddr이라는 구조체 sockaddr_in 선언	
	socklen_t addrlen;	// 소켓 구조체의 크기를 addrlen으로 선언
	char buf[BUFSIZE + 1];	// BUFSIZE+1 크기의 문자열 생성

	while (1) {	// 무한반복
		// accept()
		addrlen = sizeof(clientaddr);	//addrlen을 clientaddr의 사이즈로 저장
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);	// accept함수로 클라이언트의 요청이 들어올 때 까지 대기하고 요청이 오면 새로운 소켓 생성 후 반환, accept(소켓이름, 소켓정보가 들어간 구조체 포인터, 소켓크기 주소값)
		if (client_sock == INVALID_SOCKET) {	//실패시 "accept()" 출력 후 break;
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];	// INET_ADDRSTRLEN(IPv4주소를 문자열로 표한하기 위해 충분한 길이의 메크로 상수)크기의 addr문자열 생성
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));	// 네트워크 주소를 표준형식으로 반환하여 문자열로 나타내는 함수 inet_ntop(주소체계, sin_addr의 주소, 저장할 문자열, 문자열의 크기)
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));	//접속한 클라이언트의 주소와 포트번호(호스트 바이트 순서로 변환한 포트번호) 출력

		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);	//recv함수로 데이터를 받고 성공하면 0반환
			if (retval == SOCKET_ERROR) {	// 실패할 시 "recv()" 출력 후 break;
				err_display("recv()");
				break;
			}
			else if (retval == 0)	// recv한게 없을 시 break;
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';	// buf문자열 맨뒤에 '\0'추가
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);	// 받은 데이터 출력

			// 데이터 보내기
			retval = send(client_sock, buf, retval, 0);	// send함수로 buf문자열의 데이터 보내기 성공하면 0반환
			if (retval == SOCKET_ERROR) {	// 실패할 시 "send()" 출력 후 break;
				err_display("send()");
				break;
			}
		}

		// 소켓 닫기
		close(client_sock);	// 클라이언트 소켓 닫기
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));	// 클라이언트 정보 출력
	}

	// 소켓 닫기
	close(listen_sock);	// 대기소켓 닫기
	return 0;	// 프로그램 종료
}
