#include "client/client.h"

int main(int argc, char** argv) {
    Client::handle_client("token_1", "localhost:7000");
    return 0;
}
