{ gcc
, curl
, cjson
, stdenv
}: stdenv.mkDerivation {
  pname = "disque";
  version = "0.1";

  buildInputs = [
    gcc
    (curl.override { websocketSupport = true; })
    cjson
  ];
  src = ./.;

  buildPhase = ''
    gcc -c disque.c -o disque.o
    gcc -c rest.c -o rest.o
    gcc -shared -o libdisque.so -lcurl -lcjson disque.o rest.o
  '';

  installPhase = ''
    mkdir -p $out/lib
    mkdir -p $out/include
    cp libdisque.so $out/lib
    cp disque.h $out/include
  '';
}
