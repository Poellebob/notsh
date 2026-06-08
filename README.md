# notsh

Minimal CLI note-taking tool.

## Usage

```
note              Create a new note (opens $EDITOR)
note open <name>  Open an existing note
note list         List all notes
```

## Build

```
nix build .#notsh        # nix flake
nix run . -- --help      # run directly

just build  # without nix
just run
```
