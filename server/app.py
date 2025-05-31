import socket
import random
import time

HOST = '127.0.0.1'  # localhost
PORT = 9000         # same port your OrderReader connects to

def generate_order_line():
    order_type = random.choice(['BUY', 'SELL'])
    price = round(random.uniform(10, 200), 2)
    quantity = round(random.uniform(1, 100), 2)
    order_id = random.randint(1000, 9999)
    timestamp = int(time.time())
    return f"{order_type},{price},{quantity},{order_id},{timestamp}\n"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    server_socket.bind((HOST, PORT))
    server_socket.listen()
    print(f"Server listening on {HOST}:{PORT}")

    conn, addr = server_socket.accept()
    with conn:
        print(f"Connected by {addr}")
        while True:
            order_line = generate_order_line()
            print(f"Sending: {order_line.strip()}")
            conn.sendall(order_line.encode())
            time.sleep(1)  # send one order per second