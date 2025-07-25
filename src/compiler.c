#include "compiler.h"
#include "vm.h"

bool compile(TokenArray* tokens, Chunk* chunk) {
    (void)tokens;

    write_chunk(chunk, OP_BIPUSH, 1);
    write_chunk(chunk, 10, 1);
    write_chunk(chunk, OP_INEG, 1);
    write_chunk(chunk, OP_BIPUSH, 1);
    write_chunk(chunk, 20, 1);
    write_chunk(chunk, OP_IADD, 1);
    write_chunk(chunk, OP_PRINT, 1);
    write_chunk(chunk, OP_RETURN, 1);

    return true;
}
