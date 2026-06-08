{ lib, stdenv, meson, ninja }:

stdenv.mkDerivation {
  pname = "notsh";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ meson ninja ];

  meta = with lib; {
    description = "A minimal command-line note-taking utility";
    longDescription = ''
      notsh is a minimal command-line note-taking utility written in C.
      It allows you to create, open, and list notes stored as markdown files.
    '';
    license = licenses.mit;
    platforms = platforms.unix;
    mainProgram = "note";
  };
}
