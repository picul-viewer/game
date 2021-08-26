## Game

### Summary
This is my pet project - simple wanna-be-game-engine I use for experiments and development for fun. Note that it is not intended for any serious use so a lot of decisions are based just on memes (including end-stage NIH syndrom and using almost pure "C with classes and templates"). Also it contains a lot of not fully implemented features, and lots of things are not working right.

### Some features one might find interesting
 - A bunch of common structures (custom allocators, hash set, etc.)
 - Almost successful try to develop decent malloc replacement (`heap_branch` branch)
 - Overcomplicated resource management system
 - Render is actually pretty simple - don't have any frame graph yet, which prevents its' development

### Really want to run this?
OK, after cloning the repository open `game.sln` from the root directory and build `demo` and `resource_compiler` projects. Run resource compiler first - this compiles all the resources for the engine; then execute demo. Controls:
 - `WASD/mouse` - move/rotate camera
 - `Ctrl` - move camera a bit slower
 - `Shift` - move camera a bit faster
 - `F9` - some render statistics
 - `~` - console, supported commads are `log camera` and `quit`
 - `Esc` - exit