# Learnraria
Hello everyone, I am Krozis Kane and I currently learn C++. As my practice I chose to create a small game using SFML library. This game will be a tile-based game, which appearance and game mechanics will be like in the Terraria game.  
I chose Terraria because it's one of my favorite games. It is interesting for me to understand how its varied mechanics are implemented and try to reproduce it in C++.  
I also assume that there are people who share my interest. This is why I have made the code open source and distributed under the GNU General Public License v3.0, so that anyone interested can freely use my work in their own projects.
Used materials (images, sounds, etc.) are trademarks and copyrights of Re-Logic.

## About branches and versions
Each branch in this repository will be dedicated to specific game version.  
I started to upload the code from 0.4 version, because early versions of the code were more like stages of forming a base for something bigger.  
The code is commented in detail. So far, all comments are in Russian.

## Сurrent version description
Current game version is 0.4.  
For now, the game is just a window with primitivly generated "tile" game field and a character under your control. Game window resolution and game world size can be changed via its options in the code. You can move the character across the game field (with W, A, S, D and Space keys) and harvest/place its tiles (with LMB/RMB) within a certain radius from the character. Game field tiles and the character have its own collisions. Camera is centered relativly to the character, like in Terraria. I have also added some stuff like smooth movement and animations for all character actions.  
In future versions I'll add here a changelog section with a list of specific changes.

## Currently planned content
As stated above, 0.4 version is just a base for more interesting things. The plan for future versions is presented below.  

**v0.5:**
- Add main menu to the game
- Add random generation for game field
- Add ability to save and load world
- Add sounds and music

**v0.6:**
- Rework character class, improve its interface (add Terraria-like inventory and hotbar, add HP and mana bars, etc.)
- Add more tiles in randomgen algorithm for use as crafting materials (clay, sand, etc.)
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
- [ArmorHelper](https://forums.terraria.org/index.php?threads/armorhelper-sprite-armor-sets-30x-times-faster.68744/) for Terraria character textures file
