#include "builder/builder.c"

#define BUILD_AAF       4
#define BUILD_GRAD      VIRIDIS

int main() {
    builder_FileBuild(BUILD_AAF, BUILD_GRAD);
    system("start " FILE_BITMAP);
    return 0;
}
