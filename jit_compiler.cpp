#include <iostream>
#include <sys/mman.h>
#include <cstring>

//int function()
//{
//    return 21;
//}


const size_t ARGS_COUNT = 11;
unsigned char function_code[] = {
        0x55,
        0x48, 0x89, 0xe5,
        0xb8, 0x15, 0x00, 0x00, 0x00,
        0x5d,
        0xc3
};


void unmap(void *ptr, size_t sz) {
    int res = munmap(ptr, sz);
    if (res != 0) {
        perror("Error clear of mapped memory.");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 1 || argc > 2) {
        printf("Expected 0 or 1 arguments\nUsage: jit_compiler or jit_compiler <number>\n");
        return -1;
    }

    if (argc == 2) {
        int num = std::stoi(std::string(argv[1]));

        unsigned char bytes[] = {
                static_cast<unsigned char>(num & 0xFF),
                static_cast<unsigned char>((num >> 8) & 0xFF),
                static_cast<unsigned char>((num >> 16) & 0xFF),
                static_cast<unsigned char>((num >> 24) & 0xFF)
        };
        for (int i = 5; i < 9; ++i) {
            function_code[i] = bytes[i - 5];
        }
    }

    void *ptr = mmap(nullptr,
                     ARGS_COUNT * sizeof(unsigned char),
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1,
                     0);

    if (ptr == (void *) -1) {
        perror("Error with mmap(2).");
        return -1;
    }

    memcpy(ptr, function_code, ARGS_COUNT);

    if (mprotect(ptr, ARGS_COUNT, PROT_READ | PROT_EXEC) != 0) {
        perror("Error with mprotect");
        unmap(ptr, ARGS_COUNT);
        return -1;
    }

    auto f = (int (*)()) ptr;
    printf("%d\n", f());

    unmap(ptr, ARGS_COUNT);
    return 0;
}