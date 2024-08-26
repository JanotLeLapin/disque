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
    gcc -c util.c -o util.o
    gcc -c gateway.c -o gateway.o
    gcc -c rest.c -o rest.o
    gcc -c parse.c -o parse.o
    gcc -shared -o libdisque.so -lcurl -lcjson util.o gateway.o rest.o parse.o
  '';

  installPhase = ''
    mkdir -p $out/lib
    mkdir -p $out/include
    cp libdisque.so $out/lib
    cp disque.h $out/include
  '';
}
