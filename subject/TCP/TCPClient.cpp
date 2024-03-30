#include "Common.h" // common.h 헤더파일을 include 하여 자주사용하는 헤더파일 및 오류처리를 위한 함수 사용

char *SERVERIP = (char *)"127.0.0.1";	// 서버 IP 주소는 "127.0.0.1 (localhost)"
#define SERVERPORT 9000					// 서버 포트 번호 9000
#define BUFSIZE    512					// 서버 버퍼 사이즈 512

int main(int argc, char *argv[])		//main에 매게변수 두개 받는 이유는..? 없어도 돌아가긴함
{
	int retval;							// retval이라는 int형 변수 선언

	// 명령행 인수가 있으면 IP 주소로 사용
	if (argc > 1) SERVERIP = argv[1];

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // sock라는 SOCKET 생성, socket(통신영역지정(IPv4),프로토콜지정(TCP),프로토콜 값 지정)
	if (sock == INVALID_SOCKET) err_quit("socket()"); // sock가 제대로 생성되지 않으면 socket()이라는 메세지와 함께 프로그램 종료

	// connect()
	struct sockaddr_in serveraddr;	// sockaddr_in이라는 구조체를 serveraddr로 선언
	memset(&serveraddr, 0, sizeof(serveraddr)); // 메모리를 0으로 채움(초기화) memset(serveraddr 시작주소, 초기값, 초기값으로 채울 크기) 
	serveraddr.sin_family = AF_INET;	//sockaddr_in 내부 sin_family(주소체계) 변수값을 AF_INET(IPv4)로 변경
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);	//ip주소를 binary형태로 변환하여 serveraddr.sin_addr에 저장 inet_pton(주소체계, 서버아이피, 저장할 변수의 주소)
	serveraddr.sin_port = htons(SERVERPORT);	// TCP/IP 바이트 순서로 값을 변환하여 저장한다
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));	// 소켓에 서버를 연결하며 성공하면 0 아닐시, -1 을 반환 connect(소켓이름, 서버에 주소정보를 담고있는 구조체의 포인터, 서버주소의 크기)
	if (retval == SOCKET_ERROR) err_quit("connect()");	// connect()함수가 연결 실패할 시 "connect()"라 띄우고 프로그램 종료

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];	// buf라는 변수를 BUFSIZE+1크기의 문자열로 선언 
	int len;	// int형식의 len변수 선언

	// 서버와 데이터 통신
	while (1) {		// 무한반복 (break; 나오기 전까지)
		// 데이터 입력
		printf("\n[보낼 데이터] ");	// [보낼 데이터] 출력
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)	// 파일에 문자열을 가져오는데 그 값이 NULL일시 break; fgets(값 넣을 문자열, 넣을 문자열의 길이, 입력받을 형식(표준입력 stdin))
			break;

		// '\n' 문자 제거
		len = (int)strlen(buf);	// len에 buf의 길이를 int형으로 변환하여 저장
		if (buf[len - 1] == '\n')	// buf[가장마지막 바로 앞순서]가 '\n일시 \0으로 변환
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)		// buf길이가 0일시 break;
			break;

		// 데이터 보내기
		retval = send(sock, buf, (int)strlen(buf), 0);	// send 함수로 소켓에 연결되어있는 다른 프로세스에 데이터 보냄 retval에는 보낸 데이터의 byte크기를 반환, send(소켓이름, 보낼 데이터를 가진 문자열, 문자열의 길이, 0)
		if (retval == SOCKET_ERROR) {	// 만약 정상적으로 보내지지 않는다면, "send()"를 출력하고 break;
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);	// 보낸 데이터 byte를 출력

		// 데이터 받기
		retval = recv(sock, buf, retval, MSG_WAITALL);	// send로 보낸 데이터를 받기위해 recv함수 사용, 반환값은 받은 데이터의 byte크기, recv(소켓이름, 데이터 받을 문자열, 보낸데이터의 byte크기, MSG_WAITALL(송수신 함수가 데이터 모두 받을때 까지 대기))
		if (retval == SOCKET_ERROR) {	// 정상적으로 받지 못한다면 "recv()"를 출력하고 break;
			err_display("recv()");
			break;
		}
		else if (retval == 0)	// 데이터를 받지 못해도 break;
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';		// 문자열 마지막에 '\0'추가
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);	// 받은 데이터 byte크기 출력
		printf("[받은 데이터] %s\n", buf);	// 받은 데이터 문자열 출력
	}

	// 소켓 닫기
	close(sock);	// 소켓닫기
	return 0;	// 프로그램 종료
}
