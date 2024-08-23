{ gcc
, curl
, cjson
, clang-tools
, mkShell
}: mkShell {
  buildInputs = [ gcc curl cjson clang-tools ];
}
