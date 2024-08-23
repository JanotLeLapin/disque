{ gcc
, curl
, clang-tools
, mkShell
}: mkShell {
  buildInputs = [ gcc curl clang-tools ];
}
