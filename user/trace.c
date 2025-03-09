#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int i;
    char *nargv[MAXARG];

    if (argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')) {
        fprintf(2, "Usage: %s mask command\n", argv[0]);
        exit(1);
    }

    if (trace(atoi(argv[1])) < 0) {
        fprintf(2, "%s: trace failed\n", argv[0]);
        exit(1);
    }

    // Sao chép các đối số cho chương trình cần chạy
    for (i = 2; i < argc && i - 2 < MAXARG - 1; i++) {
        nargv[i - 2] = argv[i];
    }
    nargv[i - 2] = 0;  // Đảm bảo danh sách tham số kết thúc bằng NULL

    if (nargv[0] == 0) {
        fprintf(2, "trace: no command to execute\n");
        exit(1);
    }

    exec(nargv[0], nargv);
    fprintf(2, "trace: exec failed\n");
    exit(1);
}
