#include <iostream>
#include <nlohmann/json.hpp>
#include <map>
#include <setups.h>
#include <compiler.h>

using json = nlohmann::json;

int main(int argc, char** argv) {
    // for (int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }
    setup_dir(argv[1]);

    compiler cmp(argv[1], BUILD_MODE_MAIN::RT);
    cmp.assign_rt_mode(RT_MODE::LINEAR);

    compiler::RETURN_STATUS finish_code = cmp.build();
    if (finish_code == compiler::SUCCESS)
        return 0;
    else return 1;
}