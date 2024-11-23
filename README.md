# Multi-threaded Web Server Project

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Technologies Used](#technologies-used)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Running the Server](#running-the-server)
- [HTTP Methods](#http-methods)
  - [GET Method](#get-method)
  - [POST Method](#post-method)
  - [PUT Method](#put-method)
  - [DELETE Method](#delete-method)
  - [Error Handling](#error-handling)
- [Concurrency](#concurrency)
  - [Thread Pooling](#thread-pooling)
- [Authentication and Security](#authentication-and-security)
  - [Password Protection](#password-protection)
- [Client Testing](#client-testing)
- [Log File](#log-file)
- [Screenshots](#screenshots)
- [Acknowledgements](#acknowledgements)

---

## Overview

This project implements a **multi-threaded web server** capable of handling multiple client connections concurrently. The server supports key HTTP methods (`GET`, `POST`, `PUT`), employs **thread pooling** for efficient connection handling, and includes **basic password protection** for sensitive operations.

---

## Features

- **Multi-threading with Thread Pooling:** Efficiently handles multiple client requests using a fixed-size thread pool.
- **Static File Hosting:** Serves static files (HTML, CSS, etc.) from the `static/` directory.
- **HTTP Method Support:** Supports `GET`, `POST`, and `PUT` requests.
- **Password Protection:** Secures specific endpoints with a basic password mechanism.
- **Request Logging:** Logs all incoming requests to `server_log.txt`.
- **Error Handling:** Returns appropriate error codes for invalid requests.
- **Scalability:** Handles multiple clients concurrently without performance degradation.

---

## Project Structure

```plaintext
project-root/
├── static/             # Directory containing static files
│   ├── index.html      # Default test HTML file
│   ├── post_test.html  # HTML for POST method testing
│   ├── put_file.txt    # Text file for PUT testing
├── .gitignore          # Git ignore file
├── client.py           # Python script for single client testing
├── multi_client.py     # Python script for multiple client simulation
├── README.md           # Project documentation
├── server              # Compiled server executable
├── server_log.txt      # Log file for server activity
└── server.c            # C source code for the web server
```

---

## Technologies Used

- **Programming Language:** C (Server)
- **Thread Management:** POSIX Threads (pthreads)
- **Client-Side Testing:** Python 3 (`socket` library)
- **HTTP Protocol Handling**
- **Password Management:** Secure password verification using hashing.

---

## Getting Started

### Prerequisites

- GCC (GNU Compiler Collection)
- Python 3.x
- Basic understanding of HTTP methods, multi-threading, and authentication.

### Installation

1. Clone the repository:
   ```bash
   git clone git@github.com:ishankkumar-007/web-server-in-C.git
    ```

2. Compile the server:
   ```bash
   gcc server.c -o server
   ```

3. Place your test files in the `static/` directory.

### Running the Server

1. Start the server:
   ```bash
   ./server
   ```
---

## HTTP Methods

### GET Method

- **Purpose:** Fetch static files from the `static/` directory.
- **How It Works:**  
  1. Parses the requested file path from the client’s HTTP request.  
  2. Searches for the file in the `static/` directory.  
  3. If the file exists, it sends a `200 OK` response with the file content.  
  4. If not, it sends a `404 Not Found` response.

### POST Method

- **Purpose:** Receive data from the client.
- **How It Works:**  
  1. Parses the request body to extract data sent by the client.  
  2. Processes the data (e.g., logs or writes to a file).  
  3. Sends a `200 OK` or `201 Created` response to confirm receipt.

### PUT Method

- **Purpose:** Upload or update files on the server.
- **How It Works:**  
  1. Extracts the file name and content from the HTTP request.  
  2. Writes the content to a file in the `static/` directory.  
  3. Sends `201 Created` (new file) or `200 OK` (updated file) based on the operation.

### DELETE Method

- **Purpose:** Remove files from the server's `static/` directory.  
- **How It Works:**  
  1. The client sends an HTTP request specifying the file to delete.  
  2. The server locates the file in the `static/` directory.  
  3. If the file exists:  
      - Deletes the file.  
      - Sends a `200 OK` response to confirm the deletion.  
  4. If the file does not exist, the server responds with `File Not Found`.  
  5. If the client does not have appropriate permissions (e.g., missing password), the server sends a `401 Unauthorized Authentication required` response.  


- **Server Response Codes:**  
  - `200 OK`: File successfully deleted.  
  - `404 Not Found`: File does not exist.  
  - `401 Unauthorized`: Password required or incorrect.  
  - `405 Method Not Allowed`: DELETE method is disabled or unsupported for the requested resource.

### Error Handling

- **Unsupported Methods:** Sends `405 Method Not Allowed`.  
- **File Not Found (GET):** Returns `404 Not Found`.

---

## Concurrency

### Thread Pooling

- **Purpose:** Efficiently manage server resources and handle multiple clients concurrently without creating excessive threads.  
- **Implementation:**  
  1. The server initializes a fixed-size thread pool at startup.  
  2. Incoming client requests are placed in a task queue.  
  3. Worker threads from the pool pick up tasks from the queue and process them.  
  4. Once a thread finishes handling a client, it waits for the next task in the queue.  
- **Advantages:**  
  - Reduces thread creation overhead.  
  - Ensures stable performance under heavy load.  
  - Prevents server crashes due to unbounded thread creation.

---

## Authentication and Security

### Password Protection

- **Purpose:** Secure sensitive endpoints (e.g., file upload via PUT).  
- **How It Works:**  
  1. The client includes a password in the HTTP header for sensitive requests.  
  2. The server verifies the password against a stored hash.  
  3. If the password is correct, the request proceeds; otherwise, the server returns a `401 Unauthorized` response.  
- **Configuration:** The password is hardcoded in the server for simplicity.

---

## Client Testing

### Single Client

- Use the `client.py` script to test the server with basic requests:
  ```bash
  python3 client.py
  ```

### Multi-Client Simulation

- Use the `multi_client.py` script to simulate multiple concurrent client connections:
  ```bash
  python3 multi_client.py
  ```

---

## Log File

The `server_log.txt` file logs all incoming requests, including:
- **Timestamp:** When the request was received.  
- **Client IP Address:** Identifies the client.  
- **Request Type:** Indicates the HTTP method (e.g., GET, POST).  
- **Response Code:** Shows the server's response code (e.g., `200 OK`, `404 Not Found`).  

---

## Screenshots

1. **Server Running**  
    ![Server Running Screenshot](images/server_running.png)

2. **Client Requests**  
   ![Client Output Screenshot](images/client_output.png)

---

