/*
** RCS:
** $Log$
*/

#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <arpa/inet.h>

class HttpsServer {
public:
    HttpsServer(int port, const std::string& certFile, const std::string& keyFile);
    ~HttpsServer();
    void start();

private:
    int server_fd;
    SSL_CTX* ssl_ctx;

    void init_openssl();
    void cleanup_openssl();
    void create_server_socket(int port);
    SSL_CTX* create_ssl_context();
    void configure_ssl_context(SSL_CTX* ctx, const std::string& certFile, const std::string& keyFile);
    void handle_client(int client_fd);
};

HttpsServer::HttpsServer(int port, const std::string& certFile, const std::string& keyFile) {
    init_openssl();
    ssl_ctx = create_ssl_context();
    configure_ssl_context(ssl_ctx, certFile, keyFile);
    create_server_socket(port);
}

HttpsServer::~HttpsServer() {
    close(server_fd);
    SSL_CTX_free(ssl_ctx);
    cleanup_openssl();
}

void HttpsServer::init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void HttpsServer::cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX* HttpsServer::create_ssl_context() {
    const SSL_METHOD* method = SSLv23_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Unable to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void HttpsServer::configure_ssl_context(SSL_CTX* ctx, const std::string& certFile, const std::string& keyFile) {
    if (SSL_CTX_use_certificate_file(ctx, certFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void HttpsServer::create_server_socket(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }
}

void HttpsServer::handle_client(int client_fd) {
    SSL* ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        const char reply[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
        SSL_write(ssl, reply, strlen(reply));
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
}

void HttpsServer::start() {
    while (1) {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&addr, &len);
        if (client_fd < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        handle_client(client_fd);
    }
}

int main() {
    const int port = 4433;
    const std::string certFile = "server.crt";
    const std::string keyFile = "server.key";

    HttpsServer server(port, certFile, keyFile);
    server.start();

    return 0;
}
