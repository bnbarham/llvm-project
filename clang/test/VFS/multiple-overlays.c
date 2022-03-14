// RUN: rm -rf %t
// RUN: split-file %s %t
// RUN: sed -e "s@EXTERNAL_DIR@%{/t:regex_replacement}/B@g" -e "s@NAME_DIR@%{/t:regex_replacement}/A@g" %t/vfs/base.yaml > %t/vfs/a-b.yaml
// RUN: sed -e "s@EXTERNAL_DIR@%{/t:regex_replacement}/C@g" -e "s@NAME_DIR@%{/t:regex_replacement}/B@g" %t/vfs/base.yaml > %t/vfs/b-c.yaml

// Overlays should not be transitive, ie. given overlays of A -> B and B -> C, A should not remap to
// C.

// RUN: %clang_cc1 -Werror -I %t/A -ivfsoverlay %t/vfs/b-c.yaml -ivfsoverlay %t/vfs/a-b.yaml -fsyntax-only -E -C %t/main.c 2>&1 | FileCheck --check-prefix=FROM_B %s
// FROM_B: # 1 "{{.*(/|\\\\)B(/|\\\\)}}Header.h"
// FROM_B: // Header.h in B

// RUN: %clang_cc1 -Werror -I %t/B -ivfsoverlay %t/vfs/b-c.yaml -ivfsoverlay %t/vfs/a-b.yaml -fsyntax-only -E -C %t/main.c 2>&1 | FileCheck --check-prefix=FROM_C %s
// FROM_C: # 1 "{{.*(/|\\\\)C(/|\\\\)}}Header.h"
// FROM_C: // Header.h in C

//--- main.c
#include "Header.h"

//--- A/Header.h
// Header.h in A

//--- B/Header.h
// Header.h in B

//--- C/Header.h
// Header.h in C

//--- vfs/base.yaml
{
  'version': 0,
  'redirecting-with': 'fallthrough',
  'roots': [
    { 'name': 'NAME_DIR',
      'type': 'directory-remap',
      'external-contents': 'EXTERNAL_DIR'
    }
  ]
}
