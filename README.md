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
nix build .#notsh       # nix flake
nix run .               # run directly
```

### without nix

**Requirements:**
- `meson`
- `ninja`
- `clang`
- `just`

```
just build
./build/note
```
