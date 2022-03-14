// RUN: rm -rf %t
// RUN: mkdir -p %t/Underlying
// RUN: mkdir -p %t/Overlay
// RUN: echo '// B.h in Underlying' > %t/Underlying/B.h
// RUN: echo '#ifdef NESTED' >> %t/Underlying/B.h
// RUN: echo '#include "C.h"' >> %t/Underlying/B.h
// RUN: echo '#endif' >> %t/Underlying/B.h
// RUN: echo '// C.h in Underlying' > %t/Underlying/C.h
// RUN: echo '// C.h in Overlay' > %t/Overlay/C.h

// 1) Underlying -> Overlay (C.h found, B.h falling back to Underlying)
// RUN: sed -e "s@INPUT_DIR@%{/t:regex_replacement}/Overlay@g" -e "s@OUT_DIR@%{/t:regex_replacement}/Underlying@g" %S/Inputs/vfsoverlay-directory.yaml > %t/vfs.yaml
// RUN: %clang_cc1 -Werror -I %t/Underlying -ivfsoverlay %t/vfs.yaml -fsyntax-only -E -C %s 2>&1 | FileCheck --check-prefix=DIRECT %s
// RUN: %clang_cc1 -Werror -I %t/Underlying -ivfsoverlay %t/vfs.yaml -fsyntax-only -DNESTED -E -C %s 2>&1 | FileCheck --check-prefix=DIRECT %s
// RUN: sed -e "s@INPUT_DIR@Overlay@g" -e "s@OUT_DIR@%{/t:regex_replacement}/Underlying@g" %S/Inputs/vfsoverlay-directory-relative.yaml > %t/vfs-relative.yaml
// RUN: %clang_cc1 -Werror -I %t/Underlying -ivfsoverlay %t/vfs-relative.yaml -fsyntax-only -E -C %s 2>&1 | FileCheck --check-prefix=DIRECT %s

// DIRECT: # 1 "{{.*(/|\\\\)Underlying(/|\\\\)}}B.h"
// DIRECT: {{^}}// B.h in Underlying
// DIRECT: # 1 "{{.*(/|\\\\)Overlay(/|\\\\)}}C.h"
// DIRECT: {{^}}// C.h in Overlay

#include "B.h"
#ifndef NESTED
#include "C.h"
#endif
