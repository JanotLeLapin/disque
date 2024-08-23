{ gcc
, curl
, stdenv
}: stdenv.mkDerivation {
  pname = "disque";
  version = "0.1";

  buildInputs = [ gcc curl ];
  src = ./.;

  buildPhase = ''
    gcc -c disque.c -o disque.o
    gcc -shared -o libdisque.so -lcurl disque.o
  '';

  installPhase = ''
    mkdir -p $out/lib
    mkdir -p $out/include
    cp libdisque.so $out/lib
    cp disque.h $out/include
  '';
}
