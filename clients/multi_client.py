import socket
import base64
import threading

HOST = '127.0.0.1'
PORT = 8080

def send_request(request):
    """Function to send HTTP requests to the server and print the response."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(request.encode())  # Send the request
        response = s.recv(1024)     # Receive the server's response
        print('Received:', response.decode())  # Print the response

# Basic authentication: username:password in Base64
auth_header = f"Authorization: Basic {base64.b64encode(b'ishank:ishank').decode()}"

def client_task():
    """Simulate the client task to send different HTTP requests."""
    # GET Request
    get_request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    print("Sending GET Request:")
    send_request(get_request)

    # POST Request
    post_request = "POST /post_test.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n"
    print("\nSending POST Request:")
    send_request(post_request)

    # PUT Request (requires authentication)
    put_message = "Hello, world!!!"
    put_request = f"PUT /put_file.txt HTTP/1.1\r\nHost: localhost\r\n{auth_header}\r\nContent-Length: {len(put_message)}\r\n\r\n{put_message}"
    print("\nSending PUT Request:")
    send_request(put_request)

    # DELETE Request (requires authentication)
    delete_request = f"DELETE /delete_test.txt HTTP/1.1\r\nHost: localhost\r\n{auth_header}\r\n\r\n"
    print("\nSending DELETE Request:")
    send_request(delete_request)

def run_multiple_clients(num_clients):
    """Function to run multiple client tasks concurrently."""
    threads = []

    # Create and start client threads
    for i in range(num_clients):
        thread = threading.Thread(target=client_task)
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

# Number of simulated clients
NUM_CLIENTS = 5

# Start simulating multiple clients
run_multiple_clients(NUM_CLIENTS)
