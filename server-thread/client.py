import socket


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(('localhost', 3000))

print sock.recv(1024)

sock.send('bye')

sock.close()
