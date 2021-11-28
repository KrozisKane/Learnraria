# Learnraria
Hello everyone, I am Krozis Kane and I currently learn C++. As my practice I chose to create a small game using SFML library. This game will be a tile-based game, which appearance and game mechanics will be like in the Terraria game.  
I chose Terraria because it's one of my favorite games. It is interesting for me to understand how its varied mechanics are implemented and try to reproduce it in C++.  
I also assume that there are people who share my interest. This is why I have made the code open source and distributed under the GNU General Public License v3.0, so that anyone interested can freely use my work in their own projects.
Used materials (images, sounds, etc.) are trademarks and copyrights of Re-Logic.

## About branches and versions
Each branch in this repository is dedicated to a specific game version.

## Сurrent version description
Current game version is 0.5.
You can move the character across the game field (with A, D and Space keys) and mine/place its tiles (with LMB/RMB) within a certain radius from the character.
### Changelog
Game:
- Changed tile size and tile types textures to be like in the Terraria
- Added items, which drop from mined tiles
- Made the game field random generated and added new tiles to it (stone, iron ore, copper ore, gold ore, wood and wooden planks)
- Reworked hotbar for new tile types
- Added trees to the game field
- Added walls to the game field
- Reworked the background
- Added main menu
- Added save and load ability
- Added sounds and music

Code:
- Added a class for items
- Added a class for tiles
- Added a class for character and items physics
- Variable, class and method names improved and now reflect their purpose
- Deleted unnecessary comments and translated remaining of them to English

## Planned content
**v0.6:**
- Rework the interface class (add Terraria-like inventory and hotbar, add HP and mana bars, etc.)
- Add more tiles in randomgen for use as crafting materials (clay, sand, etc.)
- Add crafting system and some crafting recipes
- Add interactive equipment (workbench, furnace, etc.)
- Add some weapons, tools and armor

**v0.7:**
- Add some enemies (slimes and zombies)
- Add first boss (The Eye of Cthulhu)

## What I use
- C++ 14
- SFML library v.2.5.1 
- Visual Studio Community 2019
- [ArmorHelper](https://forums.terraria.org/index.php?threads/armorhelper-sprite-armor-sets-30x-times-faster.68744/) for creating Terraria-like character textures file
- [TConvert](https://forums.terraria.org/index.php?threads/tconvert-extract-content-files-and-convert-them-back.61706/) for extracting resources from the Terraria
