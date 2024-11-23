import socket
import base64

HOST = '127.0.0.1'
PORT = 8080

def send_request(request):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(request.encode())
        response = s.recv(1024)
        print('Received', response.decode())

# Basic authentication: username:password in Base64
auth_header = f"Authorization: Basic {base64.b64encode(b'ishank:ishank').decode()}"

# GET Request
get_request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
print("GET Request:")
send_request(get_request)

# POST Request
post_request = "POST /post_test.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n"
print("\nPOST Request:")
send_request(post_request)

# PUT Request (requires authentication)
put_message = "Hello, world!!!"
put_request = f"PUT /put_file.txt HTTP/1.1\r\nHost: localhost\r\n{auth_header}\r\nContent-Length: {len(put_message)}\r\n\r\n{put_message}"
print("\nPUT Request:")
send_request(put_request)

# DELETE Request (requires authentication)
delete_request = f"DELETE /delete_test.txt HTTP/1.1\r\nHost: localhost\r\n{auth_header}\r\n\r\n"
print("\nDELETE Request:")
send_request(delete_request)
