#include "slipa/slipa.hpp"
#include <string>

int main(int argc, char *argv[])
{
    using namespace slipa;
    std::string msg = "abobusччячсыв\xC0\xDB";
    std::string encoded;
    Write(msg, [&](auto sv) -> CannotFail {
        encoded += sv;
        return {};
    });
    std::string back;
    Read(encoded, [&](auto sv) -> CannotFail {
        back += sv;
        return {};
    });
    assert(msg == back);
    return 0;
}
